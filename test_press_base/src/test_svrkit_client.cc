// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <deque>
#include <string>

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base/common.h"
#include "base/event.h"
#include "base/log.h"
#include "base/mutex.h"
#include "base/util.h"

#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_svrkit_client.h"

namespace test {
RegisterBusiClient(svrkit, SvrkitBusiClient);

unsigned short MsgHeadExportGetCmdId(const MsgHeadExport_t *msg_head) { /*{{{*/
  return ntohs(msg_head->hCmdID);
} /*}}}*/

base::Code ParsePbMsg(const char *buf, int buf_len, MsgHeadExport_t *&msg_head, char *&msg, int &msg_len) { /*{{{*/
  int pkg_len = 0;

  if (buf == NULL || buf_len < 4) {
    return base::kInvalidParam;
  }

  pkg_len = ntohl(*(const int *)buf);
  if (pkg_len < (int)sizeof(MsgHeadExport_t) || pkg_len > (int)kMaxPackage) {
    return base::kLess;
  }
  buf += 4;
  if (buf_len != pkg_len + 4) {
    return base::kNotEqual;
  }

  msg_head = (MsgHeadExport_t *)buf;
  int body_len = ntohl(msg_head->iBodyLen);
  if (pkg_len != (int)(sizeof(MsgHeadExport_t) + body_len)) {
    return base::kInvalidParam;
  }
  buf += sizeof(MsgHeadExport_t);

  if (buf_len == sizeof(MsgHeadExport_t) + 4) {
    msg = NULL;
    msg_len = 0;
    return base::kOk;
  }

  // check wether has MsgSubHeadExport_t
  msg = (char *)buf;
  msg_len = body_len;

  return base::kOk;
} /*}}}*/

base::Code SerializePbMsg(const MsgHeadExport_t *msg_head, const char *msg, int msg_len, char *buf,
                          int &buf_len) { /*{{{*/
  if (msg_head == NULL || msg_len < 0 || buf == NULL) {
    return base::kInvalidParam;
  }

  if (buf_len < (int)(4 + sizeof(MsgHeadExport_t) + msg_len)) {
    return base::kLess;
  }
  int *total_len = (int *)buf;
  buf += 4;
  ((MsgHeadExport_t *)buf)->hMagic = msg_head->hMagic;
  ((MsgHeadExport_t *)buf)->bVersion = msg_head->bVersion;
  ((MsgHeadExport_t *)buf)->bHeadLen = msg_head->bHeadLen;
  ((MsgHeadExport_t *)buf)->iBodyLen = htonl(msg_len);
  ((MsgHeadExport_t *)buf)->hCmdID = msg_head->hCmdID;
  // 设置为0，不检验checksum
  ((MsgHeadExport_t *)buf)->hHeadChkSum = htonl(0);
  ((MsgHeadExport_t *)buf)->iXForwardFor = msg_head->iXForwardFor;
  ((MsgHeadExport_t *)buf)->sReserved2[0] = 0;
  ((MsgHeadExport_t *)buf)->sReserved2[1] = 0;
  ((MsgHeadExport_t *)buf)->sReserved2[2] = 0;
  ((MsgHeadExport_t *)buf)->sReserved2[3] = 0;
  ((MsgHeadExport_t *)buf)->iUin = msg_head->iUin;
  ((MsgHeadExport_t *)buf)->iResult = msg_head->iResult;
  ((MsgHeadExport_t *)buf)->sReserved[0] = 1;
  ((MsgHeadExport_t *)buf)->sReserved[1] = 0;
  ((MsgHeadExport_t *)buf)->sReserved[2] = 0;
  ((MsgHeadExport_t *)buf)->sReserved[3] = 0;
  buf += sizeof(MsgHeadExport_t);
  buf_len = 4 + sizeof(MsgHeadExport_t);

  if (msg != NULL) {
    memcpy(buf, msg, msg_len);
    buf_len += msg_len;
  }
  *total_len = buf_len - 4;
  *total_len = htonl(*total_len);

  return base::kOk;
} /*}}}*/

base::Code SvrkitProtoFunc(const char *src_data, int src_data_len, int *real_len) { /*{{{*/
  if (src_data == NULL || src_data_len < 0 || real_len == NULL) return base::kInvalidParam;

  if (src_data_len < 4) return base::kDataNotEnough;

  int pkg_len = ntohl(*(const int *)src_data);
  if (pkg_len < (int)sizeof(MsgHeadExport_t) || pkg_len > (int)kMaxPackage) {
    return base::kInvalidParam;
  }

  if (src_data_len < pkg_len + 4) return base::kDataNotEnough;

  MsgHeadExport_t *msg_head = (MsgHeadExport_t *)(src_data + 4);
  int body_len = ntohl(msg_head->iBodyLen);
  if (pkg_len != (int)(sizeof(MsgHeadExport_t) + body_len)) {
    return base::kInvalidParam;
  }

  *real_len = pkg_len + 4;

  return base::kOk;
} /*}}}*/

SvrkitBusiClient::SvrkitBusiClient(const std::string &client_name) : BusiClient(client_name), is_init_(false) { /*{{{*/
} /*}}}*/

SvrkitBusiClient::~SvrkitBusiClient() { /*{{{*/
} /*}}}*/

base::Code SvrkitBusiClient::Init(const std::string &dst_ip_port_proto) { /*{{{*/
  std::deque<std::string> ip_port_proto;
  base::Code ret = base::Strtok(dst_ip_port_proto, base::kColon, &ip_port_proto);
  if (ret != base::kOk) return ret;
  if (ip_port_proto.size() < 3 || ip_port_proto.size() > 4) return base::kInvalidParam;
  if (ip_port_proto[2] != kSvrkitStr) return base::kInvalidParam;

  dst_ip_ = ip_port_proto[0];
  dst_port_ = (uint16_t)atoi(ip_port_proto[1].c_str());
  dst_ip_port_ = ip_port_proto[0] + ":" + ip_port_proto[1];

  base::EventType event_type = base::kPoll;
#if defined(__linux__)
  event_type = base::kEPoll;
#endif
  ret = tcp_client_.Init(event_type, SvrkitProtoFunc);
  if (ret != base::kOk) return ret;

  ret = tcp_client_.Connect(dst_ip_, dst_port_);
  if (ret != base::kOk) return ret;

  is_init_ = true;

  return base::kOk;
} /*}}}*/

BusiClient *SvrkitBusiClient::Create() { /*{{{*/
  return new SvrkitBusiClient(*this);
} /*}}}*/

base::Code SvrkitBusiClient::SendAndRecv(uint16_t cmd, const std::string &req, std::string *resp) { /*{{{*/
  if (resp == NULL) return base::kInvalidParam;
  if (!is_init_) return base::kNotInit;
  resp->clear();

  MsgHeadExport_t msg_head;
  memset(&msg_head, 0, sizeof(msg_head));
  msg_head.bHeadLen = sizeof(msg_head);
  msg_head.hCmdID = htons(cmd);

  std::string tmp_str;
  tmp_str.resize(4 + sizeof(msg_head) + req.size());
  int tmp_len = (int)tmp_str.size();
  base::Code ret = SerializePbMsg(&msg_head, req.c_str(), req.size(), const_cast<char *>(tmp_str.data()), tmp_len);
  if (ret != base::kOk) return ret;
  if (tmp_len != (int)tmp_str.size()) return base::kNotEqual;

  ret = tcp_client_.SendNative(tmp_str);
  if (ret != base::kOk) return ret;

  std::string stream_resp;
  ret = tcp_client_.Recv(&stream_resp);
  if (ret != base::kOk) return ret;

  MsgHeadExport_t *msg_rsp_head = NULL;
  char *buf = NULL;
  ret = ParsePbMsg(stream_resp.data(), stream_resp.size(), msg_rsp_head, buf, tmp_len);
  if (ret != base::kOk) return ret;
  resp->assign(buf, tmp_len);

  return ret;
} /*}}}*/

}  // namespace test
