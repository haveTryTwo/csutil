// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>

#include <utility>

#include "base/algo.h"
#include "base/anns.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

namespace {

TEST_D(ANNS, Test_Normal_BruteForceANNS, "暴力搜索最近邻") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}};
  std::vector<double> query = {2.5, 3.5};

  std::vector<double> nearest;
  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);
  EXPECT_EQ(nearest[0], 2.0);
  EXPECT_EQ(nearest[1], 3.0);

  fprintf(stderr, "Nearest neighbor: \n");
  for (double val : nearest) {
    fprintf(stderr, "%f ", val);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST_D(ANNS, Test_Exception_BruteForceANNS, "暴力搜索最近邻异常测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}};
  std::vector<double> query = {2.5, 3.5, 1, 1, 1, 1};

  std::vector<double> nearest;
  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(ANNS, Test_NullPointer_BruteForceANNS, "空指针参数测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{1.0, 2.0}, {2.0, 3.0}};
  std::vector<double> query = {1.5, 2.5};

  // 测试空指针参数
  Code ret = BruteForceANNS(data, query, nullptr);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(ANNS, Test_EmptyData_BruteForceANNS, "空数据集测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data;  // 空数据集
  std::vector<double> query = {1.0, 2.0};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(ANNS, Test_SinglePoint_BruteForceANNS, "单点数据集测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{3.0, 4.0}};
  std::vector<double> query = {1.0, 1.0};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);
  EXPECT_EQ(nearest[0], 3.0);
  EXPECT_EQ(nearest[1], 4.0);
} /*}}}*/

TEST_D(ANNS, Test_IdenticalPoints_BruteForceANNS, "相同点测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}};
  std::vector<double> query = {1.0, 1.0};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);
  EXPECT_EQ(nearest[0], 1.0);
  EXPECT_EQ(nearest[1], 1.0);
} /*}}}*/

TEST_D(ANNS, Test_HighDimensional_BruteForceANNS, "高维数据测试") { /*{{{*/
  using namespace base;
  // 5维数据点
  std::vector<std::vector<double>> data = {
      {1.0, 2.0, 3.0, 4.0, 5.0}, {2.0, 3.0, 4.0, 5.0, 6.0}, {0.0, 1.0, 2.0, 3.0, 4.0}};
  std::vector<double> query = {1.5, 2.5, 3.5, 4.5, 5.5};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 5);

  // 验证完整的最近邻结果：应该找到第一个点 {1.0, 2.0, 3.0, 4.0, 5.0}
  // 注意：第1个和第2个点距离相同，算法返回第一个找到的点
  std::vector<double> expected = {1.0, 2.0, 3.0, 4.0, 5.0};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(nearest[i], expected[i]);
  }

  // 打印结果用于调试验证
  fprintf(stderr, "High-dimensional test - Query: [");
  for (size_t i = 0; i < query.size(); ++i) {
    fprintf(stderr, "%.1f%s", query[i], (i < query.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "]\n");

  fprintf(stderr, "High-dimensional test - Nearest: [");
  for (size_t i = 0; i < nearest.size(); ++i) {
    fprintf(stderr, "%.1f%s", nearest[i], (i < nearest.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "]\n");
} /*}}}*/

TEST_D(ANNS, Test_NegativeValues_BruteForceANNS, "负数数据测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{-1.0, -2.0}, {-3.0, -4.0}, {-0.5, -1.5}};
  std::vector<double> query = {-1.2, -2.1};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);

  // 验证完整结果：应该找到 {-1.0, -2.0} 作为最近邻
  std::vector<double> expected = {-1.0, -2.0};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(nearest[i], expected[i]);
  }

  fprintf(stderr, "Negative values test - Nearest: [%.1f, %.1f]\n", nearest[0], nearest[1]);
} /*}}}*/

TEST_D(ANNS, Test_ZeroValues_BruteForceANNS, "零值数据测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {{0.0, 0.0}, {1.0, 1.0}, {-1.0, -1.0}};
  std::vector<double> query = {0.1, 0.1};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);

  // 验证完整结果：应该找到 {0.0, 0.0} 作为最近邻
  std::vector<double> expected = {0.0, 0.0};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(nearest[i], expected[i]);
  }

  fprintf(stderr, "Zero values test - Nearest: [%.1f, %.1f]\n", nearest[0], nearest[1]);
} /*}}}*/

TEST_D(ANNS, Test_LargeDataset_BruteForceANNS, "大数据集性能测试") { /*{{{*/
  using namespace base;

  // 创建较大的数据集
  std::vector<std::vector<double>> data;
  for (int i = 0; i < 1000; ++i) {
    data.push_back({static_cast<double>(i), static_cast<double>(i * 2)});
  }

  std::vector<double> query = {500.5, 1001.0};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);

  fprintf(stderr, "Large dataset test - Nearest neighbor: [%f, %f]\n", nearest[0], nearest[1]);
} /*}}}*/

TEST_D(ANNS, Test_ExtremeValues_BruteForceANNS, "极值数据测试") { /*{{{*/
  using namespace base;
  std::vector<std::vector<double>> data = {
      {std::numeric_limits<double>::max() / 2, 1.0}, {std::numeric_limits<double>::lowest() / 2, 1.0}, {0.0, 0.0}};
  std::vector<double> query = {1.0, 1.0};
  std::vector<double> nearest;

  Code ret = BruteForceANNS(data, query, &nearest);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(nearest.size(), 2);

  // 验证完整结果：应该找到 {0.0, 0.0} 作为最近邻
  std::vector<double> expected = {0.0, 0.0};
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(nearest[i], expected[i]);
  }

  fprintf(stderr, "Extreme values test - Nearest: [%.1f, %.1f]\n", nearest[0], nearest[1]);
} /*}}}*/

}  // namespace
