// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_RPC_CLIENT_H_
#define SOCK_RPC_CLIENT_H_

#include <stdint.h>

#include <string>

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

  Code Init(EventType evt_type, DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
            FormatUserDataFunc format_user_data_func);

  Code SendAndRecv(const std::string &user_requset, std::string *user_response);

 private:
  TcpClient tcp_client_;
  GetUserDataFunc get_user_data_func_;
  FormatUserDataFunc format_user_data_func_;
}; /*}}}*/

}  // namespace base
#endif
