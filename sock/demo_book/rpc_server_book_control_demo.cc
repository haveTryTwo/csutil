// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_channel.h"
#include "sock/rpc_server.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include "base/daemon.h"
#include "base/status.h"
#include "sock/demo_book/book_common.h"
#include "sock/demo_book/proto/book.pb.h"

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

/**
 * @brief 向指定后端 DAO 发起一次 protobuf RPC 调用
 *        复用线程局部常驻连接（RpcChannel），避免 connect-per-request 造成 TIME_WAIT 堆积；
 *        连接断开时由 RpcChannel 自动重连并重试一次
 * @param ip 后端服务 ip
 * @param port 后端服务端口
 * @param req BookReq 请求
 * @param resp BookResp 响应（输出）
 * @return kOk 调用成功；其他为连接或收发失败
 */
static base::Code CallBackend(const std::string &ip, uint16_t port, const book_mgr::BookReq &req,
                              book_mgr::BookResp *resp) { /*{{{*/
  base::RpcChannel *channel = base::RpcChannel::Get(ip, port);
  return channel->SendAndRecv(req, resp);
} /*}}}*/

/**
 * @brief 从用户配置中读取两个后端 DAO 的地址
 */
static base::Code LoadBackendAddr(const base::Config &conf, std::string *leveldb_ip, uint16_t *leveldb_port,
                                  std::string *cache_ip, uint16_t *cache_port) { /*{{{*/
  std::string leveldb_port_str;
  std::string cache_port_str;

  base::Code ret = conf.GetValue(book_mgr::kLevelDbServerIp, leveldb_ip);
  if (ret != base::kOk) return ret;
  ret = conf.GetValue(book_mgr::kLevelDbServerPort, &leveldb_port_str);
  if (ret != base::kOk) return ret;
  ret = conf.GetValue(book_mgr::kCacheServerIp, cache_ip);
  if (ret != base::kOk) return ret;
  ret = conf.GetValue(book_mgr::kCacheServerPort, &cache_port_str);
  if (ret != base::kOk) return ret;

  *leveldb_port = (uint16_t)atoi(leveldb_port_str.c_str());
  *cache_port = (uint16_t)atoi(cache_port_str.c_str());

  return base::kOk;
} /*}}}*/

/**
 * @brief 处理读请求：Cache-Aside（先查缓存，未命中回源 LevelDB 并回写缓存）
 */
static void HandleRead(const book_mgr::BookReq &book_req, const std::string &leveldb_ip, uint16_t leveldb_port,
                       const std::string &cache_ip, uint16_t cache_port, book_mgr::BookResp *book_resp) { /*{{{*/
  const std::string &book_id = book_req.get_req().book_id();

  // 1) 先查缓存
  book_mgr::BookResp cache_resp;
  base::Code ret = CallBackend(cache_ip, cache_port, book_req, &cache_resp);
  if (ret == base::kOk && cache_resp.has_get_resp() &&
      cache_resp.get_resp().base().ret_code() == book_mgr::kBookRetOk) {
    book_resp->mutable_get_resp()->CopyFrom(cache_resp.get_resp());  // 命中，from_cache=true
    return;
  }

  // 2) 缓存未命中，回源 LevelDB
  book_mgr::BookResp db_resp;
  ret = CallBackend(leveldb_ip, leveldb_port, book_req, &db_resp);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(book_resp->mutable_get_resp()->mutable_base(), book_mgr::kBookRetBackendErr,
                           "call leveldb dao failed");
    return;
  }

  book_resp->mutable_get_resp()->CopyFrom(db_resp.get_resp());
  book_resp->mutable_get_resp()->set_from_cache(false);

  // 3) 回源命中则回写缓存（best-effort，失败仅告警）
  if (db_resp.get_resp().base().ret_code() == book_mgr::kBookRetOk && db_resp.get_resp().has_book()) {
    book_mgr::BookReq write_back = book_mgr::MakeCreateReq(db_resp.get_resp().book());
    book_mgr::BookResp wb_resp;
    base::Code wb_ret = CallBackend(cache_ip, cache_port, write_back, &wb_resp);
    if (wb_ret != base::kOk) {
      LOG_ERR("write back cache failed for book_id:%s, ret:%d\n", book_id.c_str(), wb_ret);
    }
  }
} /*}}}*/

/**
 * @brief 处理写请求（Create）：仅持久化 LevelDB，不预热缓存
 *        采用标准 Cache-Aside：新建数据不主动写缓存，由后续读请求回填
 */
static void HandleCreate(const book_mgr::BookReq &book_req, const std::string &leveldb_ip, uint16_t leveldb_port,
                         const std::string &cache_ip, uint16_t cache_port, book_mgr::BookResp *book_resp) { /*{{{*/
  (void)cache_ip;
  (void)cache_port;

  book_mgr::BookResp db_resp;
  base::Code ret = CallBackend(leveldb_ip, leveldb_port, book_req, &db_resp);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(book_resp->mutable_create_resp()->mutable_base(), book_mgr::kBookRetBackendErr,
                           "call leveldb dao failed");
    return;
  }
  book_resp->mutable_create_resp()->CopyFrom(db_resp.create_resp());
} /*}}}*/

/**
 * @brief 处理更新/删除：先操作 LevelDB，成功后使缓存失效
 * @param is_update true 为 Update，false 为 Delete
 */
static void HandleUpdateOrDelete(const book_mgr::BookReq &book_req, bool is_update, const std::string &leveldb_ip,
                                 uint16_t leveldb_port, const std::string &cache_ip, uint16_t cache_port,
                                 book_mgr::BookResp *book_resp) { /*{{{*/
  book_mgr::BookResp db_resp;
  base::Code ret = CallBackend(leveldb_ip, leveldb_port, book_req, &db_resp);

  int32_t backend_ret_code = book_mgr::kBookRetOk;
  std::string book_id;
  if (is_update) {
    if (ret != base::kOk) {
      book_mgr::FillBaseResp(book_resp->mutable_update_resp()->mutable_base(), book_mgr::kBookRetBackendErr,
                             "call leveldb dao failed");
      return;
    }
    book_resp->mutable_update_resp()->CopyFrom(db_resp.update_resp());
    backend_ret_code = db_resp.update_resp().base().ret_code();
    book_id = book_req.update_req().book().book_id();
  } else {
    if (ret != base::kOk) {
      book_mgr::FillBaseResp(book_resp->mutable_delete_resp()->mutable_base(), book_mgr::kBookRetBackendErr,
                             "call leveldb dao failed");
      return;
    }
    book_resp->mutable_delete_resp()->CopyFrom(db_resp.delete_resp());
    backend_ret_code = db_resp.delete_resp().base().ret_code();
    book_id = book_req.delete_req().book_id();
  }

  // 使缓存失效（best-effort）
  if (backend_ret_code == book_mgr::kBookRetOk) {
    book_mgr::BookReq invalidate = book_mgr::MakeDeleteReq(book_id);
    book_mgr::BookResp inv_resp;
    base::Code inv_ret = CallBackend(cache_ip, cache_port, invalidate, &inv_resp);
    if (inv_ret != base::kOk) {
      LOG_ERR("invalidate cache failed for book_id:%s, ret:%d\n", book_id.c_str(), inv_ret);
    }
  }
} /*}}}*/

/**
 * @brief Control 逻辑控制模块的 protobuf RPC 处理函数
 *        解析 oneof 判定操作类型，按 Cache-Aside 策略编排两个后端 DAO
 * @param conf 用户配置（含两个后端 DAO 的地址）
 * @param req BookReq protobuf 请求
 * @param resp BookResp protobuf 响应
 * @return kOk 成功
 */
base::Code ControlPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
                              ::google::protobuf::Message *resp) { /*{{{*/
  if (req == NULL || resp == NULL) return base::kInvalidParam;

  const book_mgr::BookReq *book_req = dynamic_cast<const book_mgr::BookReq *>(req);
  book_mgr::BookResp *book_resp = dynamic_cast<book_mgr::BookResp *>(resp);
  if (book_req == NULL || book_resp == NULL) return base::kInvalidParam;

  std::string leveldb_ip;
  std::string cache_ip;
  uint16_t leveldb_port = 0;
  uint16_t cache_port = 0;
  base::Code ret = LoadBackendAddr(conf, &leveldb_ip, &leveldb_port, &cache_ip, &cache_port);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(book_resp->mutable_base(), book_mgr::kBookRetBackendErr, "load backend addr failed");
    return base::kOk;
  }

  switch (book_req->req_body_case()) {
    case book_mgr::BookReq::kCreateReq:
      HandleCreate(*book_req, leveldb_ip, leveldb_port, cache_ip, cache_port, book_resp);
      break;
    case book_mgr::BookReq::kGetReq:
      HandleRead(*book_req, leveldb_ip, leveldb_port, cache_ip, cache_port, book_resp);
      break;
    case book_mgr::BookReq::kUpdateReq:
      HandleUpdateOrDelete(*book_req, true, leveldb_ip, leveldb_port, cache_ip, cache_port, book_resp);
      break;
    case book_mgr::BookReq::kDeleteReq:
      HandleUpdateOrDelete(*book_req, false, leveldb_ip, leveldb_port, cache_ip, cache_port, book_resp);
      break;
    case book_mgr::BookReq::REQ_BODY_NOT_SET:
    default:
      book_mgr::FillBaseResp(book_resp->mutable_base(), book_mgr::kBookRetInvalidReq, "req_body not set");
      break;
  }

  return base::kOk;
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  // 压测时可通过环境变量提高日志级别（如 5=kFatalErrorLevel）以屏蔽框架逐请求 trace，降低 IO 开销
  const char *log_level_env = getenv("BOOK_LOG_LEVEL");
  if (log_level_env != NULL) SetLogLevel(atoi(log_level_env));

  std::string conf_path = "./conf/control_server.conf";
  if (argc == 1) {
    fprintf(stderr, "config path using default: %s\n\n", conf_path.c_str());
  } else {
    int32_t opt = 0;
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
      switch (opt) {
        case 's':
          conf_path = optarg;
          break;
        case 'h':
          Help(argv[0]);
          return 0;
        default:
          fprintf(stderr, "Not right options\n");
          Help(argv[0]);
          return -1;
      }
    }
  }

  Config config;
  Code ret = config.LoadFile(conf_path);
  if (ret != kOk) {
    fprintf(stderr, "Failed to load conf:%d, then use no config\n", ret);
  }

  int is_daemon = 0;
  ret = config.GetInt32Value(kDaemonKey, &is_daemon);
  if (ret != kOk) {
    fprintf(stderr, "Failed to get daemon conf:%d, then use no daemon\n", ret);
  }

  if (is_daemon == 1) {
    fprintf(stderr, "Keep daemon\n");
    ret = DaemonAndKeepAlive();
    if (ret != kOk) return EXIT_FAILURE;
  } else {
    fprintf(stderr, "Keep not daemon\n");
  }

  std::string user_conf_path = "./conf/control_user_info.conf";
  Config user_conf;
  ret = user_conf.LoadFile(user_conf_path);
  if (ret != kOk) {
    LOG_ERR("Failed to load user conf:%s, ret:%d\n", user_conf_path.c_str(), ret);
    return ret;
  }

  book_mgr::BookReq req_prototype;
  book_mgr::BookResp resp_prototype;

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   ControlPbRpcAction, &req_prototype, &resp_prototype);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
