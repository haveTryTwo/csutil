// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_CREATE_CC_FILE_H_
#define TOOLS_CREATE_CC_FILE_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools
{
base::Code CreateCCFile(const std::string &file_name, const std::string &func_name);
}

#endif
