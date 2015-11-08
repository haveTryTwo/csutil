// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DAEMON_H_
#define BASE_DAEMON_H_

#include "base/status.h"

namespace base
{

Code Daemon();
Code DaemonAndKeepAlive();

}

#endif
