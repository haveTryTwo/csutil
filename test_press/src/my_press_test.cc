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

#include "base/status.h"

#include "test_press_base/include/test_press_base.h"

class LoopPressObject : public test::PressObject {
 public:
  LoopPressObject(const std::string &test_name) : test::PressObject(test_name) { num_ = 0; }

  virtual ~LoopPressObject() {}

 public:
  virtual test::PressObject *Create();

  virtual base::Code Init(const std::string &dst_ip);
  virtual base::Code ExecBody();
  virtual bool IsOver();

 private:
  int num_;
};

// NOTE: this is vary import! So register a press object
Register(LoopPress, LoopPressObject);

test::PressObject *LoopPressObject::Create() { /*{{{*/
  return new LoopPressObject(*this);
} /*}}}*/

base::Code LoopPressObject::Init(const std::string &dst_ip_port_protos) { /*{{{*/
  base::Code ret = test::PressObject::Init(dst_ip_port_protos);
  if (ret != base::kOk) return ret;

  return base::kOk;
} /*}}}*/

bool PressHttpObject::IsOver() { return num_ > 10000; }

base::Code LoopPressObject::ExecBody() { /*{{{*/
  if (IsOver()) {
    return base::kExitOk;
  }

  usleep(1000);

  num_++;

  return base::kOk;
} /*}}}*/
