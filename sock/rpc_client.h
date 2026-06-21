// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_RPC_CLIENT_H_
#define SOCK_RPC_CLIENT_H_

#include <stdint.h>

#include <string>

#include <google/protobuf/message.h>

#include "base/event.h"
#include "base/status.h"
#include "sock/base_client.h"
#include "sock/rpc_proto.h"
#include "sock/tcp_client.h"

namespace base {

class RpcClient { /*{{{*/
 public:
  RpcClient(const std::string &ip, uint16_t port);
  ~RpcClient();

  Code Init();
  Code Init(DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
            FormatUserDataFunc format_user_data_func);
  Code Init(EventType evt_type, DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
            FormatUserDataFunc format_user_data_func);

  Code SendAndRecv(const std::string &user_requset, std::string *user_response);

  /**
   * @brief Protobuf 版本的 SendAndRecv，自动完成序列化和反序列化
   * @param request 请求 protobuf 消息，序列化后发送
   * @param response 响应 protobuf 消息，接收后反序列化填充
   * @return kOk 成功；kInvalidParam 参数无效；kSerializePBFailed 序列化失败；kParseProtobufFailed 反序列化失败
   */
  Code SendAndRecv(const ::google::protobuf::Message &request, ::google::protobuf::Message *response);

  /**
   * @brief 设置底层连接的收发超时时间（毫秒），-1 表示永久等待
   * @param max_wait_time_ms 超时毫秒数
   * @return kOk 成功；kInvalidParam 参数无效（小于 -1）
   */
  Code SetMaxWaitTimeMs(int max_wait_time_ms);

 private:
  TcpClient tcp_client_;
  GetUserDataFunc get_user_data_func_;
  FormatUserDataFunc format_user_data_func_;
}; /*}}}*/

}  // namespace base
#endif
