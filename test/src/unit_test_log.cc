// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/file_util.h"
#include "base/status.h"
#include "base/util.h"
#include "base/log.h"
#include "base/random.h"

#include "test_base/include/test_base.h"

TEST(LOG, Normal_LogInfo) { /*{{{*/
  using namespace base;

  InitLog("../log/test.log.%Y-%m-%d", kInfoLevel);
  LOG_FATAL_ERR("this is fatal err");
  LOG_ERR("this is err");
  LOG_WARN("this is warning");
  LOG_INFO("this is info");
  LOG_DEBUG("this is debug");

  std::string info_log;
  Code ret = GetRandStr(1024*10, &info_log);
  EXPECT_EQ(ret, kOk);
  LOG_INFO("very long log:%s\n", info_log.c_str());
} /*}}}*/
