// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_SLIST_H_
#define MY_SLIST_H_

#include "my_simple_alloc.h"
#include "my_stl_algobase.h"
#include "my_stl_construct.h"
#include "my_stl_iterator.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"

class __slist_node_base {
 public:
  __slist_node_base* next;
};

template <class T>
class __slist_node : public __slist_node_base {
 public:
  T data;
};

inline __slist_node_base* __slist_make_link(__slist_node_base* prev_node,
                                            __slist_node_base* new_node) {
  new_node->next = prev_node->next;
  prev_node->next = new_node;
  return new_node;
};

inline size_t __slist_size(__slist_node_base* node) {
  size_t result = 0;
  for (; node != 0; node = node->next) {
    ++result;
  }

  return result;
};

class __slist_iterator_base {
 public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef my_forward_iterator_tag iterator_category;

  __slist_node_base* base_node;  // point to the base struct

  __slist_iterator_base(__slist_node_base* x) : base_node(x) {}

  void increment() { base_node = base_node->next; }

  bool operator==(const __slist_iterator_base& x) const { return base_node == x.base_node; }

  bool operator!=(const __slist_iterator_base& x) const { return base_node != x.base_node; }
};

template <class T, class Ref, class Ptr>
class __slist_iterator : public __slist_iterator_base {
 public:
  typedef __slist_iterator<T, T&, T*> iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;
  typedef __slist_iterator<T, Ref, Ptr> self;

  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef __slist_node<T> list_node;

  explicit __slist_iterator(list_node* x) : __slist_iterator_base(x) {}

  __slist_iterator() : __slist_iterator_base(0) {}

  __slist_iterator(const __slist_iterator& x) : __slist_iterator_base(x.base_node) {}

  reference operator*() const { return ((list_node*)base_node)->data; }

  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    increment();
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }
};

template <class T, class Alloc = my_alloc>
class my_slist {
 public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef __slist_iterator<T, T&, T*> iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;

 private:
  typedef __slist_node<T> list_node;
  typedef __slist_node_base list_node_base;
  typedef __slist_iterator_base iterator_base;

  typedef my_simple_alloc<list_node, Alloc> list_node_allocator;

  static list_node* create_node(const value_type& x) {
    list_node* node = list_node_allocator ::allocate();
    try {
      my_construct(&node->data, x);
      node->next = 0;
    } catch (...) {
      list_node_allocator ::deallocate(node);
    }
    return node;
  }

  static void destroy_node(list_node* node) {
    my_destroy(&node->data);
    list_node_allocator ::deallocate(node);
  }

 private:
  list_node_base head;  // this is a real

 public:
  my_slist() { head.next = 0; }

  ~my_slist() { clear(); }

 public:
  iterator begin() { return iterator((list_node*)head.next); }

  const_iterator begin() const { return iterator((list_node*)head.next); }

  iterator end() { return iterator(0); }

  const_iterator end() const { return const_iterator(0); }

  size_type size() const { return __slist_size(head.next); }

  bool empty() const { return head.next == 0; }

  void swap(my_slist& s) {
    list_node_base* tmp = head.next;
    head.next = s.head.next;
    s.head.next = tmp;
  }

 public:
  void clear() {
    list_node* cur = (list_node*)head.next;
    while (cur != 0) {
      list_node* tmp = cur;
      cur = (list_node*)cur->next;
      destroy_node(tmp);
    }
    head.next = 0;
  }

  reference front() {
    if (!empty()) {
      return *begin();
    }
    throw;
  }

  void push_front(const value_type& x) { __slist_make_link(&head, create_node(x)); }

  void pop_front() {
    if (!empty()) {
      list_node* node = (list_node*)head.next;
      head.next = node->next;
      destroy_node(node);
    }
  }
};

#endif
