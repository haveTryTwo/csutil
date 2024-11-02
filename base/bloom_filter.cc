// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bloom_filter.h"

#include <limits.h>
#include <string.h>

#include "base/hash.h"

namespace base {

BloomFilter::BloomFilter(uint32_t bits_per_key, uint32_t keys_num, uint32_t hash_num)
    : bits_per_key_(bits_per_key),
      keys_num_(keys_num),
      hash_num_(hash_num),
      bytes_(NULL),
      bytes_size_(0),
      is_init_(false) { /*{{{*/ } /*}}}*/

BloomFilter::~BloomFilter() { /*{{{*/
  if (bytes_ != NULL) {
    delete[] bytes_;
    bytes_ = NULL;
  }
} /*}}}*/

Code BloomFilter::Init() { /*{{{*/
  uint64_t bits_size = keys_num_ * bits_per_key_;
  if (bits_size > UINT_MAX) return kInvalidLength;

  bytes_size_ = (bits_size + 7) / 8;
  bytes_ = new uint8_t[bytes_size_];
  if (bytes_ == NULL) return kNewFailed;
  memset(bytes_, 0, bytes_size_);

  is_init_ = true;
  return kOk;
} /*}}}*/

Code BloomFilter::Put(const std::string &key) { /*{{{*/
  if (!is_init_) return kNotInit;

  uint32_t hash_value = Murmur32(key, 0xefac1970);
  for (uint32_t i = 0; i < hash_num_; ++i) {
    uint32_t bits_pos = hash_value % (bytes_size_ * 8);
    bytes_[bits_pos / 8] |= 1 << (bits_pos % 8);
    uint32_t tmp_value = (hash_value >> 17) | (hash_value << 15);
    hash_value += tmp_value;
  }

  return kOk;
} /*}}}*/

Code BloomFilter::CheckExist(const std::string &key, bool *exist) { /*{{{*/
  if (exist == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  *exist = false;

  uint32_t hash_value = Murmur32(key, 0xefac1970);
  for (uint32_t i = 0; i < hash_num_; ++i) {
    uint32_t bits_pos = hash_value % (bytes_size_ * 8);
    if ((bytes_[bits_pos / 8] & (1 << (bits_pos % 8))) == 0) return kOk;

    uint32_t tmp_value = (hash_value >> 17) | (hash_value << 15);
    hash_value += tmp_value;
  }

  *exist = true;
  return kOk;
} /*}}}*/

Code BloomFilter::GetBytesStr(std::string *arr) { /*{{{*/
  if (arr == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  arr->resize(bytes_size_);
  memcpy((char *)(arr->data()), bytes_, bytes_size_);

  return kOk;
} /*}}}*/

}  // namespace base
