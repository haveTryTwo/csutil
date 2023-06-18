// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_HASHTABLE_H_
#define MY_HASHTABLE_H_

#include "my_simple_alloc.h"
#include "my_stl_algobase.h"
#include "my_stl_construct.h"
#include "my_stl_function.h"
#include "my_stl_hash_fun.h"
#include "my_stl_iterator.h"
#include "my_stl_pair.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"
#include "my_vector.h"

static const int __my_stl_num_primes = 28;
static const unsigned long __my_stl_prime_list[__my_stl_num_primes] = {
    53,        97,        193,       389,       769,        1543,         3079,
    6151,      12289,     24593,     49157,     98317,      196613,       393241,
    786433,    1572869,   3145739,   6291469,   12582917,   25165843,     50331653,
    100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473ul, 4294967291ul};

// in the 28 primes, find near or equal to that prime
inline unsigned long __my_stl_next_prime(unsigned long n) {
  const unsigned long* first = __my_stl_prime_list;
  const unsigned long* last = __my_stl_prime_list + __my_stl_num_primes;
  const unsigned long* pos = my_lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}

/***********************************************************************
 * hashtable_node
 ***********************************************************************/
template <class Value>
class __my_hashtable_node {
 public:
  __my_hashtable_node* next;
  Value val;
};

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
class my_hashtable;

/***********************************************************************
 * hashtable_iterator
 ***********************************************************************/
template <class Value, class Key, class Ref, class Pointer, class HashFun, class ExtractKey,
          class EqualKey, class Alloc>
class __my_hashtable_iterator {
 public:
  typedef my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc> hashtable;
  typedef __my_hashtable_iterator<Value, Key, Ref, Pointer, HashFun, ExtractKey, EqualKey, Alloc>
      iterator;
  //	typedef __my_hashtable_iterator<Value, Key, const Ref, const Pointer, HashFun, ExtractKey,
  //EqualKey, Alloc> const_iterator;

  typedef __my_hashtable_node<Value> node;

 public:
  typedef my_forward_iterator_tag iterator_category;
  typedef Value value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef Ref reference;
  typedef Pointer pointer;

 public:
  node* cur;      // iterator point to the node
  hashtable* ht;  // keep the relation of containter if the situation that from one bucket to
                  // another bucket

  __my_hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}

  __my_hashtable_iterator() {}

  template <class V, class K, class R, class P, class H, class ExK, class EqK, class A>
  __my_hashtable_iterator(__my_hashtable_iterator<V, K, R, P, H, ExK, EqK, A> x) {
    cur = x.cur;
    ht = x.ht;
  }

  template <class V, class K, class R, class P, class H, class ExK, class EqK, class A>
  iterator& operator=(__my_hashtable_iterator<V, K, R, P, H, ExK, EqK, A> x) {
    if ((void*)this == (void*)&x) {
      return *this;
    }

    cur = x.cur;
    ht = x.ht;
    return *this;
  }

 public:
  reference operator*() const { return cur->val; }

  pointer operator->() const { return &(operator*()); }

  iterator& operator++();
  iterator operator++(int);

  bool operator==(const iterator& it) { return this->cur == it.cur; }

  bool operator!=(const iterator& it) { return this->cur != it.cur; }
};

template <class Value, class Key, class Ref, class Pointer, class HashFun, class ExtractKey,
          class EqualKey, class Alloc>
__my_hashtable_iterator<Value, Key, Ref, Pointer, HashFun, ExtractKey, EqualKey, Alloc>&
__my_hashtable_iterator<Value, Key, Ref, Pointer, HashFun, ExtractKey, EqualKey,
                        Alloc>::operator++() {
  const node* old = cur;
  cur = cur->next;
  if (!cur) {
    // current bucket is in the end, and the start is the next bucket
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size()) {
      cur = ht->buckets[bucket];
    }
  }
  return *this;
}

template <class Value, class Key, class Ref, class Pointer, class HashFun, class ExtractKey,
          class EqualKey, class Alloc>
__my_hashtable_iterator<Value, Key, Ref, Pointer, HashFun, ExtractKey, EqualKey, Alloc>
__my_hashtable_iterator<Value, Key, Ref, Pointer, HashFun, ExtractKey, EqualKey, Alloc>::operator++(
    int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

/***********************************************************************
 * hashtable
 ***********************************************************************/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey,
          class Alloc = my_alloc>
class my_hashtable {
 public:
  typedef HashFun hasher;
  typedef EqualKey key_equal;
  typedef size_t size_type;

  typedef Key key_type;
  typedef Value value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef ptrdiff_t difference_type;

 private:
  hasher hash;
  key_equal equals;
  ExtractKey get_key;

  typedef __my_hashtable_node<Value> node;
  typedef my_simple_alloc<node, Alloc> node_allocator;
  typedef my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc> hashtable;

  size_type num_elements;  // number of the exist nodes in the hashtable

  my_vector<node*, Alloc> buckets;  /// use vector to finish the functions

  // make the hashtable_iterator to be friend class, so the iterator can call the buckets
  friend class __my_hashtable_iterator<Value, Key, Value&, Value*, HashFun, ExtractKey, EqualKey,
                                       Alloc>;
  friend class __my_hashtable_iterator<Value, Key, const Value&, const Value*, HashFun, ExtractKey,
                                       EqualKey, Alloc>;

 public:
  typedef __my_hashtable_iterator<Value, Key, Value&, Value*, HashFun, ExtractKey, EqualKey, Alloc>
      iterator;
  typedef __my_hashtable_iterator<Value, Key, const Value&, const Value*, HashFun, ExtractKey,
                                  EqualKey, Alloc>
      const_iterator;

 public:
  node* new_node(const value_type& obj) {
    node* n = node_allocator::allocate();
    n->next = 0;
    try {
      my_construct(&n->val, obj);
      return n;
    } catch (...) {
      node_allocator::deallocate(n);
    }
  }

  void delete_node(node* n) {
    my_destroy(&n->val);
    node_allocator::deallocate(n);
  }

 public:
  my_hashtable(size_type n, const HashFun& hf, const EqualKey& eql)
      : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
    initialize_buckets(n);
  }

 private:
  void initialize_buckets(size_type n) {
    const size_type n_buckets = next_size(n);
    buckets.reserve(n_buckets);
    buckets.insert(buckets.end(), n_buckets, (node*)0);
    num_elements = 0;
  }

  size_type next_size(unsigned long n) const { return __my_stl_next_prime(n); }

 public:
  // find the max size of prime
  size_type max_bucket_count() const { return __my_stl_prime_list[__my_stl_num_primes - 1]; }

  size_type bucket_count() const { return buckets.size(); }

  // in one bucket, count the number in this bucket
  size_type elems_in_bucket(size_type n) {
    if (n < buckets.size()) {
      size_type num = 0;
      for (node* cur = buckets[n]; cur; cur = cur->next) {
        ++num;
      }
      return num;
    } else {
      return 0;
    }
  }

 public:
  iterator begin() {
    if (num_elements == 0) {
      return iterator((node*)0, this);
    } else {
      for (size_type bucket = 0; bucket < buckets.size(); ++bucket) {
        node* cur = buckets[bucket];
        if (cur != 0) {
          return iterator(cur, this);
        }
      }
    }
  }

  const_iterator begin() const {
    if (num_elements == 0) {
      return const_iterator((node*)0, const_cast<hashtable*>(this));
    } else {
      for (size_type bucket = 0; bucket < buckets.size(); ++bucket) {
        node* cur = buckets[bucket];
        if (cur != 0) {
          return const_iterator(cur, const_cast<hashtable*>(this));
        }
      }
    }
  }

  iterator end() { return iterator((node*)0, this); }

  const_iterator end() const { return const_iterator((node*)0, const_cast<hashtable*>(this)); }

  size_type size() const { return num_elements; }

  size_type max_size() const { return -1; }

  bool empty() const { return num_elements == 0; }

  void swap(my_hashtable& x) {
    my_swap(num_elements, x.num_elements);
    buckets.swap(x.buckets);
  }

  hasher hash_func() const { return hash; }

  key_equal key_eq() const { return equals; }

 public:
  my_pair<iterator, bool> insert_unique(const value_type& obj) {
    resize(num_elements + 1);
    return insert_unique_noresize(obj);
  }

  iterator insert_equal(const value_type& obj) {
    resize(num_elements + 1);
    return insert_equal_noresize(obj);
  }

  template <class InputIterator>
  void insert_unique(InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
      insert_unique(*first);
    }
  }

  template <class InputIterator>
  void insert_equal(InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
      insert_equal(*first);
    }
  }

  void clear();
  void copy_from(const hashtable& ht);

  size_type bkt_num(const value_type& obj, size_t n) const { return bkt_num_key(get_key(obj), n); }

  size_type bkt_num(const value_type& obj) const { return bkt_num_key(get_key(obj)); }

  size_type bkt_num_key(const key_type& key) const { return bkt_num_key(key, buckets.size()); }

  size_type bkt_num_key(const key_type& key, size_t n) const { return hash(key) % n; }

 public:
  void resize(size_type num_elements_hint);

  my_pair<iterator, bool> insert_unique_noresize(const value_type& obj);
  iterator insert_equal_noresize(const value_type& obj);

  size_type erase(const key_type& key);
  void erase(iterator position);
  void erase(const_iterator position);
  void erase(iterator first, iterator last);
  void erase(const_iterator first, const_iterator last);

 public:
  iterator lower_bound(const key_type& key);
  const_iterator lower_bound(const key_type& key) const;

  iterator upper_bound(const key_type& key);
  const_iterator upper_bound(const key_type& key) const;

  iterator find(const key_type& key);
  const_iterator find(const key_type& key) const;

  value_type& find_or_insert(const value_type& obj);

  size_type count(const key_type& key);
  size_type count(const key_type& key) const;

  my_pair<iterator, iterator> equal_range(const key_type& key);
  my_pair<const_iterator, const_iterator> equal_range(const key_type& key) const;
};

/*******
 * find_or_insert :
 ********/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::value_type&
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::find_or_insert(
    const value_type& obj) {
  iterator it = find(get_key(obj));
  if (it != end()) {
    return *it;
  } else {
    my_pair<iterator, bool> p = insert_unique(obj);
    it = p.first;
    return *it;
  }
}

/******
 * erase :
 *******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::size_type
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::erase(const key_type& key) {
  size_type count_num = count(key);
  if (count_num == 0) {
    // if there is no such key
    return 0;
  }

  size_type n = bkt_num_key(key);
  node* first = buckets[n];
  my_pair<iterator, iterator> p = equal_range(key);
  if (first == p.first.cur) {
    buckets[n] = p.second.cur;

  } else {
    while (first->next != p.first.cur) {
      first = first->next;
    }
    first->next = p.second.cur;
  }

  // free the space
  iterator it = p.first;
  while (it != p.second) {
    iterator tmp = it;
    ++tmp;
    delete_node(it.cur);
    it = tmp;
  }

  // substarct the number of nodes to be the rest number
  num_elements = num_elements - count_num;
  return count_num;
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
inline void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::erase(
    iterator position) {
  if (position == end()) {
    return;
  }

  size_type n = bkt_num(*position);
  node* first = buckets[n];
  if (first == position.cur) {
    buckets[n] = position.cur->next;
  } else {
    while (first->next != position.cur) {
      first = first->next;
    }
    first->next = position.cur->next;
  }
  delete_node(position.cur);
  --num_elements;
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
inline void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::erase(
    const_iterator position) {
  if (position == end()) {
    return;
  }

  size_type n = bkt_num(*position);
  node* first = buckets[n];
  if (first == position.cur) {
    buckets[n] = position.cur->next;
  } else {
    while (first->next != position.cur) {
      first = first->next;
    }
    first->next = position.cur->next;
  }
  delete_node(position.cur);
  --num_elements;
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
inline void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::erase(iterator first,
                                                                                  iterator last) {
  if (first == last) {
    return;
  }

  if (first == begin() && last == end()) {
    clear();
    return;
  }

  while (first != last) {
    iterator tmp = first;
    ++tmp;
    erase(first);
    first = tmp;
  }
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
inline void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::erase(
    const_iterator first, const_iterator last) {
  if (first == last) {
    return;
  }

  if (first == begin() && last == end()) {
    clear();
    return;
  }

  while (first != last) {
    const_iterator tmp = first;
    ++tmp;
    erase(first);
    first = tmp;
  }
}

/*****
 * function: resize
 * : to resize the size of the hashtable
 ******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::resize(
    size_type num_elements_hint) {
  const size_type old_n = buckets.size();
  if (num_elements_hint > old_n) {
    // if the new size is bigger, so reallocate the size
    const size_type n = next_size(num_elements_hint);
    if (n > old_n) {
      my_vector<node*, Alloc> tmp(n, (node*)0);
      try {
        for (size_type bucket = 0; bucket < old_n; ++bucket) {
          node* first = buckets[bucket];
          while (first) {
            size_type new_bucket = bkt_num(first->val, n);
            buckets[bucket] = first->next;
            first->next = tmp[new_bucket];  // put the value in the new bucket
            tmp[new_bucket] = first;
            first = buckets[bucket];
          }
        }
        buckets.swap(tmp);
      } catch (...) {
        throw;
      }
    }
  }
}

/*****
 * inset_unique_noresize :
 ******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
my_pair<typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator, bool>
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(
    const value_type& obj) {
  const size_type n = bkt_num(obj);
  node* first = buckets[n];
  for (node* cur = first; cur; cur = cur->next) {
    if (equals(get_key(obj), get_key(cur->val))) {
      return my_pair<iterator, bool>(iterator(cur, this), false);
    }
  }

  node* tmp = new_node(obj);
  tmp->next = buckets[n];
  buckets[n] = tmp;
  ++num_elements;  // add the number of nodes
  return my_pair<iterator, bool>(iterator(tmp, this), true);
}

/*******
 * insert_equal_noresize :
 ********/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(
    const value_type& obj) {
  const size_type n = bkt_num(obj);
  node* first = buckets[n];

  for (node* cur = first; cur; cur = cur->next) {
    if (equals(get_key(obj), get_key(cur->val))) {
      node* tmp = new_node(obj);
      tmp->next = cur->next;
      cur->next = tmp;
      ++num_elements;
      return iterator(tmp, this);
    }
  }

  node* tmp = new_node(obj);
  tmp->next = buckets[n];
  buckets[n] = tmp;
  ++num_elements;
  return iterator(tmp, this);
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::clear() {
  for (size_type i = 0; i < buckets.size(); ++i) {
    node* cur = buckets[i];
    while (cur != 0) {
      node* next = cur->next;
      delete_node(cur);
      cur = next;
    }
    buckets[i] = 0;
  }

  num_elements = 0;  // make the number of node to zero]
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
void my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::copy_from(
    const my_hashtable& ht) {
  buckets.clear();
  buckets.reserve(ht.buckets.size());
  buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
  try {
    for (size_type i = 0; i < ht.buckets.size(); ++i) {
      if (const node* cur = ht.buckets[i]) {
        node* copy = new_node(cur->val);
        buckets[i] = copy;
        for (node* next = cur->next; next; cur = next, next = cur->next) {
          copy->next = new_node(next->val);
          copy = copy->next;
        }
      }
    }
    num_elements = ht.num_elements;
  } catch (...) {
    clear();
  }
}

/****
 * equal_range : find the range that all of the keys are equal to key
 ******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
my_pair<typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator,
        typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator>
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::equal_range(const key_type& key) {
  iterator first = lower_bound(key);
  iterator last = upper_bound(key);
  return my_pair<iterator, iterator>(first, last);
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
my_pair<typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator,
        typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator>
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::equal_range(
    const key_type& key) const {
  const_iterator first = lower_bound(key);
  const_iterator last = upper_bound(key);
  return my_pair<const_iterator, const_iterator>(first, last);
}

/*****
 * count : count the number that the node's key is equal to the key
 ******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::size_type
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::count(const key_type& key) {
  my_pair<iterator, iterator> p = equal_range(key);
  const size_type n = my_distance(p.first, p.second);
  return n;
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::size_type
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::count(const key_type& key) const {
  my_pair<const_iterator, const_iterator> p = equal_range(key);
  const size_type n = my_distance(p.first, p.second);
  return n;
}

/****
 * find
 ******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::find(const key_type& key) {
  return lower_bound(key);
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::find(const key_type& key) const {
  return lower_bound(key);
}

/***
 * lower bound
 *****/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::lower_bound(const key_type& key) {
  if (empty()) {
    return iterator((node*)0, this);
  }

  size_type n = bkt_num_key(key);
  node* first = 0;
  for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {
  }

  return iterator(first, this);
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::lower_bound(
    const key_type& key) const {
  if (empty()) {
    return iterator((node*)0, this);
  }

  size_type n = bkt_num_key(key);
  node* first = 0;
  for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {
  }

  return const_iterator(first, const_cast<hashtable*>(this));
}

/*****
 * upper bound
 *******/
template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::upper_bound(const key_type& key) {
  if (empty()) {
    return iterator((node*)0, this);
  }

  size_type n = bkt_num_key(key);
  node* first = 0;
  for (first = buckets[n]; first; first = first->next) {
    if (equals(get_key(first->val), key)) {
      node* tmp = first->next;
      for (; tmp; tmp = tmp->next) {
        if (equals(get_key(tmp->val), key)) {
        } else {
          // if the key is not equal to the tmp->key, then return this as the upper
          return iterator(tmp, this);
        }
      }
      if (!tmp) {
        // if tmp is 0, then find the next iterator
        size_type num = n;
        while (!tmp && ++num < buckets.size()) {
          tmp = buckets[num];
        }
        return iterator(tmp, this);
      }
    }
  }

  return iterator((node*)0, this);
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator
my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::upper_bound(
    const key_type& key) const {
  if (empty()) {
    return const_iterator((node*)0, const_cast<hashtable*>(this));
  }

  size_type n = bkt_num_key(key);
  node* first = 0;
  for (first = buckets[n]; first; first = first->next) {
    if (equals(get_key(first->val), key)) {
      node* tmp = first->next;
      for (; tmp; tmp = tmp->next) {
        if (equals(get_key(tmp->val), key)) {
        } else {
          // if the key is not equal to the tmp->key, then return this as the upper
          return const_iterator(tmp, const_cast<hashtable*>(this));
        }
      }
      if (!tmp) {
        // if tmp is 0, then find the next iterator
        size_type num = n;
        while (!tmp && ++num < buckets.size()) {
          tmp = buckets[num];
        }
        return const_iterator(tmp, const_cast<hashtable*>(this));
      }
    }
  }

  return const_iterator((node*)0, const_cast<hashtable*>(this));
}

template <class Value, class Key, class HashFun, class ExtractKey, class EqualKey, class Alloc>
bool operator==(const my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>& x,
                const my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>& y) {
  if (x.size() == y.size()) {
    typedef typename my_hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>::const_iterator
        const_iterator;

    const_iterator end1 = x.end();
    const_iterator end2 = y.end();

    const_iterator begin1 = x.begin();
    const_iterator begin2 = y.begin();

    while (begin1 != end1 && begin2 != end2 && *begin1 == *begin2) {
      ++begin1;
      ++begin2;
    }

    return begin1 == end1 && begin2 == end2;
  }

  return false;
}

#endif
