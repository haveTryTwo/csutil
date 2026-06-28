// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/load_balancer.h"

#include <stdlib.h>

namespace base {

/**
 * @brief 轮询负载均衡：每次取游标对候选数取模，游标自增
 */
class RoundRobinLoadBalancer : public LoadBalancer { /*{{{*/
 public:
  RoundRobinLoadBalancer() : cursor_(0) {}

  virtual Code Pick(const std::vector<Endpoint> &candidates, const std::string &hash_key, Endpoint *out) {
    (void)hash_key;
    if (out == NULL) return kInvalidParam;
    if (candidates.empty()) return kNotFound;

    uint32_t idx = cursor_++ % (uint32_t)candidates.size();
    *out = candidates[idx];
    return kOk;
  }

 private:
  uint32_t cursor_;  // Pick 调用串行化于 ServiceEntry 锁内，无需原子
}; /*}}}*/

/**
 * @brief 随机负载均衡
 */
class RandomLoadBalancer : public LoadBalancer { /*{{{*/
 public:
  virtual Code Pick(const std::vector<Endpoint> &candidates, const std::string &hash_key, Endpoint *out) {
    (void)hash_key;
    if (out == NULL) return kInvalidParam;
    if (candidates.empty()) return kNotFound;

    uint32_t idx = (uint32_t)(rand() % (int)candidates.size());
    *out = candidates[idx];
    return kOk;
  }
}; /*}}}*/

/**
 * @brief 加权轮询：按 weight 累加，落在权重区间的实例被选中
 */
class WeightedLoadBalancer : public LoadBalancer { /*{{{*/
 public:
  WeightedLoadBalancer() : cursor_(0) {}

  virtual Code Pick(const std::vector<Endpoint> &candidates, const std::string &hash_key, Endpoint *out) {
    (void)hash_key;
    if (out == NULL) return kInvalidParam;
    if (candidates.empty()) return kNotFound;

    uint32_t total_weight = 0;
    for (uint32_t i = 0; i < (uint32_t)candidates.size(); ++i) {
      total_weight += (candidates[i].weight == 0 ? 1 : candidates[i].weight);
    }

    uint32_t pos = cursor_++ % total_weight;
    uint32_t acc = 0;
    for (uint32_t i = 0; i < (uint32_t)candidates.size(); ++i) {
      acc += (candidates[i].weight == 0 ? 1 : candidates[i].weight);
      if (pos < acc) {
        *out = candidates[i];
        return kOk;
      }
    }

    *out = candidates[0];
    return kOk;
  }

 private:
  uint32_t cursor_;
}; /*}}}*/

/**
 * @brief 哈希负载均衡：按 hash_key 取模选址（同 key 命中同实例，利于缓存亲和）
 *        P0 采用简单取模；一致性哈希 + 虚拟节点为后续增强
 */
class HashLoadBalancer : public LoadBalancer { /*{{{*/
 public:
  virtual Code Pick(const std::vector<Endpoint> &candidates, const std::string &hash_key, Endpoint *out) {
    if (out == NULL) return kInvalidParam;
    if (candidates.empty()) return kNotFound;

    uint64_t h = Hash(hash_key);
    uint32_t idx = (uint32_t)(h % (uint64_t)candidates.size());
    *out = candidates[idx];
    return kOk;
  }

 private:
  /**
   * @brief FNV-1a 64 位哈希
   */
  static uint64_t Hash(const std::string &key) {
    uint64_t h = 14695981039346656037ULL;
    for (uint32_t i = 0; i < (uint32_t)key.size(); ++i) {
      h ^= (uint64_t)(unsigned char)key[i];
      h *= 1099511628211ULL;
    }
    return h;
  }
}; /*}}}*/

LoadBalancer *CreateLoadBalancer(LbStrategy strategy) { /*{{{*/
  switch (strategy) {
    case kLbRandom:
      return new RandomLoadBalancer();
    case kLbWeight:
      return new WeightedLoadBalancer();
    case kLbHash:
      return new HashLoadBalancer();
    case kLbRoundRobin:
    default:
      return new RoundRobinLoadBalancer();
  }
} /*}}}*/

}  // namespace base
