// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
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
Code GetUInt32(const std::string &str, uint32_t *num);
Code GetInt64(const std::string &str, int64_t *num);
Code GetUInt64(const std::string &str, uint64_t *num);

Code GetInt32ByHex(const std::string &hex_str, int *num);
Code GetUInt32ByHex(const std::string &hex_str, uint32_t *num);
Code GetInt64ByHex(const std::string &hex_str, int64_t *num);
Code GetUInt64ByHex(const std::string &hex_str, uint64_t *num);

Code GetInt64(const std::string &str, int64_t *num, int base);
Code GetUInt64(const std::string &str, uint64_t *num, int base);

Code GetUpDivValue(uint64_t dividend, uint64_t divisor, uint64_t *value); 

Code BigAdd(const std::string &ln, const std::string &rn, std::string *reslut);
Code BigSub(const std::string &ln, const std::string &rn, std::string *result);

Code GetMaxCommonDivisor(uint64_t first, uint64_t second, uint64_t *comm_divisor);
Code GetMinCommonMultiple(uint32_t first, uint32_t second, uint64_t *comm_mul);

Code ReverseBits(uint64_t source, uint64_t *dest);
}

#endif
