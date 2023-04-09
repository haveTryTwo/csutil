// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BIT_ARR_H_
#define BASE_BIT_ARR_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base {

/**
 * Note: The internal struct of bit array is like : 11011011 10101101 11110000
 *                                                      0        1       2
 */
class BitArr {
 public:
  explicit BitArr(uint32_t bits_num);
  ~BitArr();

 public:
  Code Init();
  Code Put(uint32_t index, bool value);
  Code Get(uint32_t index, bool *value);
  Code Clear();
  Code Size(uint32_t *size);
  Code ToString(std::string *str);

 private:
  BitArr(const BitArr &bit_arr);
  BitArr &operator=(const BitArr &bit_arr);

 private:
  uint32_t bits_num_;
  char *bits_;
  bool is_init_;
};

}  // namespace base

#endif
