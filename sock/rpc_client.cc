// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

namespace base {
RpcClient::RpcClient(const std::string &ip, uint16_t port) : tcp_client_(ip, port) {}

RpcClient::~RpcClient() {}

Code RpcClient::Init(EventType evt_type, DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
                     FormatUserDataFunc format_user_data_func) { /*{{{*/
  Code ret = tcp_client_.Init(evt_type, data_proto_func);
  if (ret != kOk) return ret;

  get_user_data_func_ = get_user_data_func;
  format_user_data_func_ = format_user_data_func;

  ret = tcp_client_.Connect();
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

Code RpcClient::SendAndRecv(const std::string &user_requset, std::string *user_response) { /*{{{*/
  if (user_response == NULL) return kInvalidParam;

  std::string encode_request;
  Code ret = format_user_data_func_(user_requset, &encode_request);
  if (ret != kOk) return ret;

  ret = tcp_client_.SendNative(encode_request);
  if (ret != kOk) return ret;

  std::string encode_response;
  ret = tcp_client_.Recv(&encode_response);
  if (ret != kOk) return ret;

  ret = get_user_data_func_(encode_response.data(), encode_response.size(), user_response);
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

}  // namespace base
