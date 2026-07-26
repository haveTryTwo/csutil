// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "base/common.h"
#include "base/elevator.h"
#include "base/status.h"
#include "base/time.h"

#include "test_base/include/test_base.h"

// ============================================================================
// FCFS 正常测试
// ============================================================================

TEST_D(Elevator, FCFS_Normal_BasicSequence, "FCFS基本序列测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorFCFS(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);

  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());
  for (uint32_t i = 0; i < requests.size(); ++i) {
    EXPECT_EQ(requests[i], result.serve_order[i]);
  }

  // 手动验证总距离：|50-82|+|82-170|+|170-43|+|43-140|+|140-24|+|24-16|+|16-190|
  // = 32 + 88 + 127 + 97 + 116 + 8 + 174 = 642
  EXPECT_EQ((uint32_t)642, result.total_distance);

  fprintf(stderr, "FCFS: total_distance=%u, serve_order_size=%u\n",
          result.total_distance, (uint32_t)result.serve_order.size());
} /*}}}*/

TEST_D(Elevator, FCFS_Normal_SingleRequest, "FCFS单个请求测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 10;
  std::vector<uint32_t> requests = {50};
  ElevatorResult result;

  Code ret = ElevatorFCFS(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)1, (uint32_t)result.serve_order.size());
  EXPECT_EQ((uint32_t)50, result.serve_order[0]);
  EXPECT_EQ((uint32_t)40, result.total_distance);
} /*}}}*/

TEST_D(Elevator, FCFS_Normal_SameFloor, "FCFS请求与当前楼层相同") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 5;
  std::vector<uint32_t> requests = {5, 5, 5};
  ElevatorResult result;

  Code ret = ElevatorFCFS(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)0, result.total_distance);
} /*}}}*/

// ============================================================================
// SSTF 正常测试
// ============================================================================

TEST_D(Elevator, SSTF_Normal_BasicSequence, "SSTF基本序列测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorSSTF(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // SSTF 应选择最近的楼层：50->43->24->16->82->140->170->190
  EXPECT_EQ((uint32_t)43, result.serve_order[0]);
  EXPECT_EQ((uint32_t)24, result.serve_order[1]);
  EXPECT_EQ((uint32_t)16, result.serve_order[2]);
  EXPECT_EQ((uint32_t)82, result.serve_order[3]);
  EXPECT_EQ((uint32_t)140, result.serve_order[4]);
  EXPECT_EQ((uint32_t)170, result.serve_order[5]);
  EXPECT_EQ((uint32_t)190, result.serve_order[6]);

  // 总距离: |50-43|+|43-24|+|24-16|+|16-82|+|82-140|+|140-170|+|170-190|
  // = 7+19+8+66+58+30+20 = 208
  EXPECT_EQ((uint32_t)208, result.total_distance);

  fprintf(stderr, "SSTF: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, SSTF_Normal_AllSameDistance, "SSTF所有请求距离相同") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {40, 60};
  ElevatorResult result;

  Code ret = ElevatorSSTF(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)2, (uint32_t)result.serve_order.size());
  // 距离相同取先出现的：40 先于 60
  EXPECT_EQ((uint32_t)40, result.serve_order[0]);
  EXPECT_EQ((uint32_t)60, result.serve_order[1]);
  // 总距离: |50-40|+|40-60| = 10+20 = 30
  EXPECT_EQ((uint32_t)30, result.total_distance);
} /*}}}*/

// ============================================================================
// SCAN 正常测试
// ============================================================================

TEST_D(Elevator, SCAN_Normal_GoingUp, "SCAN向上运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  uint32_t max_floor = 199;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向上: 50->82->140->170->190->(199)->43->24->16
  EXPECT_EQ((uint32_t)82, result.serve_order[0]);
  EXPECT_EQ((uint32_t)140, result.serve_order[1]);
  EXPECT_EQ((uint32_t)170, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)43, result.serve_order[4]);
  EXPECT_EQ((uint32_t)24, result.serve_order[5]);
  EXPECT_EQ((uint32_t)16, result.serve_order[6]);

  // 总距离: (199-50) + (199-16) = 149 + 183 = 332
  EXPECT_EQ((uint32_t)332, result.total_distance);

  fprintf(stderr, "SCAN Up: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, SCAN_Normal_GoingDown, "SCAN向下运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  uint32_t max_floor = 199;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorSCAN(current_floor, kElevatorDown, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向下: 50->43->24->16->(0)->82->140->170->190
  EXPECT_EQ((uint32_t)43, result.serve_order[0]);
  EXPECT_EQ((uint32_t)24, result.serve_order[1]);
  EXPECT_EQ((uint32_t)16, result.serve_order[2]);
  EXPECT_EQ((uint32_t)82, result.serve_order[3]);
  EXPECT_EQ((uint32_t)140, result.serve_order[4]);
  EXPECT_EQ((uint32_t)170, result.serve_order[5]);
  EXPECT_EQ((uint32_t)190, result.serve_order[6]);

  // 总距离: (50-0) + (190-0) = 50 + 190 = 240
  EXPECT_EQ((uint32_t)240, result.total_distance);

  fprintf(stderr, "SCAN Down: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, SCAN_Normal_AllOneDirection, "SCAN所有请求在同一方向") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 10;
  uint32_t max_floor = 100;
  std::vector<uint32_t> requests = {20, 30, 50};
  ElevatorResult result;

  Code ret = ElevatorSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)20, result.serve_order[0]);
  EXPECT_EQ((uint32_t)30, result.serve_order[1]);
  EXPECT_EQ((uint32_t)50, result.serve_order[2]);
  // 总距离: 50 - 10 = 40（不需要反向）
  EXPECT_EQ((uint32_t)40, result.total_distance);
} /*}}}*/

// ============================================================================
// LOOK 正常测试
// ============================================================================

TEST_D(Elevator, LOOK_Normal_GoingUp, "LOOK向上运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向上: 50->82->140->170->190(反向)->43->24->16
  EXPECT_EQ((uint32_t)82, result.serve_order[0]);
  EXPECT_EQ((uint32_t)140, result.serve_order[1]);
  EXPECT_EQ((uint32_t)170, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)43, result.serve_order[4]);
  EXPECT_EQ((uint32_t)24, result.serve_order[5]);
  EXPECT_EQ((uint32_t)16, result.serve_order[6]);

  // 总距离: (190-50) + (190-16) = 140 + 174 = 314
  EXPECT_EQ((uint32_t)314, result.total_distance);

  fprintf(stderr, "LOOK Up: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, LOOK_Normal_GoingDown, "LOOK向下运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorLOOK(current_floor, kElevatorDown, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向下: 50->43->24->16(反向)->82->140->170->190
  EXPECT_EQ((uint32_t)43, result.serve_order[0]);
  EXPECT_EQ((uint32_t)24, result.serve_order[1]);
  EXPECT_EQ((uint32_t)16, result.serve_order[2]);
  EXPECT_EQ((uint32_t)82, result.serve_order[3]);
  EXPECT_EQ((uint32_t)140, result.serve_order[4]);
  EXPECT_EQ((uint32_t)170, result.serve_order[5]);
  EXPECT_EQ((uint32_t)190, result.serve_order[6]);

  // 总距离: (50-16) + (190-16) = 34 + 174 = 208
  EXPECT_EQ((uint32_t)208, result.total_distance);

  fprintf(stderr, "LOOK Down: total_distance=%u\n", result.total_distance);
} /*}}}*/

// ============================================================================
// C-SCAN 正常测试
// ============================================================================

TEST_D(Elevator, CSCAN_Normal_GoingUp, "C-SCAN向上运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  uint32_t max_floor = 199;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorCSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向上: 50->82->140->170->190->(跳到0)->16->24->43
  EXPECT_EQ((uint32_t)82, result.serve_order[0]);
  EXPECT_EQ((uint32_t)140, result.serve_order[1]);
  EXPECT_EQ((uint32_t)170, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)16, result.serve_order[4]);
  EXPECT_EQ((uint32_t)24, result.serve_order[5]);
  EXPECT_EQ((uint32_t)43, result.serve_order[6]);

  // 总距离: (190-50) + (190-199)跳转 + (16-0)+(24-16)+(43-24)
  // = 140 + 9 + 16+8+19 = 192
  uint32_t expected_dist = (190 - 50) + (199 - 190) + (16 - 0) + (24 - 16) + (43 - 24);
  EXPECT_EQ(expected_dist, result.total_distance);

  fprintf(stderr, "C-SCAN Up: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, CSCAN_Normal_GoingDown, "C-SCAN向下运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  uint32_t max_floor = 199;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorCSCAN(current_floor, kElevatorDown, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向下: 50->43->24->16->(跳到max_floor)->190->170->140->82
  EXPECT_EQ((uint32_t)43, result.serve_order[0]);
  EXPECT_EQ((uint32_t)24, result.serve_order[1]);
  EXPECT_EQ((uint32_t)16, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)170, result.serve_order[4]);
  EXPECT_EQ((uint32_t)140, result.serve_order[5]);
  EXPECT_EQ((uint32_t)82, result.serve_order[6]);

  // 总距离: (50-43)+(43-24)+(24-16)+(16-0)跳转+(199-190)+(190-170)+(170-140)+(140-82)
  uint32_t expected_dist = (50 - 16) + 16 + (199 - 190) + (190 - 82);
  EXPECT_EQ(expected_dist, result.total_distance);

  fprintf(stderr, "C-SCAN Down: total_distance=%u\n", result.total_distance);
} /*}}}*/

// ============================================================================
// C-LOOK 正常测试
// ============================================================================

TEST_D(Elevator, CLOOK_Normal_GoingUp, "C-LOOK向上运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorCLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向上: 50->82->140->170->190->(跳到16)->16->24->43
  EXPECT_EQ((uint32_t)82, result.serve_order[0]);
  EXPECT_EQ((uint32_t)140, result.serve_order[1]);
  EXPECT_EQ((uint32_t)170, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)16, result.serve_order[4]);
  EXPECT_EQ((uint32_t)24, result.serve_order[5]);
  EXPECT_EQ((uint32_t)43, result.serve_order[6]);

  // 总距离: (190-50) + 0(跳转) + (24-16)+(43-24) = 140 + 0 + 8 + 19 = 167
  uint32_t expected_dist = (190 - 50) + (24 - 16) + (43 - 24);
  EXPECT_EQ(expected_dist, result.total_distance);

  fprintf(stderr, "C-LOOK Up: total_distance=%u\n", result.total_distance);
} /*}}}*/

TEST_D(Elevator, CLOOK_Normal_GoingDown, "C-LOOK向下运行测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorCLOOK(current_floor, kElevatorDown, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)requests.size(), (uint32_t)result.serve_order.size());

  // 向下: 50->43->24->16->(跳到190)->190->170->140->82
  EXPECT_EQ((uint32_t)43, result.serve_order[0]);
  EXPECT_EQ((uint32_t)24, result.serve_order[1]);
  EXPECT_EQ((uint32_t)16, result.serve_order[2]);
  EXPECT_EQ((uint32_t)190, result.serve_order[3]);
  EXPECT_EQ((uint32_t)170, result.serve_order[4]);
  EXPECT_EQ((uint32_t)140, result.serve_order[5]);
  EXPECT_EQ((uint32_t)82, result.serve_order[6]);

  // 总距离: (50-16) + 0(跳转) + (190-170)+(170-140)+(140-82)
  // = 34 + 0 + 20+30+58 = 142
  uint32_t expected_dist = (50 - 16) + (190 - 82);
  EXPECT_EQ(expected_dist, result.total_distance);

  fprintf(stderr, "C-LOOK Down: total_distance=%u\n", result.total_distance);
} /*}}}*/

// ============================================================================
// 异常/边界测试
// ============================================================================

TEST_D(Elevator, Exception_NullResult, "所有算法的空指针参数测试") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> requests = {10, 20, 30};

  EXPECT_EQ(kInvalidParam, ElevatorFCFS(5, requests, NULL));
  EXPECT_EQ(kInvalidParam, ElevatorSSTF(5, requests, NULL));
  EXPECT_EQ(kInvalidParam, ElevatorSCAN(5, kElevatorUp, requests, 100, NULL));
  EXPECT_EQ(kInvalidParam, ElevatorLOOK(5, kElevatorUp, requests, NULL));
  EXPECT_EQ(kInvalidParam, ElevatorCSCAN(5, kElevatorUp, requests, 100, NULL));
  EXPECT_EQ(kInvalidParam, ElevatorCLOOK(5, kElevatorUp, requests, NULL));
} /*}}}*/

TEST_D(Elevator, Exception_EmptyRequests, "所有算法的空请求测试") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> empty_requests;
  ElevatorResult result;

  EXPECT_EQ(kOk, ElevatorFCFS(5, empty_requests, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());
  EXPECT_EQ((uint32_t)0, result.total_distance);

  EXPECT_EQ(kOk, ElevatorSSTF(5, empty_requests, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());

  EXPECT_EQ(kOk, ElevatorSCAN(5, kElevatorUp, empty_requests, 100, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());

  EXPECT_EQ(kOk, ElevatorLOOK(5, kElevatorUp, empty_requests, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());

  EXPECT_EQ(kOk, ElevatorCSCAN(5, kElevatorUp, empty_requests, 100, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());

  EXPECT_EQ(kOk, ElevatorCLOOK(5, kElevatorUp, empty_requests, &result));
  EXPECT_EQ((uint32_t)0, (uint32_t)result.serve_order.size());
} /*}}}*/

TEST_D(Elevator, Exception_CurrentFloorExceedsMax, "SCAN/C-SCAN当前楼层超过最大楼层测试") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> requests = {10, 20};
  ElevatorResult result;

  EXPECT_EQ(kInvalidParam, ElevatorSCAN(101, kElevatorUp, requests, 100, &result));
  EXPECT_EQ(kInvalidParam, ElevatorCSCAN(101, kElevatorUp, requests, 100, &result));
} /*}}}*/

TEST_D(Elevator, Exception_FloorZero, "起始楼层为0的边界测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 0;
  std::vector<uint32_t> requests = {5, 10, 15};
  ElevatorResult result;

  Code ret = ElevatorFCFS(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)15, result.total_distance);

  ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)15, result.total_distance);
} /*}}}*/

TEST_D(Elevator, Exception_DuplicateRequests, "包含重复楼层请求的测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {30, 30, 70, 70};
  ElevatorResult result;

  Code ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)4, (uint32_t)result.serve_order.size());

  // 向上: 50->70->70->30->30
  EXPECT_EQ((uint32_t)70, result.serve_order[0]);
  EXPECT_EQ((uint32_t)70, result.serve_order[1]);
  EXPECT_EQ((uint32_t)30, result.serve_order[2]);
  EXPECT_EQ((uint32_t)30, result.serve_order[3]);
  // 总距离: |50-70|+|70-70|+|70-30|+|30-30| = 20+0+40+0 = 60
  EXPECT_EQ((uint32_t)60, result.total_distance);
} /*}}}*/

TEST_D(Elevator, Exception_AllRequestsOnCurrentFloor, "所有请求都在当前楼层") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 25;
  std::vector<uint32_t> requests = {25, 25, 25};
  ElevatorResult result;

  Code ret = ElevatorSSTF(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)3, (uint32_t)result.serve_order.size());
  EXPECT_EQ((uint32_t)0, result.total_distance);

  ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ((uint32_t)0, result.total_distance);
} /*}}}*/

// ============================================================================
// 算法对比测试
// ============================================================================

TEST_D(Elevator, Compare_AllAlgorithms, "六种算法对比测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  uint32_t max_floor = 199;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;

  Code ret = ElevatorFCFS(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t fcfs_dist = result.total_distance;

  ret = ElevatorSSTF(current_floor, requests, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t sstf_dist = result.total_distance;

  ret = ElevatorSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t scan_dist = result.total_distance;

  ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t look_dist = result.total_distance;

  ret = ElevatorCSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t cscan_dist = result.total_distance;

  ret = ElevatorCLOOK(current_floor, kElevatorUp, requests, &result);
  EXPECT_EQ(kOk, ret);
  uint32_t clook_dist = result.total_distance;

  fprintf(stderr, "\n=== Elevator Algorithm Comparison ===\n");
  fprintf(stderr, "  Requests: ");
  for (uint32_t i = 0; i < requests.size(); ++i) {
    fprintf(stderr, "%u ", requests[i]);
  }
  fprintf(stderr, "\n  Current floor: %u, Max floor: %u\n", current_floor, max_floor);
  fprintf(stderr, "  FCFS   total distance: %u\n", fcfs_dist);
  fprintf(stderr, "  SSTF   total distance: %u\n", sstf_dist);
  fprintf(stderr, "  SCAN   total distance: %u\n", scan_dist);
  fprintf(stderr, "  LOOK   total distance: %u\n", look_dist);
  fprintf(stderr, "  C-SCAN total distance: %u\n", cscan_dist);
  fprintf(stderr, "  C-LOOK total distance: %u\n", clook_dist);
  fprintf(stderr, "=====================================\n\n");

  // SSTF 通常总距离最小
  EXPECT_GE(fcfs_dist, sstf_dist);
  // LOOK 应不大于 SCAN（LOOK 不需要跑到端点）
  EXPECT_GE(scan_dist, look_dist);
} /*}}}*/

// ============================================================================
// 压力/性能测试
// ============================================================================

TEST_D(Elevator, Press_LargeRequests, "大量请求压力测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 500;
  uint32_t max_floor = 999;
  uint32_t num_requests = 10000;
  std::vector<uint32_t> requests;
  requests.reserve(num_requests);
  srand(42);
  for (uint32_t i = 0; i < num_requests; ++i) {
    requests.push_back(rand() % (max_floor + 1));
  }

  ElevatorResult result;
  Time timer;

  timer.Begin();
  Code ret = ElevatorFCFS(current_floor, requests, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(num_requests, (uint32_t)result.serve_order.size());
  fprintf(stderr, "FCFS   (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();

  timer.Begin();
  ret = ElevatorSSTF(current_floor, requests, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "SSTF   (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();

  timer.Begin();
  ret = ElevatorSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "SCAN   (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();

  timer.Begin();
  ret = ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "LOOK   (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();

  timer.Begin();
  ret = ElevatorCSCAN(current_floor, kElevatorUp, requests, max_floor, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "C-SCAN (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();

  timer.Begin();
  ret = ElevatorCLOOK(current_floor, kElevatorUp, requests, &result);
  timer.End();
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "C-LOOK (%u requests): distance=%u, ", num_requests, result.total_distance);
  timer.PrintDiffTime();
} /*}}}*/

TEST_D(Elevator, Press_RepeatedExecution, "重复执行稳定性测试") { /*{{{*/
  using namespace base;

  uint32_t current_floor = 50;
  std::vector<uint32_t> requests = {82, 170, 43, 140, 24, 16, 190};
  ElevatorResult result;
  uint32_t repeat_count = 100000;

  Time timer;
  timer.Begin();
  for (uint32_t i = 0; i < repeat_count; ++i) {
    ElevatorLOOK(current_floor, kElevatorUp, requests, &result);
  }
  timer.End();

  EXPECT_EQ((uint32_t)314, result.total_distance);
  fprintf(stderr, "LOOK repeated %u times: ", repeat_count);
  timer.PrintDiffTime();
} /*}}}*/
