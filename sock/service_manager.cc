// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/service_manager.h"

#include <stdlib.h>

#include "base/log.h"
#include "base/time.h"

namespace base {

ServiceManager::ServiceManager()
    : map_mu_(), services_(), provider_(NULL), hc_(), inited_(false), drop_hook_(NULL), drop_ctx_(NULL) { /*{{{*/
} /*}}}*/

ServiceManager::~ServiceManager() { /*{{{*/
  std::map<std::string, ServiceEntry *>::iterator it = services_.begin();
  for (; it != services_.end(); ++it) {
    delete it->second->lb;
    delete it->second;
  }
  services_.clear();
} /*}}}*/

ServiceManager *ServiceManager::Instance() { /*{{{*/
  static ServiceManager instance;
  return &instance;
} /*}}}*/

Code ServiceManager::Init(ServiceProvider *provider, const HealthConfig &hc) { /*{{{*/
  if (provider == NULL) return kInvalidParam;

  MutexLock ml(&map_mu_);
  provider_ = provider;
  hc_ = hc;
  inited_ = true;
  return kOk;
} /*}}}*/

void ServiceManager::SetConnDropHook(ConnDropFunc func, void *ctx) { /*{{{*/
  MutexLock ml(&map_mu_);
  drop_hook_ = func;
  drop_ctx_ = ctx;
} /*}}}*/

Code ServiceManager::RegisterService(const std::string &service_name, LbStrategy lb) { /*{{{*/
  ServiceProvider *provider = NULL;
  {
    MutexLock ml(&map_mu_);
    if (!inited_ || provider_ == NULL) return kNotInit;
    if (services_.find(service_name) != services_.end()) return kOk;  // 幂等

    ServiceEntry *entry = new ServiceEntry();
    entry->service_name = service_name;
    entry->lb_strategy = lb;
    entry->lb = CreateLoadBalancer(lb);
    entry->version = 0;
    services_[service_name] = entry;
    provider = provider_;
  }

  // 首次拉取（失败不阻塞启动，留空列表由后续 Watch 回灌或选址时降级）
  std::vector<Endpoint> eps;
  Code ret = provider->Resolve(service_name, &eps);
  if (ret == kOk) {
    ServiceEntry *entry = NULL;
    {
      MutexLock ml(&map_mu_);
      entry = FindEntry(service_name);
    }
    if (entry != NULL) {
      MutexLock el(&entry->mutex);
      MergeEndpoints(entry, eps);
    }
  } else {
    LOG_ERR("resolve service:%s failed on register, ret:%d, will rely on watch/fallback\n",
            service_name.c_str(), ret);
  }

  // 订阅变化（热加载新增/摘除）
  provider->Watch(service_name, &ServiceManager::OnEndpointsChangedCb, this);
  return kOk;
} /*}}}*/

Code ServiceManager::Pick(const std::string &service_name, const std::string &hash_key, Endpoint *out) { /*{{{*/
  if (out == NULL) return kInvalidParam;

  ServiceEntry *entry = NULL;
  {
    MutexLock ml(&map_mu_);
    entry = FindEntry(service_name);
  }
  if (entry == NULL) return kNotFound;

  MutexLock el(&entry->mutex);
  if (entry->endpoints.empty()) return kNotFound;

  uint64_t now = NowMs();
  std::vector<Endpoint> candidates;
  for (uint32_t i = 0; i < (uint32_t)entry->endpoints.size(); ++i) {
    Endpoint &ep = entry->endpoints[i];

    // 摘除冷却到期：转半开，参与试探
    if (ep.state == kEndpointEjected && now >= ep.ejected_at_ms &&
        now - ep.ejected_at_ms >= hc_.eject_cooldown_ms) {
      ep.state = kEndpointHalfOpen;
      ep.half_open_success = 0;
      ep.last_probe_ms = now;
    }

    if (ep.state == kEndpointHealthy || ep.state == kEndpointHalfOpen) candidates.push_back(ep);
  }

  // 全部被摘且未到冷却：降级在全量实例中随机试探，避免无节点可用
  if (candidates.empty()) {
    LOG_ERR("service:%s all endpoints unhealthy, degrade to random retry\n", service_name.c_str());
    uint32_t idx = (uint32_t)(rand() % (int)entry->endpoints.size());
    *out = entry->endpoints[idx];
    return kOk;
  }

  return entry->lb->Pick(candidates, hash_key, out);
} /*}}}*/

void ServiceManager::Report(const std::string &service_name, const Endpoint &ep, bool ok) { /*{{{*/
  ServiceEntry *entry = NULL;
  {
    MutexLock ml(&map_mu_);
    entry = FindEntry(service_name);
  }
  if (entry == NULL) return;

  MutexLock el(&entry->mutex);
  uint64_t now = NowMs();

  for (uint32_t i = 0; i < (uint32_t)entry->endpoints.size(); ++i) {
    Endpoint &cur = entry->endpoints[i];
    if (cur.ip != ep.ip || cur.port != ep.port) continue;

    if (ok) {
      cur.consecutive_fails = 0;
      if (cur.state == kEndpointHalfOpen) {
        cur.half_open_success++;
        if (cur.half_open_success >= hc_.half_open_success) {
          cur.state = kEndpointHealthy;
          cur.half_open_success = 0;
        }
      }
      return;
    }

    // 失败处理
    if (cur.state == kEndpointHalfOpen) {
      // 半开试探失败：退回摘除，重置冷却
      cur.state = kEndpointEjected;
      cur.ejected_at_ms = now;
      cur.half_open_success = 0;
      cur.consecutive_fails = 0;
      NotifyConnDrop(cur.ip, cur.port);
    } else if (cur.state == kEndpointHealthy) {
      cur.consecutive_fails++;
      if (cur.consecutive_fails >= hc_.fail_threshold) {
        if (EvictAllowed(entry)) {
          cur.state = kEndpointEjected;
          cur.ejected_at_ms = now;
          cur.consecutive_fails = 0;
          LOG_ERR("service:%s eject endpoint %s:%u after %u fails\n", service_name.c_str(), cur.ip.c_str(),
                  (uint32_t)cur.port, hc_.fail_threshold);
          NotifyConnDrop(cur.ip, cur.port);
        } else {
          // 雪崩保护：不再摘除，将失败计数封顶，避免溢出并保持"带病转发"
          cur.consecutive_fails = hc_.fail_threshold;
        }
      }
    }
    return;
  }
} /*}}}*/

void ServiceManager::OnEndpointsChangedCb(const std::string &service_name, const std::vector<Endpoint> &endpoints,
                                          void *ctx) { /*{{{*/
  if (ctx == NULL) return;
  ServiceManager *self = (ServiceManager *)ctx;

  ServiceEntry *entry = NULL;
  {
    MutexLock ml(&self->map_mu_);
    entry = self->FindEntry(service_name);
  }
  if (entry == NULL) return;

  MutexLock el(&entry->mutex);
  self->MergeEndpoints(entry, endpoints);
} /*}}}*/

ServiceEntry *ServiceManager::FindEntry(const std::string &service_name) { /*{{{*/
  std::map<std::string, ServiceEntry *>::iterator it = services_.find(service_name);
  if (it == services_.end()) return NULL;
  return it->second;
} /*}}}*/

void ServiceManager::MergeEndpoints(ServiceEntry *entry, const std::vector<Endpoint> &latest) { /*{{{*/
  std::vector<Endpoint> merged;
  merged.reserve(latest.size());

  // 1) 以最新列表为准，保留已存在实例的健康状态；新增实例默认健康
  for (uint32_t i = 0; i < (uint32_t)latest.size(); ++i) {
    Endpoint ep = latest[i];  // 含最新 weight
    bool found = false;
    for (uint32_t j = 0; j < (uint32_t)entry->endpoints.size(); ++j) {
      const Endpoint &old_ep = entry->endpoints[j];
      if (old_ep.ip == ep.ip && old_ep.port == ep.port) {
        ep.state = old_ep.state;
        ep.consecutive_fails = old_ep.consecutive_fails;
        ep.half_open_success = old_ep.half_open_success;
        ep.ejected_at_ms = old_ep.ejected_at_ms;
        ep.last_probe_ms = old_ep.last_probe_ms;
        found = true;
        break;
      }
    }
    (void)found;
    merged.push_back(ep);
  }

  // 2) 已移除实例：清理其连接
  for (uint32_t j = 0; j < (uint32_t)entry->endpoints.size(); ++j) {
    const Endpoint &old_ep = entry->endpoints[j];
    bool still_present = false;
    for (uint32_t i = 0; i < (uint32_t)latest.size(); ++i) {
      if (latest[i].ip == old_ep.ip && latest[i].port == old_ep.port) {
        still_present = true;
        break;
      }
    }
    if (!still_present) NotifyConnDrop(old_ep.ip, old_ep.port);
  }

  entry->endpoints.swap(merged);
  entry->version++;
} /*}}}*/

bool ServiceManager::EvictAllowed(const ServiceEntry *entry) const { /*{{{*/
  uint32_t total = (uint32_t)entry->endpoints.size();
  if (total == 0) return false;

  uint32_t healthy = 0;
  for (uint32_t i = 0; i < total; ++i) {
    if (entry->endpoints[i].state == kEndpointHealthy) healthy++;
  }

  // 摘除当前这个健康实例后剩余健康占比是否仍达标
  double ratio_after = (double)(healthy > 0 ? healthy - 1 : 0) / (double)total;
  return ratio_after >= hc_.min_healthy_ratio;
} /*}}}*/

void ServiceManager::NotifyConnDrop(const std::string &ip, uint16_t port) { /*{{{*/
  // drop_hook_ 在 Init 阶段设置、运行期只读，直接读取即可
  if (drop_hook_ != NULL) drop_hook_(ip, port, drop_ctx_);
} /*}}}*/

uint64_t ServiceManager::NowMs() { /*{{{*/
  struct timeval tm;
  Code ret = Time::GetTime(&tm);
  if (ret != kOk) return 0;

  return (uint64_t)tm.tv_sec * 1000 + (uint64_t)tm.tv_usec / 1000;
} /*}}}*/

}  // namespace base
