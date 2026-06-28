// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_LOAD_BALANCER_H_
#define SOCK_LOAD_BALANCER_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"
#include "sock/service_provider.h"

namespace base {

/**
 * @brief 负载均衡器抽象：从给定候选实例中选一个
 *
 * 候选列表由 ServiceManager 预先过滤（仅含健康/半开实例）后传入，
 * 故 LB 实现只负责"挑选"，不感知健康策略。
 */
class LoadBalancer { /*{{{*/
 public:
  virtual ~LoadBalancer() {}

  /**
   * @brief 从候选实例中选一个
   * @param candidates 可选实例（已由上层过滤，非空时所有元素均可被选中）
   * @param hash_key 哈希键（仅 hash 策略使用，其它策略忽略）
   * @param out 选中的实例（输出）
   * @return kOk 成功；kInvalidParam out 为空；kNotFound 候选为空
   */
  virtual Code Pick(const std::vector<Endpoint> &candidates, const std::string &hash_key, Endpoint *out) = 0;
}; /*}}}*/

/**
 * @brief 按策略创建负载均衡器（调用方负责 delete）
 * @param strategy 策略枚举
 * @return 新建的 LoadBalancer 指针；非法策略回退为轮询
 */
LoadBalancer *CreateLoadBalancer(LbStrategy strategy);

}  // namespace base
#endif
