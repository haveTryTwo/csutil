// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/algo.h"
#include "base/coding.h"
#include "base/common.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST_D(HyperLogLog, Test_Normal_Add, "基本功能测试") { /*{{{*/
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
} /*}}}*/

TEST_D(HyperLogLog, Test_Init_InvalidParam, "初始化无效参数测试") { /*{{{*/
  using namespace base;

  // 测试p值过小
  HyperLogLog hll1(3);
  Code ret = hll1.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试p值过大
  HyperLogLog hll2(32);
  ret = hll2.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试p值过大导致内存问题
  HyperLogLog hll3(25);
  ret = hll3.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(HyperLogLog, Test_Init_ValidParam, "初始化有效参数测试") { /*{{{*/
  using namespace base;

  // 测试有效的p值范围
  for (unsigned p = 4; p <= 20; ++p) {
    HyperLogLog hll(p);
    Code ret = hll.Init();
    EXPECT_EQ(ret, kOk);
    EXPECT_EQ(hll.IsDense(), false);
  }
} /*}}}*/

TEST_D(HyperLogLog, Test_Add_Without_Init, "未初始化添加元素测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(16);

  // 未调用Init就添加元素
  Code ret = hll.Add(123);
  EXPECT_EQ(ret, kNotInit);
} /*}}}*/

TEST_D(HyperLogLog, Test_AddHash_Normal, "直接添加哈希值测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(8);
  Code ret = hll.Init();
  EXPECT_EQ(ret, kOk);

  // 添加不同类型的hash值
  ret = hll.AddHash(0x123456789ABCDEFULL);
  EXPECT_EQ(ret, kOk);

  ret = hll.AddHash(0);
  EXPECT_EQ(ret, kOk);

  ret = hll.AddHash(UINT64_MAX);
  EXPECT_EQ(ret, kOk);

  double estimate = hll.Estimate();
  EXPECT_GT(estimate, 0.0);
} /*}}}*/

TEST_D(HyperLogLog, Test_Dense_Conversion, "稀疏到密集模式转换测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(6);  // 小的p值，容易触发dense转换
  Code ret = hll.Init();
  EXPECT_EQ(ret, kOk);

  EXPECT_EQ(hll.IsDense(), false);

  // 添加足够多的不同元素触发dense转换
  for (int i = 0; i < 1000; ++i) {
    ret = hll.Add(i);
    EXPECT_EQ(ret, kOk);
  }

  // 应该已经转换为dense模式
  EXPECT_EQ(hll.IsDense(), true);
} /*}}}*/

TEST_D(HyperLogLog, Test_Merge_NotInit, "合并未初始化对象测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(16);
  HyperLogLog hll2(16);

  // 两个都未初始化
  Code ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kNotInit);

  // 只有一个初始化
  ret = hll1.Init();
  EXPECT_EQ(ret, kOk);
  ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kNotInit);
} /*}}}*/

TEST_D(HyperLogLog, Test_Merge_InvalidParam, "合并不同参数对象测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(16);
  HyperLogLog hll2(14);  // 不同的p值

  Code ret = hll1.Init();
  EXPECT_EQ(ret, kOk);
  ret = hll2.Init();
  EXPECT_EQ(ret, kOk);

  // p值不同，应该返回错误
  ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(HyperLogLog, Test_Merge_Sparse_Mode, "稀疏模式合并测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(16);
  HyperLogLog hll2(16);

  Code ret = hll1.Init();
  EXPECT_EQ(ret, kOk);
  ret = hll2.Init();
  EXPECT_EQ(ret, kOk);

  // 添加少量元素保持sparse模式
  for (int i = 0; i < 50; ++i) {
    ret = hll1.Add(i);
    EXPECT_EQ(ret, kOk);
    ret = hll2.Add(i + 100);
    EXPECT_EQ(ret, kOk);
  }

  EXPECT_EQ(hll1.IsDense(), false);
  EXPECT_EQ(hll2.IsDense(), false);

  double est1 = hll1.Estimate();
  double est2 = hll2.Estimate();

  ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kOk);

  double merged_est = hll1.Estimate();
  EXPECT_GT(merged_est, est1);
  EXPECT_GT(merged_est, est2);
} /*}}}*/

TEST_D(HyperLogLog, Test_Merge_Dense_Mode, "密集模式合并测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(8);  // 小p值容易转dense
  HyperLogLog hll2(8);

  Code ret = hll1.Init();
  EXPECT_EQ(ret, kOk);
  ret = hll2.Init();
  EXPECT_EQ(ret, kOk);

  // 添加足够多元素触发dense转换
  for (int i = 0; i < 500; ++i) {
    ret = hll1.Add(i);
    EXPECT_EQ(ret, kOk);
    ret = hll2.Add(i + 1000);
    EXPECT_EQ(ret, kOk);
  }

  EXPECT_EQ(hll1.IsDense(), true);
  EXPECT_EQ(hll2.IsDense(), true);

  double est1 = hll1.Estimate();
  double est2 = hll2.Estimate();

  ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kOk);

  double merged_est = hll1.Estimate();
  EXPECT_GT(merged_est, est1);
  EXPECT_GT(merged_est, est2);
} /*}}}*/

TEST_D(HyperLogLog, Test_Merge_Mixed_Mode, "混合模式合并测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(10);
  HyperLogLog hll2(10);

  Code ret = hll1.Init();
  EXPECT_EQ(ret, kOk);
  ret = hll2.Init();
  EXPECT_EQ(ret, kOk);

  // hll1保持sparse，hll2转为dense
  for (int i = 0; i < 50; ++i) {
    ret = hll1.Add(i);
    EXPECT_EQ(ret, kOk);
  }

  for (int i = 0; i < 2000; ++i) {
    ret = hll2.Add(i + 10000);
    EXPECT_EQ(ret, kOk);
  }

  EXPECT_EQ(hll1.IsDense(), false);
  EXPECT_EQ(hll2.IsDense(), true);

  ret = hll1.Merge(hll2);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(hll1.IsDense(), true);  // 合并后应该变为dense
} /*}}}*/

TEST_D(HyperLogLog, Test_Estimate_Accuracy, "估计精度测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(16);
  Code ret = hll.Init();
  EXPECT_EQ(ret, kOk);

  // 测试不同数量级的估计精度
  std::vector<int> test_sizes = {100, 1000, 10000, 50000};

  for (int size : test_sizes) {
    HyperLogLog test_hll(16);
    ret = test_hll.Init();
    EXPECT_EQ(ret, kOk);

    for (int i = 0; i < size; ++i) {
      ret = test_hll.Add(i);
      EXPECT_EQ(ret, kOk);
    }

    double estimate = test_hll.Estimate();
    double error_rate = std::abs(estimate - size) / size;

    fprintf(stderr, "Size: %d, Estimate: %f, Error rate: %f\n", size, estimate, error_rate);

    // HyperLogLog的标准误差约为1.04/sqrt(m)，m=2^16时约为1.6%
    EXPECT_LT(error_rate, 0.1);  // 允许10%的误差范围
  }
} /*}}}*/

TEST_D(HyperLogLog, Test_Memory_Usage, "内存使用量测试") { /*{{{*/
  using namespace base;

  // 测试不同p值的内存使用
  for (unsigned p = 8; p <= 16; ++p) {
    HyperLogLog hll(p);
    Code ret = hll.Init();
    EXPECT_EQ(ret, kOk);

    size_t sparse_memory = hll.MemoryBytesEstimate();

    // 添加元素直到转换为dense
    for (int i = 0; i < 50000; ++i) {
      ret = hll.Add(i);
      EXPECT_EQ(ret, kOk);
      if (hll.IsDense()) break;
    }

    size_t dense_memory = hll.MemoryBytesEstimate();

    fprintf(stderr, "p=%u: sparse=%zu bytes, dense=%zu bytes\n", p, sparse_memory, dense_memory);

    // Dense模式内存使用应该是2^p字节
    EXPECT_EQ(dense_memory, 1u << p);
  }
} /*}}}*/

TEST_D(HyperLogLog, Test_String_Add, "字符串类型添加测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(14);
  Code ret = hll.Init();
  EXPECT_EQ(ret, kOk);

  // 测试添加字符串
  std::vector<std::string> strings = {"hello",     "world",       "hyperloglog", "test", "string",
                                      "algorithm", "cardinality", "estimation",  "data", "structure"};

  for (const auto& str : strings) {
    ret = hll.Add(str);
    EXPECT_EQ(ret, kOk);
  }

  double estimate = hll.Estimate();
  EXPECT_GT(estimate, 5.0);   // 应该能检测到多个不同字符串
  EXPECT_LT(estimate, 20.0);  // 但不应该过高

  fprintf(stderr, "String test estimate: %f\n", estimate);
} /*}}}*/

TEST_D(HyperLogLog, Test_Duplicate_Elements, "重复元素处理测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll(12);
  Code ret = hll.Init();
  EXPECT_EQ(ret, kOk);

  // 添加重复元素
  int unique_count = 1000;
  int duplicates = 5;

  for (int dup = 0; dup < duplicates; ++dup) {
    for (int i = 0; i < unique_count; ++i) {
      ret = hll.Add(i);
      EXPECT_EQ(ret, kOk);
    }
  }

  double estimate = hll.Estimate();
  double error_rate = std::abs(estimate - unique_count) / unique_count;

  fprintf(stderr, "Duplicate test - Unique: %d, Estimate: %f, Error: %f\n", unique_count, estimate, error_rate);

  // 即使有重复，估计值应该接近唯一元素数量
  EXPECT_LT(error_rate, 0.15);
} /*}}}*/

TEST_D(HyperLogLog, Test_Move_Semantics, "移动语义测试") { /*{{{*/
  using namespace base;
  HyperLogLog hll1(14);
  Code ret = hll1.Init();
  EXPECT_EQ(ret, kOk);

  // 添加一些元素
  for (int i = 0; i < 1000; ++i) {
    ret = hll1.Add(i);
    EXPECT_EQ(ret, kOk);
  }

  double original_estimate = hll1.Estimate();

  // 测试移动构造
  HyperLogLog hll2 = std::move(hll1);
  double moved_estimate = hll2.Estimate();

  EXPECT_EQ(original_estimate, moved_estimate);

  // 测试移动赋值
  HyperLogLog hll3(14);
  hll3 = std::move(hll2);
  double assigned_estimate = hll3.Estimate();

  EXPECT_EQ(original_estimate, assigned_estimate);
} /*}}}*/
