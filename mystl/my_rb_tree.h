// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_RB_TREE_H_
#define MY_RB_TREE_H_

#include "my_simple_alloc.h"
#include "my_stl_algobase.h"
#include "my_stl_construct.h"
#include "my_stl_function.h"
#include "my_stl_iterator.h"
#include "my_stl_pair.h"
#include "my_type_traits.h"
#include "my_uninitialized_func.h"

typedef bool __my_rb_tree_color_type;
const __my_rb_tree_color_type __my_rb_tree_red = false;   // red color
const __my_rb_tree_color_type __my_rb_tree_black = true;  // black color

class __my_rb_tree_node_base {
 public:
  typedef __my_rb_tree_color_type color_type;
  typedef __my_rb_tree_node_base* base_ptr;

  color_type color;  // the color of the node
  base_ptr parent;   // the parent node of the node
  base_ptr left;     // the left child of node
  base_ptr right;    // the right child of node

  static base_ptr minimum(base_ptr x) {
    while (x->left != 0) {
      x = x->left;
    }
    return x;
  }

  static base_ptr maximum(base_ptr x) {
    while (x->right != 0) {
      x = x->right;
    }
    return x;
  }
};

template <class Value>
class __my_rb_tree_node : public __my_rb_tree_node_base {
 public:
  typedef __my_rb_tree_node<Value>* link_type;
  Value value_field;  // the value of node
};

class __my_rb_tree_base_iterator {
 public:
  typedef __my_rb_tree_node_base ::base_ptr base_ptr;
  typedef my_bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t difference_type;

  base_ptr node;

  void increment() {
    if (node->right != 0) {
      // the right child, and the left
      node = node->right;
      while (node->left != 0) {
        node = node->left;
      }
    } else {
      base_ptr y = node->parent;
      while (node == y->right) {
        node = y;
        y = node->parent;
      }
      if (node->right != y) {
        node = y;
      }
    }
  }

  void decrement() {
    if (node->color == __my_rb_tree_red && node->parent->parent == node) {
      node = node->right;
    } else if (node->left != 0) {
      node = node->left;
      while (node->right != 0) {
        node = node->right;
      }
    } else {
      base_ptr y = node->parent;
      while (node == y->left) {
        node = y;
        y = node->parent;
      }
      node = y;
    }
  }
};

template <class Value, class Ref, class Ptr>
class __my_rb_tree_iterator : public __my_rb_tree_base_iterator {
 public:
  typedef Value value_type;
  typedef Ref reference;
  typedef Ptr pointer;

  typedef __my_rb_tree_iterator<Value, Value&, Value*> iterator;
  typedef __my_rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
  typedef __my_rb_tree_iterator<Value, Ref, Ptr> self;
  typedef __my_rb_tree_node<Value>* link_type;

  __my_rb_tree_iterator() {}

  explicit __my_rb_tree_iterator(link_type x) { node = x; }

  __my_rb_tree_iterator(const iterator& it) { node = it.node; }

  reference operator*() const { return link_type(node)->value_field; }

  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    increment();
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  self& operator--() {
    decrement();
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  bool operator==(const self& x) const { return node == x.node; }

  bool operator!=(const self& x) const { return node != x.node; }
};

inline void __my_rb_tree_rebalance(__my_rb_tree_node_base* x, __my_rb_tree_node_base*& root);
inline void __my_rb_tree_rotate_left(__my_rb_tree_node_base* x, __my_rb_tree_node_base*& root);
inline void __my_rb_tree_rotate_right(__my_rb_tree_node_base* x, __my_rb_tree_node_base*& root);

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = my_alloc>
class my_rb_tree {
 protected:
  typedef void* void_pointer;
  typedef __my_rb_tree_node_base* base_ptr;
  typedef __my_rb_tree_node<Value> rb_tree_node;
  typedef my_simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
  typedef __my_rb_tree_color_type color_type;

 public:
  typedef Key key_type;
  typedef Value value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef ptrdiff_t difference_type;
  typedef rb_tree_node* link_type;
  typedef size_t size_type;

 protected:
  link_type get_node() { return rb_tree_node_allocator ::allocate(); }

  void put_node(link_type x) { rb_tree_node_allocator ::deallocate(x); }

  link_type create_node(const value_type& x) {
    link_type tmp = get_node();
    try {
      my_construct(&tmp->value_field, x);
    } catch (...) {
      put_node(tmp);
      return 0;
    }
    return tmp;
  }

  link_type clone_node(link_type x) {
    link_type tmp = create_node(x->value_field);
    tmp->color = x->color;
    tmp->left = 0;
    tmp->right = 0;
    return tmp;
  }

  void destroy_node(link_type p) {
    my_destroy(&p->value_field);
    put_node(p);
  }

 protected:
  size_type node_count;  // the number of the tree
  link_type header;      //
  Compare key_compare;

  link_type& root() const { return (link_type&)header->parent; }

  link_type& leftmost() const { return (link_type&)header->left; }

  link_type& rightmost() const { return (link_type&)header->right; }

  // to get the number of link_type x easily
  static link_type& left(link_type x) { return (link_type&)(x->left); }

  static link_type& right(link_type x) { return (link_type&)(x->right); }

  static link_type& parent(link_type x) { return (link_type&)(x->parent); }

  static reference value(link_type x) { return x->value_field; }

  static const Key& key(link_type x) { return KeyOfValue()(value(x)); }

  static color_type& color(link_type x) { return (color_type&)x->color; }

  // the next six functions is getting the number of base_ptr x easily
  static link_type& left(base_ptr x) { return (link_type&)(x->left); }

  static link_type& right(base_ptr x) { return (link_type&)(x->right); }

  static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }

  static reference value(base_ptr x) { return ((link_type)x)->value_field; }

  static const Key& key(base_ptr x) { return KeyOfValue()(value(x)); }

  static color_type& color(base_ptr x) { return (color_type&)x->color; }

  // TO GET THE MAX AND MIX VALUE IN THE TREE
  static link_type minimum(link_type x) { return (link_type)__my_rb_tree_node_base ::minimum(x); }

  static link_type maximum(link_type x) { return (link_type)__my_rb_tree_node_base ::maximum(x); }

 public:
  typedef __my_rb_tree_iterator<value_type, reference, pointer> iterator;
  typedef __my_rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

 private:
  iterator __insert(base_ptr x, base_ptr y, const value_type& v);
  link_type __copy(link_type x, link_type p);

  void __erase(link_type x) {
    // without sort the rb_tree
    while (x != 0) {
      __erase((link_type)x->right);
      link_type y = (link_type)x->left;
      destroy_node(x);
      x = y;
    }
  }

  void init() {
    header = get_node();
    color(header) = __my_rb_tree_red;

    root() = 0;
    leftmost() = header;
    rightmost() = header;
  }

 public:
  my_rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) { init(); }

  ~my_rb_tree() {
    clear();
    put_node(header);
  }

  my_rb_tree(const my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
    init();
    key_compare = x.key_compare;
    if (x.root() != 0) {
      root() = __copy(x.root(), header);
      leftmost() = minimum(root());
      rightmost() = maximum(root());
      node_count = x.node_count;
    }
  }

  my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(
      const my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
    if (this == &x) {
      return *this;
    }
    clear();

    key_compare = x.key_compare;

    if (x.root() != 0) {
      root() = __copy(x.root(), header);
      leftmost() = minimum(root());
      rightmost() = maximum(root());
      node_count = x.node_count;
    }

    return *this;
  }

 public:
  Compare key_comp() const { return key_compare; }

  iterator begin() { return iterator(leftmost()); }

  const_iterator begin() const { return iterator(leftmost()); }

  iterator end() { return iterator(header); }

  const_iterator end() const { return iterator(header); }

  bool empty() { return node_count == 0; }

  size_type size() const { return node_count; }

  size_type max_size() const { return size_type(-1); }

  void swap(my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

 public:
  // insert x into my_rb_tree
  // and the value of x can't be already in the tree
  my_pair<iterator, bool> insert_unique(const value_type& x);

  // insert x into my_rb_tree
  // and the value of x can be already
  my_pair<iterator, bool> insert_equal(const value_type& x);

  template <class InputIterator>
  void insert_unique(InputIterator first, InputIterator last) {
    while (first != last) {
      insert_unique(*first);
      ++first;
    }
  }

  template <class InputIterator>
  void insert_equal(InputIterator first, InputIterator last) {
    while (first != last) {
      insert_equal(*first);
      ++first;
    }
  }

  void insert_equal(iterator position, const value_type& x) { insert_equal(x); }

  void clear() {
    __erase(root());
    root() = 0;
    leftmost() = header;
    rightmost() = header;
    node_count = 0;
  }

  void erase(iterator position);
  void erase(const_iterator position);
  void erase(iterator first, iterator last);
  void erase(const_iterator first, const_iterator last);
  size_type erase(const Key& k);

  // get the lower position that the number key equals k
  iterator lower_bound(link_type x, link_type y, const Key& k);
  const_iterator lower_bound(link_type x, link_type y, const Key& k) const;

  // get the upper postion that the number key equals k
  iterator upper_bound(link_type x, link_type y, const Key& k);
  const_iterator upper_bound(link_type x, link_type y, const Key& k) const;

  // find the lower position that the key equal k
  iterator find(const Key& k);
  const_iterator find(const Key& k) const;

  // count the number that how many keys equal k
  size_type count(const Key& k);
  size_type count(const Key& k) const;

  // get the range that the keys equal k in
  my_pair<iterator, iterator> equal_range(const Key& k);
  my_pair<const_iterator, const_iterator> equal_range(const Key& k) const;
};

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::swap(
    my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
  // swap the Compare
  Compare comp_tmp = key_compare;
  key_compare = x.key_compare;
  x.key_compare = comp_tmp;

  my_swap(node_count, x.node_count);

  if (root() == 0 && x.root() == 0) {
    return;
  } else if (root() == 0) {
    root() = x.root();
    x.root()->parent = header;
    leftmost() = minimum(root());
    rightmost() = maximum(root());

    x.root() = 0;
    x.leftmost() = x.header;
    x.rightmost() = x.header;
    return;
  } else if (x.root() == 0) {
    x.root() = root();
    root()->parent = x.header;
    x.leftmost() = x.minimum(x.root());
    x.rightmost() = x.maximum(x.root());

    root() = 0;
    leftmost() = header;
    rightmost() = header;
    return;
  } else {
    link_type tmp = x.root();

    x.root() = root();
    root()->parent = x.header;
    x.leftmost() = x.minimum(x.root());
    x.rightmost() = x.maximum(x.root());

    root() = tmp;
    tmp->parent = header;
    leftmost() = minimum(root());
    rightmost() = maximum(root());

    return;
  }
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(link_type x, link_type y,
                                                                const Key& k) {
  while (x != 0) {
    if (!key_compare(key(x), k)) {
      y = x;
      x = left(x);
    } else {
      x = right(x);
    }
  }
  return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(link_type x, link_type y,
                                                                const Key& k) const {
  while (x != 0) {
    if (!key_compare(key(x), k)) {
      y = x;
      x = left(x);
    } else {
      x = right(x);
    }
  }
  return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(link_type x, link_type y,
                                                                const Key& k) {
  while (x != 0) {
    if (key_compare(k, key(x))) {
      y = x;
      x = left(x);
    } else {
      x = right(x);
    }
  }
  return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(link_type x, link_type y,
                                                                const Key& k) const {
  while (x != 0) {
    if (key_compare(k, key(x))) {
      y = x;
      x = left(x);
    } else {
      x = right(x);
    }
  }
  return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
my_pair<typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
        typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) {
  link_type y = header;
  link_type x = root();

  while (x != 0) {
    if (key_compare(k, key(x))) {
      // k < key(x)
      y = x;
      x = left(x);
    } else if (key_compare(key(x), k)) {
      // key(x) < k
      x = right(x);
    } else {
      // key(x) == k
      link_type _x = x;
      link_type _y = y;
      y = x;
      x = left(x);
      _x = right(_x);

      return my_pair<iterator, iterator>(lower_bound(x, y, k), upper_bound(_x, _y, k));
    }
  }
  return my_pair<iterator, iterator>(iterator(y), iterator(y));
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
my_pair<typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator,
        typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator>
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) const {
  link_type y = header;
  link_type x = root();

  while (x != 0) {
    if (key_compare(k, key(x))) {
      // k < key(x)
      y = x;
      x = left(x);
    } else if (key_compare(key(x), k)) {
      // key(x) < k
      x = right(x);
    } else {
      // key(x) == k
      link_type _x = x;
      link_type _y = y;
      y = x;
      x = left(x);
      _x = right(_x);

      return my_pair<const_iterator, const_iterator>(lower_bound(x, y, k), upper_bound(_x, _y, k));
    }
  }
  return my_pair<const_iterator, const_iterator>(const_iterator(y), const_iterator(y));
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) {
  my_pair<iterator, iterator> p = equal_range(k);
  const size_type n = my_distance(p.first, p.second);
  return n;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
  my_pair<const_iterator, const_iterator> p = equal_range(k);
  const size_type n = my_distance(p.first, p.second);
  return n;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
  link_type y = header;
  link_type x = root();

  const_iterator j = lower_bound(x, y, k);
  return (j == end() || key_compare(k, key(j.node)) ? end() : j);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
  link_type y = header;
  link_type x = root();

  iterator j = lower_bound(x, y, k);
  return (j == end() || key_compare(k, key(j.node)) ? end() : j);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
my_pair<typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v) {
  link_type y = header;
  link_type x = root();
  while (x != 0) {
    y = x;
    x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
  }

  return my_pair<iterator, bool>(__insert(x, y, v), true);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
my_pair<typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v) {
  link_type y = header;
  link_type x = root();

  bool comp = true;

  while (x != 0) {
    y = x;
    comp = key_compare(KeyOfValue()(v), key(x));
    x = comp ? left(x) : right(x);
  }

  iterator j = iterator(y);
  if (comp) {
    if (j == begin()) {
      return my_pair<iterator, bool>(__insert(x, y, v), true);
    } else {
      --j;
    }
  }
  if (key_compare(key(j.node), KeyOfValue()(v))) {
    return my_pair<iterator, bool>(__insert(x, y, v), true);
  }

  return my_pair<iterator, bool>(j, false);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr _x, base_ptr _y,
                                                             const Value& v) {
  link_type x = (link_type)_x;
  link_type y = (link_type)_y;
  link_type z;

  if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
    z = create_node(v);
    left(y) = z;
    if (y == header) {
      root() = z;
      rightmost() = z;
    } else if (y == leftmost()) {
      leftmost() = z;
    }
  } else {
    z = create_node(v);
    right(y) = z;
    if (y == rightmost()) {
      rightmost() = z;
    }
  }

  parent(z) = y;
  left(z) = 0;
  right(z) = 0;

  __my_rb_tree_insert_rebalance(z, header->parent);
  ++node_count;
  return iterator(z);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
  link_type top = clone_node(x);

  top->parent = p;

  try {
    if (right(x) != 0) {
      top->right = __copy(right(x), top);
    }
    p = top;
    x = left(x);
    while (x != 0) {
      link_type y = clone_node(x);
      p->left = y;
      y->parent = p;
      if (right(x) != 0) {
        y->right = __copy(right(x), y);
      }
      p = y;
      x = left(x);
    }
  } catch (...) {
    __erase(top);
    throw;
  }

  return top;
}

///// need to think more carfully
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last) {
  if (first == begin() && last == end()) {
    clear();
  } else {
    iterator tmp = first;
    ++tmp;
    while (first != last) {
      if (tmp != last) {
        tmp = first;
        ++tmp;
      }
      if (tmp == last) {
        erase(first);
        return;
      }
      erase(first);
    }
  }
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const_iterator first,
                                                               const_iterator last) {
  if (first == begin() && last == end()) {
    clear();
  } else {
    iterator tmp = first;
    ++tmp;
    while (first != last) {
      if (tmp != last) {
        tmp = first;
        ++tmp;
      }
      if (tmp == last) {
        erase(first);
        return;
      }
      erase(first);
    }
  }
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& k) {
  my_pair<iterator, iterator> p = equal_range(k);
  size_type old_size = size();
  erase(p.first, p.second);

  return old_size - size();
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
  iterator tmp = position;
  link_type y;
  link_type x;
  if (position.node->left == 0 || position.node->right == 0) {
    // if the right or the left of the position is null
    y = (link_type)position.node;
  } else {
    ++tmp;
    y = (link_type)tmp.node;
  }

  if (y->left != 0) {
    x = (link_type)y->left;
  } else {
    x = (link_type)y->right;
  }

  if (x == 0 && y == root()) {
    // if the node is the last node, then delete the node, and return;
    root() = 0;
    leftmost() = header;
    rightmost() = header;
    destroy_node(y);
    --node_count;
    return;
  }
  if (x == 0) {
    if (y == leftmost()) {
      leftmost() = (link_type)y->parent;
    } else if (y == rightmost()) {
      rightmost() = (link_type)y->parent;
    }
  } else if (x != 0) {
    x->parent = y->parent;
  }
  if (y == root()) {
    // if y is the root, then delete y, and put x color to black
    root() = x;
    leftmost() = x;
    rightmost() = x;
    x->color = __my_rb_tree_black;
    destroy_node(y);
    --node_count;
    return;
  } else if (y == y->parent->left) {
    y->parent->left = x;
    if (x != 0 && x->parent == root()) {
      // to make the left most
      leftmost() = x;
    }
  } else {
    y->parent->right = x;
    if (x != 0 && x->parent == root()) {
      // to make the right most
      rightmost() = x;
    }
  }

  if (y != position.node) {
    my_construct(&(link_type(position.node))->value_field, y->value_field);
  }

  if (y->color == __my_rb_tree_black) {
    base_ptr p = y->parent;
    __my_rb_tree_erase_reblance(x, p, header->parent);
  }

  destroy_node(y);
  --node_count;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void my_rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const_iterator position) {
  cout << *position << "AA" << endl;
  const_iterator tmp = position;
  link_type y;
  link_type x;
  if (position.node->left == 0 || position.node->right == 0) {
    // if the right or the left of the position is null
    y = (link_type)position.node;
  } else {
    ++tmp;
    y = (link_type)tmp.node;
  }

  if (y->left != 0) {
    x = (link_type)y->left;
  } else {
    x = (link_type)y->right;
  }

  if (x == 0 && y == root()) {
    // if the node is the last node, then delete the node, and return;
    root() = 0;
    leftmost() = header;
    rightmost() = header;
    destroy_node(y);
    --node_count;
    return;
  }
  if (x == 0) {
    if (y == leftmost()) {
      leftmost() = (link_type)y->parent;
    } else if (y == rightmost()) {
      rightmost() = (link_type)y->parent;
    }
  } else if (x != 0) {
    x->parent = y->parent;
  }
  if (y == root()) {
    // if y is the root, then delete y, and put x color to black
    root() = x;
    leftmost() = x;
    rightmost() = x;
    x->color = __my_rb_tree_black;
    destroy_node(y);
    --node_count;
    return;
  } else if (y == y->parent->left) {
    y->parent->left = x;
    if (x != 0 && x->parent == root()) {
      // to make the left most
      leftmost() = x;
    }
  } else {
    y->parent->right = x;
    if (x != 0 && x->parent == root()) {
      // to make the right most
      rightmost() = x;
    }
  }

  if (y != position.node) {
    my_construct(&(link_type(position.node))->value_field, y->value_field);
  }

  if (y->color == __my_rb_tree_black) {
    base_ptr p = y->parent;
    __my_rb_tree_erase_reblance(x, p, header->parent);
  }

  destroy_node(y);
  --node_count;
}

inline void __my_rb_tree_erase_reblance(__my_rb_tree_node_base* x, __my_rb_tree_node_base* p,
                                        __my_rb_tree_node_base*& root) {
  __my_rb_tree_node_base* w;

  while (x == 0) {
    // if x == 0, then deal with this specially
    if (p->left == x) {
      w = p->right;
      if (w->color == __my_rb_tree_red) {
        w->color = __my_rb_tree_black;
        p->color = __my_rb_tree_red;
        __my_rb_tree_rotate_left(p, root);
        w = p->right;
      }
      if (w->color == __my_rb_tree_black && w->left == 0 && w->right == 0) {
        w->color = __my_rb_tree_red;
        x = p;
      } else {
        if (w->left != 0) {
          w->left->color = __my_rb_tree_black;
          w->color = __my_rb_tree_red;
          __my_rb_tree_rotate_right(w, root);
          w = p->right;
        }
        if (w->right != 0) {
          w->color = p->color;
          p->color = __my_rb_tree_black;
          w->right->color = __my_rb_tree_black;
          __my_rb_tree_rotate_left(p, root);
          return;
        }
      }
    } else if (p->right == x) {
      w = p->left;
      if (w->color == __my_rb_tree_red) {
        w->color = __my_rb_tree_black;
        p->color = __my_rb_tree_red;
        __my_rb_tree_rotate_right(p, root);
        w = p->left;
      }
      if (w->color == __my_rb_tree_black && w->left == 0 && w->right == 0) {
        w->color = __my_rb_tree_red;
        x = p;
      } else {
        if (w->right != 0) {
          w->right->color = __my_rb_tree_black;
          w->color = __my_rb_tree_red;
          __my_rb_tree_rotate_left(w, root);
          w = p->left;
        }
        if (w->left != 0) {
          w->color = p->color;
          p->color = __my_rb_tree_black;
          w->left->color = __my_rb_tree_black;
          __my_rb_tree_rotate_right(p, root);
          return;
        }
      }
    }
  }

  while (x != root && x->color == __my_rb_tree_black) {
    if (x == x->parent->left) {
      w = x->parent->right;
      if (w->color == __my_rb_tree_red) {
        // if the color of the w is red
        w->color = __my_rb_tree_black;
        x->parent->color = __my_rb_tree_red;
        __my_rb_tree_rotate_left(x->parent, root);
        w = x->parent->right;
      }
      if (w->left->color == __my_rb_tree_black && w->right->color == __my_rb_tree_black) {
        w->color = __my_rb_tree_red;
        x = x->parent;
      } else {
        if (w->right->color == __my_rb_tree_black) {
          w->left->color = __my_rb_tree_black;
          w->color = __my_rb_tree_red;
          __my_rb_tree_rotate_right(w, root);
          w = x->parent->right;
        }
        if (w->right->color == __my_rb_tree_red) {
          w->color = x->parent->color;
          x->parent->color = __my_rb_tree_black;
          w->right->color = __my_rb_tree_black;
          __my_rb_tree_rotate_left(x->parent, root);
          x = root;
        }
      }
    } else {
      w = x->parent->left;
      if (w->color == __my_rb_tree_red) {
        // if the color of the w is red
        w->color = __my_rb_tree_black;
        x->parent->color = __my_rb_tree_red;
        __my_rb_tree_rotate_right(x->parent, root);
        w = x->parent->left;
      }
      if (w->left->color == __my_rb_tree_black && w->right->color == __my_rb_tree_black) {
        w->color = __my_rb_tree_red;
        x = x->parent;
      } else {
        if (w->left->color == __my_rb_tree_black) {
          w->right->color = __my_rb_tree_black;
          w->color = __my_rb_tree_red;
          __my_rb_tree_rotate_left(w, root);
          w = x->parent->left;
        }
        if (w->left->color == __my_rb_tree_red) {
          w->color = x->parent->color;
          x->parent->color = __my_rb_tree_black;
          w->left->color = __my_rb_tree_black;
          __my_rb_tree_rotate_right(x->parent, root);
          x = root;
        }
      }
    }
  }
  x->color = __my_rb_tree_black;
}

inline void __my_rb_tree_insert_rebalance(__my_rb_tree_node_base* x,
                                          __my_rb_tree_node_base*& root) {
  x->color = __my_rb_tree_red;
  while (x != root && x->parent->color == __my_rb_tree_red) {
    if (x->parent == x->parent->parent->left) {
      // left
      __my_rb_tree_node_base* y = x->parent->parent->right;
      if (y && y->color == __my_rb_tree_red) {
        x->parent->color = __my_rb_tree_black;
        y->color = __my_rb_tree_black;
        x->parent->parent->color = __my_rb_tree_red;
        x = x->parent->parent;
      } else {
        if (x == x->parent->right) {
          // x is the right
          x = x->parent;
          __my_rb_tree_rotate_left(x, root);
        }
        x->parent->color = __my_rb_tree_black;
        x->parent->parent->color = __my_rb_tree_red;
        __my_rb_tree_rotate_right(x->parent->parent, root);
      }
    } else {
      // x->parent is the right of x->parent->parent
      __my_rb_tree_node_base* y = x->parent->parent->left;
      if (y && y->color == __my_rb_tree_red) {
        x->parent->color = __my_rb_tree_black;
        x->parent->parent->color = __my_rb_tree_red;
        y->color = __my_rb_tree_black;
        x = x->parent->parent;
      } else {
        if (x == x->parent->left) {
          // x is the left of its parent
          x = x->parent;
          __my_rb_tree_rotate_right(x, root);
        }
        x->parent->color = __my_rb_tree_black;
        x->parent->parent->color = __my_rb_tree_red;
        __my_rb_tree_rotate_left(x->parent->parent, root);
      }
    }
  }
  root->color = __my_rb_tree_black;
}

inline void __my_rb_tree_rotate_left(__my_rb_tree_node_base* x, __my_rb_tree_node_base*& root) {
  __my_rb_tree_node_base* y = x->right;
  x->right = y->left;
  if (y->left != 0) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x == root) {
    root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

inline void __my_rb_tree_rotate_right(__my_rb_tree_node_base* x, __my_rb_tree_node_base*& root) {
  __my_rb_tree_node_base* y = x->left;
  x->left = y->right;
  if (y->right != 0) {
    y->right->parent = x;
  }

  y->parent = x->parent;
  if (x == root) {
    root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->right = x;
  x->parent = y;
}

#endif
