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
#include "sock/file_service_provider.h"
#include "sock/rpc_conn_pool.h"
#include "sock/service_manager.h"

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

/**
 * @brief 连接池清理钩子：实例被摘除/下线时丢弃其连接（方案B连接池模式下生效）
 * @param ip 实例 ip
 * @param port 实例端口
 * @param ctx 未使用
 */
static void DropPoolConn(const std::string &ip, uint16_t port, void *ctx) { /*{{{*/
  (void)ctx;
  if (base::RpcChannel::IsPoolMode()) base::RpcConnPool::Instance()->DropAddr(ip, port);
} /*}}}*/

/**
 * @brief 向指定下游服务发起一次 protobuf RPC 调用（经服务发现选址）
 *        1) ServiceManager 在健康实例中按策略选址；2) 复用 RpcChannel 长连接收发；
 *        3) 按调用结果上报健康（仅连接/超时类错误计入熔断，业务错误不计入），驱动异常自动摘除
 * @param service_name 下游服务名（book_mgr::kSvcLevelDbDao / kSvcCacheDao）
 * @param hash_key 选址哈希键（hash 策略用，如 book_id；其它策略忽略）
 * @param req BookReq 请求
 * @param resp BookResp 响应（输出）
 * @return kOk 调用成功；kNotFound 无可用实例；其他为连接或收发失败
 */
static base::Code CallBackend(const std::string &service_name, const std::string &hash_key,
                              const book_mgr::BookReq &req, book_mgr::BookResp *resp) { /*{{{*/
  base::Endpoint ep;
  base::Code ret = base::ServiceManager::Instance()->Pick(service_name, hash_key, &ep);
  if (ret != base::kOk) {
    LOG_ERR("no available endpoint for service:%s, ret:%d\n", service_name.c_str(), ret);
    return ret;
  }

  base::RpcChannel *channel = base::RpcChannel::Get(ep.ip, ep.port);
  ret = channel->SendAndRecv(req, resp);

  base::ServiceManager::Instance()->Report(service_name, ep, !base::RpcChannel::IsConnError(ret));
  return ret;
} /*}}}*/

/**
 * @brief 处理读请求：Cache-Aside（先查缓存，未命中回源 LevelDB 并回写缓存）
 */
static void HandleRead(const book_mgr::BookReq &book_req, book_mgr::BookResp *book_resp) { /*{{{*/
  const std::string &book_id = book_req.get_req().book_id();

  // 1) 先查缓存
  book_mgr::BookResp cache_resp;
  base::Code ret = CallBackend(book_mgr::kSvcCacheDao, book_id, book_req, &cache_resp);
  if (ret == base::kOk && cache_resp.has_get_resp() &&
      cache_resp.get_resp().base().ret_code() == book_mgr::kBookRetOk) {
    book_resp->mutable_get_resp()->CopyFrom(cache_resp.get_resp());  // 命中，from_cache=true
    return;
  }

  // 2) 缓存未命中，回源 LevelDB
  book_mgr::BookResp db_resp;
  ret = CallBackend(book_mgr::kSvcLevelDbDao, book_id, book_req, &db_resp);
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
    base::Code wb_ret = CallBackend(book_mgr::kSvcCacheDao, book_id, write_back, &wb_resp);
    if (wb_ret != base::kOk) {
      LOG_ERR("write back cache failed for book_id:%s, ret:%d\n", book_id.c_str(), wb_ret);
    }
  }
} /*}}}*/

/**
 * @brief 处理写请求（Create）：仅持久化 LevelDB，不预热缓存
 *        采用标准 Cache-Aside：新建数据不主动写缓存，由后续读请求回填
 */
static void HandleCreate(const book_mgr::BookReq &book_req, book_mgr::BookResp *book_resp) { /*{{{*/
  const std::string &book_id = book_req.create_req().book().book_id();

  book_mgr::BookResp db_resp;
  base::Code ret = CallBackend(book_mgr::kSvcLevelDbDao, book_id, book_req, &db_resp);
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
static void HandleUpdateOrDelete(const book_mgr::BookReq &book_req, bool is_update,
                                 book_mgr::BookResp *book_resp) { /*{{{*/
  const std::string &hash_id =
      is_update ? book_req.update_req().book().book_id() : book_req.delete_req().book_id();
  book_mgr::BookResp db_resp;
  base::Code ret = CallBackend(book_mgr::kSvcLevelDbDao, hash_id, book_req, &db_resp);

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
    base::Code inv_ret = CallBackend(book_mgr::kSvcCacheDao, book_id, invalidate, &inv_resp);
    if (inv_ret != base::kOk) {
      LOG_ERR("invalidate cache failed for book_id:%s, ret:%d\n", book_id.c_str(), inv_ret);
    }
  }
} /*}}}*/

/**
 * @brief 处理列表查询：直接转发 LevelDB DAO（范围扫描，不经缓存、不回写缓存）
 * @param book_req List 请求
 * @param book_resp List 响应（输出）
 */
static void HandleList(const book_mgr::BookReq &book_req, book_mgr::BookResp *book_resp) { /*{{{*/
  const std::string &prefix = book_req.list_req().prefix();  // 选址哈希键，空则等效轮询
  book_mgr::BookResp db_resp;
  base::Code ret = CallBackend(book_mgr::kSvcLevelDbDao, prefix, book_req, &db_resp);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(book_resp->mutable_list_resp()->mutable_base(), book_mgr::kBookRetBackendErr,
                           "call leveldb dao failed");
    return;
  }
  book_resp->mutable_list_resp()->CopyFrom(db_resp.list_resp());
} /*}}}*/

/**
 * @brief Control 逻辑控制模块的 protobuf RPC 处理函数
 *        解析 oneof 判定操作类型，按 Cache-Aside 策略编排两个后端 DAO；
 *        下游地址经服务发现（ServiceManager）动态选址，支持多实例与异常自动摘除
 * @param conf 用户配置（此处不再读取后端地址，地址由服务发现维护）
 * @param req BookReq protobuf 请求
 * @param resp BookResp protobuf 响应
 * @return kOk 成功
 */
base::Code ControlPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
                              ::google::protobuf::Message *resp) { /*{{{*/
  (void)conf;
  if (req == NULL || resp == NULL) return base::kInvalidParam;

  const book_mgr::BookReq *book_req = dynamic_cast<const book_mgr::BookReq *>(req);
  book_mgr::BookResp *book_resp = dynamic_cast<book_mgr::BookResp *>(resp);
  if (book_req == NULL || book_resp == NULL) return base::kInvalidParam;

  switch (book_req->req_body_case()) {
    case book_mgr::BookReq::kCreateReq:
      HandleCreate(*book_req, book_resp);
      break;
    case book_mgr::BookReq::kGetReq:
      HandleRead(*book_req, book_resp);
      break;
    case book_mgr::BookReq::kUpdateReq:
      HandleUpdateOrDelete(*book_req, true, book_resp);
      break;
    case book_mgr::BookReq::kDeleteReq:
      HandleUpdateOrDelete(*book_req, false, book_resp);
      break;
    case book_mgr::BookReq::kListReq:
      HandleList(*book_req, book_resp);
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

  // 下游连接复用模式选择：
  //   - 默认：方案A 线程局部长连接（连接数 = worker 线程数 × 下游地址数）
  //   - 设置 BOOK_CONN_POOL_SIZE>0：方案B 共享连接池（连接数 = min(并发, 池上限) × 下游地址数）
  //     可选 BOOK_CONN_POOL_IDLE_MS（空闲回收）、BOOK_CONN_POOL_ACQUIRE_MS（借取超时）
  const char *pool_size_env = getenv("BOOK_CONN_POOL_SIZE");
  if (pool_size_env != NULL && atoi(pool_size_env) > 0) {
    uint32_t pool_size = (uint32_t)atoi(pool_size_env);
    uint32_t idle_ms = 60000;
    uint32_t acquire_ms = 1000;
    const char *idle_env = getenv("BOOK_CONN_POOL_IDLE_MS");
    if (idle_env != NULL) idle_ms = (uint32_t)atoi(idle_env);
    const char *acquire_env = getenv("BOOK_CONN_POOL_ACQUIRE_MS");
    if (acquire_env != NULL) acquire_ms = (uint32_t)atoi(acquire_env);
    RpcChannel::EnablePoolMode(pool_size, idle_ms, acquire_ms);
    fprintf(stderr, "conn pool(B) enabled: size=%u idle_ms=%u acquire_ms=%u\n", pool_size, idle_ms, acquire_ms);
  } else {
    fprintf(stderr, "conn reuse(A) enabled: thread-local persistent connection\n");
  }

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

  // 服务发现初始化：文件数据源 + 注册两个下游 DAO 服务 + 启动热加载（mtime 轮询）
  // provider 须长于 server.Run() 生命周期，故定义在此（main 栈上，进程退出才析构）
  static FileServiceProvider provider(user_conf_path);
  HealthConfig hc;
  ServiceManager::Instance()->Init(&provider, hc);
  ServiceManager::Instance()->SetConnDropHook(&DropPoolConn, NULL);
  ServiceManager::Instance()->RegisterService(book_mgr::kSvcLevelDbDao, kLbRoundRobin);
  ServiceManager::Instance()->RegisterService(book_mgr::kSvcCacheDao, kLbHash);
  ret = provider.Start(hc.probe_interval_ms);
  if (ret != kOk) {
    LOG_ERR("Failed to start service discovery hot-reload, ret:%d\n", ret);
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
