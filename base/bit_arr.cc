// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bit_arr.h"

#include <string.h>

#include "base/common.h"

namespace base {

/**
 * @brief Constructor of BitArr
 * @param bits_num Number of bits in the bit array
 */
BitArr::BitArr(uint32_t bits_num) : bits_num_(bits_num), bits_(NULL), is_init_(false) {}

/**
 * @brief Destructor of BitArr
 */
BitArr::~BitArr() {
  if (bits_ != NULL) {
    delete[] bits_;
    bits_ = NULL;
  }
}

/**
 * @brief Initialize the bit array and allocate memory
 * @return kOk if success, kInvalidParam if bits_num is 0, kNewFailed if memory allocation fails
 */
Code BitArr::Init() {
  if (bits_num_ == 0) return kInvalidParam;

  uint32_t bytes_num = (bits_num_ + kBitsPerByte - 1) / kBitsPerByte;
  bits_ = new char[bytes_num];
  if (bits_ == NULL) return kNewFailed;
  memset(bits_, 0, bytes_num);
  is_init_ = true;

  return kOk;
}

/**
 * @brief Set the bit at the specified index to the given value
 * @param index The index of the bit to set (0-based)
 * @param value The boolean value to set (true for 1, false for 0)
 * @return kOk if success, kInvalidParam if index is out of range, kNotInit if not initialized
 */
Code BitArr::Put(uint32_t index, bool value) {
  if (index >= bits_num_) return kInvalidParam;
  if (!is_init_) return kNotInit;

  uint32_t bytes_index = index / kBitsPerByte;
  uint32_t bits_index = index % kBitsPerByte;
  uint8_t bits_value = 1 << (kBitsPerByte - bits_index - 1);

  if (value) {
    bits_[bytes_index] |= bits_value;
  } else {
    bits_value = ~bits_value;
    bits_[bytes_index] &= bits_value;
  }

  return kOk;
}

/**
 * @brief Get the bit value at the specified index
 * @param index The index of the bit to get (0-based)
 * @param value Pointer to store the bit value (true for 1, false for 0)
 * @return kOk if success, kInvalidParam if index is out of range or value is NULL, kNotInit if not initialized
 */
Code BitArr::Get(uint32_t index, bool *value) {
  if (index >= bits_num_ || value == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  *value = false;

  uint32_t bytes_index = index / kBitsPerByte;
  uint32_t bits_index = index % kBitsPerByte;
  uint8_t bits_value = 1 << (kBitsPerByte - bits_index - 1);

  uint8_t byte = (bits_[bytes_index] & bits_value);
  if (byte != 0) {
    *value = true;
  }

  return kOk;
}

/**
 * @brief Clear all bits in the array (set all bits to 0)
 * @return kOk if success, kNotInit if not initialized
 */
Code BitArr::Clear() {
  if (!is_init_) return kNotInit;

  uint32_t bytes_num = (bits_num_ + kBitsPerByte - 1) / kBitsPerByte;
  memset(bits_, 0, bytes_num);

  return kOk;
}

/**
 * @brief Get the total number of bits in the array
 * @param size Pointer to store the size of the bit array
 * @return kOk if success, kInvalidParam if size is NULL, kNotInit if not initialized
 */
Code BitArr::Size(uint32_t *size) {
  if (size == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  *size = bits_num_;

  return kOk;
}

/**
 * @brief Convert the bit array to a string representation
 * @param str Pointer to store the string representation (most significant bit first)
 * @return kOk if success, kInvalidParam if str is NULL, kNotInit if not initialized
 */
Code BitArr::ToString(std::string *str) {
  if (str == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  for (int32_t index = static_cast<int32_t>(bits_num_) - 1; index >= 0; --index) {
    bool value = false;
    Code ret = Get(static_cast<uint32_t>(index), &value);
    if (ret != kOk) return ret;

    if (value) {
      str->append(1, '1');
    } else {
      str->append(1, '0');
    }
  }

  return kOk;
}

}  // namespace base
