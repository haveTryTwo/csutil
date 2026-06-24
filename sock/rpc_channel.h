// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_RPC_CHANNEL_H_
#define SOCK_RPC_CHANNEL_H_

#include <stdint.h>

#include <string>

#include <google/protobuf/message.h>

#include "base/status.h"
#include "sock/rpc_client.h"

namespace base {

/**
 * @brief 线程局部可复用 RPC 通道（方案A：thread-local 长连接 + 断线自动重连重试）
 *
 * 用于“服务内部调用下游服务”的连接复用：每个线程对每个下游地址维持一条常驻连接，
 * 用完不关；当连接断开（对端关闭、超时、读写错误）时自动重连并重试一次。
 * 业务 handler 仅需调用 RpcChannel::Get(ip, port)->SendAndRecv(req, &resp)，
 * 无需感知连接建立/复用/重连等细节，避免 connect-per-request 导致的 TIME_WAIT 堆积。
 */
class RpcChannel { /*{{{*/
 public:
  /**
   * @brief 获取到指定下游地址的线程局部可复用通道（按 "ip:port" 缓存）
   * @param ip 下游服务 ip
   * @param port 下游服务端口
   * @return 线程局部 RpcChannel 指针，随线程/进程生命周期存活，调用方不得 delete
   */
  static RpcChannel *Get(const std::string &ip, uint16_t port);

  /**
   * @brief 发送一次 protobuf 请求并接收响应；连接类错误时自动重连并重试一次
   * @param req 请求消息
   * @param resp 响应消息（输出参数）
   * @return kOk 成功；其他为连接错误码或业务/框架错误码
   */
  Code SendAndRecv(const ::google::protobuf::Message &req, ::google::protobuf::Message *resp);

  /**
   * @brief 设置收发超时时间（毫秒），-1 表示永久等待；不调用则沿用框架默认值
   *        仅对方案A（线程局部长连接）模式生效
   * @param ms 超时毫秒数
   * @return kOk 成功；kInvalidParam 参数无效
   */
  Code SetTimeoutMs(int ms);

  /**
   * @brief 全局启用方案B共享连接池模式（进程级，建议在对外服务前调用一次）；
   *        启用后 SendAndRecv 改为“从池借/还”，业务接口不变。不调用则默认方案A。
   * @param max_conn_per_addr 每个下游地址最大连接数，须 > 0
   * @param idle_timeout_ms 空闲连接最长保留时间(毫秒)，0 表示不回收
   * @param acquire_timeout_ms 池满且无空闲时的最长等待时间(毫秒)
   * @return kOk 成功；kInvalidParam 参数无效
   */
  static Code EnablePoolMode(uint32_t max_conn_per_addr, uint32_t idle_timeout_ms, uint32_t acquire_timeout_ms);

  /**
   * @brief 当前是否处于方案B连接池模式
   * @return true 为连接池模式；false 为线程局部长连接模式
   */
  static bool IsPoolMode();

  /**
   * @brief 判断错误码是否属于连接类错误（需要重连重试 / 标记坏连接）
   * @param ret 错误码
   * @return true 为连接类错误；false 为业务/框架错误
   */
  static bool IsConnError(Code ret);

 private:
  RpcChannel(const std::string &ip, uint16_t port);
  ~RpcChannel();
  RpcChannel(const RpcChannel &);
  RpcChannel &operator=(const RpcChannel &);

  /**
   * @brief 确保底层连接已初始化（首次连接失败也置为已初始化，由发送时自动重连）
   * @return kOk 成功；其他为不可恢复的初始化错误码
   */
  Code EnsureInit();

  /**
   * @brief 方案B：从连接池借取连接完成一次收发，用完归还（坏连接不复用）
   */
  Code SendViaPool(const ::google::protobuf::Message &req, ::google::protobuf::Message *resp);

  /**
   * @brief 在给定连接上完成一次收发，连接类错误时重连并重试一次
   * @param client 目标连接
   * @param req 请求
   * @param resp 响应（输出）
   * @param broken 输出：重试后仍为连接类错误则置 true（供连接池判定是否丢弃）
   * @return kOk 成功；其他为错误码
   */
  static Code SendOnClient(RpcClient *client, const ::google::protobuf::Message &req,
                           ::google::protobuf::Message *resp, bool *broken);

 private:
  std::string ip_;
  uint16_t port_;
  int timeout_ms_;  // kTimeoutUnset 表示未设置，沿用框架默认
  bool inited_;
  RpcClient client_;
}; /*}}}*/

}  // namespace base
#endif
