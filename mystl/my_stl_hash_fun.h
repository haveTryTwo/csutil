// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_HASH_FUN_H_
#define MY_STL_HASH_FUN_H_

#include <cstddef>
#include <string>

using std::size_t;

inline size_t __my_stl_hash_string(const char* s) {
  unsigned long h = 0;
  for (; *s; ++s) {
    h = 5 * h + *s;
  }
  return size_t(h);
}

/**
 * hash function : base hash
 */
template <class Key>
class my_hash {};

// hash : char*
template <>
class my_hash<char*> {
 public:
  size_t operator()(const char* s) const { return __my_stl_hash_string(s); }
};

// hash : const char*
template <>
class my_hash<const char*> {
 public:
  size_t operator()(const char* s) const { return __my_stl_hash_string(s); }
};

// hash : char
template <>
class my_hash<char> {
 public:
  size_t operator()(char x) const { return x; }
};

// hash : unsigned char
template <>
class my_hash<unsigned char> {
 public:
  size_t operator()(unsigned char x) const { return x; }
};

// hash : signed char
template <>
class my_hash<signed char> {
 public:
  size_t operator()(signed char x) const { return x; }
};

// hash : short
template <>
class my_hash<short> {
 public:
  size_t operator()(short x) const { return x; }
};

// hash : unsigned short
template <>
class my_hash<unsigned short> {
 public:
  size_t operator()(unsigned short x) const { return x; }
};

// hash : int
template <>
class my_hash<int> {
 public:
  size_t operator()(int x) const { return x; }
};

// hash : unsigned int
template <>
class my_hash<unsigned int> {
 public:
  size_t operator()(unsigned int x) const { return x; }
};

// hash : long
template <>
class my_hash<long> {
 public:
  size_t operator()(long x) const { return x; }
};

// hash : unsigned long
template <>
class my_hash<unsigned long> {
 public:
  size_t operator()(unsigned long x) const { return x; }
};

// hash : float
template <>
class my_hash<float> {
 public:
  size_t operator()(float x) const { return x; }
};

// hash : double
template <>
class my_hash<double> {
 public:
  size_t operator()(double x) const { return x; }
};

// hash : string
template <>
class my_hash<std::string> {
 public:
  size_t operator()(std::string x) const {
    unsigned long h = 0;
    typedef std::string::iterator iterator;
    for (iterator it = x.begin(); it != x.end(); ++it) {
      h = 5 * h + *it;
    }
    return size_t(h);
  }
};

#endif
