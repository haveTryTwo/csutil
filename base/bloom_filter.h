// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BLOOM_FILTER_H_
#define BASE_BLOOM_FILTER_H_

#include <stdint.h>

#include <string>

#include "base/status.h"

namespace base {

class BloomFilter {
 public:
  BloomFilter(uint32_t bits_per_key, uint32_t keys_num, uint32_t hash_num);
  ~BloomFilter();

 public:
  Code Init();
  Code Put(const std::string &key);
  Code CheckExist(const std::string &key, bool *exist);
  Code GetBytesStr(std::string *arr);

 private:
  uint32_t bits_per_key_;
  uint32_t keys_num_;
  uint32_t hash_num_;
  uint8_t *bytes_;
  uint32_t bytes_size_;
  bool is_init_;
};

}  // namespace base

#endif
