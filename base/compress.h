// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMPRESS_H_
#define BASE_COMPRESS_H_

#include "base/status.h"

namespace base {

class BaseCompress {
 public:
  BaseCompress() {}
  virtual ~BaseCompress() {}

 public:
  virtual Code Compress(const std::string &uncompressed, std::string *compressed);
  virtual Code Uncompress(const std::string &compressed, std::string *uncompressed);
};

}  // namespace base

#endif  // BASE_COMPRESS_H_
