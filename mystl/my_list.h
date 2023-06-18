// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_LIST_H_
#define MY_LIST_H_

#include "my_simple_alloc.h"
#include "my_stl_construct.h"
#include "my_stl_iterator.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"

class __my_list_node_base {
 public:
  __my_list_node_base* prev;
  __my_list_node_base* next;

  static void transfer(__my_list_node_base& x, __my_list_node_base& y);
};

template <class T>
class __my_list_node : public __my_list_node_base {
 public:
  T data;
};

template <class T>
class __my_list_iterator { /*{{{*/
 public:
  typedef __my_list_iterator<T> self;
  typedef __my_list_node<T>* link_type;
  typedef __my_list_node_base* base_link_type;

  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef T& reference;
  typedef T* pointer;
  typedef my_bidirectional_iterator_tag iterator_category;
  typedef size_t size_type;

  base_link_type base_node;

 public:
  __my_list_iterator() {}

  explicit __my_list_iterator(base_link_type x) : base_node(x) {}

  __my_list_iterator(const self& x) : base_node(x.base_node) {}

 public:
  bool operator==(const self& x) const { return base_node == x.base_node; }

  bool operator!=(const self& x) const { return base_node != x.base_node; }

  reference operator*() const { return static_cast<link_type>(base_node)->data; }

  pointer operator->() const { return &static_cast<link_type>(base_node)->data; }

  self& operator++() {
    base_node = base_node->next;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    base_node = base_node->next;
    return tmp;
  }

  self& operator--() {
    base_node = base_node->prev;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    base_node = base_node->prev;
    return tmp;
  }
}; /*}}}*/

template <class T>
class __my_list_const_iterator {
 public:
  typedef __my_list_const_iterator<T> self;
  typedef const __my_list_node<T>* link_type;
  typedef const __my_list_node_base* base_link_type;
  typedef __my_list_iterator<T> iterator;

  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef const T& reference;
  typedef const T* pointer;
  typedef my_bidirectional_iterator_tag iterator_category;
  typedef size_t size_type;

  base_link_type base_node;

 public:
  __my_list_const_iterator() {}

  explicit __my_list_const_iterator(base_link_type x) : base_node(x) {}

  __my_list_const_iterator(const iterator& x) : base_node(x.base_node) {}

 public:
  bool operator==(const self& x) const { return base_node == x.base_node; }

  bool operator!=(const self& x) const { return base_node != x.base_node; }

  reference operator*() const { return static_cast<link_type>(base_node)->data; }

  pointer operator->() const { return &static_cast<link_type>(base_node)->data; }

  self& operator++() {
    base_node = base_node->next;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    base_node = base_node->next;
    return tmp;
  }

  self& operator--() {
    base_node = base_node->prev;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    base_node = base_node->prev;
    return tmp;
  }
};

template <class T>
inline bool operator==(__my_list_iterator<T> x, __my_list_const_iterator<T> y) {
  return x.base_node == y.base_node;
}

template <class T>
inline bool operator!=(__my_list_iterator<T> x, __my_list_const_iterator<T> y) {
  return x.base_node != y.base_node;
}

template <class T, class Alloc = my_alloc>
class my_list {
 protected:
  typedef __my_list_node<T> list_node;

  typedef my_simple_alloc<list_node, Alloc> list_node_allocator;

 public:
  typedef list_node* link_type;

  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef my_bidirectional_iterator_tag iterator_catagory;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef __my_list_iterator<T> iterator;
  typedef __my_list_const_iterator<T> const_iterator;
  typedef size_t size_type;

 protected:
  link_type node;

  link_type get_node() { return list_node_allocator ::allocate(); }

  void put_node(link_type p) { list_node_allocator ::deallocate(p); }

  link_type create_node(const T& x) {
    link_type p = get_node();
    my_construct(&p->data, x);
    return p;
  }

  void destroy_node(link_type p) {
    my_destroy(&p->data);
    put_node(p);
  }

  iterator insert(iterator position, const T& x) {
    link_type tmp = create_node(x);
    tmp->next = position.base_node;
    tmp->prev = position.base_node->prev;
    position.base_node->prev->next = tmp;
    position.base_node->prev = tmp;
    return iterator(tmp);
  }

  iterator erase(iterator position) {
    link_type next_node = static_cast<link_type>(position.base_node->next);
    link_type prev_node = static_cast<link_type>(position.base_node->prev);
    next_node->prev = prev_node;
    prev_node->next = next_node;
    destroy_node(static_cast<link_type>(position.base_node));
    return iterator(next_node);
  }

 public:
  my_list() { empty_initialize(); }

  my_list(size_type n, const T& x) {
    empty_initialize();

    my_fill_initialize(n, x);
  }

  my_list(const my_list& x) {
    empty_initialize();

    my_initialize_dispatch(x.begin(), x.end(), __my_false_type());
  }

  my_list& operator=(const my_list& x) {
    if (this == &x) {
      return *this;
    }

    clear();
    my_initialize_dispatch(x.begin(), x.end(), __my_false_type());

    return *this;
  }

 protected:
  void empty_initialize() {
    node = get_node();
    node->next = node;
    node->prev = node;
  }

  void my_fill_initialize(size_type n, const T& x) {
    for (; n > 0; --n) {
      push_back(x);
    }
  }

  template <class InputIterator>
  void my_initialize_dispatch(InputIterator first, InputIterator last, __my_false_type) {
    for (; first != last; ++first) {
      push_back(*first);
    }
  }

 public:
  iterator begin() { return iterator(node->next); }

  const_iterator begin() const { return const_iterator(node->next); }

  iterator end() { return iterator(node); }

  const_iterator end() const { return const_iterator(node); }

  bool empty() const { return node->next == node; }

  size_type size() const {
    size_type result = 0;
    result = my_distance(begin(), end());
    return result;
  }

  reference front()  // to get the value of the front of the list
  {
    return *begin();
  }

  const_reference front() const { return *begin(); }

  reference back()  // to get the value of the end of the list
  {
    iterator tmp = end();
    --tmp;
    return *tmp;
  }

  const_reference back() const {
    const_iterator tmp = end();
    --tmp;
    return *tmp;
  }

  void push_back(const T& x)  // to push an item into the end of list
  {
    insert(end(), x);
  }

  void push_front(const T& x)  // to push an item into the front of the list
  {
    insert(begin(), x);
  }

  void pop_front()  // to erase an item from the front
  {
    if (begin() != end()) {
      // if there are still some items
      erase(begin());
    } else {
      return;
    }
  }

  void pop_back()  // to erase an item from the back
  {
    if (begin() != end()) {
      // if there are still some items
      iterator tmp = end();
      erase(--tmp);
    } else {
      return;
    }
  }

  void clear();                 // to clear all the list
  void remove(const T& value);  // to remove the items that equal value
  void unique();                // to remove the items that must be equal and

 protected:
  // position mustn't be in the middle of first and last
  void transfer(iterator position, iterator first, iterator last) {
    if (position != last) {
      last.base_node->prev->next = position.base_node;
      first.base_node->prev->next = last.base_node;
      position.base_node->prev->next = first.base_node;
      link_type tmp = static_cast<link_type>(position.base_node->prev);
      position.base_node->prev = last.base_node->prev;
      last.base_node->prev = first.base_node->prev;
      first.base_node->prev = tmp;
    }
  }

 public:
  void splice(iterator position, my_list& x) {
    if (!x.empty()) {
      transfer(position, x.begin(), x.end());
    }
  }

  void splice(iterator position, my_list&, iterator i) {
    iterator j = i;
    ++j;
    if (position == i || position == j) {
      return;
    }

    transfer(position, i, j);
  }

  void splice(iterator position, my_list&, iterator first, iterator last) {
    if (first != last) {
      transfer(position, first, last);
    }
  }

  // to merge the my_list to be an one my_list, and the my_list must be sorted already;
  void merge(my_list& x);

  // to reverse the my_list
  void reverse();

  // to sort the my_list;
  void sort();

  // swap the my_lists
  void swap(my_list& x);
};

template <class T, class Alloc>
void my_list<T, Alloc>::clear() {
  link_type cur = static_cast<link_type>(node->next);
  while (cur != node) {
    link_type tmp = cur;
    cur = static_cast<link_type>(cur->next);
    destroy_node(tmp);
  }

  node->prev = node;
  node->next = node;
}

template <class T, class Alloc>
void my_list<T, Alloc>::remove(const T& value) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (*first == value) {
      erase(first);
    }
    first = next;
  }
}

template <class T, class Alloc>
void my_list<T, Alloc>::unique() {
  iterator first = begin();
  iterator last = end();
  if (last == first) {
    return;
  }

  iterator next = first;
  while (++next != last) {
    if (*first == *next) {
      erase(next);
    } else {
      first = next;
    }
    next = first;
  }
}

template <class T, class Alloc>
void my_list<T, Alloc>::merge(my_list<T, Alloc>& x) {
  iterator first1 = begin();
  iterator end1 = end();
  iterator first2 = x.begin();
  iterator end2 = x.end();

  while (first1 != end1 && first2 != end2) {
    if (*first2 < *first1) {
      iterator next = first2;
      transfer(first1, first2, ++next);
      first2 = next;
    } else {
      ++first1;
    }
  }

  if (first2 != end2) {
    transfer(end1, first2, end2);
  }
}

template <class T, class Alloc>
void my_list<T, Alloc>::reverse() {
  if (node->next == node || node->next->next == node) {
    return;
  }

  iterator first = begin();
  ++first;
  while (first != end()) {
    iterator old = first;
    ++first;
    transfer(begin(), old, first);
  }
}

template <class T, class Alloc>
void my_list<T, Alloc>::sort() {
  if (node->next == node || node->next->next == node) {
    return;
  }

  my_list<T, Alloc> carry;
  my_list<T, Alloc> counter;

  while (!empty()) {
    carry.splice(carry.begin(), *this, begin());
    counter.merge(carry);
  }

  swap(counter);
  // transfer(end(), counter.begin(), counter.end());
}

template <class T, class Alloc>
void my_list<T, Alloc>::swap(my_list<T, Alloc>& x) {
  if (empty() && !x.empty()) {
    transfer(end(), x.begin(), x.end());
  } else if (!empty() && x.empty()) {
    transfer(x.end(), begin(), end());
  } else if (!empty() && !x.empty()) {
    iterator tmp = end();
    --tmp;
    transfer(end(), x.begin(), x.end());
    ++tmp;
    transfer(x.end(), begin(), tmp);
  }
}

#endif
