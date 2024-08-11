// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base/common.h"
#include "base/log.h"
#include "base/status.h"

#include "test_press_base/include/test_http_client.h"
#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_rpc_client.h"

class PressHttpAndRpcObject : public test::PressObject {
 public:
  PressHttpAndRpcObject(const std::string &test_name) : test::PressObject(test_name) { num_ = 0; }

  virtual ~PressHttpAndRpcObject() {}

 public:
  virtual test::PressObject *Create();

  virtual base::Code Init(const std::string &dst_ip);
  virtual base::Code ExecBody();
  virtual bool IsOver();

 private:
  int num_;
};

// NOTE: this is vary import! So register a press object
Register(PressHttpAndRpc, PressHttpAndRpcObject);

test::PressObject *PressHttpAndRpcObject::Create() { /*{{{*/
  return new PressHttpAndRpcObject(*this);
} /*}}}*/

base::Code PressHttpAndRpcObject::Init(const std::string &dst_ip_port_protos) { /*{{{*/
  base::Code ret = test::PressObject::Init(dst_ip_port_protos);
  if (ret != base::kOk) return ret;

  return base::kOk;
} /*}}}*/

bool PressHttpAndRpcObject::IsOver() { /*{{{*/
  return num_ > 10000;
} /*}}}*/

base::Code PressHttpAndRpcObject::ExecBody() { /*{{{*/
  if (IsOver()) {
    return base::kExitOk;
  }

  test::BusiClient *http_busi_client = GetBusiClient(test::kHttpStr);
  test::BusiClient *rpc_busi_client = GetBusiClient(test::kRpcStr);

  if (http_busi_client == NULL || rpc_busi_client == NULL) {
    LOG_ERR("Failed to get %s or %s client", test::kHttpStr.c_str(), test::kRpcStr.c_str());
    return base::kInvalidParam;
  }
  std::string resp;
  std::string relative_url = "/";
  base::Code ret = http_busi_client->SendAndRecv(relative_url, "", &resp);
  if (ret != base::kOk) {
    LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);
  }

  ret = rpc_busi_client->SendAndRecv("client!", &resp);
  if (ret != base::kOk) {
    LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);
  }

  num_++;

  return ret;
} /*}}}*/
