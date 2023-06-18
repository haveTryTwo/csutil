// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_SET_H_
#define MY_SET_H_

#include "my_rb_tree.h"

template <class Key, class Compare = my_less<Key>, class Alloc = my_alloc>
class my_set {
 public:
  typedef Key key_type;
  typedef Key value_type;
  typedef Compare key_compare;
  typedef Compare value_compare;

 private:
  typedef my_rb_tree<key_type, value_type, my_identity<value_type>, key_compare, Alloc> rep_type;

  rep_type t;

 public:
  typedef typename rep_type ::const_pointer pointer;
  typedef typename rep_type ::const_pointer const_pointer;
  typedef typename rep_type ::const_reference reference;
  typedef typename rep_type ::const_reference const_reference;
  typedef typename rep_type ::const_iterator iterator;
  typedef typename rep_type ::const_iterator const_iterator;
  typedef typename rep_type ::size_type size_type;
  typedef typename rep_type ::difference_type difference_type;

 public:
  my_set() : t(Compare()) {}

  explicit my_set(const Compare& comp) : t(comp) {}

  template <class InputIterator>
  my_set(InputIterator first, InputIterator last) : t(Compare()) {
    t.insert_unique(first, last);
  }

  template <class InputIterator>
  my_set(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
    t.insert_unique(first, last);
  }

  my_set(const my_set<Key, Compare, Alloc>& x) : t(x.t) {}

  my_set<Key, Compare, Alloc>& operator=(const my_set<Key, Compare, Alloc>& x) {
    if (this == &x) {
      return *this;
    }

    t.clear();  // to clear the tree t

    t = x.t;
    return *this;
  }

 public:
  key_compare key_comp() const { return t.key_comp(); }

  value_compare value_comp() const { return t.key_comp(); }

  iterator begin() const { return t.begin(); }

  iterator end() const { return t.end(); }

  bool empty() const { return t.empty(); }

  size_type size() const { return t.size(); }

  size_type max_size() const { return t.max_size(); }

  void swap(my_set<Key, Compare, Alloc>& x) { t.swap(x.t); }

 public:
  typedef my_pair<iterator, bool> pair_iterator_bool;

  my_pair<iterator, bool> insert(const value_type& x) {
    my_pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
    return my_pair<iterator, bool>(p.first, p.second);
  }

  iterator insert(iterator position, const value_type& x) {
    typedef typename rep_type ::iterator rep_iterator;
    return t.insert_unique((rep_iterator&)position, x);
  }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    t.insert_unique(first, last);
  }

  void erase(iterator position) {
    typedef typename rep_type ::iterator rep_iterator;
    t.erase((rep_iterator&)position);
  }

  void erase(const key_type& x) { t.erase(x); }

  void erase(iterator first, iterator last) {
    typedef typename rep_type ::iterator rep_iterator;
    t.erase((rep_iterator&)first, (rep_iterator&)last);
  }

  void clear() { t.clear(); }

  iterator find(const key_type& x) const { return t.find(x); }

  size_type count(const key_type& x) const { return t.count(x); }

  iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }

  iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }

  my_pair<iterator, iterator> equal_range(const key_type& x) const { return t.equal_range(x); }
};

#endif
