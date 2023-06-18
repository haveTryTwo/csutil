// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_ALGO_H_
#define MY_STL_ALGO_H_

#include "my_stl_algobase.h"

/**************************************
 * function
 * unique
 **************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator my_set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                            InputIterator2 last2, OutputIterator result) {
  while (first1 != last1 && first2 != last2) {
    if (*first1 < *first2) {
      *result = *first1;
      ++first1;
    } else if (*first2 < *first1) {
      *result = *first2;
      ++first2;
    } else {
      *result = *first1;
      ++first1;
      ++first2;
    }
    ++result;
  }

  return my_copy(first2, last2, my_copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator my_set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                            InputIterator2 last2, OutputIterator result, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first1, *first2)) {
      *result = *first1;
      ++first1;
    } else if (comp(*first2, *first1)) {
      *result = *first2;
      ++first2;
    } else {
      *result = *first1;
      ++first1;
      ++first2;
    }
    ++result;
  }

  return my_copy(first2, last2, my_copy(first1, last1, result));
}
/*********************************************************/

/**************************************
 * function
 * set_intersection
 **************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator my_set_intersection(InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, InputIterator2 last2,
                                   OutputIterator result) {
  while (first1 != last1 && first2 != last2) {
    if (*first1 < *first2) {
      ++first1;
    } else if (*first2 < *first1) {
      ++first2;
    } else {
      *result = *first1;
      ++first1;
      ++first2;
      ++result;
    }
  }
  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator my_set_intersection(InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, InputIterator2 last2,
                                   OutputIterator result, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first1, *first2)) {
      ++first1;
    } else if (comp(*first2, *first1)) {
      ++first2;
    } else {
      *result = *first1;
      ++first1;
      ++first2;
      ++result;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * set_difference
 **************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator my_set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                 InputIterator2 last2, OutputIterator result) {
  while (first1 != last1 && first2 != last2) {
    if (*first1 < *first2) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (*first2 < *first1) {
      ++first2;
    } else {
      ++first1;
      ++first2;
    }
  }

  return my_copy(first1, last1, result);
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator my_set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                 InputIterator2 last2, OutputIterator result, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first1, *first2)) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (comp(*first2, *first1)) {
      ++first2;
    } else {
      ++first1;
      ++first2;
    }
  }

  return my_copy(first1, last1, result);
}
/*********************************************************/

/**************************************
 * function
 * set_symmetric_difference
 **************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator my_set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                           InputIterator2 first2, InputIterator2 last2,
                                           OutputIterator result) {
  while (first1 != last1 && first2 != last2) {
    if (*first1 < *first2) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (*first2 < *first1) {
      *result = *first2;
      ++first2;
      ++result;
    } else {
      ++first1;
      ++first2;
    }
  }

  return my_copy(first1, last1, my_copy(first2, last2, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator my_set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                           InputIterator2 first2, InputIterator2 last2,
                                           OutputIterator result, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first1, *first2)) {
      *result = *first1;
      ++first1;
      ++result;
    } else if (comp(*first2, *first1)) {
      *result = *first2;
      ++first2;
      ++result;
    } else {
      ++first1;
      ++first2;
    }
  }

  return my_copy(first1, last1, my_copy(first2, last2, result));
}
/*********************************************************/

/**************************************
 * function
 * adjacent_find
 **************************************/
template <class ForwardIterator>
ForwardIterator my_adjacent_find(ForwardIterator first, ForwardIterator last) {
  if (first == last) {
    return last;
  }
  ForwardIterator next = first;
  while (++next != last) {
    if (*next == *first) {
      return first;
    }
    first = next;
  }
  return last;
}

template <class ForwardIterator, class BinaryPredicate>
ForwardIterator my_adjacent_find(ForwardIterator first, ForwardIterator last,
                                 BinaryPredicate binary_pred) {
  if (first == last) {
    return last;
  }
  ForwardIterator next = first;
  while (++next != last) {
    if (binary_pred(*next, *first)) {
      return first;
    }
    first = next;
  }
  return last;
}
/*********************************************************/

/**************************************
 * function
 * count
 **************************************/
template <class InputIterator, class T>
typename my_iterator_traits<InputIterator>::difference_type my_count(InputIterator first,
                                                                     InputIterator last,
                                                                     const T& value) {
  typename my_iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first) {
    if (*first == value) {
      ++n;
    }
  }
  return n;
}

template <class InputIterator, class T, class Size>
void my_count(InputIterator first, InputIterator last, const T& value, Size& n) {
  for (; first != last; ++first) {
    if (*first == value) {
      ++n;
    }
  }
}
/*********************************************************/

/**************************************
 * function
 * count_if
 **************************************/
template <class InputIterator, class Predicate>
typename my_iterator_traits<InputIterator>::difference_type my_count_if(InputIterator first,
                                                                        InputIterator last,
                                                                        Predicate pred) {
  typename my_iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first) {
    if (pred(*first)) {
      ++n;
    }
  }
  return n;
}

template <class InputIterator, class Predicate, class Size>
void my_count_if(InputIterator first, InputIterator last, Predicate pred, Size& n) {
  for (; first != last; ++first) {
    if (pred(*first)) {
      ++n;
    }
  }
}
/*********************************************************/

/**************************************
 * function
 * find
 **************************************/
template <class InputIterator, class T>
InputIterator my_find(InputIterator first, InputIterator last, const T& value) {
  while (first != last && *first != value) {
    ++first;
  }
  return first;
}
/*********************************************************/

/**************************************
 * function
 * find_if
 **************************************/
template <class InputIterator, class Predicate>
InputIterator my_find_if(InputIterator first, InputIterator last, Predicate pred) {
  while (first != last && !pred(*first)) {
    ++first;
  }
  return first;
}
/*********************************************************/

/**************************************
 * function
 * search
 **************************************/
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 my_search(ForwardIterator1 first1, ForwardIterator1 last1,
                                  ForwardIterator2 first2, ForwardIterator2 last2) {
  return __my_search(first1, last1, first2, last2, distance_type(first1), distance_type(first2));
}

template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2>
ForwardIterator1 __my_search(ForwardIterator1 first1, ForwardIterator1 last1,
                             ForwardIterator2 first2, ForwardIterator2 last2, Distance1*,
                             Distance2*) {
  Distance1 d1 = 0;
  d1 = my_distance(first1, last1);
  Distance2 d2 = 0;
  d2 = my_distance(first2, last2);

  if (abs(d1) < abs(d2)) {
    return last1;
  }

  ForwardIterator1 current1 = first1;
  ForwardIterator2 current2 = first2;

  while (current2 != last2) {
    if (*current1 == *current2) {
      ++current1;
      ++current2;
    } else {
      if (d1 == d2) {
        return last1;
      } else {
        current1 = ++first1;
        current2 = first2;
        --d1;
      }
    }
  }

  return first1;
}

template <class ForwardIterator1, class ForwardIterator2, class Compare>
inline ForwardIterator1 my_search(ForwardIterator1 first1, ForwardIterator1 last1,
                                  ForwardIterator2 first2, ForwardIterator2 last2, Compare comp) {
  return __my_search(first1, last1, first2, last2, distance_type(first1), distance_type(first2),
                     comp);
}

template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2,
          class Compare>
ForwardIterator1 __my_search(ForwardIterator1 first1, ForwardIterator1 last1,
                             ForwardIterator2 first2, ForwardIterator2 last2, Distance1*,
                             Distance2*, Compare comp) {
  Distance1 d1 = 0;
  d1 = my_distance(first1, last1);
  Distance2 d2 = 0;
  d2 = my_distance(first2, last2);

  if (abs(d1) < abs(d2)) {
    return last1;
  }

  ForwardIterator1 current1 = first1;
  ForwardIterator2 current2 = first2;

  while (current2 != last2) {
    if (comp(*current1, *current2)) {
      ++current1;
      ++current2;
    } else {
      if (d1 == d2) {
        return last1;
      } else {
        current1 = ++first1;
        current2 = first2;
        --d1;
      }
    }
  }

  return first1;
}
/*********************************************************/

/**************************************
 * function
 * find_end
 **************************************/
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 my_find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                                    ForwardIterator2 first2, ForwardIterator2 last2) {
  typedef typename my_iterator_traits<ForwardIterator1>::iterator_category category1;
  typedef typename my_iterator_traits<ForwardIterator2>::iterator_category category2;

  return __my_find_end(first1, last1, first2, last2, category1(), category2());
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 __my_find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2,
                               my_forward_iterator_tag, my_forward_iterator_tag) {
  if (first2 == last2) {
    return last1;
  } else {
    ForwardIterator1 result = last1;
    while (true) {
      ForwardIterator1 new_result = my_search(first1, last1, first2, last2);
      if (new_result == last1) {
        return result;
      } else {
        result = new_result;
        first1 = new_result;
        ++first1;
      }
    }
  }
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 __my_find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                     BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                     my_bidirectional_iterator_tag, my_bidirectional_iterator_tag) {
  typedef my_reverse_iterator<BidirectionalIterator1> reviter1;
  typedef my_reverse_iterator<BidirectionalIterator2> reviter2;

  reviter1 rlast1(first1);
  reviter2 rlast2(first2);

  reviter1 rresult = my_search(reviter1(last1), rlast1, reviter2(last2), rlast2);

  if (rresult == rlast1) {
    return last1;
  } else {
    BidirectionalIterator1 result = rresult.base();
    my_advance(result, -my_distance(first2, last2));
    return result;
  }
}

template <class ForwardIterator1, class ForwardIterator2, class Compare>
inline ForwardIterator1 my_find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                                    ForwardIterator2 first2, ForwardIterator2 last2, Compare comp) {
  typedef typename my_iterator_traits<ForwardIterator1>::iterator_category category1;
  typedef typename my_iterator_traits<ForwardIterator2>::iterator_category category2;

  return __my_find_end(first1, last1, first2, last2, category1(), category2(), comp);
}

template <class ForwardIterator1, class ForwardIterator2, class Compare>
ForwardIterator1 __my_find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2,
                               my_forward_iterator_tag, my_forward_iterator_tag, Compare comp) {
  if (first2 == last2) {
    return last1;
  } else {
    ForwardIterator1 result = last1;
    while (true) {
      ForwardIterator1 new_result = my_search(first1, last1, first2, last2, comp);
      if (new_result == last1) {
        return result;
      } else {
        result = new_result;
        first1 = new_result;
        ++first1;
      }
    }
  }
}

template <class BidirectionalIterator1, class BidirectionalIterator2, class Compare>
BidirectionalIterator1 __my_find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
                                     BidirectionalIterator2 first2, BidirectionalIterator2 last2,
                                     my_bidirectional_iterator_tag, my_bidirectional_iterator_tag,
                                     Compare comp) {
  typedef my_reverse_iterator<BidirectionalIterator1> reviter1;
  typedef my_reverse_iterator<BidirectionalIterator2> reviter2;

  reviter1 rlast1(first1);
  reviter2 rlast2(first2);

  reviter1 rresult = my_search(reviter1(last1), rlast1, reviter2(last2), rlast2, comp);

  if (rresult == rlast1) {
    return last1;
  } else {
    BidirectionalIterator1 result = rresult.base();
    my_advance(result, -my_distance(first2, last2));
    return result;
  }
}
/*********************************************************/

/**************************************
 * function
 * find_first_of
 **************************************/
template <class InputIterator, class ForwardIterator>
InputIterator my_find_first_of(InputIterator first1, InputIterator last1, ForwardIterator first2,
                               ForwardIterator last2) {
  for (; first1 != last1; ++first1) {
    for (ForwardIterator iter = first2; iter != last2; ++iter) {
      if (*first1 == *iter) {
        return first1;
      }
    }
  }

  return last1;
}

template <class InputIterator, class ForwardIterator, class Compare>
InputIterator my_find_first_of(InputIterator first1, InputIterator last1, ForwardIterator first2,
                               ForwardIterator last2, Compare comp) {
  for (; first1 != last1; ++first1) {
    for (ForwardIterator iter = first2; iter != last2; ++iter) {
      if (comp(*first1, *iter)) {
        return first1;
      }
    }
  }

  return last1;
}
/*********************************************************/

/**************************************
 * function
 * for_each
 **************************************/
template <class InputIterator, class Function>
Function my_for_each(InputIterator first, InputIterator last, Function f) {
  for (; first != last; ++first) {
    f(*first);
  }
  return f;
}
/*********************************************************/

/**************************************
 * function
 * generate
 **************************************/
template <class ForwardIterator, class Generator>
void my_generate(ForwardIterator first, ForwardIterator last, Generator gen) {
  for (; first != last; ++first) {
    *first = gen();
  }
}
/*********************************************************/

/**************************************
 * function
 * generate_n
 **************************************/
template <class OutputIterator, class Size, class Generator>
OutputIterator my_generate_n(OutputIterator first, Size n, Generator gen) {
  for (; n > 0; --n, ++first) {
    *first = gen();
  }
  return first;
}
/*********************************************************/

/**************************************
 * function
 * includes
 **************************************/
template <class InputIterator1, class InputIterator2>
bool my_includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                 InputIterator2 last2) {
  while (first1 != last1 && first2 != last2) {
    if (*first2 < *first1) {
      return false;
    } else if (*first1 < *first2) {
      return false;
    } else {
      ++first1;
      ++first2;
    }
  }

  return first2 == last2;
}

template <class InputIterator1, class InputIterator2, class Compare>
bool my_includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                 InputIterator2 last2, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first2, *first1)) {
      return false;
    } else if (comp(*first1, *first2)) {
      return false;
    } else {
      ++first1;
      ++first2;
    }
  }

  return first2 == last2;
}
/*********************************************************/

/**************************************
 * function
 * max_element
 **************************************/
template <class ForwardIterator>
ForwardIterator my_max_element(ForwardIterator first, ForwardIterator last) {
  if (first == last) {
    return last;
  }
  ForwardIterator result = first;
  while (++first != last) {
    if (*result < *first) {
      result = first;
    }
  }
  return result;
}

template <class ForwardIterator, class Compare>
ForwardIterator my_max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
  if (first == last) {
    return last;
  }
  ForwardIterator result = first;
  while (++first != last) {
    if (comp(*result, *first)) {
      result = first;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * min_element
 **************************************/
template <class ForwardIterator>
ForwardIterator my_min_element(ForwardIterator first, ForwardIterator last) {
  if (first == last) {
    return last;
  }
  ForwardIterator result = first;
  while (++first != last) {
    if (*first < *result) {
      result = first;
    }
  }
  return result;
}

template <class ForwardIterator, class Compare>
ForwardIterator my_min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
  if (first == last) {
    return last;
  }
  ForwardIterator result = first;
  while (++first != last) {
    if (comp(*first, *result)) {
      result = first;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * merge
 **************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator my_merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                        InputIterator2 last2, OutputIterator result) {
  while (first1 != last1 && first2 != last2) {
    if (*first2 < *first1) {
      *result = *first2;
      ++first2;
    } else {
      *result = *first1;
      ++first1;
    }
    ++result;
  }
  return my_copy(first1, last1, my_copy(first2, last2, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator my_merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                        InputIterator2 last2, OutputIterator result, Compare comp) {
  while (first1 != last1 && first2 != last2) {
    if (comp(*first2, *first1)) {
      *result = *first2;
      ++first2;
    } else {
      *result = *first1;
      ++first1;
    }
    ++result;
  }
  return my_copy(first1, last1, my_copy(first2, last2, result));
}
/*********************************************************/

/**************************************
 * function
 * partition
 **************************************/
template <class BidirectionalIteraotr, class Predicate>
BidirectionalIteraotr my_partition(BidirectionalIteraotr first, BidirectionalIteraotr last,
                                   Predicate pred) {
  while (true) {
    while (true) {
      if (first == last) {
        return first;
      } else if (pred(*first)) {
        ++first;
      } else {
        break;
      }
    }
    --last;
    while (true) {
      if (first == last) {
        return first;
      } else if (!pred(*last)) {
        --last;
      } else {
        break;
      }
    }
    my_iter_swap(first, last);
    ++first;
  }
}
/*********************************************************/

/**************************************
 * function
 * remove
 **************************************/
template <class ForwardIterator, class T>
ForwardIterator my_remove(ForwardIterator first, ForwardIterator last, const T& value) {
  first = my_find(first, last, value);
  ForwardIterator next = first;
  return first == last ? first : my_remove_copy(++next, last, first, value);
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator my_remove_copy(InputIterator first, InputIterator last, OutputIterator result,
                              const T& value) {
  for (; first != last; ++first) {
    if (*first != value) {
      *result = *first;
      ++result;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * remove_if
 **************************************/
template <class ForwardIterator, class Predicate>
ForwardIterator my_remove_if(ForwardIterator first, ForwardIterator last, Predicate pred) {
  first = my_find_if(first, last, pred);
  ForwardIterator next = first;
  return first == last ? first : my_remove_copy_if(++next, last, first, pred);
}

template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator my_remove_copy_if(InputIterator first, InputIterator last, OutputIterator result,
                                 Predicate pred) {
  for (; first != last; ++first) {
    if (!pred(*first)) {
      *result = *first;
      ++result;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * replace
 **************************************/
template <class ForwardIterator, class T>
void my_replace(ForwardIterator first, ForwardIterator last, const T& old_value,
                const T& new_value) {
  for (; first != last; ++first) {
    if (*first == old_value) {
      *first = new_value;
    }
  }
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator my_replace_copy(InputIterator first, InputIterator last, OutputIterator result,
                               const T& old_value, const T& new_value) {
  for (; first != last; ++first, ++result) {
    *result = *first == old_value ? new_value : *first;
  }
  return result;
}

template <class ForwardIterator, class Predicate, class T>
void my_replace_if(ForwardIterator first, ForwardIterator last, Predicate pred,
                   const T& new_value) {
  for (; first != last; ++first) {
    if (pred(*first)) {
      *first = new_value;
    }
  }
}

template <class Iterator, class OutputIterator, class Predicate, class T>
OutputIterator my_replace_copy_if(Iterator first, Iterator last, OutputIterator result,
                                  Predicate pred, const T& new_value) {
  for (; first != last; ++first, ++result) {
    if (pred(*first)) {
      *result = new_value;
    } else {
      *result = *first;
    }
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * reverse
 **************************************/
template <class BidirectionalIterator>
inline void my_reverse(BidirectionalIterator first, BidirectionalIterator last) {
  __my_reverse(first, last, iterator_category(first));
}

template <class BidirectionalIterator>
void __my_reverse(BidirectionalIterator first, BidirectionalIterator last,
                  my_bidirectional_iterator_tag) {
  while (true) {
    if (first == last || first == --last) {
      return;
    } else {
      my_iter_swap(first++, last);
    }
  }
}

template <class RandomAccessIterator>
void __my_reverse(RandomAccessIterator first, RandomAccessIterator last,
                  my_random_access_iterator_tag) {
  while (first < last) {
    my_iter_swap(first++, --last);
  }
}

template <class BidirectionalIterator, class OutputIterator>
OutputIterator my_reverse_copy(BidirectionalIterator first, BidirectionalIterator last,
                               OutputIterator result) {
  while (first != last) {
    --last;
    *result = *last;
    ++result;
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * rotate
 **************************************/
template <class ForwardIterator>
inline void my_rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
  if (first == middle || middle == last) {
    return;
  }
  __my_rotate(first, middle, last, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class Distance>
void __my_rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last, Distance*,
                 my_forward_iterator_tag) {
  for (ForwardIterator i = middle;;) {
    my_iter_swap(first, i);
    ++first;
    ++i;
    if (first == middle) {
      if (i == last) {
        return;
      }
      middle = i;
    } else if (i == last) {
      i = middle;
    }
  }
}

template <class BidirectionalIterator, class Distance>
void __my_rotate(BidirectionalIterator first, BidirectionalIterator middle,
                 BidirectionalIterator last, Distance*, my_bidirectional_iterator_tag) {
  my_reverse(first, middle);
  my_reverse(middle, last);
  my_reverse(first, last);
}

template <class RandomAccessIterator, class Distance>
void __my_rotate(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last,
                 Distance*, my_random_access_iterator_tag) {
  Distance n = __my_gcd(last - first, middle - first);
  while (n--) {
    __my_rotate_cycle(first, last, first + n, middle - first, value_type(first));
  }
}

template <class EuclideanRingElement>
EuclideanRingElement __my_gcd(EuclideanRingElement m, EuclideanRingElement n) {
  while (n != 0) {
    EuclideanRingElement t = m % n;
    m = n;
    n = t;
  }
  return m;
}

template <class RandomAccessIterator, class Distance, class T>
void __my_rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator initial, Distance shift, T*) {
  T value = *initial;
  RandomAccessIterator ptr1 = initial;
  RandomAccessIterator ptr2 = ptr1 + shift;
  while (ptr2 != initial) {
    *ptr1 = *ptr2;
    ptr1 = ptr2;
    if (last - ptr2 > shift) {
      ptr2 += shift;
    } else {
      ptr2 = first + (shift - (last - ptr2));
    }
  }
  *ptr1 = value;
}

template <class ForwardIterator, class OutputIterator>
OutputIterator my_rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
                              OutputIterator result) {
  return my_copy(first, middle, my_copy(middle, last, result));
}
/*********************************************************/

/**************************************
 * function
 * search_n
 **************************************/
template <class ForwardIterator, class Integer, class T>
ForwardIterator my_search_n(ForwardIterator first, ForwardIterator last, Integer count,
                            const T& value) {
  if (count < 0) {
    return first;
  } else {
    first = my_find(first, last, value);
    while (first != last) {
      Integer n = count - 1;
      ForwardIterator i = first;
      ++i;
      while (i != last && n != 0 && *i == value) {
        ++i;
        --n;
      }
      if (n == 0) {
        return first;
      } else {
        first = my_find(i, last, value);
      }
    }
    return last;
  }
}

template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator my_search_n(ForwardIterator first, ForwardIterator last, Integer count,
                            const T& value, BinaryPredicate binary_pred) {
  if (count < 0) {
    return first;
  } else {
    while (first != last) {
      if (binary_pred(*first, value)) {
        break;
      }
      ++first;
    }
    while (first != last) {
      Integer n = count - 1;
      ForwardIterator i = first;
      ++i;
      while (i != last && n != 0 && binary_pred(*i, value)) {
        ++i;
        --n;
      }
      if (n == 0) {
        return first;
      } else {
        while (i != last) {
          if (binary_pred(*i, value)) {
            break;
          }
          ++i;
        }
        first = i;
      }
    }
    return last;
  }
}
/*********************************************************/

/**************************************
 * function
 * swap_ranges
 **************************************/
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 my_swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1,
                                ForwardIterator2 first2, ForwardIterator2 last2) {
  for (; first1 != last1; ++first1, ++first2) {
    my_iter_swap(first1, first2);
  }
  return first2;
}
/*********************************************************/

/**************************************
 * function
 * transform
 **************************************/
template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator my_transform(InputIterator first, InputIterator last, OutputIterator result,
                            UnaryOperation op) {
  for (; first != last; ++first, ++result) {
    *result = op(*first);
  }
  return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator my_transform(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                            OutputIterator result, BinaryOperation binary_op) {
  for (; first1 != last1; ++first1, ++first2, ++result) {
    *result = binary_op(*first1, *first2);
  }
  return result;
}
/*********************************************************/

/**************************************
 * function
 * unique
 **************************************/
template <class ForwardIterator>
ForwardIterator my_unique(ForwardIterator first, ForwardIterator last) {
  first = my_adjacent_find(first, last);
  return my_unique_copy(first, last, first);
}

template <class InputIterator, class OutputIterator>
inline OutputIterator my_unique_copy(InputIterator first, InputIterator last,
                                     OutputIterator result) {
  if (first == last) {
    return result;
  }
  return __my_unique_copy(first, last, result, iterator_category(result));
}

template <class InputIterator, class ForwardIterator>
ForwardIterator __my_unique_copy(InputIterator first, InputIterator last, ForwardIterator result,
                                 my_forward_iterator_tag) {
  *result = *first;
  while (++first != last) {
    if (*result != *first) {
      *++result = *first;
    }
  }
  return ++result;
}

template <class InputIterator, class OutputIterator>
inline OutputIterator __my_unique_copy(InputIterator first, InputIterator last,
                                       OutputIterator result, my_output_iterator_tag) {
  return __my_unique_copy(first, last, result, value_type(first));
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator __my_unique_copy(InputIterator first, InputIterator last, OutputIterator result,
                                T*) {
  T value = *first;
  *result = value;
  while (++first != last) {
    if (value != *first) {
      value = *first;
      *++result = value;
    }
  }
  return result;
}

template <class ForwardIterator, class BinaryOperation>
ForwardIterator my_unique(ForwardIterator first, ForwardIterator last, BinaryOperation binary_op) {
  first = my_adjacent_find(first, last, binary_op);
  return my_unique_copy(first, last, first, binary_op);
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
inline OutputIterator my_unique_copy(InputIterator first, InputIterator last, OutputIterator result,
                                     BinaryOperation binary_op) {
  if (first == last) {
    return result;
  }
  return __my_unique_copy(first, last, result, iterator_category(result), binary_op);
}

template <class InputIterator, class ForwardIterator, class BinaryOperation>
ForwardIterator __my_unique_copy(InputIterator first, InputIterator last, ForwardIterator result,
                                 my_forward_iterator_tag, BinaryOperation binary_op) {
  *result = *first;
  while (++first != last) {
    if (!binary_op(*result, *first)) {
      *++result = *first;
    }
  }
  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
inline OutputIterator __my_unique_copy(InputIterator first, InputIterator last,
                                       OutputIterator result, my_output_iterator_tag,
                                       BinaryOperation binary_op) {
  return __my_unique_copy(first, last, result, value_type(first), binary_op);
}

template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator __my_unique_copy(InputIterator first, InputIterator last, OutputIterator result, T*,
                                BinaryOperation binary_op) {
  T value = *first;
  *result = value;
  while (++first != last) {
    if (!binary_op(value, *first)) {
      value = *first;
      *++result = value;
    }
  }
  return result;
}

/*********************************************************/

#endif
