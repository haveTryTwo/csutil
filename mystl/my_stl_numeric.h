// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_NUMERIC_H_
#define MY_STL_NUMERIC_H_

#include "my_stl_function.h"

/**
 * accumulate : to accumulate all the elements
 */
template <class InputIterator, class T>
T my_accumulate(InputIterator first, InputIterator last, T init) {
  for (; first != last; ++first) {
    init = init + *first;
  }

  return init;
}

template <class InputIterator, class T, class BinaryOperation>
T my_accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_obj) {
  for (; first != last; ++first) {
    init = binary_obj(init, *first);
  }

  return init;
}

/******
 * adjacent_difference : to difference the adjacent elements
 *******/
template <class InputIterator, class OutputIterator>
OutputIterator my_adjacent_difference(InputIterator first, InputIterator last,
                                      OutputIterator result) {
  if (first == last) {
    return result;
  }
  *result = *first;
  return __my_adjacent_difference(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator __my_adjacent_difference(InputIterator first, InputIterator last,
                                        OutputIterator result, T*) {
  T value = *first;
  while (++first != last) {
    T tmp = *first;
    *++result = tmp - value;
    value = tmp;
  }

  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator my_adjacent_difference(InputIterator first, InputIterator last,
                                      OutputIterator result, BinaryOperation binary_op) {
  if (first == last) {
    return result;
  }
  *result = *first;
  return __my_adjacent_difference(first, last, result, value_type(first), binary_op);
}

template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator __my_adjacent_difference(InputIterator first, InputIterator last,
                                        OutputIterator result, T*, BinaryOperation binary_op) {
  T value = *first;
  while (++first != last) {
    T tmp = *first;
    *++result = binary_op(tmp, value);
    value = tmp;
  }

  return ++result;
}

/******
 * inner_product: to
 *******/
template <class InputIterator1, class InputIterator2, class T>
T my_inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
  for (; first1 != last1; ++first1, ++first2) {
    init = init + (*first1) * (*first2);
  }
  return init;
}

template <class InputIterator1, class InputIterator2, class T, class BinaryOperation1,
          class BinaryOperation2>
T my_inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
                   BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
  for (; first1 != last1; ++first1, ++first2) {
    init = binary_op1(init, binary_op2(*first1, *first2));
  }
  return init;
}

/******
 * partial_sum
 *******/
template <class InputIterator, class OutputIterator>
OutputIterator my_partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
  if (first == last) {
    return result;
  }
  *result = *first;
  return __my_partial_sum(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator my_partial_sum(InputIterator first, InputIterator last, OutputIterator result, T*) {
  T value = *first;
  while (++first != last) {
    value = value + *first;
    *++result = value;
  }

  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator my_partial_sum(InputIterator first, InputIterator last, OutputIterator result,
                              BinaryOperation binary_op) {
  if (first == last) {
    return result;
  }
  *result = *first;
  return __my_partial_sum(first, last, result, value_type(first), binary_op);
}

template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator my_partial_sum(InputIterator first, InputIterator last, OutputIterator result, T*,
                              BinaryOperation binary_op) {
  T value = *first;
  while (++first != last) {
    value = binary_op(value, *first);
    *++result = value;
  }

  return ++result;
}

/*****
 * power :
 *******/
template <class T, class Integer>
inline T my_power(T x, Integer n) {
  return my_power(x, n, my_multiplies<T>());
}

template <class T, class Integer, class MonoidOperation>
T my_power(T x, Integer n, MonoidOperation op) {
  if (n == 0) {
    return my_identity_element(op);
  } else {
    while ((n & 1) == 0) {
      n >>= 1;
      x = op(x, x);
    }
    T result = x;
    n >>= 1;
    while (n != 0) {
      x = op(x, x);
      if ((n & 1) != 0) {
        result = op(result, x);
      }
      n >>= 1;
    }
    return result;
  }
}

/******
 * iota :
 *******/
template <class ForwardIterator, class T>
void my_iota(ForwardIterator first, ForwardIterator last, T value) {
  while (first != last) {
    *first++ = value++;
  }
}
#endif
