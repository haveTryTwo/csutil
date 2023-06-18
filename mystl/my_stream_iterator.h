// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STREAM_ITERATOR_H_
#define MY_STREAM_ITERATOR_H_

#include "my_stl_iterator.h"

/**************************************************
 * istream_iterator
 ***************************************************/
template <class T, class Distance = ptrdiff_t>
class my_istream_iterator {
 protected:
  istream* stream;
  T value;
  bool end_marker;

  void read() {
    end_marker = (*stream) ? true : false;
    if (end_marker) {
      *stream >> value;
    }
    end_marker = (*stream) ? true : false;
  }

 public:
  typedef my_input_iterator_tag iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef const T* pointer;
  typedef const T& reference;

  my_istream_iterator() : stream(&cin), end_marker(false) {}

  my_istream_iterator(istream& s) : stream(&s) { read(); }

  reference operator*() const { return value; }

  pointer operator->() const { return &(operator*()); }

  my_istream_iterator<T, Distance>& operator++() {
    read();
    return *this;
  }

  my_istream_iterator<T, Distance> operator++(int) {
    my_istream_iterator<T, Distance> tmp = *this;
    read();
    return tmp;
  }

  template <class T_value, class D>
  friend bool operator==(const my_istream_iterator<T_value, D>& x,
                         const my_istream_iterator<T_value, D>& y);

  template <class T_value, class D>
  friend bool operator!=(const my_istream_iterator<T_value, D>& x,
                         const my_istream_iterator<T_value, D>& y);
};

template <class T, class Distance>
bool operator==(const my_istream_iterator<T, Distance>& x,
                const my_istream_iterator<T, Distance>& y) {
  return x.stream == y.stream;
}

template <class T, class Distance>
bool operator!=(const my_istream_iterator<T, Distance>& x,
                const my_istream_iterator<T, Distance>& y) {
  return x.stream != y.stream;
}
/************************************************************/

/**************************************************
 * ostream_iterator
 ***************************************************/
template <class T>
class my_ostream_iterator {
 protected:
  ostream* stream;
  const char* string;  // user define string that need to out put on screen

 public:
  typedef my_output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  my_ostream_iterator(ostream& s) : stream(&s), string(0) {}

  my_ostream_iterator(ostream& s, const char* c) : stream(&s), string(c) {}

  my_ostream_iterator<T>& operator=(const T& value) {
    *stream << value;
    if (string) {
      *stream << string;
    }
    return *this;
  }

  my_ostream_iterator<T>& operator*() { return *this; }

  my_ostream_iterator<T>& operator++() { return *this; }

  my_ostream_iterator<T> operator++(int) { return *this; }
};
/************************************************************/

#endif
