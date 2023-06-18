// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_FUNCTION_H_
#define MY_STL_FUNCTION_H_

/***
 * sigle parameter, and result
 ***/
template <class Arg, class Result>
class my_unary_function {
 public:
  typedef Arg argument_type;
  typedef Result result_type;
};

/***
 * double parameters, and result
 ***/
template <class Arg1, class Arg2, class Result>
class my_binary_function {
 public:
  typedef Arg1 first_argument_type;
  typedef Arg2 second_argument_type;
  typedef Result result_type;
};

/***
 * identity function : the function won't change anything
 ***/
template <class T>
class my_identity : public my_unary_function<T, T> {
 public:
  const T& operator()(const T& x) const { return x; }
};

/****
 *select the first node of the pair
 ***/
template <class Pair>
class my_select1st : public my_unary_function<Pair, typename Pair ::first_type> {
 public:
  const typename Pair ::first_type& operator()(const Pair& x) const { return x.first; }
};

/***
 *select the second node of the pair
 ***/
template <class Pair>
class my_select2nd : public my_unary_function<Pair, typename Pair ::second_type> {
 public:
  const typename Pair ::second_type& operator()(const Pair& x) const { return x.second; }
};

/***
 * ignore the scond parameter, return the first parameter
 ****/
template <class Arg1, class Arg2>
class my_project1st : public my_binary_function<Arg1, Arg2, Arg1> {
 public:
  Arg1 operator()(const Arg1& x, const Arg2& y) const { return x; }
};

/***
 * ignore the first parameter, return the second parameter
 ***/
template <class Arg1, class Arg2>
class my_project2nd : public my_binary_function<Arg1, Arg2, Arg2> {
 public:
  Arg2 operator()(const Arg1& x, const Arg2& y) const { return y; }
};

/************************************************************
 *  Arithmetic functors
 *************************************************************/
// plus
template <class T>
class my_plus : public my_binary_function<T, T, T> {
 public:
  T operator()(const T& x, const T& y) const { return x + y; }
};

// minus
template <class T>
class my_minus : public my_binary_function<T, T, T> {
 public:
  T operator()(const T& x, const T& y) const { return x - y; }
};

// multiplies
template <class T>
class my_multiplies : public my_binary_function<T, T, T> {
 public:
  T operator()(const T& x, const T& y) const { return x * y; }
};

// divides
template <class T>
class my_divides : public my_binary_function<T, T, T> {
 public:
  T operator()(const T& x, const T& y) const { return x / y; }
};

// modulus
template <class T>
class my_modules : public my_binary_function<T, T, T> {
 public:
  T operator()(const T& x, const T& y) const { return x % y; }
};

// negation
template <class T>
class my_negate : public my_unary_function<T, T> {
 public:
  T operator()(const T& x) const { return -x; }
};

/************************************************************/

/************************************************************
 *Relational funtors
 ************************************************************/

/****
 * equal : check that whether the first parameter is equal to the second parameter
 ****/
template <class T>
class my_equal_to : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x == y; }
};

// not equal
template <class T>
class my_not_equal_to : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x != y; }
};

// greater
template <class T>
class my_greater : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x > y; }
};

// greater than or equal
template <class T>
class my_greater_equal : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x >= y; }
};

/***
 * check that whether the first parameter is less than the second parameter
 ***/
template <class T>
class my_less : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x < y; }
};

// less than or equal
template <class T>
class my_less_equal : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x <= y; }
};

/************************************************************/

/************************************************************
 * Logical funtors
 *************************************************************/

// and
template <class T>
class my_logical_and : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x && y; }
};

// or
template <class T>
class my_logical_or : public my_binary_function<T, T, bool> {
 public:
  bool operator()(const T& x, const T& y) const { return x || y; }
};

// not
template <class T>
class my_logical_not : public my_unary_function<T, bool> {
 public:
  bool operator()(const T& x) const { return !x; }
};

/************************************************************/

/************************************************************
 * identity element
 *************************************************************/
template <class T>
inline T my_identity_element(my_plus<T>) {
  return T(0);
}

template <class T>
inline T my_identity_element(my_multiplies<T>) {
  return T(1);
}

/************************************************************/

/************************************************************
 * unary_negate
 *************************************************************/
template <class Predicate>
class my_unary_negate : public my_unary_function<typename Predicate::argument_type, bool> {
 protected:
  Predicate pred;  // member

 public:
  explicit my_unary_negate(const Predicate& x) : pred(x) {}

  bool operator()(const typename Predicate::argument_type& value) const { return !pred(value); }
};

template <class Predicate>
inline my_unary_negate<Predicate> my_not1(const Predicate& pred) {
  return my_unary_negate<Predicate>(pred);
}
/************************************************************/

/************************************************************
 * binary_negate
 *************************************************************/
template <class Predicate>
class my_binary_negate : public my_binary_function<typename Predicate::first_argument_type,
                                                   typename Predicate::second_argument_type, bool> {
 protected:
  Predicate pred;

 public:
  explicit my_binary_negate(const Predicate& x) : pred(x) {}

  bool operator()(const typename Predicate::first_argument_type& x,
                  const typename Predicate::second_argument_type& y) const {
    return !pred(x, y);
  }
};

template <class Predicate>
inline my_binary_negate<Predicate> my_not2(const Predicate& pred) {
  return my_binary_negate<Predicate>(pred);
}
/************************************************************/

/************************************************************
 * binder1st
 *************************************************************/
template <class Operation>
class my_binder1st : public my_unary_function<typename Operation::second_argument_type,
                                              typename Operation::result_type> {
 protected:
  Operation op;
  typename Operation::first_argument_type value;

 public:
  my_binder1st(const Operation& x, const typename Operation::first_argument_type& y)
      : op(x), value(y) {}

  typename Operation::result_type operator()(
      const typename Operation::second_argument_type& x) const {
    return op(value, x);
  }
};

template <class Operation, class T>
inline my_binder1st<Operation> my_bind1st(const Operation& op, const T& x) {
  typedef typename Operation::first_argument_type arg1_type;
  return my_binder1st<Operation>(op, arg1_type(x));
}
/************************************************************/

/************************************************************
 * binder2nd
 *************************************************************/
template <class Operation>
class my_binder2nd : public my_unary_function<typename Operation::first_argument_type,
                                              typename Operation::result_type> {
 protected:
  Operation op;
  typename Operation::second_argument_type value;

 public:
  my_binder2nd(const Operation& x, const typename Operation::second_argument_type& y)
      : op(x), value(y) {}

  typename Operation::result_type operator()(const typename Operation::first_argument_type& x) {
    return op(x, value);
  }
};

template <class Operation, class T>
inline my_binder2nd<Operation> my_bind2nd(const Operation& op, const T& x) {
  typedef typename Operation::second_argument_type argu2_type;
  return my_binder2nd<Operation>(op, argu2_type(x));
}
/************************************************************/

/************************************************************
 * unary_compose
 *************************************************************/
template <class Operation1, class Operation2>
class my_unary_compose : public my_unary_function<typename Operation2::argument_type,
                                                  typename Operation1::result_type> {
 protected:
  Operation1 op1;
  Operation2 op2;

 public:
  my_unary_compose(const Operation1& x, const Operation2& y) : op1(x), op2(y) {}

  typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const {
    return op1(op2(x));
  }
};

template <class Operation1, class Operation2>
inline my_unary_compose<Operation1, Operation2> my_compose1(const Operation1& op1,
                                                            const Operation2& op2) {
  return my_unary_compose<Operation1, Operation2>(op1, op2);
}
/************************************************************/

/************************************************************
 * binary_compose
 *************************************************************/
template <class Operation1, class Operation2, class Operation3>
class my_binary_compose : public my_unary_function<typename Operation2::argument_type,
                                                   typename Operation1::result_type> {
 protected:
  Operation1 op1;
  Operation2 op2;
  Operation3 op3;

 public:
  my_binary_compose(const Operation1& x, const Operation2& y, const Operation3& z)
      : op1(x), op2(y), op3(z) {}

  typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const {
    return op1(op2(x), op3(x));
  }
};

template <class Operation1, class Operation2, class Operation3>
inline my_binary_compose<Operation1, Operation2, Operation3> my_compose2(const Operation1& op1,
                                                                         const Operation2& op2,
                                                                         const Operation3& op3) {
  return my_binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}
/************************************************************/

/************************************************************
 * pointer_to_unary_function
 *************************************************************/
template <class Arg, class Result>
class my_pointer_to_unary_function : public my_unary_function<Arg, Result> {
 protected:
  Result (*ptr)(Arg);

 public:
  my_pointer_to_unary_function() {}

  explicit my_pointer_to_unary_function(Result (*x)(Arg)) : ptr(x) {}

  Result operator()(Arg x) const { return ptr(x); }
};

template <class Arg, class Result>
inline my_pointer_to_unary_function<Arg, Result> my_ptr_fun(Result (*x)(Arg)) {
  return my_pointer_to_unary_function<Arg, Result>(x);
}
/************************************************************/

/************************************************************
 * pointer_to_binary_function
 *************************************************************/
template <class Arg1, class Arg2, class Result>
class my_pointer_to_binary_function : public my_binary_function<Arg1, Arg2, Result> {
 protected:
  Result (*ptr)(Arg1, Arg2);

 public:
  my_pointer_to_binary_function() {}

  explicit my_pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) {}

  Result operator()(Arg1 x, Arg2 y) const { return ptr(x, y); }
};

template <class Arg1, class Arg2, class Result>
inline my_pointer_to_binary_function<Arg1, Arg2, Result> my_ptr_fun(Result (*x)(Arg1, Arg2)) {
  return my_pointer_to_binary_function<Arg1, Arg2, Result>(x);
}
/************************************************************/

/************************************************************
 * mem_fun_t
 *************************************************************/
template <class S, class T>
class my_mem_fun_t : public my_unary_function<T*, S> {
 private:
  S (T::*f)();

 public:
  explicit my_mem_fun_t(S (T::*pf)()) : f(pf) {}

  S operator()(T* p) { return (p->*f)(); }
};

template <class S, class T>
inline my_mem_fun_t<S, T> my_mem_fun(S (T::*f)()) {
  return my_mem_fun_t<S, T>(f);
}
/************************************************************/

/************************************************************
 * const_mem_fun_t
 *************************************************************/
template <class S, class T>
class my_const_mem_fun_t : public my_unary_function<T*, S> {
 private:
  S (T::*f)() const;

 public:
  explicit my_const_mem_fun_t(S (T::*pf)() const) : f(pf) {}

  S operator()(const T* p) const { return (p->*f)(); }
};

template <class S, class T>
inline my_const_mem_fun_t<S, T> my_const_mem_fun(S (T::*f)() const) {
  return my_const_mem_fun_t<S, T>(f);
}
/************************************************************/

/************************************************************
 * mem_fun_ref_t
 *************************************************************/
template <class S, class T>
class my_mem_fun_ref_t : public my_unary_function<T, S> {
 private:
  S (T::*f)();

 public:
  explicit my_mem_fun_ref_t(S (T::*pf)()) : f(pf) {}

  S operator()(T& r) { return (r.*f)(); }
};

template <class S, class T>
inline my_mem_fun_ref_t<S, T> my_mem_fun_ref(S (T::*f)()) {
  return my_mem_fun_ref_t<S, T>(f);
}
/************************************************************/

/************************************************************
 * const_mem_fun_ref_t
 *************************************************************/
template <class S, class T>
class my_const_mem_fun_ref_t : public my_unary_function<T, S> {
 private:
  S (T::*f)() const;

 public:
  explicit my_const_mem_fun_ref_t(S (T::*pf)() const) : f(pf) {}

  S operator()(const T& r) const { return (r.*f)(); }
};

template <class S, class T>
inline my_const_mem_fun_ref_t<S, T> my_const_mem_fun_ref(S (T::*f)() const) {
  return my_const_mem_fun_ref_t<S, T>(f);
}
/************************************************************/

/************************************************************
 * mem_fun1_t
 *************************************************************/
template <class S, class T, class A>
class my_mem_fun1_t : public my_binary_function<T*, A, S> {
 private:
  S (T::*f)(A);

 public:
  explicit my_mem_fun1_t(S (T::*pf)(A)) : f(pf) {}

  S operator()(T* p, A x) { return (p->*f)(x); }
};

template <class S, class T, class A>
inline my_mem_fun1_t<S, T, A> my_mem_fun(S (T::*f)(A)) {
  return my_mem_fun1_t<S, T, A>(f);
}
/************************************************************/

/************************************************************
 * const_mem_fun1_t
 *************************************************************/
template <class S, class T, class A>
class my_const_mem_fun1_t : public my_binary_function<T*, A, S> {
 private:
  S (T::*f)(A) const;

 public:
  explicit my_const_mem_fun1_t(S (T::*pf)(A) const) : f(pf) {}

  S operator()(const T* p, A x) const { return (p->*f)(x); }
};

template <class S, class T, class A>
inline my_const_mem_fun1_t<S, T, A> my_const_mem_fun(S (T::*f)(A) const) {
  return my_const_mem_fun1_t<S, T, A>(f);
}
/************************************************************/

/************************************************************
 * mem_fun1_ref_t
 *************************************************************/
template <class S, class T, class A>
class my_mem_fun1_ref_t : public my_binary_function<T, A, S> {
 private:
  S (T::*f)(A);

 public:
  explicit my_mem_fun1_ref_t(S (T::*pf)(A)) : f(pf) {}

  S operator()(T& r, A x) { return (r.*f)(x); }
};

template <class S, class T, class A>
inline my_mem_fun1_ref_t<S, T, A> my_mem_fun_ref(S (T::*f)(A)) {
  return my_mem_fun1_ref_t<S, T, A>(f);
}

/************************************************************/

/************************************************************
 * const_mem_fun1_ref_t
 *************************************************************/
template <class S, class T, class A>
class my_const_mem_fun1_ref_t : public my_binary_function<T, A, S> {
 private:
  S (T::*f)(A) const;

 public:
  explicit my_const_mem_fun1_ref_t(S (T::*pf)(A) const) : f(pf) {}

  S operator()(const T& r, A x) const { return (r.*f)(x); }
};

template <class S, class T, class A>
inline my_const_mem_fun1_ref_t<S, T, A> my_const_mem_fun_ref(S (T::*f)(A) const) {
  return my_const_mem_fun1_ref_t<S, T, A>(f);
}

/************************************************************/

#endif
