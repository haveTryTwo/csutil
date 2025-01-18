// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_HASH_H_
#define BASE_HASH_H_

#include <stdint.h>

#include <string>

#include "base/status.h"

namespace base {

/**
 * BKDR Hash
 */
uint32_t BKDRHash(const char *str);

uint32_t BKDRHash(uint32_t num);

uint32_t CRC32(const char *str, int len);

/**
 * Note: https://zh.wikipedia.org/wiki/Murmur哈希
 */
uint32_t Murmur32(const std::string &key, uint32_t seed);

size_t HashString(const char *s);

template <typename Container, typename Key>
typename Container::const_iterator GetEqualOrUpperBound(const Container &container, const Key &key) { /*{{{*/
  typename Container::const_iterator it;
  it = container.find(key);
  if (it != container.end()) {
    return it;
  }

  it = container.upper_bound(key);
  return it;
} /*}}}*/

size_t HashCombine(size_t lhs, size_t rhs);

// highest random weight (HRW) hashing
// Note: struct Server contain id field 
template <typename Server>
Code HRWHash(const std::string &key, const std::vector<Server> &servers, Server *best_server) {
  if (servers.size() == 0 || best_server == NULL) return kInvalidParam;

  size_t highest_weight = 0;

  for (const auto &server : servers) {
    size_t server_hash = std::hash<std::string>{}(server.id);
    size_t key_hash = std::hash<std::string>{}(key);
    size_t combined_hash = HashCombine(key_hash, server_hash);

    // fprintf(stderr, "key:%s, server:%s, hash:%zu\n", key.c_str(), server.id.c_str(), combined_hash);
    if (highest_weight == 0 || combined_hash > highest_weight) {
      highest_weight = combined_hash;
      *best_server = server;
    }
  }

  return kOk;
}

}  // namespace base

#endif
