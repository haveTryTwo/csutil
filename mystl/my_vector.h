// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_VECTOR_H_
#define MY_VECTOR_H_

#include "my_simple_alloc.h"
#include "my_stl_algobase.h"
#include "my_stl_construct.h"
#include "my_stl_iterator.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"

template <class T, class Alloc = my_alloc>
class my_vector {
 public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef my_reverse_iterator<iterator> reverse_iterator;
  typedef my_reverse_iterator<const_iterator> const_reverse_iterator;

 protected:
  typedef my_simple_alloc<value_type, Alloc> my_data_allocator;

  iterator start;           // the start of the current container
  iterator finish;          // the end of the current container that has already been used
  iterator end_of_storage;  // the end of the current container that can be used

  void deallocate() {
    if (start) {
      my_data_allocator::deallocate(start, end_of_storage - start);
    }
  }

  // initialize the vector
  void fill_initialize(size_type n, const T& value) {
    start = allocate_and_fill(n, value);
    finish = start + n;
    end_of_storage = finish;
  }

  // allocate and fill the vector
  iterator allocate_and_fill(size_type n, const T& value) {
    iterator result = my_data_allocator::allocate(n);
    my_uninitialized_fill_n(result, n, value);
    return result;
  }

  template <class InputIterator, class Distance>
  inline void __fill_init(InputIterator first, InputIterator last, Distance*) {
    Distance n = my_distance(first, last);
    start = my_data_allocator::allocate(n);
    finish = start;
    end_of_storage = finish;
    for (; first != last; ++first) {
      push_back(*first);
    }
  }

 public:
  iterator begin() { return start; }

  const_iterator begin() const { return start; }

  iterator end() { return finish; }

  const_iterator end() const { return finish; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

  size_type size() const { return size_type(end() - begin()); }

  size_type capacity() const { return size_type(end_of_storage - begin()); }

  bool empty() const { return begin() == end(); }

  reference operator[](size_type n) { return *(begin() + n); }

  const_reference operator[](size_type n) const { return *(begin() + n); }

  // swap this my_vector and my_vector x;
  void swap(my_vector& x);

 public:
  // default constructor
  my_vector() : start(0), finish(0), end_of_storage(0) {}

  my_vector(size_type n, const T& value) { fill_initialize(n, value); }

  my_vector(int n, const T& value) { fill_initialize(n, value); }

  my_vector(long n, const T& value) { fill_initialize(n, value); }

  explicit my_vector(size_type n) { fill_initialize(n, T()); }

  template <class InputIterator>
  my_vector(InputIterator first, InputIterator last) {
    __fill_init(first, last, distance_type(first));
  }

  my_vector(const my_vector& myvector) {
    const size_type len = myvector.capacity();
    start = my_data_allocator::allocate(len);
    finish = my_uninitialized_copy(myvector.begin(), myvector.end(), start);
    end_of_storage = start + len;
  }

  ~my_vector() {
    my_destroy(start, finish);  // deconstruct
    deallocate();               // deallocate
  }

  my_vector& operator=(const my_vector& myvector) {
    if (this == &myvector) {
      return *this;
    }

    // deallocate the old space
    my_destroy(begin(), end());
    deallocate();

    const size_type len = myvector.capacity();
    start = my_data_allocator::allocate(len);
    finish = my_uninitialized_copy(myvector.begin(), myvector.end(), start);
    end_of_storage = start + len;

    return *this;
  }

 public:
  reference front() { return *(begin()); }

  const_reference front() const { return *(begin()); }

  reference back() { return *(end() - 1); }

  const_reference back() const { return *(end() - 1); }

  void push_back(const T& x) {
    if (finish != end_of_storage) {
      my_construct(&*finish, x);
      ++finish;
    } else {
      // if the container is full, then insert the value in new space
      insert_aux(end(), x);
    }
  }

  void pop_back() {
    if (finish != start) {
      // if there are still value in the container
      --finish;
      my_destroy(finish);
    }
  }

  void insert_aux(iterator position, const T& x);
  void insert(iterator position, size_type n, const T& x);
  iterator insert(iterator position, const T& x);

  // clear the location of the container
  iterator erase(iterator position) {
    if (position + 1 != end()) {
      my_copy(position + 1, finish, position);
    }
    --finish;
    my_destroy(finish);
    return position;
  }

  // clear the location of container between the space
  iterator erase(iterator first, iterator last) {
    iterator i = my_copy(last, finish, first);
    my_destroy(i, finish);
    finish = finish - (last - first);
    return first;
  }

  // resize current vector's size to new size
  // if the current vector's size less than new size then erase the value of extend
  // else add the new value of new extend space
  void resize(size_type new_size, const T& x) {
    if (new_size < size()) {
      erase(begin() + new_size, end());
    } else {
      insert(end(), new_size - size(), x);
    }
  }

  // resize current vector's size to the new size
  void resize(size_type new_size) { resize(new_size, T()); }

  void reserve(size_type n) {
    clear();
    if (capacity() < n) {
      // if n is bigger, then reallocate
      try {
        iterator new_start = my_data_allocator::allocate(n);
        iterator new_finish = new_start;
        deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + n;
      } catch (...) {
        throw;
      }
    } else {
      return;
    }
  }

  void clear() { erase(begin(), end()); }
};

template <class T, class Alloc>
void my_vector<T, Alloc>::insert_aux(iterator position, const T& x) {
  if (finish != end_of_storage) {
    if (position != finish) {
      my_construct(finish, *(finish - 1));
      ++finish;
      T x_copy = x;
      my_copy_backward(position, finish - 2, finish - 1);
      *position = x_copy;
    } else {
      // if insert the elememnt in the last position
      *finish = x;
      ++finish;
    }
  } else {
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    iterator new_start = my_data_allocator::allocate(len);
    iterator new_finish = new_start;
    try {
      new_finish = my_uninitialized_copy(start, position, new_start);
      my_construct(new_finish, x);
      ++new_finish;
      new_finish = my_uninitialized_copy(position, finish, new_finish);
    } catch (...) {
      my_destroy(new_start, new_finish);
      my_data_allocator::deallocate(new_start, len);
    }

    // destroy old vector space
    my_destroy(begin(), end());
    deallocate();

    // to adjust the iterator, point to the new space
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
  }
}

template <class T, class Alloc>
typename my_vector<T, Alloc>::iterator my_vector<T, Alloc>::insert(iterator position, const T& x) {
  if (finish != end_of_storage) {
    if (position != finish) {
      my_construct(finish, *(finish - 1));
      ++finish;
      T x_copy = x;
      my_copy_backward(position, finish - 2, finish - 1);
      *position = x_copy;
    } else {
      // if insert the elememnt in the last position
      *finish = x;
      ++finish;
    }
    return position;
  } else {
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    iterator new_start = my_data_allocator::allocate(len);
    iterator new_finish = new_start;
    iterator tmp_position = 0;
    try {
      new_finish = my_uninitialized_copy(start, position, new_start);
      my_construct(new_finish, x);
      tmp_position = new_finish;
      ++new_finish;
      new_finish = my_uninitialized_copy(position, finish, new_finish);
    } catch (...) {
      my_destroy(new_start, new_finish);
      my_data_allocator::deallocate(new_start, len);
    }

    // destroy old vector space
    my_destroy(begin(), end());
    deallocate();

    // to adjust the iterator, point to the new space
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
    return tmp_position;
  }
}

template <class T, class Alloc>
void my_vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
  if (n != 0) {
    if (size_type(end_of_storage - finish) >= n) {
      T x_copy = x;
      const size_type elems_after = finish - position;
      iterator old_finish = finish;
      if (elems_after > n) {
        my_uninitialized_copy(finish - n, finish, finish);
        finish += n;
        my_copy_backward(position, old_finish - n, old_finish);
        my_fill(position, position + n, x);
      } else {
        my_uninitialized_fill_n(finish, n - elems_after, x_copy);
        finish += n - elems_after;
        my_copy(position, old_finish, finish);
        finish += elems_after;
        my_fill(position, position + n, x_copy);
      }
    } else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = my_data_allocator::allocate(len);
      iterator new_finish = new_start;
      try {
        new_finish = my_uninitialized_copy(start, position, new_start);
        new_finish = my_uninitialized_fill_n(new_finish, n, x);
        new_finish = my_uninitialized_copy(position, finish, new_finish);
      } catch (...) {
        // if exception, then destroy the new allocate space;
        my_destroy(new_start, new_finish);
        my_data_allocator::deallocate(new_start, len);
        throw;
      }

      // destroy the old vector
      my_destroy(start, finish);
      deallocate();

      // allocate the new iterator
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

template <class T, class Alloc>
void my_vector<T, Alloc>::swap(my_vector<T, Alloc>& x) {
  if (size() == 0 && x.size() == 0) {
    return;
  } else if (size() == 0) {
    start = x.start;
    finish = x.finish;
    end_of_storage = x.end_of_storage;

    x.start = 0;
    x.finish = 0;
    x.end_of_storage = 0;
    return;
  } else if (x.size() == 0) {
    x.start = start;
    x.finish = finish;
    x.end_of_storage = end_of_storage;

    start = 0;
    finish = 0;
    end_of_storage = 0;
    return;
  } else {
    my_swap(start, x.start);
    my_swap(finish, x.finish);
    my_swap(end_of_storage, x.end_of_storage);

    return;
  }
}

#endif
