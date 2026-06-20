// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include "store/cache/lru_cache/src/lru_cache.h"

// LRU Cache DAO 进程内的全局缓存
static store::LRUCache g_cache;

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

/**
 * @brief 将 Book 序列化后写入缓存
 */
static base::Code HandleCachePut(const book_mgr::Book &book, book_mgr::BaseResp *base_resp) { /*{{{*/
  if (!book.has_book_id() || book.book_id().empty()) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetInvalidReq, "book_id is empty");
    return base::kOk;
  }

  std::string value;
  if (!book.SerializeToString(&value)) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetInvalidReq, "serialize book failed");
    return base::kOk;
  }

  base::Code ret = g_cache.Put(book.book_id(), value);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetStorageErr, "cache put failed");
    return base::kOk;
  }

  book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetOk, "success");
  return base::kOk;
} /*}}}*/

/**
 * @brief LRU Cache DAO 的 protobuf RPC 处理函数，按 oneof case 分支处理
 *        Read 命中返回 from_cache=true；Create/Update 写回缓存；Delete 失效缓存
 * @param conf 用户配置（未使用）
 * @param req BookReq protobuf 请求
 * @param resp BookResp protobuf 响应
 * @return kOk 成功
 */
base::Code CacheDaoPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
                               ::google::protobuf::Message *resp) { /*{{{*/
  (void)conf;
  if (req == NULL || resp == NULL) return base::kInvalidParam;

  const book_mgr::BookReq *book_req = dynamic_cast<const book_mgr::BookReq *>(req);
  book_mgr::BookResp *book_resp = dynamic_cast<book_mgr::BookResp *>(resp);
  if (book_req == NULL || book_resp == NULL) return base::kInvalidParam;

  switch (book_req->req_body_case()) {
    case book_mgr::BookReq::kCreateReq: {
      const book_mgr::Book &book = book_req->create_req().book();
      book_mgr::CreateBookResp *create_resp = book_resp->mutable_create_resp();
      base::Code ret = HandleCachePut(book, create_resp->mutable_base());
      if (create_resp->base().ret_code() == book_mgr::kBookRetOk) create_resp->set_book_id(book.book_id());
      return ret;
    }
    case book_mgr::BookReq::kUpdateReq: {
      const book_mgr::Book &book = book_req->update_req().book();
      book_mgr::UpdateBookResp *update_resp = book_resp->mutable_update_resp();
      return HandleCachePut(book, update_resp->mutable_base());
    }
    case book_mgr::BookReq::kGetReq: {
      const std::string &book_id = book_req->get_req().book_id();
      book_mgr::GetBookResp *get_resp = book_resp->mutable_get_resp();
      get_resp->set_from_cache(true);

      std::string value;
      base::Code ret = g_cache.Get(book_id, &value);
      if (ret != base::kOk) {
        book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetNotFound, "cache miss");
        return base::kOk;
      }
      if (!get_resp->mutable_book()->ParseFromString(value)) {
        book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetStorageErr, "parse cached book failed");
        return base::kOk;
      }
      book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetOk, "cache hit");
      return base::kOk;
    }
    case book_mgr::BookReq::kDeleteReq: {
      const std::string &book_id = book_req->delete_req().book_id();
      book_mgr::DeleteBookResp *delete_resp = book_resp->mutable_delete_resp();

      // 缓存失效：key 不存在也视为成功
      g_cache.Del(book_id);
      book_mgr::FillBaseResp(delete_resp->mutable_base(), book_mgr::kBookRetOk, "success");
      return base::kOk;
    }
    case book_mgr::BookReq::REQ_BODY_NOT_SET:
    default:
      book_mgr::FillBaseResp(book_resp->mutable_base(), book_mgr::kBookRetInvalidReq, "req_body not set");
      return base::kOk;
  }
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  // 压测时可通过环境变量提高日志级别（如 5=kFatalErrorLevel）以屏蔽框架逐请求 trace，降低 IO 开销
  const char *log_level_env = getenv("BOOK_LOG_LEVEL");
  if (log_level_env != NULL) SetLogLevel(atoi(log_level_env));

  std::string conf_path = "./conf/cache_server.conf";
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

  std::string user_conf_path = "./conf/cache_user_info.conf";
  Config user_conf;
  ret = user_conf.LoadFile(user_conf_path);
  if (ret != kOk) {
    LOG_ERR("Failed to load user conf:%s, ret:%d\n", user_conf_path.c_str(), ret);
    return ret;
  }

  int max_num = 0;
  int time_interval = 0;
  user_conf.GetInt32Value(book_mgr::kCacheMaxNumKey, &max_num);
  user_conf.GetInt32Value(book_mgr::kCacheTimeIntervalKey, &time_interval);

  ret = g_cache.Init((uint32_t)max_num, (uint32_t)time_interval);
  if (ret != kOk) {
    LOG_ERR("Failed to init lru cache, ret:%d\n", ret);
    return ret;
  }
  fprintf(stderr, "lru cache inited: max_num=%d, time_interval=%d\n", max_num, time_interval);

  book_mgr::BookReq req_prototype;
  book_mgr::BookResp resp_prototype;

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   CacheDaoPbRpcAction, &req_prototype, &resp_prototype);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
