// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_BOOK_COMMON_H_
#define SOCK_DEMO_BOOK_BOOK_COMMON_H_

#include <string>

#include "sock/demo_book/proto/book.pb.h"

namespace book_mgr {

// 下游 DAO 服务地址配置键名（control_user_info.conf）
const char kLevelDbServerIp[] = "leveldb_server_ip";
const char kLevelDbServerPort[] = "leveldb_server_port";
const char kCacheServerIp[] = "cache_server_ip";
const char kCacheServerPort[] = "cache_server_port";

// LevelDB DAO 配置键名（leveldb_user_info.conf）
const char kDbPathKey[] = "db_path";

// LRU Cache DAO 配置键名（cache_user_info.conf）
const char kCacheMaxNumKey[] = "max_num";
const char kCacheTimeIntervalKey[] = "time_interval";

// 业务通用返回码（写入 BaseResp.ret_code）
const int32_t kBookRetOk = 0;
const int32_t kBookRetInvalidReq = 1;
const int32_t kBookRetNotFound = 2;
const int32_t kBookRetStorageErr = 3;
const int32_t kBookRetBackendErr = 4;

/**
 * @brief 构造 Create 请求
 * @param book 待写入的图书
 * @return 设置好 oneof create_req 的 BookReq
 */
inline BookReq MakeCreateReq(const Book &book) { /*{{{*/
  BookReq req;
  req.set_op_type(OP_CREATE);
  req.mutable_create_req()->mutable_book()->CopyFrom(book);
  return req;
} /*}}}*/

/**
 * @brief 构造 Read 请求
 * @param book_id 图书主键
 * @return 设置好 oneof get_req 的 BookReq
 */
inline BookReq MakeGetReq(const std::string &book_id) { /*{{{*/
  BookReq req;
  req.set_op_type(OP_READ);
  req.mutable_get_req()->set_book_id(book_id);
  return req;
} /*}}}*/

/**
 * @brief 构造 Update 请求
 * @param book 待更新的图书（含主键）
 * @return 设置好 oneof update_req 的 BookReq
 */
inline BookReq MakeUpdateReq(const Book &book) { /*{{{*/
  BookReq req;
  req.set_op_type(OP_UPDATE);
  req.mutable_update_req()->mutable_book()->CopyFrom(book);
  return req;
} /*}}}*/

/**
 * @brief 构造 Delete 请求
 * @param book_id 图书主键
 * @return 设置好 oneof delete_req 的 BookReq
 */
inline BookReq MakeDeleteReq(const std::string &book_id) { /*{{{*/
  BookReq req;
  req.set_op_type(OP_DELETE);
  req.mutable_delete_req()->set_book_id(book_id);
  return req;
} /*}}}*/

/**
 * @brief 在响应的 BaseResp 中填写返回码和消息
 * @param base_resp 待填充的 BaseResp 指针
 * @param ret_code 返回码
 * @param ret_msg 返回消息
 */
inline void FillBaseResp(BaseResp *base_resp, int32_t ret_code, const std::string &ret_msg) { /*{{{*/
  if (base_resp == NULL) return;
  base_resp->set_ret_code(ret_code);
  base_resp->set_ret_msg(ret_msg);
} /*}}}*/

}  // namespace book_mgr

#endif
