// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/algo.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

class TestLCS : public test::Test {
 public:
  void Init() {
    test::Test::Init();
    fprintf(stderr, "TestLCS init\n");
  }

  void Destroy() {
    fprintf(stderr, "TestLCS destroy\n");
    test::Test::Destroy();
  }
};

TEST_F_D(TestLCS, TestAlgoZero, "测试空字符串情况最长子串") { /*{{{*/
  using namespace base;

  std::string str1 = "abcdfmn";
  std::string str2 = "";
  std::string result;

  Code ret = LCS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST_F(TestLCS, TestAlgoNormal) { /*{{{*/
  using namespace base;

  std::string str1 = "abcdfmn";
  std::string str2 = "abdmfn";
  std::string result;

  Code ret = LCS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("abdmn", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST(LCS, TestAlgoFull) { /*{{{*/
  using namespace base;

  std::string str1 = "abcdfmn";
  std::string str2 = "abcdfmn";
  std::string result;

  Code ret = LCS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("abcdfmn", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST(LCSS, TestAlgoZero) { /*{{{*/
  using namespace base;

  std::string str1 = "abecdfmn";
  std::string str2 = "";
  std::string result;

  Code ret = LCSS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST(LCSS, TestAlgoNormal) { /*{{{*/
  using namespace base;

  std::string str1 = "abecdfmn";
  std::string str2 = "dfcdn";
  std::string result;

  Code ret = LCSS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("cd", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST(LCSS, TestAlgoFull) { /*{{{*/
  using namespace base;

  std::string str1 = "abecdfmn";
  std::string str2 = "abecdfmn";
  std::string result;

  Code ret = LCSS(str1, str2, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ("abecdfmn", result);

  fprintf(stdout, "str1:%s\n", str1.c_str());
  fprintf(stdout, "str2:%s\n", str2.c_str());
  fprintf(stdout, "lcs :%s\n", result.c_str());
} /*}}}*/

TEST(IsPrime, Test_Normal_Num_Less_100) { /*{{{*/
  using namespace base;
  uint32_t prime_num = 0;
  for (uint32_t i = 1; i < 100; ++i) {
    bool is_prime = false;
    Code ret = IsPrime(i, &is_prime);
    EXPECT_EQ(kOk, ret);
    if (is_prime) {
      prime_num++;
      fprintf(stderr, "%u ", i);
    }
  }

  fprintf(stderr, "\n");
  fprintf(stderr, "prime_num:%u\n", prime_num);
} /*}}}*/

TEST(IsPrime, Test_Normal_Num_Less_1000) { /*{{{*/
  using namespace base;
  uint32_t prime_num = 0;
  for (uint32_t i = 1; i < 1000; ++i) {
    bool is_prime = false;
    Code ret = IsPrime(i, &is_prime);
    EXPECT_EQ(kOk, ret);
    if (is_prime) {
      prime_num++;
      fprintf(stderr, "%u ", i);
    }
  }

  fprintf(stderr, "\n");
  fprintf(stderr, "prime_num:%u\n", prime_num);
} /*}}}*/

TEST_D(IsPrime, Test_Normal_Num_Less_One_Millon, "压测百万次质数接口性能") { /*{{{*/
  using namespace base;
  uint32_t prime_num = 0;
  for (uint32_t i = 1; i < 1000 * 1000; ++i) {
    bool is_prime = false;
    Code ret = IsPrime(i, &is_prime);
    EXPECT_EQ(kOk, ret);
    if (is_prime) {
      prime_num++;
      // fprintf(stderr, "%u ", i);
    }
  }

  // fprintf(stderr, "\n");
  fprintf(stderr, "prime_num:%u\n", prime_num);
} /*}}}*/

TEST_D(IsPrime, Test_Normal_Num_Less_Ten_Millon, "压测千万次质数接口性能") { /*{{{*/
  using namespace base;
  uint32_t prime_num = 0;
  for (uint32_t i = 1; i < 1000 * 1000 * 10; ++i) {
    bool is_prime = false;
    Code ret = IsPrime(i, &is_prime);
    EXPECT_EQ(kOk, ret);
    if (is_prime) {
      prime_num++;
      // fprintf(stderr, "%u ", i);
    }
  }

  // fprintf(stderr, "\n");
  fprintf(stderr, "prime_num:%u\n", prime_num);
} /*}}}*/
