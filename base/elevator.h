// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ELEVATOR_H_
#define BASE_ELEVATOR_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"

namespace base {

/**
 * 电梯调度算法模块
 *
 * 实现了经典的磁盘/电梯调度算法，用于模拟电梯对楼层请求的服务顺序。
 * 这些算法也广泛应用于磁盘 I/O 调度场景。
 *
 * 支持的算法：
 *   1. FCFS (First Come First Served)   - 先来先服务，按请求到达顺序依次服务
 *   2. SSTF (Shortest Seek Time First)  - 最短寻道时间优先，每次服务距离最近的请求
 *   3. SCAN (Elevator Algorithm)         - 扫描算法，沿一个方向扫描到顶/底再反向
 *   4. LOOK                              - 类似 SCAN，但在最后一个请求处反向，不到顶/底
 *   5. C-SCAN (Circular SCAN)            - 循环扫描，只单方向服务，到顶/底后跳到另一端
 *   6. C-LOOK (Circular LOOK)            - 循环 LOOK，只单方向服务，在最后请求处跳回
 *
 * 参考资料：
 *   - https://en.wikipedia.org/wiki/Elevator_algorithm
 *   - https://en.wikipedia.org/wiki/SCAN_(disk_scheduling)
 *   - https://www.geeksforgeeks.org/disk-scheduling-algorithms/
 */

/**
 * 电梯运行方向枚举
 */
enum ElevatorDirection {
  kElevatorUp = 0,    // 向上运行
  kElevatorDown = 1,  // 向下运行
};

/**
 * 电梯调度结果
 *
 * 包含调度算法计算出的楼层服务顺序和总移动距离。
 */
struct ElevatorResult {
  std::vector<uint32_t> serve_order;  // 楼层服务顺序（不含起始楼层）
  uint32_t total_distance;            // 电梯总移动楼层数
};

/**
 * FCFS (First Come First Served) 先来先服务算法
 *
 * 按请求到达的先后顺序依次服务。实现简单且公平，但总移动距离可能较大。
 * 时间复杂度: O(n)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param requests      楼层请求序列（按到达顺序排列）
 * @param result        输出调度结果，包含服务顺序和总移动距离
 * @return kOk 成功; kInvalidParam 参数无效（result 为空）
 */
Code ElevatorFCFS(uint32_t current_floor, const std::vector<uint32_t>& requests,
                  ElevatorResult* result);

/**
 * SSTF (Shortest Seek Time First) 最短寻道时间优先算法
 *
 * 每次选择距当前位置最近的楼层服务。总移动距离较小，但可能导致远端请求饥饿。
 * 时间复杂度: O(n^2)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param requests      楼层请求序列
 * @param result        输出调度结果
 * @return kOk 成功; kInvalidParam 参数无效
 */
Code ElevatorSSTF(uint32_t current_floor, const std::vector<uint32_t>& requests,
                  ElevatorResult* result);

/**
 * SCAN (Elevator Algorithm) 扫描/电梯算法
 *
 * 电梯沿当前方向移动到最顶层（max_floor）或最底层（0），沿途服务所有请求，
 * 到达端点后反向继续服务。类似电梯的真实运行方式。
 * 时间复杂度: O(n log n)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param direction     电梯当前运行方向
 * @param requests      楼层请求序列
 * @param max_floor     最高楼层编号
 * @param result        输出调度结果
 * @return kOk 成功; kInvalidParam 参数无效或 current_floor > max_floor
 */
Code ElevatorSCAN(uint32_t current_floor, ElevatorDirection direction,
                  const std::vector<uint32_t>& requests, uint32_t max_floor,
                  ElevatorResult* result);

/**
 * LOOK 算法
 *
 * 类似 SCAN，但电梯不会移动到最顶层或最底层，而是在该方向上最后一个请求处反向。
 * 减少了不必要的空跑距离。
 * 时间复杂度: O(n log n)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param direction     电梯当前运行方向
 * @param requests      楼层请求序列
 * @param result        输出调度结果
 * @return kOk 成功; kInvalidParam 参数无效
 */
Code ElevatorLOOK(uint32_t current_floor, ElevatorDirection direction,
                  const std::vector<uint32_t>& requests, ElevatorResult* result);

/**
 * C-SCAN (Circular SCAN) 循环扫描算法
 *
 * 电梯只在一个方向上服务请求，到达端点后直接跳到另一端重新开始。
 * 提供更均匀的等待时间。跳转距离不计入总移动距离。
 * 时间复杂度: O(n log n)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param direction     电梯服务方向
 * @param requests      楼层请求序列
 * @param max_floor     最高楼层编号
 * @param result        输出调度结果
 * @return kOk 成功; kInvalidParam 参数无效或 current_floor > max_floor
 */
Code ElevatorCSCAN(uint32_t current_floor, ElevatorDirection direction,
                   const std::vector<uint32_t>& requests, uint32_t max_floor,
                   ElevatorResult* result);

/**
 * C-LOOK (Circular LOOK) 循环 LOOK 算法
 *
 * 类似 C-SCAN，但不移动到端点，而是在最后一个请求处直接跳到另一端最近的请求。
 * 结合了 C-SCAN 的均匀性和 LOOK 的效率。跳转距离不计入总移动距离。
 * 时间复杂度: O(n log n)，空间复杂度: O(n)
 *
 * @param current_floor 电梯当前所在楼层
 * @param direction     电梯服务方向
 * @param requests      楼层请求序列
 * @param result        输出调度结果
 * @return kOk 成功; kInvalidParam 参数无效
 */
Code ElevatorCLOOK(uint32_t current_floor, ElevatorDirection direction,
                   const std::vector<uint32_t>& requests, ElevatorResult* result);

}  // namespace base

#endif
