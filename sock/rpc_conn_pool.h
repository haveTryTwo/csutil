// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_RPC_CONN_POOL_H_
#define SOCK_RPC_CONN_POOL_H_

#include <stdint.h>

#include <deque>
#include <map>
#include <string>

#include "base/mutex.h"
#include "base/status.h"
#include "sock/rpc_client.h"

namespace base {

/**
 * @brief 共享下游连接池（方案B：per-address connection pool）
 *
 * 为每个下游地址(ip:port)维护一个共享连接池：调用方 Acquire 借一条连接，用完 Release 归还。
 * 特性：每地址最大连接数上限、空闲连接惰性回收(idle_timeout)、坏连接直接丢弃(健康检查)、
 * 借取超时(池满且无空闲时等待 acquire_timeout，超时返回 kTimeOut)。连接数与线程数解耦。
 * 全局单例，线程安全。
 */
class RpcConnPool { /*{{{*/
 public:
  static RpcConnPool *Instance();

  /**
   * @brief 配置池参数（建议在首次 Acquire 前设置；可重复调用覆盖）
   * @param max_conn_per_addr 每个下游地址最大连接数(空闲+在用)，须 > 0
   * @param idle_timeout_ms 空闲连接最长保留时间(毫秒)，0 表示不回收；超时在 Acquire 时惰性清理
   * @param acquire_timeout_ms 池满且无空闲时的最长等待时间(毫秒)
   * @return kOk 成功；kInvalidParam 参数无效
   */
  Code SetConfig(uint32_t max_conn_per_addr, uint32_t idle_timeout_ms, uint32_t acquire_timeout_ms);

  /**
   * @brief 借取一条到指定地址的可用连接（优先复用空闲，否则新建至上限，池满则等待）
   * @param ip 下游 ip
   * @param port 下游端口
   * @param client 输出借到的连接（归还前由调用方独占，不得 delete）
   * @return kOk 成功；kTimeOut 池满等待超时；其他为建连失败错误码
   */
  Code Acquire(const std::string &ip, uint16_t port, RpcClient **client);

  /**
   * @brief 归还连接；broken=true 表示连接已损坏，直接销毁不再复用
   * @param ip 下游 ip
   * @param port 下游端口
   * @param client 之前借取的连接
   * @param broken 连接是否已损坏
   * @return kOk 成功；kInvalidParam 参数无效
   */
  Code Release(const std::string &ip, uint16_t port, RpcClient *client, bool broken);

 private:
  struct IdleConn { /*{{{*/
    RpcClient *client;
    uint64_t last_used_ms;
    IdleConn() : client(NULL), last_used_ms(0) {}
  }; /*}}}*/

  struct AddrPool { /*{{{*/
    std::deque<IdleConn> idle_conns;
    uint32_t total_conns;  // 已创建连接数(空闲 + 在用)
    AddrPool() : total_conns(0) {}
  }; /*}}}*/

  RpcConnPool();
  ~RpcConnPool();
  RpcConnPool(const RpcConnPool &);
  RpcConnPool &operator=(const RpcConnPool &);

  /**
   * @brief 取得地址对应的子池，不存在则创建（调用方须持有 mu_）
   */
  AddrPool *GetOrCreatePool(const std::string &key);

  /**
   * @brief 清理子池中空闲超时的连接（调用方须持有 mu_）
   */
  void DropExpiredIdle(AddrPool *pool, uint64_t now_ms);

  static Code CreateAndInitClient(const std::string &ip, uint16_t port, RpcClient **client);
  static uint64_t NowMs();
  static std::string MakeKey(const std::string &ip, uint16_t port);

 private:
  Mutex mu_;
  Cond cond_;
  std::map<std::string, AddrPool *> pools_;
  uint32_t max_conn_per_addr_;
  uint32_t idle_timeout_ms_;
  uint32_t acquire_timeout_ms_;
}; /*}}}*/

}  // namespace base
#endif
