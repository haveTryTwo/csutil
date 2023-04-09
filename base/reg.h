// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_REG_H_
#define BASE_REG_H_

#include <string>

#include <regex.h>
#include <sys/types.h>

#include "base/status.h"

namespace base {

class Reg {
 public:
  Reg(const std::string &regex);
  ~Reg();

  Code Init();

  Code Check(const std::string &str);

 private:
  regex_t *preg_;
  std::string regex_;
};

}  // namespace base

#endif
