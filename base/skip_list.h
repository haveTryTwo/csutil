// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SKIP_LIST_H_
#define BASE_SKIP_LIST_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "base/random.h"
#include "base/status.h"

namespace base {

#define kMaxHeight 20

template <typename KeyType, typename ValueType, typename Compare>
class SkipList {
 private:
  struct SkipNode;

 public:
  SkipList();
  ~SkipList();

 public:
  Code Put(const KeyType &key, const ValueType &value);
  Code Get(const KeyType &key, ValueType *value);
  Code Del(const KeyType &key);

 public:
  Code Init();
  Code Destroy();

 public:
  class Iterator { /*{{{*/
   public:
    Iterator(const SkipList *skip_list) : skip_list_(skip_list), cur_node_(NULL) { /*{{{*/
    }                                                                              /*}}}*/

    Code SetFirst() { /*{{{*/
      cur_node_ = skip_list_->head_->next[0];
      return kOk;
    } /*}}}*/

    Code GoNext() { /*{{{*/
      if (!Valid()) return kInvalidParam;
      cur_node_ = cur_node_->next[0];
      return kOk;
    } /*}}}*/

    bool Valid() { /*{{{*/
      if (cur_node_ != NULL) return true;
      return false;
    } /*}}}*/

    Code GetKey(KeyType *key) { /*{{{*/
      if (key == NULL || !Valid()) return kInvalidParam;
      *key = cur_node_->key;
      return kOk;
    } /*}}}*/

    Code GetValue(ValueType *value) { /*{{{*/
      if (value == NULL || !Valid()) return kInvalidParam;
      *value = cur_node_->value;
      return kOk;
    } /*}}}*/

    Code GetHeight(int *height) { /*{{{*/
      if (height == NULL || !Valid()) return kInvalidParam;
      *height = cur_node_->height;
      return kOk;
    } /*}}}*/
   private:
    const SkipList *skip_list_;
    SkipNode *cur_node_;
  }; /*}}}*/

 private:
  SkipList(const SkipList &sl);
  SkipList &operator=(const SkipList &sl);

 private:
#pragma pack(push)
#pragma pack(1)
  struct SkipNode {
    KeyType key;
    ValueType value;
    int height;
    SkipNode *next[0];
  };
#pragma pack(pop)

 private:
  SkipNode *head_;
  int max_height_;
  Compare cmp_;
  bool is_init_;
};

template <typename KeyType, typename ValueType, typename Compare>
SkipList<KeyType, ValueType, Compare>::SkipList() : head_(NULL), max_height_(1), is_init_(false) {}

template <typename KeyType, typename ValueType, typename Compare>
SkipList<KeyType, ValueType, Compare>::~SkipList() {
  Destroy();
}

template <typename KeyType, typename ValueType, typename Compare>
Code SkipList<KeyType, ValueType, Compare>::Put(const KeyType &key,
                                                const ValueType &value) { /*{{{*/
  if (!is_init_) return kNotInit;

  int level = max_height_ - 1;
  assert(level >= 0);
  SkipNode *cur = head_;
  SkipNode *prevs[kMaxHeight] = {NULL};

  while (true) {
    SkipNode *next = cur->next[level];

    if (next == NULL || cmp_(next->key, key) > 0) {
      prevs[level] = cur;
      level--;
      if (level < 0) break;
    } else if (cmp_(next->key, key) == 0) {
      return kExist;
    } else {
      cur = next;
    }
  }

  int new_height = 1;
  Code ret = GetDroppingRand(kMaxHeight, &new_height);
  if (ret != kOk) return ret;

  uint32_t node_size = sizeof(SkipNode) + sizeof(SkipNode *) * new_height;
  char *tmp_mem = new char[node_size];
  if (tmp_mem == NULL) return kNewFailed;
  memset(tmp_mem, 0, node_size);

  SkipNode *new_node = new (tmp_mem) SkipNode;
  if (new_node == NULL) {
    delete[] tmp_mem;
    return kNewFailed;
  }

  new_node->key = key;
  new_node->value = value;
  new_node->height = new_height;

  int tmp_height = new_height;
  if (max_height_ < new_height) {
    for (int i = new_height - 1; i > max_height_ - 1; i--) {
      head_->next[i] = new_node;
      new_node->next[i] = NULL;
    }

    tmp_height = max_height_;
    max_height_ = new_height;
    head_->height = max_height_;
  }

  for (int i = tmp_height - 1; i >= 0; i--) {
    new_node->next[i] = prevs[i]->next[i];
    prevs[i]->next[i] = new_node;
  }

  return kOk;
} /*}}}*/

template <typename KeyType, typename ValueType, typename Compare>
Code SkipList<KeyType, ValueType, Compare>::Get(const KeyType &key, ValueType *value) { /*{{{*/
  if (!is_init_) return kNotInit;

  int level = max_height_ - 1;
  assert(level >= 0);
  SkipNode *cur = head_;
  while (true) {
    SkipNode *next = cur->next[level];
    if (next == NULL || cmp_(next->key, key) > 0) {
      level--;
      if (level < 0) break;
    } else if (cmp_(next->key, key) == 0) {
      *value = next->value;
      return kOk;
    } else {
      cur = next;
    }
  }

  return kNotFound;
} /*}}}*/

template <typename KeyType, typename ValueType, typename Compare>
Code SkipList<KeyType, ValueType, Compare>::Del(const KeyType &key) { /*{{{*/
  if (!is_init_) return kNotInit;

  int level = max_height_ - 1;
  assert(level >= 0);
  SkipNode *cur = head_;
  SkipNode *prevs[kMaxHeight] = {NULL};

  while (true) {
    SkipNode *next = cur->next[level];

    if (next == NULL || cmp_(next->key, key) >= 0) {
      prevs[level] = cur;
      level--;
      if (level < 0) break;
    } else {
      cur = next;
    }
  }
  SkipNode *old = prevs[0]->next[0];
  if (old == NULL || cmp_(old->key, key) > 0) return kNotFound;

  assert(cmp_(old->key, key) == 0);

  int old_height = old->height;
  for (int i = old_height - 1; i >= 0; --i) {
    prevs[i]->next[i] = old->next[i];
  }
  old->~SkipNode();
  char *tmp = (char *)old;
  delete[] tmp;

  assert(max_height_ == head_->height);
  for (int i = max_height_ - 1; i >= 1; --i) {
    if (head_->next[i] == NULL) {
      head_->height--;
    } else {
      break;
    }
  }
  max_height_ = head_->height;

  return kOk;
} /*}}}*/

template <typename KeyType, typename ValueType, typename Compare>
Code SkipList<KeyType, ValueType, Compare>::Init() { /*{{{*/
  uint32_t node_size = sizeof(SkipNode) + sizeof(SkipNode *) * kMaxHeight;
  char *tmp_mem = new char[node_size];
  if (tmp_mem == NULL) return kNewFailed;
  memset(tmp_mem, 0, node_size);

  head_ = new (tmp_mem) SkipNode;
  if (head_ == NULL) {
    delete[] tmp_mem;
    return kNewFailed;
  }
  head_->height = max_height_;

  Code ret = InitRand();
  if (ret != kOk) return ret;

  is_init_ = true;

  return kOk;
} /*}}}*/

template <typename KeyType, typename ValueType, typename Compare>
Code SkipList<KeyType, ValueType, Compare>::Destroy() { /*{{{*/
  if (head_ == NULL) return kOk;

  SkipNode *start = head_;
  while (start != NULL) {
    SkipNode *cur = start;
    start = start->next[0];
    cur->~SkipNode();  // Destructor when key or value has Destructor()
    char *mem = (char *)cur;
    delete[] mem;
  }

  return kOk;
} /*}}}*/

}  // namespace base

#endif
