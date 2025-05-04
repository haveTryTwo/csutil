// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>

#include "roaring.hh"

#include "base/coding.h"

#include "test_base/include/test_base.h"

TEST_D(Roaring, Normal_Test_InsertAndCheck, "检测Roaring基本使用功能") { /*{{{*/
  using namespace base;

  {
    // NOTE:htt, test roaring::Roaring
    roaring::Roaring r1;
    for (uint32_t i = 100; i < 10000; i++) {
      r1.add(i);
    }
    fprintf(stderr, "r1.cardinality = %llu\n", (unsigned long)r1.cardinality());
    EXPECT_EQ((uint64_t)9900, r1.cardinality());
    for (uint32_t i = 100; i < 10000; i++) {
      EXPECT_EQ(true, r1.contains(i));
    }
    for (uint32_t i = 0; i < 100; i++) {
      EXPECT_EQ(false, r1.contains(i));
    }
  }

  {
    roaring::Roaring64Map r2;
    for (uint64_t i = 18000000000000000100ull; i < 18000000000000001000ull; i++) {
      r2.add(i);
    }
    fprintf(stderr, "r2.cardinality = %llu\n", (unsigned long)r2.cardinality());
    EXPECT_EQ((uint64_t)900, r2.cardinality());
    for (uint64_t i = 18000000000000000100ull; i < 18000000000000001000ull; i++) {
      EXPECT_EQ(true, r2.contains(i));
    }
    for (uint32_t i = 0; i < 100; i++) {
      EXPECT_EQ(false, r2.contains(i));
    }
  }
} /*}}}*/
