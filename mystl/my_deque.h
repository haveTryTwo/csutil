// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __my_deque_h_
#define __my_deque_h_

#include "my_simple_alloc.h"
#include "my_stl_construct.h"
#include "my_stl_iterator.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"

#include <iostream>
using namespace std;

// get the size for buffer
// if n > 0 then return n;
// if sz less than 512, then return 512 / sz, or return 1
inline size_t __my_deque_buf_size(size_t n, size_t sz) {
  return n != 0 ? n : ((sz < 512) ? 512 / sz : size_t(1));
}

template <class T, class Ref, class Ptr, size_t bufsize>
class __my_deque_iterator {
 public:
  typedef __my_deque_iterator<T, T&, T*, bufsize> iterator;
  typedef __my_deque_iterator<T, const T&, const T*, bufsize> const_iterator;

  // get the size of buffer
  static size_t buffer_size() { return __my_deque_buf_size(bufsize, sizeof(T)); }

  typedef my_random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef T** map_pointer;
  typedef __my_deque_iterator self;

  T* cur;            // pointer to the current of current buffer
  T* first;          // pointer to the first of current buffer
  T* last;           // pointer to the last of current buffer
  map_pointer node;  // pointer to the controller

  __my_deque_iterator(T* x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}

  __my_deque_iterator(const iterator& x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

  __my_deque_iterator() : cur(0), first(0), last(0), node(0) {}

  // to put the new buffer
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + difference_type(buffer_size());
  }

  reference operator*() const { return *cur; }

  pointer operator->() const { return cur; }

  difference_type operator-(const self& x) const {
    difference_type distance =
        difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
    return distance;
  }

  //	friend difference_type operator- (const iterator& x, const iterator& y) const;

  self& operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  self& operator+=(difference_type n) {
    difference_type offset = n + cur - first;
    if (offset >= 0 && offset < difference_type(buffer_size())) {
      cur += n;
    } else {
      difference_type node_offset = offset > 0
                                        ? offset / difference_type(buffer_size())
                                        : -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * difference_type(buffer_size()));
    }
    return *this;
  }

  self operator+(difference_type n) const {
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n) { return *this += -n; }

  self operator-(difference_type n) const {
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) { return *(*this + n); }

  bool operator==(const self& x) const { return cur == x.cur; }

  bool operator!=(const self& x) const { return !(*this == x); }

  bool operator<(const self& x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
};

/*
template <class T, class Ref, class Ptr, size_t bufsize>
inline typename __my_deque_iterator<T, Ref, Ptr, bufsize> ::difference_type operator- (
                                const __my_deque_iterator<T, Ref, Ptr, bufsize>& x,
                                const __my_deque_iterator<T, Ref, Ptr, bufsize>& y) const
{
        return typename __my_deque_iterator<T, Ref, Ptr, bufsize> ::difference_type(
                              typename __my_deque_iterator<T, Ref, Ptr, bufsize> ::buffer_size())
                * (x.node - y.node - 1) + x.cur - x.first + y.last - y.cur;
}
*/

template <class T, class Alloc = my_alloc, size_t bufsize = 0>
class my_deque {
 public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef my_random_access_iterator_tag iterator_catagory;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

 public:
  typedef __my_deque_iterator<T, T&, T*, bufsize> iterator;
  typedef __my_deque_iterator<T, const T&, const T*, bufsize> const_iterator;

 protected:
  typedef pointer* map_pointer;
  typedef my_simple_alloc<value_type, Alloc> data_allocator;
  typedef my_simple_alloc<pointer, Alloc> map_allocator;

 protected:
  iterator start;   // the first node of the deque
  iterator finish;  // the last node of the deque

  map_pointer map;  // point to map, and this space is consistence, and the value of the map is
                    // pointer which 				 // pointer to the node

  size_type map_size;  // the numbers of the pointers containted in the map
  enum { initial_map_size = 8 };

 public:
  my_deque() : start(), finish(), map(0), map_size(0) { create_map_and_node(0); }

  my_deque(size_type n, const value_type& value) : start(), finish(), map(0), map_size(0) {
    fill_initialize(n, value);
  }

  my_deque(const my_deque& x) {
    create_map_and_node(x.size());
    fill_nodes(x.begin(), x.end());
  }

  my_deque& operator=(const my_deque& x) {
    if (this == &x) {
      return *this;
    }

    // deallocate the buffer and also deallocate the map
    destroy_nodes();

    create_map_and_node(x.size());
    fill_nodes(x.begin(), x.end());
  }

 protected:
  void fill_initialize(size_type n, const value_type& value);
  void create_map_and_node(size_type num_elements);

  template <class InputIterator>
  void fill_nodes(InputIterator first, InputIterator last) {
    iterator tmp = start;
    for (; first != last; ++first, ++tmp) {
      my_construct(tmp.cur, *first);
    }
  }

  void destroy_nodes();

 public:
  size_type get_initial_map_size() const { return (size_type)initial_map_size; }

  static size_type buffer_size() { return __my_deque_buf_size(bufsize, sizeof(T)); }

  pointer allocate_node() { return data_allocator ::allocate(buffer_size()); }

  void deallocate_node(pointer node) { data_allocator ::deallocate(node, buffer_size()); }

 public:
  iterator begin() { return start; }

  const_iterator begin() const { return start; }

  iterator end() { return finish; }

  const_iterator end() const { return finish; }

  reference operator[](size_type n) { return start[difference_type(n)]; }

  reference front() { return *start; }

  reference back() {
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }

  size_type size() const { return finish - start; }

  size_type max_size() const { return size_type(-1); }

  bool empty() const { return finish == start; }

 public:
  void push_back(const value_type& t) {
    if (finish.cur != finish.last - 1) {
      my_construct(finish.cur, t);
      ++finish.cur;
    } else {
      push_back_aux(t);
    }
  }

  void push_front(const value_type& t) {
    if (start.cur != start.first) {
      my_construct(start.cur - 1, t);
      --start.cur;
    } else {
      push_front_aux(t);
    }
  }

  void pop_back() {
    if (finish == start) {
      // if there is no value
      return;
    }

    if (finish.cur != finish.first) {
      --finish.cur;
      my_destroy(finish.cur);
    } else {
      pop_back_aux();
    }
  }

  void pop_front() {
    if (finish == start) {
      // if there is no value
      return;
    }

    if (start.cur != start.last - 1) {
      my_destroy(start.cur);
      ++start.cur;
    } else {
      pop_front_aux();
    }
  }

  void clear();
  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);
  iterator insert(iterator position, const value_type& x);

 protected:
  void push_back_aux(const value_type& t);
  void push_front_aux(const value_type& t);

  void pop_back_aux();
  void pop_front_aux();

  iterator insert_aux(iterator pos, const value_type& x);

  void reserve_map_at_back(size_type nodes_to_add = 1) {
    if (nodes_to_add + 1 > map_size - (finish.node - map)) {
      // if the tail of the map is not enough
      // then resize the map
      reallocate_map(nodes_to_add, false);
    }
  }

  void reserve_map_at_front(size_type nodes_to_add = 1) {
    if (nodes_to_add > start.node - map) {
      // if the begin of the map is not enough
      // then resize the map
      reallocate_map(nodes_to_add, true);
    }
  }

  void reallocate_map(size_type nodes_to_add, bool add_at_front);
};

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::fill_initialize(size_type n, const value_type& value) {
  create_map_and_node(n);

  map_pointer cur;
  try {
    // initialize the value
    for (cur = start.node; cur < finish.node; ++cur) {
      my_uninitialized_fill(*cur, *cur + buffer_size(), value);
    }
    my_uninitialized_fill(finish.first, finish.cur, value);
  } catch (...) {
  }
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::destroy_nodes() {
  clear();
  data_allocator ::deallocate(start.first, buffer_size());

  map_allocator ::deallocate(map, map_size);

  map = 0;
  map_size = 0;
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::create_map_and_node(size_type num_elements) {
  size_type num_nodes = num_elements / buffer_size() + 1;

  // a map must get more nodes, and at least the number is 8
  map_size = my_max(get_initial_map_size(), num_nodes + 2);
  map = map_allocator ::allocate(map_size);

  // the next nstart and nfinish is in the center of the map
  // so the map can extend to the head and tie side
  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;

  map_pointer cur;
  try {
    for (cur = nstart; cur <= nfinish; ++cur) {
      *cur = allocate_node();
    }
  } catch (...) {
    // if exception, the commit or rollback
    for (cur = nstart; cur <= nfinish; ++cur) {
      deallocate_node(*cur);
    }
  }

  start.set_node(nstart);
  start.cur = start.first;
  finish.set_node(nfinish);
  finish.cur = finish.first + num_elements % buffer_size();
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::push_back_aux(const value_type& t) {
  value_type t_copy = t;

  // test whether the map should be changed
  // if the map need to be changed, in this function, the map will be changed
  reserve_map_at_back();

  *(finish.node + 1) = allocate_node();
  try {
    my_construct(finish.cur, t_copy);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
  } catch (...) {
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    deallocate_node(*(finish.node + 1));
    throw;
  }
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::push_front_aux(const value_type& t) {
  value_type t_copy = t;

  // test whether the map should be changed
  reserve_map_at_front();

  *(start.node - 1) = allocate_node();

  try {
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    my_construct(start.cur, t_copy);
  } catch (...) {
    start.set_node(start.node + 1);
    start.cur = start.first;
    deallocate_node(*(finish.node - 1));
    throw;
  }
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
  size_type old_num_nodes = finish.node - start.node + 1;
  size_type new_num_nodes = old_num_nodes + nodes_to_add;

  map_pointer new_nstart;
  if (map_size > 2 * new_num_nodes) {
    // if the map still has enough space
    new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node) {
      my_copy(start.node, finish.node + 1, new_nstart);
    } else {
      my_copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
    }
  } else {
    // the size of map is not enough, then allocate one new space for the map
    size_type new_map_size = map_size + my_max(map_size, nodes_to_add) + 2;
    map_pointer new_map = map_allocator ::allocate(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);

    my_copy(start.node, finish.node + 1, new_nstart);

    // reallocate the old map
    map_allocator ::deallocate(map, map_size);

    map = new_map;
    map_size = new_map_size;
  }

  start.set_node(new_nstart);
  finish.set_node(new_nstart + old_num_nodes - 1);
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::pop_back_aux() {
  deallocate_node(finish.first);
  finish.set_node(finish.node - 1);
  finish.cur = finish.last - 1;
  my_destroy(finish.cur);
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::pop_front_aux() {
  my_destroy(start.cur);
  deallocate_node(start.first);
  start.set_node(start.node + 1);
  start.cur = start.first;
}

template <class T, class Alloc, size_t bufsize>
void my_deque<T, Alloc, bufsize>::clear() {
  // delete every buffer except the start and the finish
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
    // desconstructor
    my_destroy(*node, *node + buffer_size());
    // deallocate
    data_allocator ::deallocate(*node, buffer_size());
  }

  if (start.node != finish.node) {
    my_destroy(start.cur, start.last);
    my_destroy(finish.first, finish.cur);
    // deallocate the tail buffer, but keep the head buffer
    data_allocator ::deallocate(finish.first, buffer_size());
  } else {
    my_destroy(start.cur, finish.cur);
  }

  finish = start;
}

// clear the position of the my_deque
template <class T, class Alloc, size_t bufsize>
typename my_deque<T, Alloc, bufsize>::iterator my_deque<T, Alloc, bufsize>::erase(iterator pos) {
  iterator next = pos;
  ++next;
  difference_type index = pos - start;
  if (index < (size() >> 1)) {
    my_copy_backward(start, pos, next);
    pop_front();
  } else {
    my_copy(next, finish, pos);
    pop_back();
  }

  return start + index;
}

template <class T, class Alloc, size_t bufsize>
typename my_deque<T, Alloc, bufsize>::iterator my_deque<T, Alloc, bufsize>::erase(iterator first,
                                                                                  iterator last) {
  if (first == start && last == finish) {
    clear();
    return finish;
  } else {
    difference_type n = last - first;
    difference_type elems_before = first - start;
    if (elems_before < (size() - n) / 2) {
      my_copy_backward(start, first, last);
      iterator new_start = start + n;
      my_destroy(start, new_start);
      for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
        data_allocator ::deallocate(*cur, buffer_size());
      }
      start = new_start;
    } else {
      my_copy(last, finish, first);
      iterator new_finish = finish - n;
      my_destroy(new_finish, finish);
      for (map_pointer cur = new_finish.node + 1; cur <= finish; ++cur) {
        data_allocator ::deallocate(*cur, buffer_size());
      }
      finish = new_finish;
    }
    return start + elems_before;
  }
}

template <class T, class Alloc, size_t bufsize>
typename my_deque<T, Alloc, bufsize>::iterator my_deque<T, Alloc, bufsize>::insert(
    iterator position, const value_type& x) {
  if (position.cur == start.cur) {
    push_front(x);
    return start;
  } else if (position.cur == finish.cur) {
    push_back(x);
    iterator tmp = finish;
    --tmp;
    return tmp;
  } else {
    return insert_aux(position, x);
  }
}

template <class T, class Alloc, size_t bufsize>
typename my_deque<T, Alloc, bufsize>::iterator my_deque<T, Alloc, bufsize>::insert_aux(
    iterator pos, const value_type& x) {
  difference_type index = pos - start;
  value_type x_copy = x;
  if (index < size() / 2) {
    // start push the value, so the map and the space will extend if necessery
    push_front(front());
    iterator front1 = start;
    ++front1;
    iterator front2 = front1;
    ++front2;
    pos = start + index;
    iterator pos1 = pos;
    ++pos1;
    my_copy(front2, pos1, front1);
  } else {
    // start push the value, so the map and the space will extend if necessery
    push_back(back());
    iterator back1 = finish;
    --back1;
    iterator back2 = back1;
    --back2;
    my_copy_backward(pos, back2, back1);
  }
  *pos = x_copy;
  return pos;
}
#endif
