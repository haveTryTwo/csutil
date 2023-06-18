// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_MAP_H_
#define MY_MAP_H_

#include "my_rb_tree.h"

template <class Key, class T, class Compare = my_less<Key>, class Alloc = my_alloc>
class my_map {
 public:
  typedef Key key_type;                      // key type
  typedef T data_type;                       // data type
  typedef T mapped_type;                     //
  typedef my_pair<const Key, T> value_type;  // the value_file type (key / data);

  typedef Compare key_compare;  // key compare

  // declare class function value_compare, so that can compare the value_type
  class value_compare : public my_binary_function<value_type, value_type, bool> {
    friend class my_map<Key, T, Compare, Alloc>;

   protected:
    Compare comp;
    value_compare(Compare c) : comp(c) {}

   public:
    bool operator()(const value_type& x, const value_type& y) const {
      return comp(x.first, y.first);
    }
  };

 private:
  // using the my_rb_tree,
  typedef my_rb_tree<key_type, value_type, my_select1st<value_type>, key_compare, Alloc> rep_type;
  rep_type t;

 public:
  typedef typename rep_type::pointer pointer;
  typedef typename rep_type::const_pointer const_pointer;
  typedef typename rep_type::reference reference;
  typedef typename rep_type::const_reference const_reference;
  typedef typename rep_type::iterator iterator;
  typedef typename rep_type::const_iterator const_iterator;
  typedef typename rep_type::size_type size_type;
  typedef typename rep_type::difference_type difference_type;

 public:
  // map will use insert_unique of rb_tree
  my_map() : t(Compare()) {}

  explicit my_map(const Compare& comp) : t(comp) {}

  template <class InputIterator>
  my_map(InputIterator first, InputIterator last) : t(Compare()) {
    t.insert_unique(first, last);
  }

  template <class InputIterator>
  my_map(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
    t.insert_unique(first, last);
  }

  my_map(const my_map<Key, T, Compare, Alloc>& x) : t(x.t) {}

  my_map<Key, T, Compare, Alloc>& operator=(const my_map<Key, T, Compare, Alloc>& x) {
    if (this == &x) {
      return *this;
    }

    t = x.t;
    return *this;
  }

 public:
  key_compare key_comp() const { return t.key_comp(); }

  value_compare value_comp() const { return value_compare(t.key_comp()); }

  iterator begin() { return t.begin(); }

  const_iterator begin() const { return t.begin(); }

  iterator end() { return t.end(); }

  const_iterator end() const { return t.end(); }

  bool empty() const { return t.empty(); }

  size_type size() const { return t.size(); }

  size_type max_size() const { return t.max_size(); }

  T& operator[](const key_type& k) { return (*((insert(value_type(k, T()))).first)).second; }

  void swap(my_map<Key, T, Compare, Alloc>& x) { t.swap(x.t); }

  my_pair<iterator, bool> insert(const value_type& x) { return t.insert_unique(x); }

  iterator insert(iterator position, const value_type& x) { return t.insert_unique(position, x); }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    t.insert_unique(first, last);
  }

  void erase(iterator position) { t.erase(position); }

  size_type erase(const key_type& x) { return t.erase(x); }

  void erase(iterator first, iterator last) { t.erase(first, last); }

  void clear() { t.clear(); }

 public:
  iterator find(const key_type& x) { return t.find(x); }

  const_iterator find(const key_type& x) const { return t.find(x); }

  size_type count(const key_type& x) const { return t.count(x); }

  iterator lower_bound(const key_type& x) { return t.lower_bound(x); }

  const_iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }

  iterator upper_bound(const key_type& x) { return t.upper_bound(x); }

  const_iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }

  my_pair<iterator, iterator> equal_range(const key_type& x) { return t.equal_range(x); }

  my_pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
    return t.equal_range(x);
  }
};

#endif
