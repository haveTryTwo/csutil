// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_UTF8_H_
#define BASE_UTF8_H_

#include <stdint.h>
#include <string>
#include "base/status.h"

namespace base {

bool IsUtf8(const std::string &str);

}  // namespace base

#endif