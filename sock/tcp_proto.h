// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_TCP_PROTO_H_
#define SOCK_TCP_PROTO_H_

#include <stdint.h>

#include <string>

#include "base/status.h"

namespace base {

/**
 * Note: This function is used to get data real length when reading data from stream.
 *       And user should define this data proto function.
 *
 * @return:
 *       0 : success, and real len is set
 *       1 : data in data_buf_ is not enough, and need to read stream again
 *       other : failed code
 */
typedef Code (*DataProtoFunc)(const char *src_data, int src_data_len, int *real_len);

Code DefaultProtoFunc(const char *src_data, int src_data_len, int *real_len);

}  // namespace base

#endif
