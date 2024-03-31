// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/ip.h"
#include "base/pid_controller.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(PidController, Test_Normal_Caculate) { /*{{{*/
  using namespace base;

  PidController pid = PidController(1, 0.2, 0.1, 0.1);

  double val = 0;
  for (int i = 0; i < 50; i++) {
    double inc = pid.calculate(100, val);
    fprintf(stderr, "val:% 7.3f inc:% 7.3f\n", val, inc);
    val += inc;
  }

} /*}}}*/

TEST(PidController, Test_Normal_Caculate_DTEqual0) { /*{{{*/
  using namespace base;

  PidController pid = PidController(0, 0.2, 0.1, 0.1);

  double val = 0;
  for (int i = 0; i < 50; i++) {
    double inc = pid.calculate(100, val);
    fprintf(stderr, "val:% 7.3f inc:% 7.3f\n", val, inc);
    val += inc;
  }

} /*}}}*/
