// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALGO_H_
#define BASE_ALGO_H_

#include <stdint.h>

#include <functional>
#include <list>
#include <utility>
#include <vector>

#include "base/common.h"
#include "base/status.h"

namespace base {

// Note: Longest common sequence
Code LCS(const std::string &seq_first, const std::string &seq_second, std::string *lcs_str);

// Note: Longest common substring
Code LCSS(const std::string &seq_first, const std::string &seq_second, std::string *lcss_str);

// NOTE:htt, 验证是否为质数
Code IsPrime(uint64_t num, bool *is_prime);

Code CalculteDistance(const std::vector<double> &first_point, const std::vector<double> &second_point,
                      double *distance);

// kNN function: https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm
Code kNN(const std::vector<std::vector<double>> &points, const std::vector<double> query_point, int k,
         std::vector<std::vector<double>> *neighbors);

// NOTE:htt, https://en.wikipedia.org/wiki/Moving_average
class ExponentialMovingAverage {
 public:
  ExponentialMovingAverage(uint32_t n) : n_(n), alpha_(2.0 / (n_ + 1)), ema_(0), is_initialized_(false) {}

  // NOTE:htt, 更新EMA值的函数
  double Update(double new_value);

  // 获取当前EMA值
  double GetEma() const { return ema_; }

 private:
  uint32_t n_;           // NOTE:htt,
  double alpha_;         // NOTE:htt, 平滑因子
  double ema_;           // NOTE:htt, 当前的EMA值
  bool is_initialized_;  // NOTE:htt, 判断EMA是否已经初始化
};

// NOTE:htt, 时间轮算法是一种高效的定时器管理算法,常用于操作系统和网络系统中处理大量定时任务
// 算法通过一个循环的轮式数据结构来组织和调度定时事件,使得定时任务的添加、删除和到期处理都能
// 在常数时间内完成,即 O(1) 的时间复杂度
class TimerWheel {
 public:
  TimerWheel(uint32_t size);
  ~TimerWheel();

  void AddTimer(uint32_t timeout, std::function<void()> task);
  void Tick();

 private:
  uint32_t size_;                                        // 时间轮的大小
  int current_slot_;                                     // 当前槽的索引
  std::vector<std::list<std::function<void()>>> wheel_;  // 时间轮的数据结构
};

// NOTE:htt, Run-Length Encoding (RLE) based on auto-incrementing sequences is often used to optimize the storage of
// numeric sequences, especially when the sequence contains consecutive numbers. This approach is very useful in data
// structures like RoaringBitmap because it can efficiently compress consecutive sequences of integers.
Code RunLengthEncode(const std::vector<uint32_t> &input, std::vector<std::pair<uint32_t, uint32_t>> *encoded);
Code RunLengthDecode(const std::vector<std::pair<uint32_t, uint32_t>> &encoded, std::vector<uint32_t> *decoded);

}  // namespace base

#endif
