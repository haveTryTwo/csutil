// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_H_
#define BASE_MEMORY_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base {

Code GetProcessMemoryUsage(uint64_t *memory_usage);
Code GetProcessMemoryReadableUsage(std::string *memory_usage_readable);

}  // namespace base

#endif  // BASE_MEMORY_H_
