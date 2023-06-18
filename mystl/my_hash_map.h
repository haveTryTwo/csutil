// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_HASH_MAPH_H_
#define MY_HASH_MAPH_H_

#include "my_hashtable.h"

template <class Key, class T, class HashFun = my_hash<Key>, class EqualKey = my_equal_to<Key>,
          class Alloc = my_alloc>
class my_hash_map {
 private:
  typedef my_hashtable<my_pair<const Key, T>, Key, HashFun, my_select1st<my_pair<const Key, T> >,
                       EqualKey, Alloc>
      ht;
  ht rep;

 public:
  typedef typename ht::key_type key_type;
  typedef T data_type;
  typedef T mapped_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::reference reference;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::iterator iterator;
  typedef typename ht::const_iterator const_iterator;

  hasher hash_func() const { return rep.hash_func(); }

  key_equal key_eq() const { return rep.key_eq(); }

 public:
  my_hash_map() : rep(100, hasher(), key_equal()) {}

  explicit my_hash_map(size_type n) : rep(n, hasher(), key_equal()) {}

  my_hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}

  my_hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}

  template <class InputIterator>
  my_hash_map(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_map(InputIterator first, InputIterator last, size_type n)
      : rep(n, hasher(), key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_map(InputIterator first, InputIterator last, size_type n, const hasher& hf)
      : rep(n, hf, key_equal()) {
    rep.insert_unique(first, last);
  }

  template <class InputIterator>
  my_hash_map(InputIterator first, InputIterator last, size_type n, const hasher& hf,
              const key_equal& eql)
      : rep(n, hf, eql) {
    rep.insert_unique(first, last);
  }

 public:
  size_type size() const { return rep.size(); }

  size_type max_size() const { return rep.max_size(); }

  bool empty() const { return rep.empty(); }

  void swap(my_hash_map& hs) { rep.swap(hs.rep); }

  iterator begin() { return rep.begin(); }

  const_iterator begin() const { return rep.begin(); }

  iterator end() { return rep.end(); }

  const_iterator end() const { return rep.end(); }

 public:
  my_pair<iterator, bool> insert(const value_type& obj) { return rep.insert_unique(obj); }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    rep.insert_unique(first, last);
  }

  my_pair<iterator, bool> insert_noresize(const value_type& obj) {
    return rep.insert_unique_noresize(obj);
  }

  iterator find(const key_type& key) { return rep.find(key); }

  const_iterator find(const key_type& key) const { return rep.find(key); }

  T& operator[](const key_type& key) { return rep.find_or_insert(value_type(key, T())).second; }

  size_type count(const key_type& key) const { return rep.count(key); }

  my_pair<iterator, iterator> equal_range(const key_type& key) { return rep.equal_range(key); }

  my_pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
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
  template <class K, class DataType, class H, class E, class A>
  friend bool operator==(const my_hash_map<K, DataType, H, E, A>& x,
                         const my_hash_map<K, DataType, H, E, A>& y);
};

template <class Key, class T, class HashFun, class EqualKey, class Alloc>
bool operator==(const my_hash_map<Key, T, HashFun, EqualKey, Alloc>& x,
                const my_hash_map<Key, T, HashFun, EqualKey, Alloc>& y) {
  return x.rep == y.rep;
}

#endif
