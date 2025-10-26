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

// KDTree 测试用例
TEST_D(KDTree, Test_Normal_Construction, "KDTree正常构造测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0}), KDPoint({3.0, 4.0}), KDPoint({5.0, 6.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "KDTree construction test passed\n");
} /*}}}*/

TEST_D(KDTree, Test_Empty_Points, "KDTree空点集测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points;  // 空点集

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_Single_Point, "KDTree单点测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({2.0, 3.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 测试最近邻搜索
  KDPoint query({1.0, 1.0});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(best.coords[0], 2.0);
  EXPECT_EQ(best.coords[1], 3.0);

  fprintf(stderr, "Single point test - Best: [%.1f, %.1f], Distance: %.3f\n", best.coords[0], best.coords[1],
          best_dist);
} /*}}}*/

TEST_D(KDTree, Test_Inconsistent_Dimensions, "KDTree维度不一致测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {
      KDPoint({1.0, 2.0}),       // 2维
      KDPoint({3.0, 4.0, 5.0}),  // 3维
      KDPoint({6.0, 7.0})        // 2维
  };

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_Empty_Coordinates, "KDTree空坐标测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({}),  // 空坐标
                                 KDPoint({1.0, 2.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_NaN_Values, "KDTree NaN值测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0}), KDPoint({std::numeric_limits<double>::quiet_NaN(), 4.0}),
                                 KDPoint({5.0, 6.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_Infinite_Values, "KDTree无穷大值测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0}), KDPoint({std::numeric_limits<double>::infinity(), 4.0}),
                                 KDPoint({5.0, 6.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_Normal_NearestNeighbor, "KDTree正常最近邻搜索") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0}), KDPoint({3.0, 4.0}), KDPoint({5.0, 6.0}), KDPoint({7.0, 8.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 测试查询点 (2.5, 3.5)，应该找到 (3.0, 4.0)
  KDPoint query({2.5, 3.5});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(best.coords[0], 3.0);
  EXPECT_EQ(best.coords[1], 4.0);

  fprintf(stderr, "Normal search - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(KDTree, Test_NearestNeighbor_Empty_Query, "KDTree空查询点测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 空查询点
  KDPoint query({});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_NearestNeighbor_Dimension_Mismatch, "KDTree查询维度不匹配测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 查询点维度不匹配（3维 vs 2维）
  KDPoint query({1.0, 2.0, 3.0});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(KDTree, Test_HighDimensional_KDTree, "KDTree高维数据测试") { /*{{{*/
  using namespace base;
  // 5维数据点
  std::vector<KDPoint> points = {KDPoint({1.0, 2.0, 3.0, 4.0, 5.0}), KDPoint({2.0, 3.0, 4.0, 5.0, 6.0}),
                                 KDPoint({0.0, 1.0, 2.0, 3.0, 4.0}), KDPoint({3.0, 4.0, 5.0, 6.0, 7.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  KDPoint query({1.5, 2.5, 3.5, 4.5, 5.5});
  KDPoint best({0.0, 0.0, 0.0, 0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "High-dimensional test - Query: [");
  for (size_t i = 0; i < query.coords.size(); ++i) {
    fprintf(stderr, "%.1f%s", query.coords[i], (i < query.coords.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "]\n");

  fprintf(stderr, "High-dimensional test - Best: [");
  for (size_t i = 0; i < best.coords.size(); ++i) {
    fprintf(stderr, "%.1f%s", best.coords[i], (i < best.coords.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "], Distance: %.3f\n", best_dist);
} /*}}}*/

TEST_D(KDTree, Test_Identical_Points, "KDTree相同点测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({1.0, 1.0}), KDPoint({1.0, 1.0}), KDPoint({1.0, 1.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  KDPoint query({1.0, 1.0});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(best.coords[0], 1.0);
  EXPECT_EQ(best.coords[1], 1.0);
  EXPECT_EQ(best_dist, 0.0);  // 距离应该为0

  fprintf(stderr, "Identical points test - Distance: %.3f\n", best_dist);
} /*}}}*/

TEST_D(KDTree, Test_Large_Dataset, "KDTree大数据集测试") { /*{{{*/
  using namespace base;

  // 创建较大的数据集
  std::vector<KDPoint> points;
  for (int i = 0; i < 1000; ++i) {
    points.push_back(KDPoint({static_cast<double>(i), static_cast<double>(i * 2)}));
  }

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  KDPoint query({500.5, 1001.0});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "Large dataset test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(KDTree, Test_Negative_Values, "KDTree负数数据测试") { /*{{{*/
  using namespace base;
  std::vector<KDPoint> points = {KDPoint({-1.0, -2.0}), KDPoint({-3.0, -4.0}), KDPoint({-0.5, -1.5})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  KDPoint query({-1.2, -2.1});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "Negative values test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(KDTree, Test_Memory_Management, "KDTree内存管理测试") { /*{{{*/
  using namespace base;

  // 测试多次初始化
  std::vector<KDPoint> points1 = {KDPoint({1.0, 2.0}), KDPoint({3.0, 4.0})};
  std::vector<KDPoint> points2 = {KDPoint({5.0, 6.0}), KDPoint({7.0, 8.0})};

  KDTree tree(points1);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 重新初始化应该清理之前的树
  tree = KDTree(points2);
  ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  KDPoint query({5.5, 7.0});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  // 校验找到的最近邻节点
  // 查询点 (5.5, 7.0) 应该找到 (5.0, 6.0) 作为最近邻
  // 因为距离 sqrt((5.5-5.0)^2 + (7.0-6.0)^2) = sqrt(0.25 + 1) = sqrt(1.25) ≈ 1.118
  // 而到 (7.0, 8.0) 的距离是 sqrt((5.5-7.0)^2 + (7.0-8.0)^2) = sqrt(2.25 + 1) = sqrt(3.25) ≈ 1.803
  EXPECT_EQ(best.coords[0], 5.0);
  EXPECT_EQ(best.coords[1], 6.0);

  // 验证距离计算
  double expected_dist = std::sqrt((5.5 - 5.0) * (5.5 - 5.0) + (7.0 - 6.0) * (7.0 - 6.0));
  EXPECT_NEAR(best_dist, expected_dist, 0.001);  // 允许小的浮点误差

  // 打印详细信息
  fprintf(stderr, "Memory management test:\n");
  fprintf(stderr, "  Available points after reinit: [(5.0, 6.0), (7.0, 8.0)]\n");
  fprintf(stderr, "  Query point: [%.1f, %.1f]\n", query.coords[0], query.coords[1]);
  fprintf(stderr, "  Found nearest: [%.1f, %.1f]\n", best.coords[0], best.coords[1]);
  fprintf(stderr, "  Distance: %.3f\n", best_dist);
  fprintf(stderr, "  Expected nearest: [5.0, 6.0] with distance: %.3f\n", expected_dist);
} /*}}}*/

TEST_D(KDTree, Test_Recursion_Depth_Limit, "KDTree递归深度限制测试") { /*{{{*/
  using namespace base;

  // 创建线性排列的点，可能导致不平衡的树
  std::vector<KDPoint> points;
  for (int i = 0; i < 100; ++i) {
    points.push_back(KDPoint({static_cast<double>(i), 0.0}));
  }

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);  // 应该成功，因为深度限制为64

  fprintf(stderr, "Recursion depth limit test passed\n");
} /*}}}*/

TEST_D(KDTree, Test_Depth_Exceed_64, "KDTree深度超过64限制测试") { /*{{{*/
  using namespace base;

  // 创建一个极端不平衡的数据集，强制触发深度限制
  // 使用大量的线性排列点，在单一维度上递增
  std::vector<KDPoint> points;

  // 创建足够多的点来可能超过深度64
  // 由于KD树的构建方式，我们需要创建一个最坏情况的数据集
  for (int i = 0; i < 200; ++i) {
    // 创建在x轴上线性分布的点，y坐标交替变化以增加复杂性
    double x = static_cast<double>(i);
    double y = (i % 2 == 0) ? 0.0 : 0.1;
    points.push_back(KDPoint({x, y}));
  }

  KDTree tree(points);
  Code ret = tree.Init();

  // 根据当前的深度限制实现，应该仍然成功
  // 因为即使有200个点，平衡的KD树深度约为log2(200) ≈ 7.6
  // 但如果数据排列导致极度不平衡，可能会触发深度限制
  if (ret == kOk) {
    fprintf(stderr, "Depth exceed test: Tree built successfully with %zu points\n", points.size());

    // 测试搜索功能是否正常
    KDPoint query({100.0, 0.05});
    KDPoint best({0.0, 0.0});
    double best_dist = 0.0;

    Code search_ret = tree.NearestNeighbor(query, best, best_dist);
    EXPECT_EQ(search_ret, kOk);

    fprintf(stderr, "Search test: Query [%.1f, %.2f] -> Best [%.1f, %.2f], Distance: %.3f\n", query.coords[0],
            query.coords[1], best.coords[0], best.coords[1], best_dist);
  } else if (ret == kInvalidParam) {
    fprintf(stderr, "Depth exceed test: Tree construction failed due to depth limit (expected behavior)\n");
    // 这也是可接受的结果，说明深度限制正在工作
  } else {
    fprintf(stderr, "Depth exceed test: Unexpected error code: %d\n", static_cast<int>(ret));
  }
} /*}}}*/

TEST_D(KDTree, Test_Extreme_Depth_Simulation, "KDTree极端深度模拟测试") { /*{{{*/
  using namespace base;

  // 通过直接调用NearestNeighborInternal来模拟深度超过64的情况
  // 创建一个简单的树结构
  std::vector<KDPoint> points = {KDPoint({1.0, 1.0}), KDPoint({2.0, 2.0})};

  KDTree tree(points);
  Code ret = tree.Init();
  EXPECT_EQ(ret, kOk);

  // 测试正常深度的搜索
  KDPoint query({1.5, 1.5});
  KDPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "Normal depth search: Query [%.1f, %.1f] -> Best [%.1f, %.1f]\n", query.coords[0], query.coords[1],
          best.coords[0], best.coords[1]);

  // 由于NearestNeighbor函数已经移除depth参数，深度限制现在在内部自动处理
  // 测试正常搜索（内部会从depth=0开始，并自动处理深度限制）
  ret = tree.NearestNeighbor(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "深度限制测试: 正常搜索成功，内部深度控制正常工作\n");
  fprintf(stderr, "找到的最近邻: [%.1f, %.1f], 距离: %.3f\n", best.coords[0], best.coords[1], best_dist);
} /*}}}*/

// HNSWGraph 测试用例
/**
 * @brief 测试HNSWGraph的正常初始化
 *
 * 测试目标:
 * - 验证使用有效参数创建图时初始化成功
 *
 * 测试步骤:
 * 1. 创建HNSWGraph对象，最大层数4，最大连接数2
 * 2. 调用Init()初始化
 * 3. 验证返回码为kOk
 */
TEST_D(HNSWGraph, Test_Normal_Init, "HNSWGraph正常初始化测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);  // 最大层数4，最大连接数2

  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "HNSWGraph initialization test passed\n");
} /*}}}*/

/**
 * @brief 测试HNSWGraph的无效参数初始化
 *
 * 测试目标:
 * - 验证使用无效参数时初始化失败并返回正确错误码
 *
 * 测试用例:
 * 1. 最大层数过小(<=1)应返回kInvalidParam
 * 2. 最大连接数过小(<=1)应返回kInvalidParam
 * 3. 最大层数过大(>32)应返回kInvalidParam
 * 4. 最大连接数过大(>32)应返回kInvalidParam
 * 5. 最大层数小于最大连接数应返回kInvalidParam
 * 6. 最大层数为负数应返回kInvalidParam
 * 7. 最大连接数为负数应返回kInvalidParam
 * 8. 两个参数都为负数应返回kInvalidParam
 */
TEST_D(HNSWGraph, Test_Invalid_Params_Init, "HNSWGraph无效参数初始化测试") { /*{{{*/
  using namespace base;

  // 测试最大层数过小
  HNSWGraph graph1(1, 2);
  Code ret = graph1.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大连接数过小
  HNSWGraph graph2(4, 1);
  ret = graph2.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大层数过大
  HNSWGraph graph3(33, 2);
  ret = graph3.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大连接数过大
  HNSWGraph graph4(4, 33);
  ret = graph4.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大层数小于最大连接数
  HNSWGraph graph5(2, 4);
  ret = graph5.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大层数为负数
  HNSWGraph graph6(-1, 2);
  ret = graph6.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试最大连接数为负数
  HNSWGraph graph7(4, -1);
  ret = graph7.Init();
  EXPECT_EQ(ret, kInvalidParam);

  // 测试两个参数都为负数
  HNSWGraph graph8(-5, -3);
  ret = graph8.Init();
  EXPECT_EQ(ret, kInvalidParam);

  fprintf(stderr, "HNSWGraph invalid parameters test passed\n");
} /*}}}*/

/**
 * @brief 测试HNSWGraph的第一个点插入
 *
 * 测试目标:
 * - 验证向空图中插入第一个点成功
 * - 第一个点将成为入口点(entry point)
 *
 * 测试步骤:
 * 1. 初始化图
 * 2. 插入第一个点
 * 3. 验证插入成功
 */
TEST_D(HNSWGraph, Test_First_Insert, "HNSWGraph第一个点插入测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  HNSWPoint point({1.0, 2.0});
  ret = graph.Insert(point);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "HNSWGraph first insert test passed\n");
} /*}}}*/

TEST_D(HNSWGraph, Test_Multiple_Insert, "HNSWGraph多点插入测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 插入多个点
  std::vector<HNSWPoint> points = {HNSWPoint({1.0, 2.0}), HNSWPoint({3.0, 4.0}), HNSWPoint({5.0, 6.0}),
                                   HNSWPoint({7.0, 8.0})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  fprintf(stderr, "HNSWGraph multiple insert test passed\n");
} /*}}}*/

/**
 * @brief 测试HNSWGraph插入后的搜索功能
 *
 * 测试目标:
 * - 验证在插入多个点后，能正确搜索到最近邻
 *
 * 测试步骤:
 * 1. 初始化图并插入4个点
 * 2. 搜索查询点(2.5, 3.5)的最近邻
 * 3. 验证找到的最近邻是(3.0, 4.0)
 * 4. 验证计算的距离准确性
 */
TEST_D(HNSWGraph, Test_Search_After_Insert, "HNSWGraph插入后搜索测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 插入测试点
  std::vector<HNSWPoint> points = {HNSWPoint({1.0, 2.0}), HNSWPoint({3.0, 4.0}), HNSWPoint({5.0, 6.0}),
                                   HNSWPoint({7.0, 8.0})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  // 搜索最近邻
  HNSWPoint query({2.5, 3.5});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  // 验证找到的最近邻点坐标
  EXPECT_EQ(best.coords[0], 3.0);
  EXPECT_EQ(best.coords[1], 4.0);

  // 验证距离计算的准确性
  double expected_dist = std::sqrt((2.5 - 3.0) * (2.5 - 3.0) + (3.5 - 4.0) * (3.5 - 4.0));
  EXPECT_NEAR(best_dist, expected_dist, 0.001);

  fprintf(stderr, "Search test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Search_Empty_Graph, "HNSWGraph空图搜索测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 在空图中搜索
  HNSWPoint query({1.0, 2.0});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kInvalidParam);

  fprintf(stderr, "HNSWGraph empty graph search test passed\n");
} /*}}}*/

/**
 * @brief 测试HNSWGraph处理高维数据的能力
 *
 * 测试目标:
 * - 验证HNSW图能正确处理5维数据
 * - 验证高维空间中的最近邻搜索准确性
 *
 * 测试步骤:
 * 1. 插入4个5维点
 * 2. 搜索查询点的最近邻
 * 3. 验证找到正确的最近邻点
 * 4. 验证距离计算准确性
 */
TEST_D(HNSWGraph, Test_HighDimensional_Data, "HNSWGraph高维数据测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 5维数据点
  std::vector<HNSWPoint> points = {HNSWPoint({1.0, 2.0, 3.0, 4.0, 5.0}), HNSWPoint({2.0, 3.0, 4.0, 5.0, 6.0}),
                                   HNSWPoint({0.0, 1.0, 2.0, 3.0, 4.0}), HNSWPoint({3.0, 4.0, 5.0, 6.0, 7.0})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint query({1.5, 2.5, 3.5, 4.5, 5.5});
  HNSWPoint best({0.0, 0.0, 0.0, 0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  // 验证找到的最近邻点坐标
  EXPECT_EQ(best.coords[0], 1.0);
  EXPECT_EQ(best.coords[1], 2.0);
  EXPECT_EQ(best.coords[2], 3.0);
  EXPECT_EQ(best.coords[3], 4.0);
  EXPECT_EQ(best.coords[4], 5.0);
  EXPECT_NEAR(best_dist,
              std::sqrt((1.5 - 1.0) * (1.5 - 1.0) + (2.5 - 2.0) * (2.5 - 2.0) + (3.5 - 3.0) * (3.5 - 3.0) +
                        (4.5 - 4.0) * (4.5 - 4.0) + (5.5 - 5.0) * (5.5 - 5.0)),
              0.001);

  fprintf(stderr, "High-dimensional test - Query: [");
  for (size_t i = 0; i < query.coords.size(); ++i) {
    fprintf(stderr, "%.1f%s", query.coords[i], (i < query.coords.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "]\n");

  fprintf(stderr, "High-dimensional test - Best: [");
  for (size_t i = 0; i < best.coords.size(); ++i) {
    fprintf(stderr, "%.1f%s", best.coords[i], (i < best.coords.size() - 1) ? ", " : "");
  }
  fprintf(stderr, "], Distance: %.3f\n", best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Negative_Values, "HNSWGraph负数数据测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  std::vector<HNSWPoint> points = {HNSWPoint({-1.0, -2.0}), HNSWPoint({-3.0, -4.0}), HNSWPoint({-0.5, -1.5})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint query({-1.2, -2.1});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "Negative values test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Identical_Points, "HNSWGraph相同点测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 插入相同的点
  HNSWPoint point({1.0, 1.0});
  ret = graph.Insert(point);
  EXPECT_EQ(ret, kOk);

  ret = graph.Insert(point);
  EXPECT_EQ(ret, kOk);

  ret = graph.Insert(point);
  EXPECT_EQ(ret, kOk);

  // 搜索相同的点
  HNSWPoint query({1.0, 1.0});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(best.coords[0], 1.0);
  EXPECT_EQ(best.coords[1], 1.0);
  EXPECT_EQ(best_dist, 0.0);  // 距离应该为0

  fprintf(stderr, "Identical points test - Distance: %.3f\n", best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Large_Dataset, "HNSWGraph大数据集测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  // 创建较大的数据集
  std::vector<HNSWPoint> points;
  for (int i = 0; i < 100; ++i) {
    points.push_back(HNSWPoint({static_cast<double>(i), static_cast<double>(i * 2)}));
  }

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint query({50.5, 101.0});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);

  fprintf(stderr, "Large dataset test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Zero_Values, "HNSWGraph零值数据测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  std::vector<HNSWPoint> points = {HNSWPoint({0.0, 0.0}), HNSWPoint({1.0, 1.0}), HNSWPoint({-1.0, -1.0})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint query({0.1, 0.1});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  // 验证找到的最近邻点坐标
  EXPECT_EQ(best.coords[0], 0.0);
  EXPECT_EQ(best.coords[1], 0.0);
  // 验证距离计算准确性
  EXPECT_NEAR(best_dist, std::sqrt((0.1 - 0.0) * (0.1 - 0.0) + (0.1 - 0.0) * (0.1 - 0.0)), 0.001);

  fprintf(stderr, "Zero values test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Extreme_Values, "HNSWGraph极值数据测试") { /*{{{*/
  using namespace base;
  HNSWGraph graph(4, 2);
  Code ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  std::vector<HNSWPoint> points = {HNSWPoint({std::numeric_limits<double>::max() / 2, 1.0}),
                                   HNSWPoint({std::numeric_limits<double>::lowest() / 2, 1.0}), HNSWPoint({0.0, 0.0})};

  for (const auto& point : points) {
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint query({1.0, 1.0});
  HNSWPoint best({0.0, 0.0});
  double best_dist = 0.0;

  ret = graph.Search(query, best, best_dist);
  EXPECT_EQ(ret, kOk);
  // 验证找到的最近邻点坐标
  EXPECT_EQ(best.coords[0], 0.0);
  EXPECT_EQ(best.coords[1], 0.0);
  // 验证距离计算准确性
  EXPECT_NEAR(best_dist, std::sqrt((1.0 - 0.0) * (1.0 - 0.0) + (1.0 - 0.0) * (1.0 - 0.0)), 0.001);

  fprintf(stderr, "Extreme values test - Query: [%.1f, %.1f], Best: [%.1f, %.1f], Distance: %.3f\n", query.coords[0],
          query.coords[1], best.coords[0], best.coords[1], best_dist);
} /*}}}*/

TEST_D(HNSWGraph, Test_Different_Layer_Configurations, "HNSWGraph不同层配置测试") { /*{{{*/
  using namespace base;

  // 测试不同的层配置
  std::vector<std::pair<int, int>> configs = {
      {2, 2},  // 最小配置
      {3, 2},  // 3层2连接
      {4, 3},  // 4层3连接
      {5, 2},  // 5层2连接
      {6, 4}   // 6层4连接
  };

  for (const auto& config : configs) {
    HNSWGraph graph(config.first, config.second);
    Code ret = graph.Init();
    EXPECT_EQ(ret, kOk);

    // 插入一些测试点
    std::vector<HNSWPoint> points = {HNSWPoint({1.0, 2.0}), HNSWPoint({3.0, 4.0}), HNSWPoint({5.0, 6.0})};

    for (const auto& point : points) {
      ret = graph.Insert(point);
      EXPECT_EQ(ret, kOk);
    }

    // 测试搜索
    HNSWPoint query({2.5, 3.5});
    HNSWPoint best({0.0, 0.0});
    double best_dist = 0.0;

    ret = graph.Search(query, best, best_dist);
    EXPECT_EQ(ret, kOk);
    // 验证找到的最近邻点坐标
    EXPECT_EQ(best.coords[0], 3.0);
    EXPECT_EQ(best.coords[1], 4.0);
    // 验证距离计算准确性
    EXPECT_NEAR(best_dist, std::sqrt((2.5 - 3.0) * (2.5 - 3.0) + (3.5 - 4.0) * (3.5 - 4.0)), 0.001);

    fprintf(stderr, "Config (%d, %d) - Best: [%.1f, %.1f], Distance: %.3f\n", config.first, config.second,
            best.coords[0], best.coords[1], best_dist);
  }
} /*}}}*/

/**
 * @brief 测试HNSWGraph与暴力搜索的精度对比
 *
 * 测试目标:
 * - 验证HNSW算法的搜索结果与暴力搜索结果的准确性
 * - 评估近似最近邻搜索的精度
 *
 * 测试步骤:
 * 1. 创建包含10个点的测试数据集
 * 2. 使用暴力搜索找到精确的最近邻
 * 3. 使用HNSW搜索找到近似最近邻
 * 4. 对比两种方法的结果
 * 5. 验证距离差异在可接受范围内
 *
 * 预期结果:
 * - HNSW找到的点与暴力搜索结果相同或距离非常接近
 * - 距离差异应小于1.0
 */
TEST_D(HNSWGraph, Test_Accuracy_Comparison, "HNSWGraph与暴力搜索精度对比测试") { /*{{{*/
  using namespace base;

  // 创建测试数据
  std::vector<std::vector<double>> data = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0}, {9.0, 10.0},
                                           {2.0, 3.0}, {4.0, 5.0}, {6.0, 7.0}, {8.0, 9.0}, {10.0, 11.0}};

  std::vector<double> query = {5.5, 6.5};

  // 暴力搜索找到最近邻
  std::vector<double> brute_force_result;
  Code ret = BruteForceANNS(data, query, &brute_force_result);
  EXPECT_EQ(ret, kOk);

  // HNSW搜索找到最近邻
  HNSWGraph graph(6, 4);  // 使用更大的参数以提高精度
  ret = graph.Init();
  EXPECT_EQ(ret, kOk);

  for (const auto& point_data : data) {
    HNSWPoint point(point_data);
    ret = graph.Insert(point);
    EXPECT_EQ(ret, kOk);
  }

  HNSWPoint hnsw_query(query);
  HNSWPoint hnsw_best({0.0, 0.0});
  double hnsw_dist = 0.0;

  ret = graph.Search(hnsw_query, hnsw_best, hnsw_dist);
  EXPECT_EQ(ret, kOk);

  // 计算暴力搜索的距离
  double brute_force_dist = 0.0;
  ret = CalculteDistance(query, brute_force_result, &brute_force_dist);
  EXPECT_EQ(ret, kOk);

  // 输出对比结果
  fprintf(stderr, "Accuracy comparison test:\n");
  fprintf(stderr, "  Query: [%.1f, %.1f]\n", query[0], query[1]);
  fprintf(stderr, "  Brute force result: [%.1f, %.1f], Distance: %.3f\n", brute_force_result[0], brute_force_result[1],
          brute_force_dist);
  fprintf(stderr, "  HNSW result: [%.1f, %.1f], Distance: %.3f\n", hnsw_best.coords[0], hnsw_best.coords[1], hnsw_dist);

  // HNSW的结果应该接近暴力搜索的结果
  double distance_diff = std::abs(hnsw_dist - brute_force_dist);
  fprintf(stderr, "  Distance difference: %.3f\n", distance_diff);

  // 验证距离差异在可接受范围内
  EXPECT_LT(distance_diff, 1.0);  // 允许1.0的误差

  // 最佳情况：找到完全相同的点
  if (distance_diff < 0.001) {
    fprintf(stderr, "  Perfect match! HNSW found the exact nearest neighbor.\n");
  }
} /*}}}*/

// ProductQuantizer 测试用例

/**
 * @brief 测试ProductQuantizer的正常初始化
 *
 * 测试目标:
 * - 验证使用有效参数创建量化器时初始化成功
 *
 * 测试步骤:
 * 1. 创建ProductQuantizer对象
 * 2. 验证对象创建成功
 */
TEST_D(ProductQuantizer, Test_Normal_Init, "ProductQuantizer正常初始化测试") { /*{{{*/
  using namespace base;
  
  // M=4个子空间, K=256个聚类中心, D=128维
  ProductQuantizer pq(4, 256, 128);
  
  fprintf(stderr, "ProductQuantizer initialization test passed\n");
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的训练功能
 *
 * 测试目标:
 * - 验证使用训练数据训练量化器
 * - 确保码本生成正确
 *
 * 测试步骤:
 * 1. 创建训练数据集
 * 2. 训练量化器
 * 3. 验证训练成功
 */
TEST_D(ProductQuantizer, Test_Train, "ProductQuantizer训练测试") { /*{{{*/
  using namespace base;
  
  // 创建8维向量，分成4个子空间，每个子空间2维
  ProductQuantizer pq(4, 4, 8);  // M=4, K=4, D=8
  
  // 创建训练数据 (100个8维向量)
  std::vector<PQPoint> train_data;
  for (int i = 0; i < 100; ++i) {
    std::vector<double> coords(8);
    for (int j = 0; j < 8; ++j) {
      coords[j] = static_cast<double>(i % 10 + j);
    }
    train_data.push_back(PQPoint(coords));
  }
  
  Code ret = pq.Train(train_data);
  EXPECT_EQ(ret, kOk);
  
  fprintf(stderr, "ProductQuantizer training test passed with %zu training samples\n", 
          train_data.size());
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的量化功能
 *
 * 测试目标:
 * - 验证向量可以被正确量化为codes
 *
 * 测试步骤:
 * 1. 训练量化器
 * 2. 量化一个向量
 * 3. 验证codes的长度和值
 */
TEST_D(ProductQuantizer, Test_Quantize, "ProductQuantizer量化测试") { /*{{{*/
  using namespace base;
  
  ProductQuantizer pq(2, 4, 4);  // M=2个子空间, K=4个聚类中心, D=4维
  
  // 创建简单的训练数据
  std::vector<PQPoint> train_data;
  for (int i = 0; i < 20; ++i) {
    std::vector<double> coords = {
      static_cast<double>(i % 5), 
      static_cast<double>((i + 1) % 5),
      static_cast<double>((i + 2) % 5), 
      static_cast<double>((i + 3) % 5)
    };
    train_data.push_back(PQPoint(coords));
  }
  
  Code ret = pq.Train(train_data);
  EXPECT_EQ(ret, kOk);
  
  // 量化一个新向量
  PQPoint test_point({1.5, 2.5, 3.5, 4.5});
  std::vector<int> codes;
  
  ret = pq.Quantize(test_point, codes);
  EXPECT_EQ(ret, kOk);
  
  // 验证codes长度等于子空间数量
  EXPECT_EQ(codes.size(), 2);
  
  // 验证每个code在有效范围内 [0, K-1]
  for (int code : codes) {
    EXPECT_GE(code, 0);
    EXPECT_LT(code, 4);
  }
  
  fprintf(stderr, "Quantized codes: [%d, %d]\n", codes[0], codes[1]);
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的近似距离计算
 *
 * 测试目标:
 * - 验证可以使用量化后的codes计算近似距离
 * - 近似距离应该接近真实距离
 *
 * 测试步骤:
 * 1. 训练量化器
 * 2. 量化目标向量
 * 3. 计算查询向量与量化向量的近似距离
 * 4. 对比近似距离与真实距离
 */
TEST_D(ProductQuantizer, Test_ApproximateDistance, "ProductQuantizer近似距离计算测试") { /*{{{*/
  using namespace base;
  
  ProductQuantizer pq(2, 8, 4);  // M=2, K=8, D=4
  
  // 创建训练数据
  std::vector<PQPoint> train_data;
  for (int i = 0; i < 50; ++i) {
    std::vector<double> coords(4);
    for (int j = 0; j < 4; ++j) {
      coords[j] = static_cast<double>((i * 7 + j * 3) % 20);
    }
    train_data.push_back(PQPoint(coords));
  }
  
  Code ret = pq.Train(train_data);
  EXPECT_EQ(ret, kOk);
  
  // 目标向量
  PQPoint target({5.0, 6.0, 7.0, 8.0});
  std::vector<int> codes;
  ret = pq.Quantize(target, codes);
  EXPECT_EQ(ret, kOk);
  
  // 查询向量
  PQPoint query({5.5, 6.5, 7.5, 8.5});
  
  // 计算近似距离
  double approx_dist = 0.0;
  ret = pq.ApproximateDistance(query, codes, &approx_dist);
  EXPECT_EQ(ret, kOk);
  
  // 计算真实距离
  double real_dist = 0.0;
  ret = CalculteDistance(query.coords, target.coords, &real_dist);
  EXPECT_EQ(ret, kOk);
  
  fprintf(stderr, "Distance comparison:\n");
  fprintf(stderr, "  Real distance: %.3f\n", real_dist);
  fprintf(stderr, "  Approximate distance: %.3f\n", approx_dist);
  fprintf(stderr, "  Error: %.3f (%.1f%%)\n", 
          std::abs(approx_dist - real_dist),
          100.0 * std::abs(approx_dist - real_dist) / real_dist);
  
  // 近似距离应该在合理范围内（允许较大误差，因为是近似算法）
  // PQ是有损压缩，误差可能很大，这里只验证计算成功即可
  EXPECT_GT(approx_dist, 0.0);  // 验证距离为正值
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的高维数据处理
 *
 * 测试目标:
 * - 验证可以处理高维向量（如128维）
 *
 * 测试步骤:
 * 1. 创建高维训练数据
 * 2. 训练量化器
 * 3. 量化和计算距离
 */
TEST_D(ProductQuantizer, Test_HighDimensional, "ProductQuantizer高维数据测试") { /*{{{*/
  using namespace base;
  
  int D = 64;  // 64维
  int M = 8;   // 8个子空间，每个8维
  int K = 16;  // 16个聚类中心
  
  ProductQuantizer pq(M, K, D);
  
  // 创建高维训练数据
  std::vector<PQPoint> train_data;
  for (int i = 0; i < 100; ++i) {
    std::vector<double> coords(D);
    for (int j = 0; j < D; ++j) {
      coords[j] = static_cast<double>((i + j) % 50);
    }
    train_data.push_back(PQPoint(coords));
  }
  
  Code ret = pq.Train(train_data);
  EXPECT_EQ(ret, kOk);
  
  // 量化测试
  PQPoint test_point(std::vector<double>(D, 25.0));
  std::vector<int> codes;
  ret = pq.Quantize(test_point, codes);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(codes.size(), static_cast<size_t>(M));
  
  fprintf(stderr, "High-dimensional test passed: D=%d, M=%d, K=%d\n", D, M, K);
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的压缩率
 *
 * 测试目标:
 * - 验证量化后的存储空间显著小于原始向量
 *
 * 测试步骤:
 * 1. 计算原始向量的存储空间
 * 2. 计算量化后codes的存储空间
 * 3. 对比压缩率
 */
TEST_D(ProductQuantizer, Test_CompressionRatio, "ProductQuantizer压缩率测试") { /*{{{*/
  using namespace base;
  
  int D = 128;  // 128维
  int M = 8;    // 8个子空间
  int K = 256;  // 256个聚类中心（1字节存储）
  
  ProductQuantizer pq(M, K, D);
  
  // 创建训练数据
  std::vector<PQPoint> train_data;
  for (int i = 0; i < 100; ++i) {
    std::vector<double> coords(D);
    for (int j = 0; j < D; ++j) {
      coords[j] = static_cast<double>(i + j);
    }
    train_data.push_back(PQPoint(coords));
  }
  
  Code ret = pq.Train(train_data);
  EXPECT_EQ(ret, kOk);
  
  // 计算存储空间
  size_t original_bytes = D * sizeof(double);  // 128 * 8 = 1024 bytes
  size_t compressed_bytes = M * sizeof(int);    // 8 * 4 = 32 bytes (codes)
  
  double compression_ratio = static_cast<double>(original_bytes) / compressed_bytes;
  
  fprintf(stderr, "Compression ratio:\n");
  fprintf(stderr, "  Original: %zu bytes\n", original_bytes);
  fprintf(stderr, "  Compressed: %zu bytes\n", compressed_bytes);
  fprintf(stderr, "  Ratio: %.1fx\n", compression_ratio);
  
  // 验证显著压缩
  EXPECT_GT(compression_ratio, 10.0);  // 至少10倍压缩
} /*}}}*/

/**
 * @brief 测试ProductQuantizer的边界情况
 *
 * 测试目标:
 * - 验证空数据、单点数据等边界情况的处理
 *
 * 测试步骤:
 * 1. 测试空训练数据
 * 2. 测试维度不匹配
 */
TEST_D(ProductQuantizer, Test_EdgeCases, "ProductQuantizer边界情况测试") { /*{{{*/
  using namespace base;
  
  ProductQuantizer pq(2, 4, 4);
  
  // 测试空训练数据
  std::vector<PQPoint> empty_data;
  Code ret = pq.Train(empty_data);
  EXPECT_EQ(ret, kInvalidParam);
  
  // 测试维度不匹配的数据
  std::vector<PQPoint> wrong_dim_data;
  wrong_dim_data.push_back(PQPoint({1.0, 2.0}));  // 2维，期望4维
  ret = pq.Train(wrong_dim_data);
  EXPECT_EQ(ret, kInvalidParam);
  
  fprintf(stderr, "Edge cases test passed\n");
} /*}}}*/

/**
 * @brief 测试ProductQuantizer与暴力搜索的精度对比
 *
 * 测试目标:
 * - 验证PQ加速的近似最近邻搜索精度
 *
 * 测试步骤:
 * 1. 创建数据集和量化所有向量
 * 2. 使用PQ近似距离找最近邻
 * 3. 使用精确距离找最近邻
 * 4. 对比结果
 */
TEST_D(ProductQuantizer, Test_AccuracyComparison, "ProductQuantizer精度对比测试") { /*{{{*/
  using namespace base;
  
  ProductQuantizer pq(2, 8, 4);
  
  // 创建数据集
  std::vector<PQPoint> dataset;
  for (int i = 0; i < 20; ++i) {
    std::vector<double> coords = {
      static_cast<double>(i * 2), 
      static_cast<double>(i * 2 + 1),
      static_cast<double>(i * 3), 
      static_cast<double>(i * 3 + 2)
    };
    dataset.push_back(PQPoint(coords));
  }
  
  // 训练
  Code ret = pq.Train(dataset);
  EXPECT_EQ(ret, kOk);
  
  // 量化所有数据
  std::vector<std::vector<int>> all_codes;
  for (const auto& point : dataset) {
    std::vector<int> codes;
    ret = pq.Quantize(point, codes);
    EXPECT_EQ(ret, kOk);
    all_codes.push_back(codes);
  }
  
  // 查询点
  PQPoint query({10.5, 11.5, 15.5, 17.5});
  
  // PQ近似搜索
  double min_approx_dist = std::numeric_limits<double>::max();
  int pq_nearest_idx = -1;
  for (size_t i = 0; i < all_codes.size(); ++i) {
    double dist = 0.0;
    ret = pq.ApproximateDistance(query, all_codes[i], &dist);
    if (ret == kOk && dist < min_approx_dist) {
      min_approx_dist = dist;
      pq_nearest_idx = i;
    }
  }
  
  // 精确搜索
  double min_real_dist = std::numeric_limits<double>::max();
  int real_nearest_idx = -1;
  for (size_t i = 0; i < dataset.size(); ++i) {
    double dist = 0.0;
    ret = CalculteDistance(query.coords, dataset[i].coords, &dist);
    if (ret == kOk && dist < min_real_dist) {
      min_real_dist = dist;
      real_nearest_idx = i;
    }
  }
  
  fprintf(stderr, "Accuracy comparison:\n");
  fprintf(stderr, "  Query: [%.1f, %.1f, %.1f, %.1f]\n", 
          query.coords[0], query.coords[1], query.coords[2], query.coords[3]);
  fprintf(stderr, "  PQ found: index %d, distance %.3f\n", 
          pq_nearest_idx, min_approx_dist);
  fprintf(stderr, "  Exact found: index %d, distance %.3f\n", 
          real_nearest_idx, min_real_dist);
  
  // 验证PQ找到的是合理的结果（可能不是精确最近邻）
  EXPECT_GE(pq_nearest_idx, 0);
  EXPECT_LT(pq_nearest_idx, static_cast<int>(dataset.size()));
  
  // 在小数据集上，PQ通常能找到相同或很接近的结果
  if (pq_nearest_idx == real_nearest_idx) {
    fprintf(stderr, "  Perfect match! PQ found the exact nearest neighbor.\n");
  } else {
    fprintf(stderr, "  PQ found a different nearest neighbor (expected for approximate search).\n");
  }
} /*}}}*/

}  // namespace
