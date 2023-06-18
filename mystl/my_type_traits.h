// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_TYPE_TRAITS_H_
#define MY_TYPE_TRAITS_H_

struct __my_true_type {};
struct __my_false_type {};

template <class type>
struct __my_type_traits {
  typedef __my_true_type this_dummy_member_must_be_first;

  typedef __my_false_type has_trivial_default_constructor;
  typedef __my_false_type has_trivial_copy_constructor;
  typedef __my_false_type has_trivial_assignment_operator;
  typedef __my_false_type has_trivial_destructor;
  typedef __my_false_type is_POD_type;
};

template <>
struct __my_type_traits<char> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<signed char> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<unsigned char> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<short> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<unsigned short> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<int> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<unsigned int> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<long> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<unsigned long> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<float> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<double> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};

template <>
struct __my_type_traits<long double> {
  typedef __my_true_type has_trivial_default_constructor;
  typedef __my_true_type has_trivial_copy_constructor;
  typedef __my_true_type has_trivial_assignment_operator;
  typedef __my_true_type has_trivial_destructor;
  typedef __my_true_type is_POD_type;
};
#endif
