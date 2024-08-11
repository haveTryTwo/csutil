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
#include "test_press_base/include/test_http_client.h"
#include "test_press_base/include/test_press_base.h"

class PressHttpObject : public test::PressObject {
 public:
  PressHttpObject(const std::string &test_name) : test::PressObject(test_name) { num_ = 0; }

  virtual ~PressHttpObject() {}

 public:
  virtual test::PressObject *Create();

  virtual base::Code Init(const std::string &dst_ip_port_protos);
  virtual base::Code ExecBody();
  virtual bool IsOver();

 private:
  int num_;
};

// NOTE: this is vary import! So register a press object
Register(PressHttp, PressHttpObject);

test::PressObject *PressHttpObject::Create() { /*{{{*/
  return new PressHttpObject(*this);
} /*}}}*/

base::Code PressHttpObject::Init(const std::string &dst_ip_port_protos) { /*{{{*/
  base::Code ret = test::PressObject::Init(dst_ip_port_protos);
  if (ret != base::kOk) return ret;

  return base::kOk;
} /*}}}*/

bool PressHttpObject::IsOver() { return num_ > 10000; }

base::Code PressHttpObject::ExecBody() { /*{{{*/
  if (IsOver()) {
    return base::kExitOk;
  }

  test::BusiClient *busi_client = GetBusiClient(test::kHttpStr);
  if (busi_client == NULL) return base::kInvalidParam;
  std::string resp;
  std::string relative_url = "/";
  base::Code ret = busi_client->SendAndRecv(relative_url, "", &resp);
  // LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);

  num_++;

  return ret;
} /*}}}*/
