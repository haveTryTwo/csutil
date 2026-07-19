// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_SOCK_RPC_CLIENT_H_
#define TEST_SOCK_RPC_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/status.h"
#include "sock/tcp_client.h"

#include "test_press_base/include/test_busi_client.h"

namespace test {

// 与 csutil/sock 框架一致的协议注册名
const std::string kSockRpcStr = "sock_rpc";

/**
 * @brief 适配 csutil/sock RPC 协议的压测业务客户端
 *
 * 直接复用 sock/rpc_proto.cc 的编解码函数（DefaultProtoFunc/DefaultFormatUserDataFunc/
 * DefaultGetUserDataFunc），确保与 RpcServer 的线上协议
 * [Head(4B)][Magic(4B)][FrameCode(4B)][UserData] 完全一致。
 */
class SockRpcBusiClient : public BusiClient {
 public:
  SockRpcBusiClient(const std::string &client_name);
  virtual ~SockRpcBusiClient();

 public:
  /**
   * @brief 解析 ip:port:sock_rpc 并建立连接
   * @param dst_ip_port_proto 形如 127.0.0.1:9101:sock_rpc
   * @return kOk 成功；kInvalidParam 格式非法；其他为连接失败
   */
  virtual base::Code Init(const std::string &dst_ip_port_proto);

  /**
   * @brief 工厂方法，复制当前对象（按线程克隆独立连接）
   * @return 新建的 SockRpcBusiClient 指针
   */
  virtual BusiClient *Create();

 public:
  /**
   * @brief 发送业务数据并接收响应（按 sock 协议封包/拆包）
   * @param req 业务请求字节串（如序列化后的 BookReq）
   * @param resp 输出参数，业务响应字节串（如序列化后的 BookResp）
   * @return kOk 成功；kNotInit 未初始化；框架错误码(700-799) 或收发失败码
   */
  virtual base::Code SendAndRecv(const std::string &req, std::string *resp);

 private:
  bool is_init_;
  base::TcpClient tcp_client_;
};

}  // namespace test

#endif
