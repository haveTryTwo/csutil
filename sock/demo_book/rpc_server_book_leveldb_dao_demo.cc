// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_server.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "base/daemon.h"
#include "base/status.h"
#include "sock/demo_book/book_common.h"
#include "sock/demo_book/leveldb_wrapper.h"
#include "sock/demo_book/proto/book.pb.h"

// LevelDB DAO 进程内的全局存储封装
static book_mgr::LevelDbWrapper g_db_wrapper;

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

/**
 * @brief 处理 Create/Update 写入：序列化 Book 后落盘
 */
static base::Code HandlePut(const book_mgr::Book &book, book_mgr::BaseResp *base_resp) { /*{{{*/
  if (!book.has_book_id() || book.book_id().empty()) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetInvalidReq, "book_id is empty");
    return base::kOk;
  }

  std::string value;
  if (!book.SerializeToString(&value)) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetInvalidReq, "serialize book failed");
    return base::kOk;
  }

  base::Code ret = g_db_wrapper.Put(book.book_id(), value);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetStorageErr, "leveldb put failed");
    return base::kOk;
  }

  book_mgr::FillBaseResp(base_resp, book_mgr::kBookRetOk, "success");
  return base::kOk;
} /*}}}*/

/**
 * @brief 处理 List：范围扫描（前缀/游标分页）并反序列化为 Book 列表
 * @param list_req 列表请求
 * @param list_resp 列表响应（输出）
 * @return kOk（业务结果写入 list_resp.base）
 */
static base::Code HandleList(const book_mgr::ListBooksReq &list_req, book_mgr::ListBooksResp *list_resp) { /*{{{*/
  uint32_t limit = list_req.limit();
  if (limit == 0) limit = book_mgr::kDefaultListLimit;
  if (limit > book_mgr::kMaxListLimit) limit = book_mgr::kMaxListLimit;

  std::vector<std::string> keys;
  std::vector<std::string> values;
  std::string next_cursor;
  base::Code ret = g_db_wrapper.Scan(list_req.prefix(), list_req.start_after(), limit, &keys, &values, &next_cursor);
  if (ret != base::kOk) {
    book_mgr::FillBaseResp(list_resp->mutable_base(), book_mgr::kBookRetStorageErr, "leveldb scan failed");
    return base::kOk;
  }

  for (uint32_t i = 0; i < values.size(); ++i) {
    book_mgr::Book *book = list_resp->add_books();
    if (!book->ParseFromString(values[i])) list_resp->mutable_books()->RemoveLast();  // 跳过损坏记录
  }
  list_resp->set_next_cursor(next_cursor);
  list_resp->set_total(static_cast<uint32_t>(list_resp->books_size()));
  book_mgr::FillBaseResp(list_resp->mutable_base(), book_mgr::kBookRetOk, "success");
  return base::kOk;
} /*}}}*/

/**
 * @brief LevelDB DAO 的 protobuf RPC 处理函数，按 oneof case 分支处理 CRUD
 * @param conf 用户配置（未使用，DB 已在启动时初始化）
 * @param req BookReq protobuf 请求
 * @param resp BookResp protobuf 响应
 * @return kOk 成功
 */
base::Code LevelDbDaoPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
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
      base::Code ret = HandlePut(book, create_resp->mutable_base());
      if (create_resp->base().ret_code() == book_mgr::kBookRetOk) create_resp->set_book_id(book.book_id());
      return ret;
    }
    case book_mgr::BookReq::kUpdateReq: {
      const book_mgr::Book &book = book_req->update_req().book();
      book_mgr::UpdateBookResp *update_resp = book_resp->mutable_update_resp();
      return HandlePut(book, update_resp->mutable_base());
    }
    case book_mgr::BookReq::kGetReq: {
      const std::string &book_id = book_req->get_req().book_id();
      book_mgr::GetBookResp *get_resp = book_resp->mutable_get_resp();
      get_resp->set_from_cache(false);

      std::string value;
      base::Code ret = g_db_wrapper.Get(book_id, &value);
      if (ret == base::kNotFound) {
        book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetNotFound, "book not found");
        return base::kOk;
      }
      if (ret != base::kOk) {
        book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetStorageErr, "leveldb get failed");
        return base::kOk;
      }
      if (!get_resp->mutable_book()->ParseFromString(value)) {
        book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetStorageErr, "parse book failed");
        return base::kOk;
      }
      book_mgr::FillBaseResp(get_resp->mutable_base(), book_mgr::kBookRetOk, "success");
      return base::kOk;
    }
    case book_mgr::BookReq::kDeleteReq: {
      const std::string &book_id = book_req->delete_req().book_id();
      book_mgr::DeleteBookResp *delete_resp = book_resp->mutable_delete_resp();

      base::Code ret = g_db_wrapper.Delete(book_id);
      if (ret != base::kOk) {
        book_mgr::FillBaseResp(delete_resp->mutable_base(), book_mgr::kBookRetStorageErr, "leveldb delete failed");
        return base::kOk;
      }
      book_mgr::FillBaseResp(delete_resp->mutable_base(), book_mgr::kBookRetOk, "success");
      return base::kOk;
    }
    case book_mgr::BookReq::kListReq:
      return HandleList(book_req->list_req(), book_resp->mutable_list_resp());
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

  std::string conf_path = "./conf/leveldb_server.conf";
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

  std::string user_conf_path = "./conf/leveldb_user_info.conf";
  Config user_conf;
  ret = user_conf.LoadFile(user_conf_path);
  if (ret != kOk) {
    LOG_ERR("Failed to load user conf:%s, ret:%d\n", user_conf_path.c_str(), ret);
    return ret;
  }

  std::string db_path;
  ret = user_conf.GetValue(book_mgr::kDbPathKey, &db_path);
  if (ret != kOk) {
    LOG_ERR("Failed to get db_path from user conf, ret:%d\n", ret);
    return ret;
  }

  ret = g_db_wrapper.Init(db_path);
  if (ret != kOk) {
    LOG_ERR("Failed to init leveldb at path:%s, ret:%d\n", db_path.c_str(), ret);
    return ret;
  }
  fprintf(stderr, "leveldb opened at: %s\n", db_path.c_str());

  book_mgr::BookReq req_prototype;
  book_mgr::BookResp resp_prototype;

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   LevelDbDaoPbRpcAction, &req_prototype, &resp_prototype);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
