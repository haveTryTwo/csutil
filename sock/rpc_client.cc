// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

namespace base {
RpcClient::RpcClient(const std::string &ip, uint16_t port) : tcp_client_(ip, port) {}

RpcClient::~RpcClient() {}

Code RpcClient::Init() { return Init(DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc); }

Code RpcClient::Init(DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
                     FormatUserDataFunc format_user_data_func) {
  return Init(kEPoll, data_proto_func, get_user_data_func, format_user_data_func);
}

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
  if (ret != kOk) {
    // 框架错误码 [700, 799] 直接透传给上层处理
    if (IsFrameError((uint32_t)ret)) return ret;
    return ret;
  }

  return ret;
} /*}}}*/

Code RpcClient::SendAndRecv(const ::google::protobuf::Message &request,
                            ::google::protobuf::Message *response) { /*{{{*/
  if (response == NULL) return kInvalidParam;

  // 序列化请求
  std::string serialized_request;
  if (!request.SerializeToString(&serialized_request)) return kSerializePBFailed;

  // 通过字符串版本完成收发
  std::string serialized_response;
  Code ret = SendAndRecv(serialized_request, &serialized_response);
  if (ret != kOk) return ret;

  // 反序列化响应
  if (!response->ParseFromString(serialized_response)) return kParseProtobufFailed;

  return kOk;
} /*}}}*/

}  // namespace base
