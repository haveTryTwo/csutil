// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_SVRKIT_BUSI_CLIENT_H_
#define TEST_SVRKIT_BUSI_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/status.h"
#include "base/time.h"
#include "http/http_client.h"
#include "http/http_proto.h"
#include "sock/tcp_client.h"
#include "strategy/singleton.h"

#include "test_press_base/include/test_busi_client.h"

namespace test {

const uint32_t kMaxPackage = 1024 * 1024;
const std::string kSvrkitStr = "svrkit";

/**
 * Request：
 * [TotalLength(4 bytes)][MsgHeadExport_t][Protobuf]
 * TotalLength = sizeof(MsgHeadExport_t) + MsgHeadExport_t.iBodyLen
 * Response：
 * [TotalLength(4 bytes)][MsgHeadExport_t][Protobuf]
 */
#pragma pack(push)
#pragma pack(1)

/* 32 bytes */
typedef struct tMsgHeadExport {
  unsigned short hMagic;      /* 填 0 */
  unsigned char bVersion;     /* 填 0 */
  unsigned char bHeadLen;     /* sizeof( tMsgHeadExport ) */
  unsigned int iBodyLen;      /* body 长度 */
  unsigned short hCmdID;      /* 命令字 */
  unsigned short hHeadChkSum; /* 填 0 */
  unsigned int iXForwardFor; /* 如果请求通过了 proxy 中转，这个字段由 proxy 把真实的 ClientIP 填进去*/
  unsigned char sReserved2[4]; /* 暂时没用 */
  unsigned int iUin;           /* 发起请求的 UIN */
  int iResult;                 /* 请求处理的结果，表示请求是否处理成功，取值由业务层定义 */
  unsigned char sReserved[4];  /* [0]: 总是填 1,
    [1]: 暂时没用,
    [2]: 0 表示 body 部分就是 protobuf 内容，1 表示 body 部分包含多个 MsgSubHeadExport
    [3]: 0 表示当前请求按默认配置输出日志，1 表示输出详细日志*/
} MsgHeadExport_t;

#pragma pack(pop)

class SvrkitBusiClient : public BusiClient {
 public:
  SvrkitBusiClient(const std::string &client_name);
  virtual ~SvrkitBusiClient();

 public:
  virtual base::Code Init(const std::string &dst_ip_port_proto);
  virtual BusiClient *Create();

 public:
  virtual base::Code SendAndRecv(uint16_t cmd, const std::string &req, std::string *resp);

 private:
  bool is_init_;
  base::TcpClient tcp_client_;
};

}  // namespace test

#endif
