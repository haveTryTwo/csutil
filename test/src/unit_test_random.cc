// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/common.h"
#include "base/random.h"
#include "base/status.h"
#include "base/util.h"

#include "test_base/include/test_base.h"

TEST(InitRand, Test_Normal) { /*{{{*/
  using namespace base;

  Code ret = InitRand();
  EXPECT_EQ(kOk, ret);
} /*}}}*/

TEST(InitRand, Test_Press_One_Million) { /*{{{*/
  using namespace base;

  for (uint32_t i = 0; i < (uint32_t)kMillion; ++i) {
    Code ret = InitRand();
    EXPECT_EQ(kOk, ret);
  }
} /*}}}*/

TEST(GetRandStr, Test_Normal_10_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 10;
  std::string rand_str;
  Code ret = GetRandStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());
  //    fprintf(stderr, "rand string:%s\n", rand_str.c_str());
} /*}}}*/

TEST(GetRandStr, Test_Normal_1024_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 1024;
  std::string rand_str;
  Code ret = GetRandStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());
  //    fprintf(stderr, "rand string:%s\n", rand_str.c_str());
} /*}}}*/

TEST(GetRandStr, Test_Normal_Zero_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 0;
  std::string rand_str;
  Code ret = GetRandStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());
  //    fprintf(stderr, "rand string:%s\n", rand_str.c_str());
} /*}}}*/

TEST(GetRandStr, Test_Exception_NULL) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 0;
  Code ret = GetRandStr(rand_str_len, NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST(GetRandStr, Test_Press_10_Len_One_Million) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 10;
  std::string rand_str;
  Code ret = kOk;
  for (uint32_t i = 0; i < (uint32_t)kMillion; ++i) {
    ret = GetRandStr(rand_str_len, &rand_str);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(rand_str_len, rand_str.size());
  //   fprintf(stderr, "rand string:%s\n", rand_str.c_str());
} /*}}}*/

TEST(GetRandStr, Test_Press_1024_Len_TenThousand) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 1024;
  std::string rand_str;
  Code ret = kOk;
  for (uint32_t i = 0; i < (uint32_t)10 * kThousand; ++i) {
    ret = GetRandStr(rand_str_len, &rand_str);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(rand_str_len, rand_str.size());
  //    fprintf(stderr, "rand string:%s\n", rand_str.c_str());
} /*}}}*/

TEST(GetRandBinStr, Test_Normal_10_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 10;
  std::string rand_str;
  Code ret = GetRandBinStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());
  PrintBinStr(rand_str);
} /*}}}*/

TEST(GetRandBinStr, Test_Normal_1024_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 1024;
  std::string rand_str;
  Code ret = GetRandBinStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());

  PrintBinStr(rand_str);
} /*}}}*/

TEST(GetRandBinStr, Test_Normal_Zero_Len) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 0;
  std::string rand_str;
  Code ret = GetRandBinStr(rand_str_len, &rand_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(rand_str_len, rand_str.size());
} /*}}}*/

TEST(GetRandBinStr, Test_Exception_NULL) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 0;
  Code ret = GetRandBinStr(rand_str_len, NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST(GetRandBinStr, Test_Press_10_Len_One_Million) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 10;
  std::string rand_str;
  Code ret = kOk;
  for (uint32_t i = 0; i < (uint32_t)kMillion; ++i) {
    ret = GetRandBinStr(rand_str_len, &rand_str);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(rand_str_len, rand_str.size());
  PrintBinStr(rand_str);
} /*}}}*/

TEST(GetRandBinStr, Test_Press_1024_Len_TenThousand) { /*{{{*/
  using namespace base;

  uint32_t rand_str_len = 1024;
  std::string rand_str;
  Code ret = kOk;
  for (uint32_t i = 0; i < (uint32_t)10 * kThousand; ++i) {
    ret = GetRandBinStr(rand_str_len, &rand_str);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(rand_str_len, rand_str.size());
  // PrintBinStr(rand_str);
} /*}}}*/
