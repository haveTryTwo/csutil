// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CODING_H_
#define BASE_CODING_H_

#include <string>

#include <stdint.h>

#include "status.h"

namespace base
{

Code EncodeFixed32(uint32_t num, std::string *out);
Code DecodeFixed32(const std::string &in, uint32_t *value);

}

#endif
