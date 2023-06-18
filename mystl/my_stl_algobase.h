// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_ALGOBASE_H_
#define MY_STL_ALGOBASE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "my_heap.h"
#include "my_stl_iterator.h"
#include "my_stl_pair.h"
#include "my_type_traits.h"

/********************************************
 * swap:
 *********************************************/
template <class T>
inline void my_swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}
/******************************************************************/

/*********************************************
 * equal:
 ***********************************************/
template <class InputIterator1, class InputIterator2>
inline bool my_equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
  for (; first1 != last1; ++first1) {
    if (*first1 != *first2) {
      return false;
    }
  }
  return true;
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool my_equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                     BinaryPredicate binary_pred) {
  for (; first1 != last1; ++first1) {
    if (!binary_pred(*first1, *first2)) {
      return false;
    }
  }
  return true;
}
/*******************************************************************/

/*********************************************
 * fill:
 ***********************************************/
template <class ForwardIterator, class T>
void my_fill(ForwardIterator first, ForwardIterator last, const T& value) {
  for (; first != last; ++first) {
    *first = value;
  }
}
/*******************************************************************/

/*********************************************
 * fill_n:
 ***********************************************/
template <class OutputIterator, class Size, class T>
OutputIterator my_fill_n(OutputIterator first, Size n, const T& value) {
  for (; n > 0; --n, ++first) {
    *first = value;
  }

  return first;
}
/*******************************************************************/

/*********************************************
 * iter_swap:
 ***********************************************/
template <class ForwardIterator1, class ForwardIterator2>
inline void my_iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
  __my_iter_swap(a, b, value_type(a));
}

template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __my_iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*) {
  T tmp = *a;
  *a = *b;
  *b = tmp;
}

/*******************************************************************/

/*********************************************
 * max:
 ***********************************************/
template <class T>
inline const T& my_max(const T& a, const T& b) {
  return a < b ? b : a;
}

template <class T, class Compare>
inline const T& my_max(const T& a, const T& b, Compare comp) {
  return comp(a, b) ? b : a;
}
/*******************************************************************/

/*********************************************
 * min:
 ***********************************************/
template <class T>
inline const T& my_min(const T& a, const T& b) {
  return b < a ? b : a;
}

template <class T, class Compare>
inline const T& my_min(const T& a, const T& b, Compare comp) {
  return comp(b, a) ? b : a;
}
/*******************************************************************/

/*********************************************
 * lexicographical_compare:
 ***********************************************/
template <class InputIterator1, class InputIterator2>
bool my_lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                InputIterator2 last2) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (*first1 < *first2) {
      return true;
    } else if (*first2 < *first1) {
      return false;
    }
  }

  return first1 == last1 && first2 != last2;
}

template <class InputIterator1, class InputIterator2, class Compare>
bool my_lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                InputIterator2 last2, Compare comp) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (comp(*first1, *first2)) {
      return true;
    } else if (comp(*first2, *first1)) {
      return false;
    }
  }

  return first1 == last1 && first2 != last2;
}

inline bool my_lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
                                       const unsigned char* first2, const unsigned char* last2) {
  const size_t len1 = last1 - first1;
  const size_t len2 = last2 - first2;

  const int result = memcmp(first1, first2, my_min(len1, len2));
  return result != 0 ? result < 0 : len1 < len2;
}

inline bool my_lexicographical_compare(const char* first1, const char* last1, const char* first2,
                                       const char* last2) {
  const size_t len1 = last1 - first1;
  const size_t len2 = last2 - first2;

  const int result = memcmp(first1, first2, my_min(len1, len2));
  return result != 0 ? result < 0 : len1 < len2;
}
/*******************************************************************/

/*********************************************
 * mismatch:
 ***********************************************/
template <class InputIterator1, class InputIterator2>
my_pair<InputIterator1, InputIterator2> my_mismatch(InputIterator1 first1, InputIterator1 last1,
                                                    InputIterator2 first2) {
  while (first1 != last1 && *first1 == *first2) {
    ++first1;
    ++first2;
  }

  return my_pair<InputIterator1, InputIterator2>(first1, first2);
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
my_pair<InputIterator1, InputIterator2> my_mismatch(InputIterator1 first1, InputIterator1 last1,
                                                    InputIterator2 first2,
                                                    BinaryPredicate binary_pred) {
  while (first1 != last1 && binary_pred(*first1, *first2)) {
    ++first1;
    ++first2;
  }

  return my_pair<InputIterator1, InputIterator2>(first1, first2);
}
/*******************************************************************/

/*********************************************
 * copy:
 ***********************************************/
template <class InputIterator, class OutputIterator>
struct __my_copy_dispatch {
  OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
    return __my_copy(first, last, result, iterator_category(first));
  }
};

template <class T>
struct __my_copy_dispatch<T*, T*> {
  T* operator()(T* first, T* last, T* result) {
    typedef typename __my_type_traits<T>::has_trivial_assignment_operator t;
    return __my_copy_t(first, last, result, t());
  }
};

template <class T>
struct __my_copy_dispatch<const T*, T*> {
  T* operator()(const T* first, const T* last, T* result) {
    typedef typename __my_type_traits<T>::has_trivial_assignment_operator t;
    return __my_copy_t(first, last, result, t());
  }
};

// copy
template <class InputIterator, class OutputIterator>
inline OutputIterator my_copy(InputIterator first, InputIterator last, OutputIterator result) {
  return __my_copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}
// special copy
inline char* my_copy(const char* first, const char* last, char* result) {
  memmove(result, first, last - first);
  return result + (last - first);
}
// specila copy
inline wchar_t* my_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}
// InputIterator __copy
template <class InputIterator, class OutputIterator>
inline OutputIterator __my_copy(InputIterator first, InputIterator last, OutputIterator result,
                                my_input_iterator_tag) {
  bool contains = false;
  InputIterator tmp = first;
  ptrdiff_t all_len = 0;
  ptrdiff_t before_len = 0;
  while (tmp != last) {
    ++all_len;  // add all the len to get the length of first and last
    if (tmp != result) {
      if (!contains) {
        ++before_len;  // get the length of not containts
      }
    } else {
      contains = true;
    }
    ++tmp;
  }
  if (!contains) {
    for (; first != last; ++first, ++result) {
      *result = *first;
    }
    return result;
  } else {
    // if result is in the range of first and last
    if (before_len >= all_len / 2) {
      OutputIterator first_tmp = result;
      OutputIterator result_tmp = result;
      for (ptrdiff_t i = 1; i <= before_len; ++i) {
        ++result_tmp;
      }
      for (; first_tmp != last; ++first_tmp, ++result_tmp) {
        *result_tmp = *first_tmp;
      }
      last = result;
      for (; first != last; ++first, ++result) {
        *result = *first;
      }
      for (ptrdiff_t i = all_len - before_len; i > 0; --i) {
        ++result;
      }
      return result;
    } else {
      // less than all_len / 2
      ptrdiff_t in_len = all_len - before_len;
      InputIterator first_tmp = first;
      OutputIterator result_tmp = result;
      for (int i = 1; i <= in_len; ++i) {
        ++first_tmp;
        ++result_tmp;
      }
      for (; first_tmp != last; ++first_tmp, ++result_tmp) {
        // deal with the out of len
        *result_tmp = *first_tmp;
      }
      --in_len;
      while (in_len >= before_len) {
        // deal with the iterator between
        first_tmp = first;
        result_tmp = result;
        for (int i = 1; i <= in_len; ++i) {
          ++first_tmp;
          ++result_tmp;
        }
        *result_tmp = *first_tmp;
        --in_len;
      }
      last = result;
      first_tmp = first;
      result_tmp = result;
      for (; first_tmp != last; ++first_tmp, ++result_tmp) {
        // deal with the before_len,
        *result_tmp = *first_tmp;
      }
      for (int j = 1; j <= all_len; ++j) {
        ++result;
      }
      return result;
    }
  }
}

// RandomAccessIterator __copy
template <class RandomAccessIterator, class OutputIterator>
inline OutputIterator __my_copy(RandomAccessIterator first, RandomAccessIterator last,
                                OutputIterator result, my_random_access_iterator_tag) {
  return __my_copy_d(first, last, result, distance_type(first));
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator __my_copy_d(RandomAccessIterator first, RandomAccessIterator last,
                                  OutputIterator result, Distance*) {
  Distance all_len = last - first;
  RandomAccessIterator first_tmp = first;
  bool containts = false;

  for (; first_tmp != last; ++first_tmp) {
    if (first_tmp == result) {
      containts = true;
      break;
    }
  }

  if (!containts) {
    // if the result is not in the range of first and last
    for (Distance n = all_len; n > 0; --n, ++first, ++result) {
      *result = *first;
    }
    return result;
  } else {
    RandomAccessIterator reverse_first = last;
    --reverse_first;
    OutputIterator result_first = result;
    for (Distance n = all_len; n > 0; --n) {
      ++result_first;
    }
    --result_first;
    for (Distance n = all_len; n > 0; --n, --reverse_first, --result_first) {
      *result_first = *reverse_first;
    }
    for (Distance n = all_len; n > 0; --n) {
      ++result;
    }
    return result;
  }
}

template <class T>
inline T* __my_copy_t(const T* first, const T* last, T* result, __my_true_type) {
  memmove(result, first, sizeof(T) * (last - first));
  return result + (last - first);
}

template <class T>
inline T* __my_copy_t(const T* first, const T* last, T* result, __my_false_type) {
  return __my_copy_d(first, last, result, (ptrdiff_t*)0);
}
/* end of copy */
/************************************************************************/

/*********************************************
 * copy_backward:
 ***********************************************/
template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __my_copy_backward_dispatch {
  BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                    BidirectionalIterator2 result) {
    return __my_copy_backward(first, last, result, iterator_category(first));
  }
};

template <class T>
struct __my_copy_backward_dispatch<T*, T*> {
  T* operator()(T* first, T* last, T* result) {
    typedef typename __my_type_traits<T>::has_trivial_assignment_operator t;
    return __my_copy_backward_t(first, last, result, t());
  }
};

template <class T>
struct __my_copy_backward_dispatch<const T*, T*> {
  T* operator()(const T* first, const T* last, T* result) {
    typedef typename __my_type_traits<T>::has_trivial_assignment_operator t;
    return __my_copy_backward_t(first, last, result, t());
  }
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 my_copy_backward(BidirectionalIterator1 first,
                                               BidirectionalIterator1 last,
                                               BidirectionalIterator2 result) {
  return __my_copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last,
                                                                                       result);
}

inline char* my_copy_backward(const char* first, const char* last, char* result) {
  char* cur = result - (last - first);
  memmove(cur, first, last - first);
  return cur;
}

inline wchar_t* my_copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result) {
  wchar_t* cur = result - (last - first);
  memmove(cur, first, sizeof(wchar_t) * (last - first));
  return cur;
}

/* to copy the value from the first and last to the result
   but be careful that the space of result between the first and last
 */
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 __my_copy_backward(BidirectionalIterator1 first,
                                                 BidirectionalIterator1 last,
                                                 BidirectionalIterator2 result,
                                                 my_bidirectional_iterator_tag) {
  BidirectionalIterator1 first_cur = first;
  BidirectionalIterator2 cur = result;
  bool include_start = false;
  while (first_cur++ != last) {
    // to get the start of the result
    --cur;
    if (cur == last) {
      // if the start of result is included in the first and last
      include_start = true;
      break;
    }
  }

  if (include_start) {
    // if the space of result is included in the first and last
    BidirectionalIterator1 tmp_last = last;
    --tmp_last;
    BidirectionalIterator1 tmp_first = first;
    --tmp_first;
    for (; tmp_last != tmp_first; --tmp_last) {
      --result;
      *result = *tmp_last;
    }
    return result;
  } else {
    // the start of result is not included in the first and last
    first_cur = first;
    for (; first != last; ++first, ++cur) {
      *cur = *first;
    }
    while (first_cur++ != last) {
      // to get the start of the result
      --cur;
    }
    return cur;
  }
}

template <class RandomAccessIterator1, class RandomAccessIterator2>
inline RandomAccessIterator2 __my_copy_backward(RandomAccessIterator1 first,
                                                RandomAccessIterator1 last,
                                                RandomAccessIterator2 result,
                                                my_random_access_iterator_tag) {
  return __my_copy_backward_d(first, last, result, distance_type(first));
}

template <class RandomAccessIterator1, class RandomAccessIterator2, class Distance>
inline RandomAccessIterator2 __my_copy_backward_d(RandomAccessIterator1 first,
                                                  RandomAccessIterator1 last,
                                                  RandomAccessIterator2 result, Distance* d) {
  RandomAccessIterator1 first_tmp = first;
  Distance all_len = last - first;
  if (all_len <= 0) {
    throw;
  }
  bool contains = false;
  for (Distance n = all_len; n > 0; --n) {
    // to make sure whether the start of result is in the range of first and last
    if (first_tmp == result) {
      contains = true;
      break;
    } else {
      ++first_tmp;
    }
  }

  if (!contains) {
    // if the start of result is not in the range of first and last, then deal with this directly
    RandomAccessIterator1 reverse_first = last;
    --reverse_first;
    --result;
    for (Distance n = all_len; n > 0; --n, --reverse_first, --result) {
      *result = *reverse_first;
    }
    ++result;
    return result;
  } else {
    // if the start of result is in the range of first and last, then deal with this according to
    // the function
    // __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result,
    // Distance*)
    RandomAccessIterator2 result_first = result;
    for (Distance n = all_len; n > 0; --n) {
      --result_first;
    }
    __my_copy_d(first, last, result_first, d);

    return result_first;
  }
}

template <class T>
inline T* __my_copy_backward_t(const T* first, const T* last, T* result, __my_true_type) {
  T* cur = result - (last - first);
  memmove(cur, first, sizeof(T) * (last - first));
  return cur;
}

template <class T>
inline T* __my_copy_backward_t(const T* first, const T* last, T* result, __my_false_type) {
  return __my_copy_backward_d(first, last, result, (ptrdiff_t*)0);
}
/* end of function is : my_copy_backward*/
/********************************************************/

/*************************************************************/
/********************
 * lower_bound:
 *********/
template <class ForwardIterator, class T>
inline ForwardIterator my_lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
  return __my_lower_bound(first, last, value, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator my_lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                      Compare comp) {
  return __my_lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __my_lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                 Distance*, my_forward_iterator_tag) {
  Distance len = 0;
  len = my_distance(first, last);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first;
    my_advance(middle, half);
    if (*middle < value) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __my_lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                      const T& value, Distance*, my_random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (*middle < value) {
      first = middle + 1;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __my_lower_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                 Compare comp, Distance*, my_forward_iterator_tag) {
  Distance len = 0;
  len = my_distance(first, last);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first;
    my_advance(middle, half);
    if (comp(*middle, value)) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __my_lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                      const T& value, Compare comp, Distance*,
                                      my_random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (comp(*middle, value)) {
      first = middle + 1;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}
/****************************************************/

/*********************************
 * upper_bound:
 *********************************/
template <class ForwardIterator, class T>
inline ForwardIterator my_upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
  return __my_upper_bound(first, last, value, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator my_upper_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                      Compare comp) {
  return __my_upper_bound(first, last, value, comp, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __my_upper_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                 Distance*, my_forward_iterator_tag) {
  Distance len = 0;
  len = my_distance(first, last);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first;
    my_advance(middle, half);
    if (value < *middle) {
      len = half;
    } else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __my_upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                      const T& value, Distance*, my_random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (value < *middle) {
      len = half;
    } else {
      first = middle + 1;
      len = len - half - 1;
    }
  }
  return first;
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __my_upper_bound(ForwardIterator first, ForwardIterator last, const T& value,
                                 Compare comp, Distance*, my_forward_iterator_tag) {
  Distance len = 0;
  len = my_distance(first, last);
  Distance half;
  ForwardIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first;
    my_advance(middle, half);
    if (comp(value, *middle)) {
      len = half;
    } else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __my_upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                      const T& value, Compare comp, Distance*,
                                      my_random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle;

  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (comp(value, *middle)) {
      len = half;
    } else {
      first = middle + 1;
      len = len - half - 1;
    }
  }
  return first;
}
/*************************************************************/

/*********************************
 * function:
 * binary_search
 *********************************/
template <class ForwardIterator, class T>
bool my_binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
  ForwardIterator i = my_lower_bound(first, last, value);
  return i != last && !(value < *i);
}

template <class ForwardIterator, class T, class Compare>
bool my_binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
  ForwardIterator i = my_lower_bound(first, last, value, comp);
  return i != last && !comp(value, *i);
}
/****************************************************/

/*********************************
 * function:
 * next_permutation
 ********************************/
template <class BidirectionalIterator>
bool my_next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
  if (first == last) {
    return false;
  }
  BidirectionalIterator i = first;
  ++i;
  if (i == last) {
    return false;
  }
  i = last;
  --i;

  for (;;) {
    BidirectionalIterator ii = i;
    --i;
    if (*i < *ii) {
      BidirectionalIterator j = last;
      while (!(*i < *--j)) {
      }
      my_iter_swap(i, j);
      my_reverse(ii, last);
      return true;
    }
    if (i == first) {
      my_reverse(first, last);
      return false;
    }
  }
}

template <class BidirectionalIterator>
bool my_prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
  if (first == last) {
    return false;
  }
  BidirectionalIterator i = first;
  ++i;
  if (i == last) {
    return false;
  }
  i = last;
  --i;

  for (;;) {
    BidirectionalIterator ii = i;
    --i;
    if (*ii < *i) {
      BidirectionalIterator j = last;
      while (!(*--j < *i)) {
      }
      my_iter_swap(i, j);
      my_reverse(ii, last);
      return true;
    }
    if (i == first) {
      my_reverse(first, last);
      return false;
    }
  }
}
/*************************************************************/

/*********************************
 * function:
 * partial_sort
 ********************************/
template <class RandomAccessIterator>
inline void my_partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                            RandomAccessIterator last) {
  __my_partial_sort(first, middle, last, value_type(first));
}

template <class RandomAccessIterator, class T>
void __my_partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                       RandomAccessIterator last, T*) {
  my_make_heap(first, middle);
  for (RandomAccessIterator i = middle; i != last; ++i) {
    if (*i < *first) {
      __my_pop_heap(first, middle, i, T(*i), distance_type(first));
    }
  }
  my_sort_heap(first, middle);
}
/****************************************************/

/*********************************
 * function:
 * partial_sort_copy
 ********************************/
template <class InputIterator, class RandomAccessIterator>
inline RandomAccessIterator my_partial_sort_copy(InputIterator first, InputIterator last,
                                                 RandomAccessIterator result_first,
                                                 RandomAccessIterator result_last) {
  __my_partial_sort_copy(first, last, result_first, result_last, value_type(first));
}

template <class InputIterator, class RandomAccessIterator, class T>
void __my_partial_sort_copy(InputIterator first, InputIterator last,
                            RandomAccessIterator result_first, RandomAccessIterator result_last,
                            T*) {
  my_make_heap(first, last);
  my_sort_heap(first, last);

  my_copy(first, last, result_first);
}
/*************************************************************/

/*********************************
 * function:
 * Insertion Sort
 ********************************/
template <class RandomAccessIterator>
void __my_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
  if (first == last) {
    return;
  }
  for (RandomAccessIterator i = first + 1; i != last; ++i) {
    __my_linear_insert(first, i, value_type(first));
  }
}

template <class RandomAccessIterator, class T>
inline void __my_linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*) {
  T value = *last;
  if (value < *first) {
    my_copy_backward(first, last, last + 1);
    *first = value;
  } else {
    __my_unguarded_linear_insert(last, value);
  }
}

template <class RandomAccessIterator, class T>
void __my_unguarded_linear_insert(RandomAccessIterator last, T value) {
  RandomAccessIterator next = last;
  --next;
  while (value < *next) {
    *last = *next;
    last = next;
    --next;
  }
  *last = value;
}
/****************************************************/

/*********************************
 * function:
 * partition
 ********************************/
template <class T>
inline const T& __my_median(const T& a, const T& b, const T& c) {
  if (a < b) {
    if (b < c) {
      return b;
    } else if (a < c) {
      return c;
    } else {
      return a;
    }
  } else {
    if (a < c) {
      return a;
    } else if (b < c) {
      return c;
    } else {
      return b;
    }
  }
}
/*************************************************************/

/*********************************
 * function:
 * unguarded_partition
 ********************************/
template <class RandomAccessIterator, class T>
RandomAccessIterator __my_unguarded_partition(RandomAccessIterator first, RandomAccessIterator last,
                                              T pivot) {
  while (true) {
    while (*first < pivot) {
      ++first;
    }
    --last;
    while (pivot < *last) {
      --last;
    }
    if (!(first < last)) {
      return first;
    }
    my_iter_swap(first, last);
    ++first;
  }
}

template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator __my_unguarded_partition(RandomAccessIterator first, RandomAccessIterator last,
                                              T pivot, Compare comp) {
  while (true) {
    while (comp(*first, pivot)) {
      ++first;
    }
    --last;
    while (comp(pivot, *last)) {
      --last;
    }
    if (!(first < last)) {
      return first;
    }
    my_iter_swap(first, last);
    ++first;
  }
}
/****************************************************/

/*********************************
 * function:
 * sort
 ********************************/
const int __my_stl_threshold = 16;

template <class RandomAccessIterator>
inline void my_sort(RandomAccessIterator first, RandomAccessIterator last) {
  if (first != last) {
    __my_introsort_loop(first, last, value_type(first), __my_lg(last - first) * 2);
    __my_final_insertion_sort(first, last);
  }
}

template <class Size>
inline Size __my_lg(Size n) {
  Size k;
  for (k = 0; n > 1; n >>= 1) {
    ++k;
  }
  return k;
}

template <class RandomAccessIterator, class T, class Size>
void __my_introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*,
                         Size depth_limit) {
  while (last - first > __my_stl_threshold) {
    if (depth_limit == 0) {
      // make heap
      my_partial_sort(first, last, last);
      return;
    }
    --depth_limit;
    // quick sort
    RandomAccessIterator cur = __my_unguarded_partition(
        first, last, T(__my_median(*first, *(first + (last - first) / 2), *(last - 1))));
    __my_introsort_loop(cur, last, value_type(first), depth_limit);
    last = cur;
  }
}

template <class RandomAccessIterator>
void __my_final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
  if (last - first > __my_stl_threshold) {
    // insertion sort
    __my_insertion_sort(first, first + __my_stl_threshold);
    __my_unguarded_insertion_sort(first + __my_stl_threshold, last);
  } else {
    __my_insertion_sort(first, last);
  }
}

template <class RandomAccessIterator>
inline void __my_unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
  __my_unguarded_insertion_sort_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __my_unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last,
                                              T*) {
  for (RandomAccessIterator i = first; i != last; ++i) {
    __my_unguarded_linear_insert(i, T(*i));
  }
}
/*************************************************************/

/*********************************
 * function:
 * equal_range
 ********************************/
template <class ForwardIterator, class T>
inline my_pair<ForwardIterator, ForwardIterator> my_equal_range(ForwardIterator first,
                                                                ForwardIterator last,
                                                                const T& value) {
  return __my_equal_range(first, last, value, distance_type(first), iterator_category(first));
}

template <class RandomAccessIterator, class T, class Distance>
my_pair<RandomAccessIterator, RandomAccessIterator> __my_equal_range(
    RandomAccessIterator first, RandomAccessIterator last, const T& value, Distance*,
    my_random_access_iterator_tag) {
  Distance len = last - first;
  Distance half;
  RandomAccessIterator middle, left, right;
  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (*middle < value) {
      first = middle + 1;
      len = len - half - 1;
    } else if (value < *middle) {
      len = half;
    } else {
      left = my_lower_bound(first, middle, value);
      right = my_upper_bound(++middle, first + len, value);
      return my_pair<RandomAccessIterator, RandomAccessIterator>(left, right);
    }
  }
  return my_pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template <class ForwardIterator, class T, class Distance>
my_pair<ForwardIterator, ForwardIterator> __my_equal_range(ForwardIterator first,
                                                           ForwardIterator last, const T& value,
                                                           Distance*, my_forward_iterator_tag) {
  Distance len = 0;
  len = my_distance(first, last);
  Distance half;
  ForwardIterator middle, left, right;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    my_advance(middle, half);
    if (*middle < value) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else if (value < *middle) {
      len = half;
    } else {
      left = my_lower_bound(first, middle, value);
      my_advance(first, len);
      right = my_upper_bound(++middle, first, value);
      return my_pair<ForwardIterator, ForwardIterator>(left, right);
    }
  }
  return my_pair<ForwardIterator, ForwardIterator>(first, first);
}
/****************************************************/

/*********************************
 * function:
 * nth_element
 ********************************/
template <class RandomAccessIterator>
inline void my_nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                           RandomAccessIterator last) {
  __my_nth_element(first, nth, last, value_type(first));
}

template <class RandomAccessIterator, class T>
void __my_nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                      RandomAccessIterator last, T*) {
  while (last - first > 3) {
    RandomAccessIterator cut = __my_unguarded_partition(
        first, last, T(__my_median(*first, *(first + (last - first) / 2), *(last - 1))));
    if (cut <= nth) {
      first = cut;
    } else {
      last = cut;
    }
  }
  __my_insertion_sort(first, last);
}

template <class RandomAccessIterator, class Compare>
inline void my_nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                           RandomAccessIterator last, Compare comp) {
  __my_nth_element(first, nth, last, value_type(first), comp);
}

template <class RandomAccessIterator, class T, class Compare>
void __my_nth_element(RandomAccessIterator first, RandomAccessIterator nth,
                      RandomAccessIterator last, T*, Compare comp) {
  while (last - first > 3) {
    RandomAccessIterator cut = __my_unguarded_partition(
        first, last, T(__my_median(*first, *(first + (last - first) / 2), *(last - 1))), comp);
    if (cut <= nth) {
      first = cut;
    } else {
      last = cut;
    }
  }
  __my_insertion_sort(first, last);
}
/*************************************************************/

/*********************************
 * function:
 * mergesort
 ********************************/
template <class BidirectionalIterator>
void my_mergesort(BidirectionalIterator first, BidirectionalIterator last) {
  typename my_iterator_traits<BidirectionalIterator>::difference_type n = my_distance(first, last);
  if (n == 0 || n == 1) {
    return;
  } else {
    BidirectionalIterator mid = first + n / 2;
    my_mergesort(first, mid);
    my_mergesort(mid, last);
    my_inplace_merge(first, mid, last);
  }
}
/****************************************************/
#endif
