// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_SERVICE_PROVIDER_H_
#define SOCK_SERVICE_PROVIDER_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"

namespace base {

/**
 * @brief 下游实例健康状态（HealthTracker 状态机）
 */
enum EndpointState {
  kEndpointHealthy = 0,   // 健康，参与负载均衡
  kEndpointHalfOpen = 1,  // 半开，放少量试探流量
  kEndpointEjected = 2,   // 已摘除，不参与选址
};

/**
 * @brief 负载均衡策略
 */
enum LbStrategy {
  kLbRoundRobin = 0,  // 轮询
  kLbRandom = 1,      // 随机
  kLbWeight = 2,      // 加权轮询
  kLbHash = 3,        // 按 key 哈希（会话/缓存亲和）
};

/**
 * @brief 一个下游实例（地址 + 运行时健康状态）
 *        健康字段由 ServiceManager 在持有 ServiceEntry 锁时维护
 */
struct Endpoint { /*{{{*/
  std::string ip;
  uint16_t port;
  uint32_t weight;  // 加权 LB 用，默认 1

  EndpointState state;
  uint32_t consecutive_fails;    // 连续失败次数
  uint32_t half_open_success;    // 半开态连续成功次数
  uint64_t ejected_at_ms;        // 摘除时刻（冷却/拉回计时）
  uint64_t last_probe_ms;        // 上次探测时刻（主动探测用）

  Endpoint()
      : port(0),
        weight(1),
        state(kEndpointHealthy),
        consecutive_fails(0),
        half_open_success(0),
        ejected_at_ms(0),
        last_probe_ms(0) {}
}; /*}}}*/

/**
 * @brief 健康检查 / 熔断参数（缺省值见构造函数）
 */
struct HealthConfig { /*{{{*/
  uint32_t fail_threshold;     // 连续失败几次触发摘除
  uint32_t eject_cooldown_ms;  // 摘除后多久进入半开探测
  uint32_t probe_interval_ms;  // 主动探测/热加载轮询间隔
  uint32_t half_open_success;  // 半开连续成功几次转健康
  double min_healthy_ratio;    // 最少健康占比（防全摘），0 表示允许全摘后降级随机

  HealthConfig()
      : fail_threshold(3),
        eject_cooldown_ms(5000),
        probe_interval_ms(3000),
        half_open_success(2),
        min_healthy_ratio(0.0) {}
}; /*}}}*/

/**
 * @brief 实例列表变化回调：Provider 探测到变化后把"全量最新列表"回灌给上层
 * @param service_name 服务名
 * @param endpoints 该服务当前全量实例
 * @param ctx 注册 Watch 时传入的上下文指针
 */
typedef void (*OnEndpointsChanged)(const std::string &service_name, const std::vector<Endpoint> &endpoints,
                                   void *ctx);

/**
 * @brief 服务发现数据源抽象（可插拔：文件 / DNS / 注册中心）
 */
class ServiceProvider { /*{{{*/
 public:
  virtual ~ServiceProvider() {}

  /**
   * @brief 首次同步拉取某服务的实例列表（启动时调用）
   * @param service_name 服务名
   * @param out 输出实例列表
   * @return kOk 成功；kNotFound 无该服务配置；其他为数据源错误
   */
  virtual Code Resolve(const std::string &service_name, std::vector<Endpoint> *out) = 0;

  /**
   * @brief 订阅某服务实例变化；数据源探测到变化时回调 cb
   * @param service_name 服务名
   * @param cb 变化回调
   * @param ctx 回调上下文
   * @return kOk 成功
   */
  virtual Code Watch(const std::string &service_name, OnEndpointsChanged cb, void *ctx) = 0;
}; /*}}}*/

/**
 * @brief 解析实例列表字符串 "ip:port[:weight]" 列表（逗号分隔），weight 缺省为 1
 * @param spec 形如 "127.0.0.1:9103:1, 127.0.0.1:9203, 127.0.0.1:9303:2"
 * @param out 输出解析得到的实例（仅填充 ip/port/weight，健康状态用默认值）
 * @return kOk 至少解析出一个实例；kInvalidParam out 为空指针；kNotFound 未解析出任何实例
 */
Code ParseEndpoints(const std::string &spec, std::vector<Endpoint> *out);

}  // namespace base
#endif
