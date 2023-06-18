// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_HASH_SET_H_
#define MY_HASH_SET_H_

#include "my_hashtable.h"

template <class Value, class HashFun = my_hash<Value>, class EqualKey = my_equal_to<Value>,
          class Alloc = my_alloc>
class my_hash_set {
 private:
  // use hashtable as the base containter
  typedef my_hashtable<Value, Value, HashFun, my_identity<Value>, EqualKey, Alloc> ht;
  ht rep;

 public:
  typedef typename ht::key_type key_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::const_pointer pointer;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::const_reference reference;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::const_iterator iterator;
  typedef typename ht::const_iterator const_iterator;

  hasher hash_func() const { return rep.hash_func(); }

  key_equal key_eq() const { return rep.key_eq(); }

 public:
  my_hash_set() : rep(100, hasher(), key_equal()) {}

  explicit my_hash_set(size_type n) : rep(n, hasher(), key_equal()) {}

  my_hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}

  my_hash_set(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}

  // the next operators use insert_unique()
  template <class InputIterator>
  my_hash_set(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_set(InputIterator first, InputIterator last, size_type n)
      : rep(n, hasher(), key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf)
      : rep(n, hf, key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf,
              const key_equal& eql)
      : rep(n, hf, eql) {
    rep.insert_unique(first, last);
  }

 public:
  size_type size() const { return rep.size(); }

  size_type max_size() const { return rep.max_size(); }

  bool empty() const { return rep.empty(); }

  void swap(my_hash_set& hs) { rep.swap(hs.rep); }

  iterator begin() const { return rep.begin(); }

  iterator end() const { return rep.end(); }

 public:
  my_pair<iterator, bool> insert(const value_type& obj) {
    my_pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
    return my_pair<iterator, bool>(p.first, p.second);
  }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    rep.insert_unique(first, last);
  }

  my_pair<iterator, bool> insert_noresize(const value_type& obj) {
    my_pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
    return my_pair<iterator, bool>(p.first, p.second);
  }

  iterator find(const key_type& key) const { return rep.find(key); }

  size_type count(const key_type& key) const { return rep.count(key); }

  my_pair<iterator, iterator> equal_range(const key_type& key) const {
    return rep.equal_range(key);
  }

  size_type erase(const key_type& key) { return rep.erase(key); }

  void erase(iterator it) { rep.erase(it); }

  void erase(iterator first, iterator last) { rep.erase(first, last); }

  void clear() { rep.clear(); }

 public:
  void resize(size_type hint) { rep.resize(hint); }

  size_type bucket_count() const { return rep.bucket_count(); }

  size_type max_bucket_count() const { return rep.max_bucket_count(); }

  size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }

 public:
  template <class V, class H, class E, class A>
  friend bool operator==(const my_hash_set<V, H, E, A>& hs1, const my_hash_set<V, H, E, A>& hs2);
};

template <class Value, class HashFun, class EqualKey, class Alloc>
inline bool operator==(const my_hash_set<Value, HashFun, EqualKey, Alloc>& hs1,
                       const my_hash_set<Value, HashFun, EqualKey, Alloc>& hs2) {
  return hs1.rep == hs2.rep;
}

#endif
