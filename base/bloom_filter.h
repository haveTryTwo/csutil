// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BLOOM_FILTER_H_
#define BASE_BLOOM_FILTER_H_

#include <stdint.h>

#include <string>

#include "base/status.h"

namespace base {

/**
 * Bloom Filter implementation for probabilistic set membership testing
 *
 * A Bloom filter is a space-efficient probabilistic data structure that is used
 * to test whether an element is a member of a set. False positive matches are
 * possible, but false negatives are not. In other words, a query returns either
 * "possibly in set" or "definitely not in set".
 *
 * This implementation uses Murmur32 hash function and generates multiple hash
 * values through bit rotation to simulate multiple independent hash functions.
 *
 * Usage example:
 *   BloomFilter bloom(10, 1000, 3);  // 10 bits per key, 1000 keys, 3 hash functions
 *   bloom.Init();
 *   bloom.Put("key1");
 *   bool exist = false;
 *   bloom.CheckExist("key1", &exist);  // exist will be true
 */
class BloomFilter {
 public:
  /**
   * Constructor
   *
   * Creates a Bloom filter with specified parameters. The filter must be
   * initialized by calling Init() before use.
   *
   * @param bits_per_key Number of bits allocated per key in the filter.
   *                     Higher values reduce false positive rate but increase
   *                     memory usage. Typical values: 8-16.
   * @param keys_num Expected number of keys to be inserted into the filter.
   *                 Used to calculate the total bit array size.
   * @param hash_num Number of hash functions to use. More hash functions reduce
   *                 false positive rate but increase computation cost.
   *                 Typical values: 3-7.
   */
  BloomFilter(uint32_t bits_per_key, uint32_t keys_num, uint32_t hash_num);

  /**
   * Destructor
   *
   * Frees the allocated bit array memory.
   */
  ~BloomFilter();

 public:
  /**
   * Initialize the Bloom filter
   *
   * Allocates and initializes the bit array based on the parameters specified
   * in the constructor. This method must be called before using Put() or
   * CheckExist().
   *
   * @return base::kOk if initialization succeeds
   * @return base::kInvalidLength if the calculated bit array size exceeds UINT_MAX
   * @return base::kNewFailed if memory allocation fails
   */
  Code Init();

  /**
   * Add a key to the Bloom filter
   *
   * Inserts a key into the filter by setting the corresponding bits in the
   * bit array. The key is hashed multiple times (hash_num_ times) using
   * different hash values derived from bit rotation, and each hash value
   * determines a bit position to set.
   *
   * @param key The key string to add to the filter
   * @return base::kOk if the key is successfully added
   * @return base::kNotInit if Init() has not been called
   */
  Code Put(const std::string &key);

  /**
   * Check if a key might exist in the filter
   *
   * Checks whether a key might be in the filter by verifying if all hash
   * positions are set. If any bit is not set, the key is definitely not
   * in the filter. If all bits are set, the key might be in the filter
   * (with a certain false positive probability).
   *
   * Note: This method can return false positives (says key exists when it doesn't)
   *       but never returns false negatives (says key doesn't exist when it does).
   *
   * @param key The key string to check
   * @param exist Output parameter: set to true if the key might exist,
   *              false if it definitely doesn't exist
   * @return base::kOk if the check completes successfully
   * @return base::kInvalidParam if exist is NULL
   * @return base::kNotInit if Init() has not been called
   */
  Code CheckExist(const std::string &key, bool *exist);

  /**
   * Get the bit array as a byte string
   *
   * Retrieves the internal bit array representation as a string of bytes.
   * This can be used for serialization, persistence, or transmission of
   * the Bloom filter state.
   *
   * @param arr Output parameter: the bit array as a byte string
   * @return base::kOk if the operation succeeds
   * @return base::kInvalidParam if arr is NULL
   * @return base::kNotInit if Init() has not been called
   */
  Code GetBytesStr(std::string *arr);

 private:
  uint32_t bits_per_key_;  ///< Number of bits allocated per key
  uint32_t keys_num_;      ///< Expected number of keys to be inserted
  uint32_t hash_num_;      ///< Number of hash functions to use
  uint8_t *bytes_;         ///< Pointer to the bit array (byte array)
  uint32_t bytes_size_;    ///< Size of the bit array in bytes
  bool is_init_;           ///< Flag indicating whether Init() has been called
};

}  // namespace base

#endif
