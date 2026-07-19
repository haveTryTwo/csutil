// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <string>

#include <stdint.h>
#include <stdlib.h>

#include "base/common.h"
#include "base/event.h"
#include "base/util.h"

#include "sock/rpc_proto.h"

#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_sock_rpc_client.h"

namespace test {

RegisterBusiClient(sock_rpc, SockRpcBusiClient);

SockRpcBusiClient::SockRpcBusiClient(const std::string &client_name)
    : BusiClient(client_name), is_init_(false) { /*{{{*/
} /*}}}*/

SockRpcBusiClient::~SockRpcBusiClient() { /*{{{*/
} /*}}}*/

base::Code SockRpcBusiClient::Init(const std::string &dst_ip_port_proto) { /*{{{*/
  std::deque<std::string> ip_port_proto;
  base::Code ret = base::Strtok(dst_ip_port_proto, base::kColon, &ip_port_proto);
  if (ret != base::kOk) return ret;
  if (ip_port_proto.size() < 3 || ip_port_proto.size() > 4) return base::kInvalidParam;
  if (ip_port_proto[2] != kSockRpcStr) return base::kInvalidParam;

  dst_ip_ = ip_port_proto[0];
  dst_port_ = (uint16_t)atoi(ip_port_proto[1].c_str());
  dst_ip_port_ = ip_port_proto[0] + ":" + ip_port_proto[1];

  base::EventType event_type = base::kPoll;
#if defined(__linux__)
  event_type = base::kEPoll;
#endif
  ret = tcp_client_.Init(event_type, base::DefaultProtoFunc);
  if (ret != base::kOk) return ret;

  ret = tcp_client_.Connect(dst_ip_, dst_port_);
  if (ret != base::kOk) return ret;

  is_init_ = true;

  return base::kOk;
} /*}}}*/

BusiClient *SockRpcBusiClient::Create() { /*{{{*/
  return new SockRpcBusiClient(*this);
} /*}}}*/

base::Code SockRpcBusiClient::SendAndRecv(const std::string &req, std::string *resp) { /*{{{*/
  if (resp == NULL) return base::kInvalidParam;
  if (!is_init_) return base::kNotInit;
  resp->clear();

  std::string encode_req;
  base::Code ret = base::DefaultFormatUserDataFunc(req, &encode_req);
  if (ret != base::kOk) return ret;

  ret = tcp_client_.SendNative(encode_req);
  if (ret != base::kOk) return ret;

  std::string stream_resp;
  ret = tcp_client_.Recv(&stream_resp);
  if (ret != base::kOk) return ret;

  ret = base::DefaultGetUserDataFunc(stream_resp.data(), stream_resp.size(), resp);
  if (ret != base::kOk) return ret;

  return base::kOk;
} /*}}}*/

}  // namespace test
