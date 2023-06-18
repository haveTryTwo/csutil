// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_ITERATOR_H_
#define MY_STL_ITERATOR_H_

#include <cstdlib>

struct my_input_iterator_tag {};
struct my_output_iterator_tag {};
struct my_forward_iterator_tag : public my_input_iterator_tag {};
struct my_bidirectional_iterator_tag : public my_forward_iterator_tag {};
struct my_random_access_iterator_tag : public my_bidirectional_iterator_tag {};

template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*,
          class Reference = T&>
struct my_iterator {
  typedef Category iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef Pointer pointer;
  typedef Reference reference;
};

// to traits
template <class Iterator>
struct my_iterator_traits {
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::pointer pointer;
  typedef typename Iterator::reference reference;
};

// just according to the native pointer,设计的偏特化
template <class T>
struct my_iterator_traits<T*> {
  typedef my_random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef T& reference;
};

// just accordint to the pointer-to-const，设计的偏特化
template <class T>
struct my_iterator_traits<const T*> {
  typedef my_random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef const T* pointer;
  typedef const T& reference;
};

// this function is designed to get the category of iterator
template <class Iterator>
inline typename my_iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
  typedef typename my_iterator_traits<Iterator>::iterator_category category;
  return category();
}

// this function is designed to get the distance type of iterator
template <class Iterator>
inline typename my_iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
  return static_cast<typename my_iterator_traits<Iterator>::difference_type*>(0);
}

// this function is designed to get the value type
template <class Iterator>
inline typename my_iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
  return static_cast<typename my_iterator_traits<Iterator>::value_type*>(0);
}

/* the next three functions are designed for the distance*/
template <class InputIterator>
inline typename my_iterator_traits<InputIterator>::difference_type __my_distance(
    InputIterator first, InputIterator last, my_input_iterator_tag) {
  typename my_iterator_traits<InputIterator>::difference_type n = 0;
  while (first != last) {
    ++first;
    ++n;
  }
  return n;
}

template <class RandomAccessIterator>
inline typename my_iterator_traits<RandomAccessIterator>::difference_type __my_distance(
    RandomAccessIterator first, RandomAccessIterator last, my_random_access_iterator_tag) {
  return last - first;
}

template <class InputIterator>
inline typename my_iterator_traits<InputIterator>::difference_type my_distance(InputIterator first,
                                                                               InputIterator last) {
  typedef typename my_iterator_traits<InputIterator>::iterator_category category;
  return __my_distance(first, last, category());
}

/*the next four functions are designed for the advance */
template <class InputIterator, class Distance>
inline void __my_advance(InputIterator& i, Distance n, my_input_iterator_tag) {
  while (n--) {
    ++i;
  }
}

template <class BidirectionalIterator, class Distance>
inline void __my_advance(BidirectionalIterator& i, Distance n, my_bidirectional_iterator_tag) {
  if (n >= 0) {
    //
    while (n--) {
      ++i;
    }
  } else {
    //
    while (n++) {
      --i;
    }
  }
}

template <class RandomAccessIterator, class Distance>
inline void __my_advance(RandomAccessIterator& i, Distance n, my_random_access_iterator_tag) {
  i += n;
}

template <class InputIterator, class Distance>
inline void my_advance(InputIterator& i, Distance n) {
  __my_advance(i, n, iterator_category(i));
}

/**************************************************
 * back_insert_iterator
 ***************************************************/
template <class Container>
class my_back_insert_iterator : public my_iterator<my_output_iterator_tag, void, void, void, void> {
 protected:
  Container* container;

 public:
  explicit my_back_insert_iterator(Container& x) : container(&x) {}

  my_back_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
    container->push_back(value);
    return *this;
  }

  my_back_insert_iterator<Container>& operator*() { return *this; }

  my_back_insert_iterator<Container>& operator++() { return *this; }

  my_back_insert_iterator<Container> operator++(int) { return *this; }
};

// this is a function that help to use the back_insert_iterator easily
template <class Container>
inline my_back_insert_iterator<Container> my_back_inserter(Container& x) {
  return my_back_insert_iterator<Container>(x);
}
/******************************************************/

/**************************************************
 * front_insert_iterator
 ***************************************************/
template <class Container>
class my_front_insert_iterator
    : public my_iterator<my_output_iterator_tag, void, void, void, void> {
 protected:
  Container* container;

 public:
  explicit my_front_insert_iterator(Container& x) : container(&x) {}

  my_front_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
    container->push_front(value);
    return *this;
  }

  my_front_insert_iterator<Container>& operator*() { return *this; }

  my_front_insert_iterator<Container>& operator++() { return *this; }

  my_front_insert_iterator<Container> operator++(int) { return *this; }
};

// this is a function that help to use the front_insert_iterator easily
template <class Container>
inline my_front_insert_iterator<Container> my_front_inserter(Container& x) {
  return my_front_insert_iterator<Container>(x);
}
/************************************************************/

/**************************************************
 * insert_iterator
 ***************************************************/
template <class Container>
class my_insert_iterator : public my_iterator<my_output_iterator_tag, void, void, void, void> {
 protected:
  Container* container;
  typename Container::iterator iter;

 public:
  explicit my_insert_iterator(Container& x, typename Container::iterator it)
      : container(&x), iter(it) {}

  my_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
    iter = container->insert(iter, value);
    ++iter;
    return *this;
  }

  my_insert_iterator<Container>& operator*() { return *this; }

  my_insert_iterator<Container>& operator++() { return *this; }

  my_insert_iterator<Container> operator++(int) { return *this; }
};

// this is a function that help to use the front_insert_iterator easily
template <class Container, class Iterator>
inline my_insert_iterator<Container> my_inserter(Container& x, Iterator i) {
  typedef typename Container::iterator iter;
  return my_insert_iterator<Container>(x, iter(i));
}
/************************************************************/

/**************************************************
 * reverse_iterator
 ***************************************************/
template <class Iterator>
class my_reverse_iterator
    : public my_iterator<typename my_iterator_traits<Iterator>::iterator_category,
                         typename my_iterator_traits<Iterator>::value_type,
                         typename my_iterator_traits<Iterator>::difference_type,
                         typename my_iterator_traits<Iterator>::pointer,
                         typename my_iterator_traits<Iterator>::reference> {
 protected:
  Iterator current;  // this is the direct direction

 public:
  typedef Iterator iterator_type;              // direct iterator type
  typedef my_reverse_iterator<Iterator> self;  // redirect iterator type

  typedef typename my_iterator_traits<Iterator>::value_type value_type;
  typedef typename my_iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename my_iterator_traits<Iterator>::difference_type difference_type;
  typedef typename my_iterator_traits<Iterator>::pointer pointer;
  typedef typename my_iterator_traits<Iterator>::reference reference;

 public:
  my_reverse_iterator() {}

  explicit my_reverse_iterator(iterator_type x) : current(x) {}

  my_reverse_iterator(const self& x) : current(x.current) {}

  template <class Iter>
  my_reverse_iterator(const my_reverse_iterator<Iter>& x) : current(x.base()) {}

  iterator_type base() const { return current; }

  reference operator*() const {
    Iterator tmp = current;
    return *--tmp;  //
  }

  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    --current;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    --current;
    return tmp;
  }

  self& operator--() {
    ++current;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    ++current;
    return tmp;
  }

  self operator+(difference_type n) const { return self(current - n); }

  self& operator+=(difference_type n) const {
    current -= n;
    return *this;
  }

  self operator-(difference_type n) const { return self(current + n); }

  self& operator-=(difference_type n) const {
    current += n;
    return *this;
  }

  reference operator[](difference_type n) const { return *(*this + n); }
};

template <class Iterator>
typename my_reverse_iterator<Iterator>::difference_type operator-(
    const my_reverse_iterator<Iterator>& x, const my_reverse_iterator<Iterator>& y) {
  // here use y.base() substract x.base(), only in this way, the result can be greater then 0
  return y.base() - x.base();
}

template <class Iterator>
inline bool operator==(const my_reverse_iterator<Iterator>& x,
                       const my_reverse_iterator<Iterator>& y) {
  return x.base() == y.base();
}

template <class Iterator>
inline bool operator!=(const my_reverse_iterator<Iterator>& x,
                       const my_reverse_iterator<Iterator>& y) {
  return x.base() != y.base();
}

template <class Iterator>
inline bool operator<(const my_reverse_iterator<Iterator>& x,
                      const my_reverse_iterator<Iterator>& y) {
  return x.base() < y.base();
}

template <class Iterator>
inline bool operator>(const my_reverse_iterator<Iterator>& x,
                      const my_reverse_iterator<Iterator>& y) {
  return x.base() > y.base();
}

template <class Iterator>
inline bool operator<=(const my_reverse_iterator<Iterator>& x,
                       const my_reverse_iterator<Iterator>& y) {
  return x.base() <= y.base();
}

template <class Iterator>
inline bool operator>=(const my_reverse_iterator<Iterator>& x,
                       const my_reverse_iterator<Iterator>& y) {
  return x.base() >= y.base();
}

template <class IteratorL, class IteratorR>
inline bool operator==(const my_reverse_iterator<IteratorL>& x,
                       const my_reverse_iterator<IteratorR>& y) {
  return x.base() == y.base();
}

template <class IteratorL, class IteratorR>
inline bool operator!=(const my_reverse_iterator<IteratorL>& x,
                       const my_reverse_iterator<IteratorR>& y) {
  return x.base() != y.base();
}

/************************************************************/

#endif
