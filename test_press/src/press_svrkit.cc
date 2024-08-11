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

#include "test_press_base/include/test_busi_client.h"
#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_svrkit_client.h"

class PressSvrkitObject : public test::PressObject {
 public:
  PressSvrkitObject(const std::string &test_name) : test::PressObject(test_name) { num_ = 0; }

  virtual ~PressSvrkitObject() {}

 public:
  virtual test::PressObject *Create();

  virtual base::Code Init(const std::string &dst_ip);
  virtual base::Code ExecBody();
  virtual bool IsOver();

 private:
  int num_;
};

// NOTE: this is vary import! So register a press object
Register(PressSvrkit, PressSvrkitObject);

test::PressObject *PressSvrkitObject::Create() { /*{{{*/
  return new PressSvrkitObject(*this);
} /*}}}*/

base::Code PressSvrkitObject::Init(const std::string &dst_ip_port_protos) { /*{{{*/
  base::Code ret = test::PressObject::Init(dst_ip_port_protos);
  if (ret != base::kOk) return ret;

  return base::kOk;
} /*}}}*/

bool PressSvrkitObject::IsOver() { /*{{{*/
  return num_ > 10000;
} /*}}}*/

base::Code PressSvrkitObject::ExecBody() { /*{{{*/
  if (IsOver()) {
    return base::kExitOk;
  }

  test::BusiClient *busi_client = GetBusiClient(test::kSvrkitStr);
  if (busi_client == NULL) {
    LOG_ERR("Failed to get client:%s", test::kSvrkitStr.c_str());
    return base::kInvalidParam;
  }

  uint32_t cmd = 1;
  std::string resp;
  base::Code ret = busi_client->SendAndRecv(cmd, "client!", &resp);
  // LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);

  num_++;

  return ret;
} /*}}}*/
