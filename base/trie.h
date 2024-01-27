// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRIE_H_
#define BASE_TRIE_H_

#include <stdint.h>

#include <string>

#include "base/common.h"
#include "base/status.h"

namespace base {

struct TrieItem {
  TrieItem *index[kNumOfLowerCaseLetter];
  uint32_t frequency;
  uint32_t full_word_frequency;

  void Clear() { /*{{{*/
    memset(index, 0, sizeof(index));
    frequency = 0;
    full_word_frequency = 0;
  } /*}}}*/
};

/**
 * Note: current trie only store lower case letter
 */
class Trie {
 public:
  Trie();
  ~Trie();

 public:
  Code Put(const std::string &key);
  Code Get(const std::string &key, uint32_t *full_word_frequency);
  Code Del(const std::string &key);

  Code ToString(std::string *info);

 public:
  Code Init();

 private:
  Code Destroy(TrieItem *trie_item);
  Code ToString(const std::string &prefix, const TrieItem *trie_item, std::string *info);

 private:
  TrieItem *root_;
  bool is_init_;
};

}  // namespace base

#endif
