// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/random.h"
#include "base/status.h"
#include "base/algo.h"

#include "test_base/include/test_base.h"

TEST(HyperLogLog, Test_Normal_Add) {/*{{{*/
  using namespace base;
  HyperLogLog hyperloglog(16);
  Code ret = hyperloglog.Init();
  EXPECT_EQ(ret, kOk);

  int num = 2000;
  for (int i = 0; i < num; ++i) {
    ret = hyperloglog.Add(i);
    EXPECT_EQ(ret, kOk);
    EXPECT_EQ(hyperloglog.IsDense(), false);
  }

  fprintf(stderr, "After add %d num, estimate:%f\n", num, hyperloglog.Estimate());
}/*}}}*/
