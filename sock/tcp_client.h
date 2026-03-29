// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_TCP_CLIENT_H_
#define SOCK_TCP_CLIENT_H_

#include <stdint.h>

#include <string>

#include "base/event.h"
#include "base/status.h"
#include "sock/base_client.h"
#include "sock/rpc_proto.h"

namespace base {

class TcpClient : public BaseClient { /*{{{*/
 public:
  TcpClient();
  TcpClient(const std::string &ip, uint16_t port);
  virtual ~TcpClient();

  Code Init(EventType evt_type, DataProtoFunc data_proto_func);

  Code Connect();
  Code Connect(const std::string &ip, uint16_t port);
  Code ReConnect();

  // Send the data without modifying.
  Code SendNative(const std::string &data);

  // Receive data occarding to DataProtoFunc
  Code Recv(std::string *data);

  void CloseConnect();

  /**
   * @brief 设置最大等待超时时间
   *
   * @param max_wait_time_ms 最大等待时间（毫秒）。
   *                         -1 表示永久等待，0 表示不等待，> 0 表示等待指定时间
   * @return base::kOk 设置成功
   * @return base::kInvalidParam 参数无效（小于 -1）
   */
  Code SetMaxWaitTimeMs(int max_wait_time_ms);

  /**
   * @brief 获取当前最大等待超时时间
   *
   * @return int 当前最大等待时间（毫秒），-1 表示永久等待
   */
  int GetMaxWaitTimeMs() const;

 private:
  Code RecvInternal();

 private:
  Event *ev_;
  int client_fd_;
  DataProtoFunc data_proto_func_;
  std::string serv_ip_;
  uint16_t serv_port_;

  char *data_buf_;
  int start_pos_;
  int end_pos_;
  int total_size_;
  bool is_init_;

  // 最大等待超时时间（毫秒），-1 表示永久等待，默认为 kDefaultMaxWaitTimeMs
  int max_wait_time_ms_;
}; /*}}}*/

}  // namespace base
#endif
