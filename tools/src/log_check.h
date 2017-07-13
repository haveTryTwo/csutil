// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_LOG_CHECK_H_
#define TOOLS_LOG_CHECK_H_

#include <map>
#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools
{

/**
 *  Note:
 *    1. %% should not be considered as placeholder, and as the same of even numbers of % 
 *    2. %1111% should not be considered as placeholder
 */
base::Code GetSizeOfNotPlaceholder(const char *str, uint32_t size, uint32_t *ignore_size);

base::Code CheckLogFormat(const std::string &cnt, bool *is_satisfied);

base::Code CheckLogFormat(const std::string &path, const std::string &log_name, std::map<uint32_t, bool> *result);

// Note: Check singel cplusplus file
base::Code CheckLogFormatForCC(const std::string &path, const std::string &log_name);

// Note: Check whole cplusplus files in directory including subdirectories 
base::Code CheckLogFormatForCCS(const std::string &dir, const std::string &log_name);

}

#endif
