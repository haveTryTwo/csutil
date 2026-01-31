// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bloom_filter.h"

#include <limits.h>
#include <string.h>

#include "base/hash.h"

namespace base {

/**
 * Constructor implementation
 *
 * Initializes member variables but does not allocate memory.
 * Call Init() after construction to allocate the bit array.
 */
BloomFilter::BloomFilter(uint32_t bits_per_key, uint32_t keys_num, uint32_t hash_num)
    : bits_per_key_(bits_per_key),
      keys_num_(keys_num),
      hash_num_(hash_num),
      bytes_(NULL),
      bytes_size_(0),
      is_init_(false) { /*{{{*/ } /*}}}*/

/**
 * Destructor implementation
 *
 * Frees the allocated bit array memory to prevent memory leaks.
 */
BloomFilter::~BloomFilter() { /*{{{*/
  if (bytes_ != NULL) {
    delete[] bytes_;
    bytes_ = NULL;
  }
} /*}}}*/

/**
 * Initialize the Bloom filter bit array
 *
 * Calculates the required bit array size based on keys_num_ and bits_per_key_,
 * allocates memory, and initializes all bits to zero.
 *
 * Bit array size calculation:
 *   - Total bits = keys_num_ * bits_per_key_
 *   - Byte size = (total_bits + 7) / 8  (round up to nearest byte)
 */
Code BloomFilter::Init() { /*{{{*/
  uint64_t bits_size = keys_num_ * bits_per_key_;
  if (bits_size > UINT_MAX) return kInvalidLength;

  // Calculate byte size: round up to nearest byte
  bytes_size_ = (bits_size + 7) / 8;
  bytes_ = new uint8_t[bytes_size_];
  if (bytes_ == NULL) return kNewFailed;
  memset(bytes_, 0, bytes_size_);

  is_init_ = true;
  return kOk;
} /*}}}*/

/**
 * Add a key to the Bloom filter
 *
 * Algorithm:
 * 1. Compute initial hash value using Murmur32 with seed 0xefac1970
 * 2. For each hash function (hash_num_ times):
 *    a. Calculate bit position: hash_value % (total_bits)
 *    b. Set the corresponding bit in the byte array
 *    c. Generate next hash value by rotating and adding:
 *       - Rotate right by 17 bits OR rotate left by 15 bits
 *       - Add to current hash value
 *
 * This bit rotation technique simulates multiple independent hash functions
 * without requiring multiple hash computations.
 */
Code BloomFilter::Put(const std::string &key) { /*{{{*/
  if (!is_init_) return kNotInit;

  // Compute initial hash value using Murmur32 with fixed seed
  uint32_t hash_value = Murmur32(key, 0xefac1970);

  // Generate hash_num_ different hash positions using bit rotation
  for (uint32_t i = 0; i < hash_num_; ++i) {
    // Calculate bit position in the bit array
    uint32_t bits_pos = hash_value % (bytes_size_ * 8);

    // Set the corresponding bit: bytes_[byte_index] |= (1 << bit_offset)
    bytes_[bits_pos / 8] |= 1 << (bits_pos % 8);

    // Generate next hash value by rotating and combining
    // This simulates using a different hash function
    uint32_t tmp_value = (hash_value >> 17) | (hash_value << 15);
    hash_value += tmp_value;
  }

  return kOk;
} /*}}}*/

/**
 * Check if a key might exist in the Bloom filter
 *
 * Algorithm:
 * 1. Compute initial hash value using the same method as Put()
 * 2. For each hash function (hash_num_ times):
 *    a. Calculate bit position: hash_value % (total_bits)
 *    b. Check if the corresponding bit is set
 *    c. If any bit is not set, the key definitely doesn't exist (return early)
 *    d. Generate next hash value using the same rotation method
 * 3. If all bits are set, the key might exist (but could be a false positive)
 *
 * Return value:
 * - *exist = false: Key definitely not in filter (no false negatives)
 * - *exist = true: Key might be in filter (possible false positive)
 */
Code BloomFilter::CheckExist(const std::string &key, bool *exist) { /*{{{*/
  if (exist == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  *exist = false;

  // Compute initial hash value using the same method as Put()
  uint32_t hash_value = Murmur32(key, 0xefac1970);

  // Check all hash positions
  for (uint32_t i = 0; i < hash_num_; ++i) {
    uint32_t bits_pos = hash_value % (bytes_size_ * 8);

    // If any bit is not set, the key definitely doesn't exist
    if ((bytes_[bits_pos / 8] & (1 << (bits_pos % 8))) == 0) {
      return kOk;  // *exist remains false
    }

    // Generate next hash value using the same rotation method as Put()
    uint32_t tmp_value = (hash_value >> 17) | (hash_value << 15);
    hash_value += tmp_value;
  }

  // All bits are set, key might exist (but could be false positive)
  *exist = true;
  return kOk;
} /*}}}*/

/**
 * Get the bit array as a byte string
 *
 * Copies the internal bit array to the output string. The string will contain
 * exactly bytes_size_ bytes, representing the current state of the Bloom filter.
 *
 * This can be used for:
 * - Serialization: Save filter state to disk
 * - Transmission: Send filter state over network
 * - Persistence: Store filter state in database
 */
Code BloomFilter::GetBytesStr(std::string *arr) { /*{{{*/
  if (arr == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  // Resize string to hold all bytes
  arr->resize(bytes_size_);
  // Copy bit array data to string
  memcpy((char *)(arr->data()), bytes_, bytes_size_);

  return kOk;
} /*}}}*/

}  // namespace base
