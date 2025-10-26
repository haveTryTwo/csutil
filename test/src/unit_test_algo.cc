// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>

#include <utility>

#include "base/algo.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

namespace {

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

}  // namespace

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

TEST_D(kNN, Test_Normal_kNN, "kNN功能验证") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> points;
  for (int i = 0; i < 5; ++i) {
    std::vector<double> tmp_point = {i * 1.0, i * 1.0, i * 1.0};
    points.push_back(tmp_point);
  }

  std::vector<double> query_point = {4, 4, 4};

  int ks[] = {2, 10};
  for (auto k : ks) {
    std::vector<std::vector<double>> neighbors;
    Code ret = kNN(points, query_point, k, &neighbors);
    EXPECT_EQ(ret, kOk);

    fprintf(stderr, "k:%d, neighbors size:%zu\n", k, neighbors.size());
    for (auto point : neighbors) {
      fprintf(stderr, "{ ");
      for (auto i : point) {
        fprintf(stderr, "%f ", i);
      }
      fprintf(stderr, " }\n");
    }
  }
} /*}}}*/

TEST_D(kNN, Test_Exception_kNN, "kNN异常场景验证") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> points;
  for (int i = 0; i < 5; ++i) {
    std::vector<double> tmp_point = {i * 1.0, i * 1.0, i * 1.0, i * 1.0};
    points.push_back(tmp_point);
  }

  std::vector<double> query_point = {4, 4, 4};

  int k = 2;
  std::vector<std::vector<double>> neighbors;
  Code ret = kNN(points, query_point, k, &neighbors);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(EMA, Test_Normal_EMA, "EMA功能验证") { /*{{{*/
  using namespace base;
  ExponentialMovingAverage ema(99);
  std::vector<double> source_values = {50.0, 51.2, 52, 49, 43, 55, 52, 43, 40, 48, 59, 79, 13, 40, 55};

  for (auto value : source_values) {
    double ema_value = ema.Update(value);
    fprintf(stderr, "source_value:%f, ema_value:%f\n", value, ema_value);
  }
} /*}}}*/

namespace {

void Print() { fprintf(stderr, "Hello World\n"); }

}  // namespace

TEST_D(TimerWheel, Test_Normal_TimerWheel, "时间轮功能验证") { /*{{{*/
  using namespace base;

  TimerWheel tw(10);      // 创建一个有10个槽的时间轮
  tw.AddTimer(3, Print);  // 添加一个3个时间单位后执行的任务
  tw.AddTimer(5, Print);  // 添加一个5个时间单位后执行的任务

  // 模拟时间的推进
  for (int i = 0; i < 6; ++i) {
    tw.Tick();
  }
} /*}}}*/

TEST_D(RunLengthEncoding, Test_Normal_RLE, "Run-Length Encoding(RLE) 验证") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> input = {1, 2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 100};
  fprintf(stderr, "source:\n");
  for (uint32_t num : input) {
    fprintf(stderr, "%u ", (unsigned)num);
  }
  fprintf(stderr, "\n");

  // RunLengthEncode
  std::vector<std::pair<uint32_t, uint32_t>> encoded;
  Code ret = RunLengthEncode(input, &encoded);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "Encoded:\n");
  for (const auto& pair : encoded) {
    fprintf(stderr, "{ %u, %u } ", (unsigned)pair.first, (unsigned)pair.second);
  }
  fprintf(stderr, "\n");

  // RunLengthDecode
  std::vector<uint32_t> decoded;
  ret = RunLengthDecode(encoded, &decoded);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "Encoded:\n");
  for (uint32_t num : decoded) {
    fprintf(stderr, "%u ", (unsigned)num);
  }
  fprintf(stderr, "\n");

  EXPECT_EQ(0, CheckEqual(input, decoded));
} /*}}}*/
