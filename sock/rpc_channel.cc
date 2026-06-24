// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_channel.h"

#include <map>

#include "sock/rpc_conn_pool.h"

namespace base {

static const int kTimeoutUnset = -2;

// 进程级模式开关：启动阶段设置、对外服务后只读，故无需加锁
static bool g_pool_mode = false;

RpcChannel::RpcChannel(const std::string &ip, uint16_t port)
    : ip_(ip), port_(port), timeout_ms_(kTimeoutUnset), inited_(false), client_(ip, port) { /*{{{*/
} /*}}}*/

RpcChannel::~RpcChannel() { /*{{{*/
} /*}}}*/

RpcChannel *RpcChannel::Get(const std::string &ip, uint16_t port) { /*{{{*/
  // 每个线程私有一张 "ip:port" -> RpcChannel 表，连接私有无锁；
  // 通道随线程/进程生命周期存活，故此处只创建不释放（数量受下游地址数约束）。
  static thread_local std::map<std::string, RpcChannel *> channels;

  std::string key = ip + ":" + std::to_string((uint32_t)port);
  std::map<std::string, RpcChannel *>::iterator it = channels.find(key);
  if (it != channels.end()) return it->second;

  RpcChannel *channel = new RpcChannel(ip, port);
  channels[key] = channel;
  return channel;
} /*}}}*/

Code RpcChannel::SendAndRecv(const ::google::protobuf::Message &req,
                             ::google::protobuf::Message *resp) { /*{{{*/
  if (resp == NULL) return kInvalidParam;

  // 方案B：从共享连接池借/还
  if (g_pool_mode) return SendViaPool(req, resp);

  // 方案A：线程局部长连接
  Code ret = EnsureInit();
  if (ret != kOk) return ret;

  return SendOnClient(&client_, req, resp, NULL);
} /*}}}*/

Code RpcChannel::SendViaPool(const ::google::protobuf::Message &req,
                             ::google::protobuf::Message *resp) { /*{{{*/
  RpcClient *client = NULL;
  Code ret = RpcConnPool::Instance()->Acquire(ip_, port_, &client);
  if (ret != kOk) return ret;  // kTimeOut 表示池满等待超时

  bool broken = false;
  ret = SendOnClient(client, req, resp, &broken);
  RpcConnPool::Instance()->Release(ip_, port_, client, broken);
  return ret;
} /*}}}*/

Code RpcChannel::SendOnClient(RpcClient *client, const ::google::protobuf::Message &req,
                              ::google::protobuf::Message *resp, bool *broken) { /*{{{*/
  Code ret = client->SendAndRecv(req, resp);
  if (ret == kOk) {
    if (broken != NULL) *broken = false;
    return kOk;
  }

  // 连接类错误：底层已 CloseConnect，下一次发送会自动重连，故重试一次
  if (IsConnError(ret)) {
    resp->Clear();
    ret = client->SendAndRecv(req, resp);
  }

  if (broken != NULL) *broken = (ret != kOk && IsConnError(ret));
  return ret;
} /*}}}*/

Code RpcChannel::EnablePoolMode(uint32_t max_conn_per_addr, uint32_t idle_timeout_ms,
                                uint32_t acquire_timeout_ms) { /*{{{*/
  Code ret = RpcConnPool::Instance()->SetConfig(max_conn_per_addr, idle_timeout_ms, acquire_timeout_ms);
  if (ret != kOk) return ret;

  g_pool_mode = true;
  return kOk;
} /*}}}*/

bool RpcChannel::IsPoolMode() { /*{{{*/ return g_pool_mode; } /*}}}*/

Code RpcChannel::SetTimeoutMs(int ms) { /*{{{*/
  if (ms < -1) return kInvalidParam;

  timeout_ms_ = ms;
  if (inited_) return client_.SetMaxWaitTimeMs(ms);
  return kOk;
} /*}}}*/

Code RpcChannel::EnsureInit() { /*{{{*/
  if (inited_) return kOk;

  Code ret = client_.Init();
  // 首次 connect 失败属连接类错误时，底层 ev_/缓冲已就绪，后续发送会自动重连，
  // 故仍视为已初始化；仅当为不可恢复错误（如资源分配失败）时才向上返回。
  if (ret != kOk && !IsConnError(ret)) return ret;

  if (timeout_ms_ != kTimeoutUnset) client_.SetMaxWaitTimeMs(timeout_ms_);
  inited_ = true;
  return kOk;
} /*}}}*/

bool RpcChannel::IsConnError(Code ret) { /*{{{*/
  switch (ret) {
    case kConnError:
    case kSocketError:
    case kConnectError:
    case kReadError:
    case kWriteError:
    case kTimeOut:
    case kNotInit:
      return true;
    default:
      return false;
  }
} /*}}}*/

}  // namespace base
