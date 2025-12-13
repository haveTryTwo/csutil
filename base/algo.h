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
static uint64_t SplitMix64(uint64_t x) { /*{{{*/
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
} /*}}}*/

// RHO: count leading zeros in a "bits_rem" wide word, return position (1..bits_rem+1)
static inline uint8_t RHO(uint64_t w, uint8_t bits_rem) { /*{{{*/
  // 边界检查
  if (bits_rem == 0 || bits_rem > 64) return 1;
  if (w == 0) return bits_rem + 1;

#if defined(__GNUG__) || defined(__clang__)
  // __builtin_clzll counts leading zeros in 64-bit value
  int leading_zeros = __builtin_clzll(w);
  int result = leading_zeros - (64 - bits_rem) + 1;

  // 确保结果在有效范围内
  if (result < 1) return 1;
  if (result > bits_rem + 1) return bits_rem + 1;

  return static_cast<uint8_t>(result);
#else
  uint8_t cnt = 1;
  if (bits_rem == 0) return 1;

  uint64_t mask = (uint64_t)1 << (bits_rem - 1);
  while ((w & mask) == 0 && cnt <= bits_rem) {
    cnt++;
    if (bits_rem == 1) break;  // 防止mask变为0
    mask >>= 1;
  }
  return cnt;
#endif
} /*}}}*/

// simplified AlphaM
static double AlphaM(size_t m) { /*{{{*/
  if (m == 16) return 0.673;
  if (m == 32) return 0.697;
  if (m == 64) return 0.709;
  return 0.7213 / (1.0 + 1.079 / (double)m);
} /*}}}*/

class HyperLogLog {/*{{{*/
 public:
  explicit HyperLogLog(unsigned p = 14) : p_(p), m_(0), bits_rem_(0), dense_(false), init_(false) {
    // 延迟初始化，在Init()中进行参数验证和内存分配
  }

  // 禁用拷贝构造和赋值，避免意外的大内存拷贝
  HyperLogLog(const HyperLogLog &) = delete;
  HyperLogLog &operator=(const HyperLogLog &) = delete;

  // 支持移动语义
  HyperLogLog(HyperLogLog &&other) noexcept
      : p_(other.p_),
        m_(other.m_),
        bits_rem_(other.bits_rem_),
        dense_(other.dense_),
        init_(other.init_),
        sparse_(std::move(other.sparse_)),
        regs_dense_(std::move(other.regs_dense_)) {
    other.init_ = false;
  }

  HyperLogLog &operator=(HyperLogLog &&other) noexcept {
    if (this != &other) {
      p_ = other.p_;
      m_ = other.m_;
      bits_rem_ = other.bits_rem_;
      dense_ = other.dense_;
      init_ = other.init_;
      sparse_ = std::move(other.sparse_);
      regs_dense_ = std::move(other.regs_dense_);
      other.init_ = false;
    }
    return *this;
  }

  // 显式析构函数
  ~HyperLogLog() {
    // std::vector会自动清理，但我们可以显式清理以释放内存
    sparse_.clear();
    sparse_.shrink_to_fit();
    regs_dense_.clear();
    regs_dense_.shrink_to_fit();
  }

  Code Init() { /*{{{*/
    if (p_ < 4 || p_ > 31) {
      return kInvalidParam;
    }

    // 检查是否会导致过大的内存分配
    if (p_ > 20) {  // 2^20 = 1MB，合理的上限
      return kInvalidParam;
    }

    m_ = 1u << p_;
    bits_rem_ = 64 - p_;

    // 检查内存分配大小是否合理
    if (m_ == 0 || m_ > (1u << 20)) {  // 防止溢出或过大分配
      return kInvalidParam;
    }

    // 安全地分配dense数组
    regs_dense_.clear();

    // 预先检查是否有足够内存（简单估算）
    size_t required_memory = m_ * sizeof(uint8_t);
    if (required_memory > 100 * 1024 * 1024) {  // 100MB上限
      return kMallocFailed;
    }

    // 分步骤分配，便于检测失败
    regs_dense_.reserve(m_);
    if (regs_dense_.capacity() < m_) {
      return kMallocFailed;
    }

    regs_dense_.resize(m_, 0);
    if (regs_dense_.size() != m_) {
      regs_dense_.clear();
      return kMallocFailed;
    }

    // 清空sparse数组
    sparse_.clear();
    sparse_.shrink_to_fit();

    init_ = true;
    return kOk;
  } /*}}}*/

  Code AddHash(uint64_t h) { /*{{{*/
    if (!init_) return kNotInit;

    uint32_t idx = static_cast<uint32_t>(h & (m_ - 1));
    uint64_t w = h >> p_;
    uint8_t r = RHO(w, bits_rem_);

    if (dense_) {
      // 边界检查
      if (idx >= regs_dense_.size()) return kInternalError;
      if (r > regs_dense_[idx]) regs_dense_[idx] = r;
    } else {
      // 检查sparse容器当前大小，防止过度增长
      if (sparse_.size() > m_) {
        return kInternalError;  // 异常情况，sparse不应该比dense更大
      }

      // sparse: keep sorted vector<pair<idx, val>>
      auto it = std::lower_bound(sparse_.begin(), sparse_.end(), idx,
                                 [](const std::pair<uint32_t, uint8_t> &a, uint32_t b) { return a.first < b; });
      if (it != sparse_.end() && it->first == idx) {
        if (r > it->second) it->second = r;
      } else {
        // 检查插入前的容量
        size_t old_size = sparse_.size();

        // 预先检查是否有足够空间
        if (sparse_.capacity() <= sparse_.size()) {
          size_t new_capacity = sparse_.capacity() * 2;
          if (new_capacity == 0) new_capacity = 1;

          // 检查新容量是否合理
          if (new_capacity > m_ * 2) {  // 不应该超过dense大小的两倍
            return kMallocFailed;
          }
        }

        sparse_.insert(it, {idx, r});

        // 验证插入是否成功
        if (sparse_.size() != old_size + 1) {
          return kMallocFailed;
        }
      }

      // 检查是否需要转换为dense模式
      if (SparseSizeBytes() > DenseThresholdBytes()) {
        Code ret = DensifyWithCheck();
        if (ret != kOk) return ret;
      }
    }

    return kOk;
  } /*}}}*/

  template <typename T>
  Code Add(const T &v) { /*{{{*/
    size_t hv = std::hash<T>{}(v);
    uint64_t x = static_cast<uint64_t>(hv);
    if (sizeof(size_t) < 8) {
      x = (x << 32) ^ (x << 17) ^ 0x9e3779b97f4a7c15ULL;
    }
    return AddHash(SplitMix64(x));
  } /*}}}*/

  /**
   * @brief 合并两个HyperLogLog对象
   * @param other 要合并的另一个HyperLogLog对象
   * @return 返回码：kOk成功，kNotInit未初始化，kInvalidParam参数无效
   */
  Code Merge(const HyperLogLog &other) { /*{{{*/
    Code ret = ValidateMergeParams(other);
    if (ret != kOk) return ret;

    if (!dense_ && !other.dense_) {
      return MergeSparseSparse(other);
    }
    return MergeToDense(other);
  } /*}}}*/

  double Estimate() const { /*{{{*/
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
  } /*}}}*/

  bool IsDense() const { return dense_; }
  size_t MemoryBytesEstimate() const { /*{{{*/
    if (dense_) return regs_dense_.size() * sizeof(uint8_t);
    return sparse_.size() * (sizeof(uint32_t) + sizeof(uint8_t)) + 64;
  } /*}}}*/

 private:
  size_t SparseSizeBytes() const { return sparse_.size() * (sizeof(uint32_t) + sizeof(uint8_t)); }
  size_t DenseThresholdBytes() const { return regs_dense_.size() * sizeof(uint8_t) / 2; }

  /**
   * @brief 验证Merge操作的参数
   * @param other 要合并的另一个HyperLogLog对象
   * @return kOk参数有效，kNotInit未初始化，kInvalidParam参数无效
   */
  Code ValidateMergeParams(const HyperLogLog &other) const { /*{{{*/
    if (!init_ || !other.init_) return kNotInit;
    if (other.p_ != p_) return kInvalidParam;
    return kOk;
  } /*}}}*/

  /**
   * @brief 合并两个sparse模式的HyperLogLog
   * @param other 要合并的另一个sparse模式的HyperLogLog对象
   * @return 返回码
   */
  Code MergeSparseSparse(const HyperLogLog &other) { /*{{{*/
    std::vector<std::pair<uint32_t, uint8_t>> out;
    size_t estimated_size = sparse_.size() + other.sparse_.size();

    if (estimated_size > m_ * 2) return kInvalidParam;

    out.reserve(estimated_size);
    if (out.capacity() < estimated_size) return kMallocFailed;

    Code ret = MergeSortedVectors(other.sparse_, out, estimated_size);
    if (ret != kOk) return ret;

    sparse_.swap(out);
    if (SparseSizeBytes() > DenseThresholdBytes()) Densify();
    return kOk;
  } /*}}}*/

  /**
   * @brief 合并两个已排序的sparse向量
   * @param other_sparse 另一个sparse向量
   * @param out 输出向量
   * @param estimated_size 预估大小
   * @return 返回码
   */
  Code MergeSortedVectors(const std::vector<std::pair<uint32_t, uint8_t>> &other_sparse,
                          std::vector<std::pair<uint32_t, uint8_t>> &out, size_t estimated_size) { /*{{{*/
    size_t i = 0, j = 0;
    while (i < sparse_.size() && j < other_sparse.size()) {
      if (sparse_[i].first < other_sparse[j].first) {
        out.push_back(sparse_[i++]);
      } else if (sparse_[i].first > other_sparse[j].first) {
        out.push_back(other_sparse[j++]);
      } else {
        out.push_back({sparse_[i].first, std::max(sparse_[i].second, other_sparse[j].second)});
        ++i;
        ++j;
      }
      if (out.size() > estimated_size) return kInternalError;
    }

    while (i < sparse_.size()) {
      out.push_back(sparse_[i++]);
      if (out.size() > estimated_size) return kInternalError;
    }

    while (j < other_sparse.size()) {
      out.push_back(other_sparse[j++]);
      if (out.size() > estimated_size) return kInternalError;
    }
    return kOk;
  } /*}}}*/

  /**
   * @brief 合并到dense模式
   * @param other 要合并的HyperLogLog对象
   * @return 返回码
   */
  Code MergeToDense(const HyperLogLog &other) { /*{{{*/
    if (!dense_) Densify();
    if (regs_dense_.size() != m_) return kInternalError;

    if (other.dense_) {
      return MergeDenseDense(other);
    }
    return MergeDenseSparse(other);
  } /*}}}*/

  /**
   * @brief 合并两个dense模式的HyperLogLog
   * @param other 要合并的dense模式的HyperLogLog对象
   * @return 返回码
   */
  Code MergeDenseDense(const HyperLogLog &other) { /*{{{*/
    if (other.regs_dense_.size() != m_) return kInvalidParam;
    for (size_t i = 0; i < m_; ++i) {
      regs_dense_[i] = std::max(regs_dense_[i], other.regs_dense_[i]);
    }
    return kOk;
  } /*}}}*/

  /**
   * @brief 将sparse模式的other合并到当前dense模式
   * @param other 要合并的sparse模式的HyperLogLog对象
   * @return 返回码
   */
  Code MergeDenseSparse(const HyperLogLog &other) { /*{{{*/
    for (const auto &kv : other.sparse_) {
      if (kv.first < m_) {
        regs_dense_[kv.first] = std::max(regs_dense_[kv.first], kv.second);
      }
    }
    return kOk;
  } /*}}}*/

  Code DensifyWithCheck() { /*{{{*/
    if (dense_) return kOk;
    if (!init_) return kNotInit;

    // 检查regs_dense_数组是否已正确初始化
    if (regs_dense_.size() != m_) {
      return kInternalError;
    }

    // 预先检查sparse数组中的所有索引是否有效
    for (const auto &kv : sparse_) {
      if (kv.first >= regs_dense_.size()) {
        return kInternalError;  // 索引越界
      }
    }

    // 执行densify操作
    for (const auto &kv : sparse_) {
      // 此时已确保索引有效，但仍保留检查以防万一
      if (kv.first < regs_dense_.size()) {
        regs_dense_[kv.first] = std::max(regs_dense_[kv.first], kv.second);
      }
    }

    // 清理sparse数据
    sparse_.clear();
    sparse_.shrink_to_fit();
    dense_ = true;

    return kOk;
  } /*}}}*/

  void Densify() { /*{{{*/
    if (dense_) return;
    for (const auto &kv : sparse_) {
      if (kv.first < regs_dense_.size()) {  // 添加边界检查
        regs_dense_[kv.first] = std::max(regs_dense_[kv.first], kv.second);
      }
    }
    sparse_.clear();
    sparse_.shrink_to_fit();
    dense_ = true;
  } /*}}}*/

 private:
  unsigned p_;
  size_t m_;
  uint8_t bits_rem_;
  bool dense_;
  bool init_ = false;

  std::vector<std::pair<uint32_t, uint8_t>> sparse_;
  std::vector<uint8_t> regs_dense_;
};/*}}}*/

}  // namespace base

#endif
