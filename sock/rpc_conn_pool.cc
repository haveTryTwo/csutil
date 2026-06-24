// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_conn_pool.h"

#include "base/time.h"
#include "sock/rpc_channel.h"

namespace base {

static const uint32_t kDefaultMaxConnPerAddr = 16;
static const uint32_t kDefaultIdleTimeoutMs = 60000;
static const uint32_t kDefaultAcquireTimeoutMs = 1000;

RpcConnPool::RpcConnPool()
    : mu_(),
      cond_(),
      pools_(),
      max_conn_per_addr_(kDefaultMaxConnPerAddr),
      idle_timeout_ms_(kDefaultIdleTimeoutMs),
      acquire_timeout_ms_(kDefaultAcquireTimeoutMs) { /*{{{*/
} /*}}}*/

RpcConnPool::~RpcConnPool() { /*{{{*/
} /*}}}*/

RpcConnPool *RpcConnPool::Instance() { /*{{{*/
  static RpcConnPool instance;
  return &instance;
} /*}}}*/

Code RpcConnPool::SetConfig(uint32_t max_conn_per_addr, uint32_t idle_timeout_ms,
                            uint32_t acquire_timeout_ms) { /*{{{*/
  if (max_conn_per_addr == 0) return kInvalidParam;

  MutexLock ml(&mu_);
  max_conn_per_addr_ = max_conn_per_addr;
  idle_timeout_ms_ = idle_timeout_ms;
  acquire_timeout_ms_ = acquire_timeout_ms;
  return kOk;
} /*}}}*/

Code RpcConnPool::Acquire(const std::string &ip, uint16_t port, RpcClient **client) { /*{{{*/
  if (client == NULL) return kInvalidParam;
  *client = NULL;

  std::string key = MakeKey(ip, port);

  MutexLock ml(&mu_);
  AddrPool *pool = GetOrCreatePool(key);

  while (true) {
    DropExpiredIdle(pool, NowMs());

    // 1) 优先复用空闲连接（idle -> in_use，total 不变）
    if (!pool->idle_conns.empty()) {
      *client = pool->idle_conns.front().client;
      pool->idle_conns.pop_front();
      return kOk;
    }

    // 2) 未达上限则新建（建连放到锁外，避免阻塞其他借取/归还）
    if (pool->total_conns < max_conn_per_addr_) {
      pool->total_conns++;  // 先预占名额
      RpcClient *new_client = NULL;
      mu_.UnLock();
      Code ret = CreateAndInitClient(ip, port, &new_client);
      mu_.Lock();
      if (ret != kOk) {
        if (pool->total_conns > 0) pool->total_conns--;  // 建连失败，归还名额
        cond_.Signal();
        return ret;
      }
      *client = new_client;
      return kOk;
    }

    // 3) 池满且无空闲：等待归还，超时返回 kTimeOut
    Code wret = cond_.TimeWait(mu_, acquire_timeout_ms_);
    if (wret == kTimeOut) return kTimeOut;
  }
} /*}}}*/

Code RpcConnPool::Release(const std::string &ip, uint16_t port, RpcClient *client, bool broken) { /*{{{*/
  if (client == NULL) return kInvalidParam;

  std::string key = MakeKey(ip, port);

  MutexLock ml(&mu_);
  AddrPool *pool = GetOrCreatePool(key);

  if (broken) {
    delete client;
    if (pool->total_conns > 0) pool->total_conns--;
  } else {
    IdleConn idle_conn;
    idle_conn.client = client;
    idle_conn.last_used_ms = NowMs();
    pool->idle_conns.push_back(idle_conn);
  }

  cond_.Signal();
  return kOk;
} /*}}}*/

RpcConnPool::AddrPool *RpcConnPool::GetOrCreatePool(const std::string &key) { /*{{{*/
  std::map<std::string, AddrPool *>::iterator it = pools_.find(key);
  if (it != pools_.end()) return it->second;

  AddrPool *pool = new AddrPool();
  pools_[key] = pool;
  return pool;
} /*}}}*/

void RpcConnPool::DropExpiredIdle(AddrPool *pool, uint64_t now_ms) { /*{{{*/
  if (idle_timeout_ms_ == 0) return;

  // idle_conns 按归还时间从旧到新排列，队首最旧，超时即回收
  while (!pool->idle_conns.empty()) {
    const IdleConn &front = pool->idle_conns.front();
    if (now_ms >= front.last_used_ms && now_ms - front.last_used_ms < idle_timeout_ms_) break;

    delete front.client;
    pool->idle_conns.pop_front();
    if (pool->total_conns > 0) pool->total_conns--;
  }
} /*}}}*/

Code RpcConnPool::CreateAndInitClient(const std::string &ip, uint16_t port, RpcClient **client) { /*{{{*/
  RpcClient *new_client = new RpcClient(ip, port);
  Code ret = new_client->Init();
  // 首次 connect 失败若为连接类错误，底层 ev_/缓冲已就绪，后续发送会自动重连，仍可入池；
  // 仅当为不可恢复错误（如资源分配失败）时才放弃。
  if (ret != kOk && !RpcChannel::IsConnError(ret)) {
    delete new_client;
    return ret;
  }

  *client = new_client;
  return kOk;
} /*}}}*/

uint64_t RpcConnPool::NowMs() { /*{{{*/
  struct timeval tm;
  Code ret = Time::GetTime(&tm);
  if (ret != kOk) return 0;

  return (uint64_t)tm.tv_sec * 1000 + (uint64_t)tm.tv_usec / 1000;
} /*}}}*/

std::string RpcConnPool::MakeKey(const std::string &ip, uint16_t port) { /*{{{*/
  return ip + ":" + std::to_string((uint32_t)port);
} /*}}}*/

}  // namespace base
