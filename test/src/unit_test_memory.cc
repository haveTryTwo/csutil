// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/common.h"
#include "base/memory.h"

#include "test_base/include/test_base.h"

TEST(GetProcessMemoryUsage, Test_Normal_Usage) { /*{{{*/
  using namespace base;

  int size[] = {kKB, kMB, kGB};
  uint64_t memory_usage = 0;
  for (int i = 0; i < static_cast<int>(sizeof(size) / sizeof(size[0])); ++i) {
    char *p = new char[size[i]];
    for (int j = 0; j < size[i]; ++j) {
      p[j] = j;
    }
    Code ret = GetProcessMemoryUsage(&memory_usage);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "size[%d]:%d, memory_usage:%lu\n", i, size[i], (unsigned long)memory_usage);
    delete p;
  }
} /*}}}*/

TEST(GetProcessMemoryReadableUsage, Test_Normal_Usage) { /*{{{*/
  using namespace base;

  int size[] = {kKB, kMB, kGB};
  std::string memory_usage;
  for (int i = 0; i < static_cast<int>(sizeof(size) / sizeof(size[0])); ++i) {
    char *p = new char[size[i]];
    for (int j = 0; j < size[i]; ++j) {
      p[j] = j;
    }
    Code ret = GetProcessMemoryReadableUsage(&memory_usage);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "size[%d]:%d, memory_usage:%s\n", i, size[i], memory_usage.c_str());
    delete p;
  }
} /*}}}*/
