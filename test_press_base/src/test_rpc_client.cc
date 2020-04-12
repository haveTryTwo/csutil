// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <deque>
#include <string>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <string.h>

#include "base/log.h"
#include "base/util.h"
#include "base/event.h"
#include "base/mutex.h"
#include "base/common.h"

#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_rpc_client.h"

namespace test
{
RegisterBusiClient(rpc, RpcBusiClient);


base::Code RpcProtoFunc(const char *src_data, int src_data_len, int *real_len)
{/*{{{*/
    if (src_data == NULL || src_data_len < 0 || real_len == NULL) return base::kInvalidParam;

    if (src_data_len < sizeof(RpcProtoHeader)) return base::kDataNotEnough;

    RpcProtoHeader *rpc_header = (RpcProtoHeader*)src_data;
    if (rpc_header->magic != kDefaultMagic) return base::kInvalidParam;

    if (src_data_len < rpc_header->len) return base::kDataNotEnough;

    *real_len = rpc_header->len;

    return base::kOk;
}/*}}}*/

RpcBusiClient::RpcBusiClient(const std::string &client_name) : BusiClient(client_name), is_init_(false)
{/*{{{*/
}/*}}}*/

RpcBusiClient::~RpcBusiClient()
{/*{{{*/
}/*}}}*/

base::Code RpcBusiClient::Init(const std::string &dst_ip_port_proto)
{/*{{{*/
    std::deque<std::string> ip_port_proto;
    base::Code ret = base::Strtok(dst_ip_port_proto, base::kColon, &ip_port_proto);
    if (ret != base::kOk) return ret;
    if (ip_port_proto.size() < 3 || ip_port_proto.size() > 4) return base::kInvalidParam;
    if (ip_port_proto[2] != kRpcStr) return base::kInvalidParam;

    dst_ip_ = ip_port_proto[0];
    dst_port_ = (uint16_t)atoi(ip_port_proto[1].c_str());
    dst_ip_port_ = ip_port_proto[0] + ":" + ip_port_proto[1];

    base::EventType event_type = base::kPoll;
#if defined(__linux__)
    event_type = base::kEPoll;
#endif
    ret = tcp_client_.Init(event_type, RpcProtoFunc);
    if (ret != base::kOk) return ret;

    ret = tcp_client_.Connect(dst_ip_, dst_port_);
    if (ret != base::kOk) return ret;

    is_init_ = true;

    return base::kOk;
}/*}}}*/

BusiClient* RpcBusiClient::Create()
{/*{{{*/
    return new RpcBusiClient(*this);
}/*}}}*/

base::Code RpcBusiClient::SendAndRecv(const std::string &req, std::string *resp)
{/*{{{*/
    if (resp == NULL) return base::kInvalidParam;
    if (!is_init_) return base::kNotInit;
    resp->clear();
    
    RpcProtoHeader rpc_proto_header;
    rpc_proto_header.magic = kDefaultMagic;
    rpc_proto_header.len = sizeof(RpcProtoHeader)+req.size();

    std::string tmp_buf((char*)&rpc_proto_header, sizeof(RpcProtoHeader));

    base::Code ret = tcp_client_.SendNative(tmp_buf+req);
    if (ret != base::kOk) return ret;

    std::string stream_resp;
    ret = tcp_client_.Recv(&stream_resp);
    if (ret != base::kOk) return ret;

    RpcProtoHeader *resp_rpc_proto_header = (RpcProtoHeader*)stream_resp.data();
    if (resp_rpc_proto_header->magic != rpc_proto_header.magic) return base::kInvalidParam;
    if (resp_rpc_proto_header->len != stream_resp.size()) return base::kInvalidParam;

    resp->assign(stream_resp, sizeof(RpcProtoHeader), stream_resp.size()-sizeof(RpcProtoHeader));

    return ret;
}/*}}}*/

}
