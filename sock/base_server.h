// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_BASE_SERVER_H_
#define SOCK_BASE_SERVER_H_

#include <stdint.h>

#include <deque>
#include <map>
#include <string>

#include "base/config.h"
#include "base/event_loop.h"
#include "base/load_ctrl.h"
#include "base/mutex.h"
#include "base/statistic.h"
#include "base/status.h"
#include "sock/tcp_proto.h"

namespace base {

class BaseServer { /*{{{*/
 public:
  BaseServer(){};
  virtual ~BaseServer(){};

 public:
  virtual Code Init() = 0;
  virtual Code Run() = 0;

 private:
  BaseServer(const BaseServer &);
  BaseServer &operator=(const BaseServer &);

}; /*}}}*/

};  // namespace base

#endif
