// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_RPC_BUSI_CLIENT_H_
#define TEST_RPC_BUSI_CLIENT_H_

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

const std::string kRpcStr = "rpc";
const uint32_t kDefaultMagic = 0x20200404;

struct RpcProtoHeader {
  uint32_t magic;
  uint32_t len;
  char real_str[0];
};

class RpcBusiClient : public BusiClient {
 public:
  RpcBusiClient(const std::string &client_name);
  virtual ~RpcBusiClient();

 public:
  virtual base::Code Init(const std::string &dst_ip_port_proto);
  virtual BusiClient *Create();

 public:
  virtual base::Code SendAndRecv(const std::string &req, std::string *resp);

 private:
  bool is_init_;
  base::TcpClient tcp_client_;
};

}  // namespace test

#endif
