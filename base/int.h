// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_INT_H_
#define BASE_INT_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base
{

Code GetInt32(const std::string &str, int *num);
Code GetInt64(const std::string &str, int64_t *num);

Code GetInt32ByHex(const std::string &hex_str, int *num);
Code GetInt64ByHex(const std::string &hex_str, int64_t *num);

Code GetInt64(const std::string &str, int64_t *num, int base);

Code GetUpDivValue(uint64_t dividend, uint64_t divisor, uint64_t *value); 

}

#endif
