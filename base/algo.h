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
// encoded foramt: {start, length}
Code RunLengthEncode(const std::vector<uint32_t> &input, std::vector<std::pair<uint32_t, uint32_t>> *encoded);
Code RunLengthDecode(const std::vector<std::pair<uint32_t, uint32_t>> &encoded, std::vector<uint32_t> *decoded);


// SplitMix64 for hashing
static uint64_t SplitMix64(uint64_t x) {/*{{{*/
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}/*}}}*/

// RHO: count leading zeros in a "bits_rem" wide word, return position (1..bits_rem+1)
static inline uint8_t RHO(uint64_t w, uint8_t bits_rem) {/*{{{*/
  if (w == 0) return bits_rem + 1;
#if defined(__GNUG__) || defined(__clang__)
  // __builtin_clzll counts leading zeros in 64-bit value
  return static_cast<uint8_t>((__builtin_clzll(w) - (64 - bits_rem) + 1));
#else
  uint8_t cnt = 1;
  uint64_t mask = (uint64_t)1 << (bits_rem - 1);
  while ((w & mask) == 0) {
    cnt++;
    mask >>= 1;
  }
  return cnt;
#endif
}/*}}}*/

// simplified AlphaM
static double AlphaM(size_t m) {/*{{{*/
  if (m == 16) return 0.673;
  if (m == 32) return 0.697;
  if (m == 64) return 0.709;
  return 0.7213 / (1.0 + 1.079 / (double)m);
}/*}}}*/

class HyperLogLog {
 public:
  explicit HyperLogLog(unsigned p = 14) : p_(p), m_(1u << p), bits_rem_(64 - p), dense_(false), init_(false), regs_dense_(m_, 0) {
  }

  Code Init() {/*{{{*/
    if (p_ < 4 || p_ > 31) {
      return kInvalidParam;
    }

    init_ = true;
    return kOk;
  }/*}}}*/

  Code AddHash(uint64_t h) {/*{{{*/
    if (!init_) return kNotInit;
    uint32_t idx = static_cast<uint32_t>(h & (m_ - 1));
    uint64_t w = h >> p_;
    uint8_t r = RHO(w, bits_rem_);
    if (dense_) {
      if (r > regs_dense_[idx]) regs_dense_[idx] = r;
    } else {
      // sparse: keep sorted vector<pair<idx, val>>
      auto it = std::lower_bound(sparse_.begin(), sparse_.end(), idx,
                                 [](const std::pair<uint32_t, uint8_t> &a, uint32_t b) { return a.first < b; });
      if (it != sparse_.end() && it->first == idx) {
        if (r > it->second) it->second = r;
      } else {
        sparse_.insert(it, {idx, r});
      }
      if (SparseSizeBytes() > DenseThresholdBytes()) Densify();
    }

    return kOk;
  }/*}}}*/

  template <typename T>
  Code Add(const T &v) {/*{{{*/
    size_t hv = std::hash<T>{}(v);
    uint64_t x = static_cast<uint64_t>(hv);
    if (sizeof(size_t) < 8) {
      x = (x << 32) ^ (x << 17) ^ 0x9e3779b97f4a7c15ULL;
    }
    return AddHash(SplitMix64(x));
  }/*}}}*/

  Code Merge(const HyperLogLog &other) {/*{{{*/
    if (other.p_ != p_) {
      return kInvalidParam;
    }
    if (!dense_ && !other.dense_) {
      std::vector<std::pair<uint32_t, uint8_t>> out;
      out.reserve(std::max(sparse_.size(), other.sparse_.size()));
      size_t i = 0, j = 0;
      while (i < sparse_.size() && j < other.sparse_.size()) {
        if (sparse_[i].first < other.sparse_[j].first) {
          out.push_back(sparse_[i++]);
        } else if (sparse_[i].first > other.sparse_[j].first) {
          out.push_back(other.sparse_[j++]);
        } else {
          out.push_back({sparse_[i].first, std::max(sparse_[i].second, other.sparse_[j].second)});
          ++i;
          ++j;
        }
      }
      while (i < sparse_.size()) out.push_back(sparse_[i++]);
      while (j < other.sparse_.size()) out.push_back(other.sparse_[j++]);
      sparse_.swap(out);
      if (SparseSizeBytes() > DenseThresholdBytes()) Densify();
    } else {
      if (!dense_) Densify();
      if (other.dense_) {
        for (size_t i = 0; i < m_; ++i) regs_dense_[i] = std::max(regs_dense_[i], other.regs_dense_[i]);
      } else {
        for (auto &kv : other.sparse_) regs_dense_[kv.first] = std::max(regs_dense_[kv.first], kv.second);
      }
    }

    return kOk;
  }/*}}}*/

  double Estimate() const {/*{{{*/
    if (!dense_) {
      size_t zeros = m_;
      double sum = 0.0;
      for (auto &kv : sparse_) --zeros;
      sum = static_cast<double>(zeros) * 1.0;
      for (auto &kv : sparse_) sum += std::pow(2.0, -static_cast<double>(kv.second));
      double E = AlphaM(m_) * static_cast<double>(m_) * static_cast<double>(m_) / sum;
      if (E <= 2.5 * m_) {
        if (zeros > 0)
          return static_cast<double>(m_) * std::log(static_cast<double>(m_) / static_cast<double>(zeros));
        else
          return E;
      }
      return E;
    } else {
      double sum = 0.0;
      size_t zeros = 0;
      for (size_t i = 0; i < m_; ++i) {
        sum += std::pow(2.0, -static_cast<double>(regs_dense_[i]));
        if (regs_dense_[i] == 0) ++zeros;
      }
      double E = AlphaM(m_) * static_cast<double>(m_) * static_cast<double>(m_) / sum;
      if (E <= 2.5 * m_) {
        if (zeros > 0)
          return static_cast<double>(m_) * std::log(static_cast<double>(m_) / static_cast<double>(zeros));
        else
          return E;
      }
      return E;
    }
  }/*}}}*/

  bool IsDense() const { return dense_; }
  size_t MemoryBytesEstimate() const {/*{{{*/
    if (dense_) return regs_dense_.size() * sizeof(uint8_t);
    return sparse_.size() * (sizeof(uint32_t) + sizeof(uint8_t)) + 64;
  }/*}}}*/

 private:
  size_t SparseSizeBytes() const { return sparse_.size() * (sizeof(uint32_t) + sizeof(uint8_t)); }
  size_t DenseThresholdBytes() const { return regs_dense_.size() * sizeof(uint8_t) / 2; }

  void Densify() {/*{{{*/
    if (dense_) return;
    for (auto &kv : sparse_) regs_dense_[kv.first] = std::max(regs_dense_[kv.first], kv.second);
    sparse_.clear();
    sparse_.shrink_to_fit();
    dense_ = true;
  }/*}}}*/

 private:
  unsigned p_;
  size_t m_;
  uint8_t bits_rem_;
  bool dense_;
  bool init_ = false;

  std::vector<std::pair<uint32_t, uint8_t>> sparse_;
  std::vector<uint8_t> regs_dense_;
};

}  // namespace base

#endif
