// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_BASE_CLIENT_H_
#define SOCK_BASE_CLIENT_H_

#include <stdint.h>

#include <string>

#include "base/event.h"
#include "base/status.h"
#include "sock/tcp_proto.h"

namespace base {

class BaseClient { /*{{{*/
 public:
  BaseClient() {}
  virtual ~BaseClient() {}

  virtual Code Init(EventType evt_type, DataProtoFunc data_proto_func) = 0;

  virtual Code Connect(const std::string &ip, uint16_t port) = 0;
  virtual Code ReConnect() = 0;

  // Send the data without modifying.
  virtual Code SendNative(const std::string &data) = 0;

  // Receive data occarding to DataProtoFunc
  virtual Code Recv(std::string *data) = 0;

  virtual void CloseConnect() = 0;
}; /*}}}*/

}  // namespace base
#endif
