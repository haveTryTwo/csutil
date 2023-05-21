// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/cpu.h"
#include "test_base/include/test_base.h"

TEST(GetCPUNum, Test_Normal_Get) { /*{{{*/
  using namespace base;

  int cpu_num = 0;
  Code ret = GetCPUNum(&cpu_num);
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "cpu num:%d\n", cpu_num);
} /*}}}*/

TEST(GetCPUNum, Test_Exception_Get) { /*{{{*/
  using namespace base;

  Code ret = GetCPUNum(NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/
