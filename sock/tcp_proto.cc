// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/tcp_proto.h"

#include "base/coding.h"
#include "base/common.h"
#include "base/msg.h"
#include "base/util.h"

namespace base {

Code DefaultProtoFunc(const char *src_data, int src_data_len, int *real_len) { /*{{{*/
  if (src_data == NULL || src_data_len < 0 || real_len == NULL) return kInvalidParam;

  if (src_data_len < kHeadLen) return kDataNotEnough;

  uint32_t len = 0;
  Code r = DecodeFixed32(std::string(src_data, src_data_len), &len);
  if (r != kOk) return r;

  if (src_data_len < (int)(kHeadLen + len)) return kDataNotEnough;

  *real_len = kHeadLen + len;

  return kOk;
} /*}}}*/

}  // namespace base
