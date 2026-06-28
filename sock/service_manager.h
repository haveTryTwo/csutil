// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_SERVICE_MANAGER_H_
#define SOCK_SERVICE_MANAGER_H_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "base/mutex.h"
#include "base/status.h"
#include "sock/load_balancer.h"
#include "sock/service_provider.h"

namespace base {

/**
 * @brief 一个下游服务的运行时视图：全量实例 + 选址器 + 版本
 */
struct ServiceEntry { /*{{{*/
  std::string service_name;
  std::vector<Endpoint> endpoints;  // 全量实例（含被摘除）
  LbStrategy lb_strategy;
  LoadBalancer *lb;
  uint64_t version;  // 实例列表版本，每次 Provider 更新自增
  Mutex mutex;       // 保护 endpoints 并发读写

  ServiceEntry() : lb_strategy(kLbRoundRobin), lb(NULL), version(0) {}
}; /*}}}*/

/**
 * @brief 客户端服务发现管理器（进程级单例）
 *
 * 职责：
 *  - 维护 service_name -> 实例集合，对接可插拔 ServiceProvider（首次 Resolve + Watch 热更新）。
 *  - 选址：在健康/半开实例中按 LB 策略挑选一个（Pick）。
 *  - 被动健康检查：依据调用结果驱动熔断摘除 / 半开恢复（Report）。
 *  - 通过连接清理 hook 在摘除/下线实例时通知连接层（与 RpcConnPool 解耦）。
 */
class ServiceManager { /*{{{*/
 public:
  /**
   * @brief 连接清理钩子：摘除/下线实例时回调，用于清理该地址的连接池连接
   * @param ip 实例 ip
   * @param port 实例端口
   * @param ctx 注册时传入的上下文
   */
  typedef void (*ConnDropFunc)(const std::string &ip, uint16_t port, void *ctx);

  static ServiceManager *Instance();

  /**
   * @brief 初始化：注册数据源与全局健康参数（进程启动调用一次）
   * @param provider 数据源（调用方持有其生命周期，须长于 ServiceManager 使用期）
   * @param hc 健康/熔断参数
   * @return kOk 成功；kInvalidParam provider 为空
   */
  Code Init(ServiceProvider *provider, const HealthConfig &hc);

  /**
   * @brief 注册一个下游服务：首次 Resolve 拉取实例并启动 Watch 热更新
   * @param service_name 服务名
   * @param lb 该服务的负载均衡策略
   * @return kOk 成功；kNotInit 未 Init；其他为数据源错误
   */
  Code RegisterService(const std::string &service_name, LbStrategy lb);

  /**
   * @brief 选址：在健康/半开实例中按策略选一个；冷却到期的摘除实例自动转半开参与试探
   * @param service_name 服务名
   * @param hash_key 哈希键（仅 hash 策略用，可传空）
   * @param out 选中的实例（输出）
   * @return kOk 成功；kNotFound 无该服务或无任何实例
   */
  Code Pick(const std::string &service_name, const std::string &hash_key, Endpoint *out);

  /**
   * @brief 上报一次调用结果，驱动被动熔断 / 半开恢复
   * @param service_name 服务名
   * @param ep 本次调用的实例
   * @param ok true 成功；false 失败（仅连接/超时类错误应判为 false，业务错误不计入）
   */
  void Report(const std::string &service_name, const Endpoint &ep, bool ok);

  /**
   * @brief 注册连接清理 hook（可选）；不注册则摘除仅靠"不再选中"自然淘汰
   * @param func 清理函数
   * @param ctx 上下文
   */
  void SetConnDropHook(ConnDropFunc func, void *ctx);

  /**
   * @brief Provider 实例变化回调入口（ctx 传 ServiceManager 实例）
   */
  static void OnEndpointsChangedCb(const std::string &service_name, const std::vector<Endpoint> &endpoints,
                                   void *ctx);

 private:
  ServiceManager();
  ~ServiceManager();
  ServiceManager(const ServiceManager &);
  ServiceManager &operator=(const ServiceManager &);

  /**
   * @brief 查找服务条目（持有 map_mu_）
   */
  ServiceEntry *FindEntry(const std::string &service_name);

  /**
   * @brief 合并最新实例列表：保留已存在实例健康状态，新增置健康，移除则清理（持有 entry->mutex）
   */
  void MergeEndpoints(ServiceEntry *entry, const std::vector<Endpoint> &latest);

  /**
   * @brief 判断当前是否允许再摘除一个实例（雪崩保护，持有 entry->mutex）
   */
  bool EvictAllowed(const ServiceEntry *entry) const;

  /**
   * @brief 通知连接层清理某地址连接（若已注册 hook）
   */
  void NotifyConnDrop(const std::string &ip, uint16_t port);

  static uint64_t NowMs();

 private:
  Mutex map_mu_;
  std::map<std::string, ServiceEntry *> services_;
  ServiceProvider *provider_;
  HealthConfig hc_;
  bool inited_;

  ConnDropFunc drop_hook_;
  void *drop_ctx_;
}; /*}}}*/

}  // namespace base
#endif
