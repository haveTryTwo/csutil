// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MESSAGE_DIGEST_H_
#define BASE_MESSAGE_DIGEST_H_

#include <sys/cdefs.h>

#include <string>

#include "base/status.h"

namespace base {

class MessageDigest {
 public:
  MessageDigest() {}
  virtual ~MessageDigest() {}

 public:
  virtual Code Init() = 0;
  virtual Code Sum(const std::string &content, std::string *dest) = 0;
};

}  // namespace base

#endif
