// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_INIT_JSON_VALUE_H_
#define TOOLS_INIT_JSON_VALUE_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace tools {

/**
 * NOTE: Initialize the corresponding value to the default value
 *  The init_keys must be key of leaf node
 *
 * src_file: the source json file
 * dst_file: the destination json file
 * init_keys: key of leaf node
 */
base::Code InitJsonValue(const std::string &src_file, const std::string &dst_file,
                         const std::string &init_keys);

/**
 * NOTE: seialize pb which content is json
 *
 * src_file: the source json file
 * dst_file: the destination json file
 */
base::Code SerializePBForJsonContent(const std::string &src_file, const std::string &dst_file,
                                     int level);

}  // namespace tools

#endif
