// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools
{

base::Code CheckLogFormat(const std::string &cnt, bool *is_satisfied);

base::Code CheckLogFormat(const std::string &path, const std::string &log_name, std::map<uint32_t, bool> *result);

base::Code CheckLogFormat(const std::string &path, const std::string &log_name);

base::Code CheckLogFormatForCC(const std::string &dir, const std::string &log_name);

}
