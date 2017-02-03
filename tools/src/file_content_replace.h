// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_FILE_CONTENT_REPLACE_H_
#define TOOLS_FILE_CONTENT_REPLACE_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools
{
// NOTE: 1. replace pos start with 0, and replace_len equals replace_str.size();
base::Code ReplaceAllCCFileContent(const std::string &dir, uint64_t replace_pos, uint64_t replace_len, const std::string &replace_str);

}

#endif
