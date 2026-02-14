// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NO_DESTRUCTOR_H_
#define BASE_NO_DESTRUCTOR_H_

#include <new>
#include <type_traits>
#include <utility>

namespace base {

// NoDestructor<T> is a wrapper that constructs an object of type T in-place
// but intentionally never calls its destructor. This is useful for creating
// function-local static objects that persist for the lifetime of the program
// without suffering from the "static destruction order fiasco".
//
// The static destruction order fiasco occurs when a static object's destructor
// depends on another static object that has already been destroyed. By using
// NoDestructor, the wrapped object is never destroyed, thus avoiding this
// problem entirely.
//
// Usage example:
//   const std::string& GetGlobalString() {
//     static base::NoDestructor<std::string> instance("hello world");
//     return *instance;
//   }
//
//   const std::map<int, std::string>& GetGlobalMap() {
//     static base::NoDestructor<std::map<int, std::string>> instance({
//       {1, "one"}, {2, "two"}, {3, "three"}
//     });
//     return *instance;
//   }
//
// Key properties:
//   - The wrapped object is constructed exactly once (thread-safe via C++11
//     function-local static initialization guarantees)
//   - The destructor of the wrapped object is never called
//   - Provides pointer and reference access to the wrapped object via
//     operator*, operator->, and Get()
//   - NoDestructor itself is not copyable or movable
//
// Reference: Chromium base::NoDestructor
//   https://chromium.googlesource.com/chromium/src/+/master/base/no_destructor.h
template <typename T>
class NoDestructor {
 public:
  // Constructs the wrapped object in-place with the given arguments.
  // Forwards all arguments to T's constructor.
  template <typename... Args>
  explicit NoDestructor(Args&&... args) { /*{{{*/
    new (storage_) T(std::forward<Args>(args)...);
  } /*}}}*/

  // Constructs the wrapped object from an initializer list.
  // Enables brace-enclosed initialization, e.g.:
  //   NoDestructor<std::vector<int>> v({1, 2, 3});
  template <typename U>
  explicit NoDestructor(std::initializer_list<U> init_list) { /*{{{*/
    new (storage_) T(init_list);
  } /*}}}*/

  // The destructor intentionally does NOT destroy the wrapped object.
  // This is the core purpose of NoDestructor.
  ~NoDestructor() = default;

  // NoDestructor is not copyable
  NoDestructor(const NoDestructor&) = delete;
  NoDestructor& operator=(const NoDestructor&) = delete;

  // NoDestructor is not movable
  NoDestructor(NoDestructor&&) = delete;
  NoDestructor& operator=(NoDestructor&&) = delete;

  // Returns a pointer to the wrapped object.
  T* Get() { /*{{{*/
    return reinterpret_cast<T*>(storage_);
  } /*}}}*/

  // Returns a const pointer to the wrapped object.
  const T* Get() const { /*{{{*/
    return reinterpret_cast<const T*>(storage_);
  } /*}}}*/

  // Returns a reference to the wrapped object.
  T& operator*() { return *Get(); }
  const T& operator*() const { return *Get(); }

  // Returns a pointer to the wrapped object for member access.
  T* operator->() { return Get(); }
  const T* operator->() const { return Get(); }

 private:
  // Aligned storage large enough to hold a T object.
  // Using alignas ensures proper alignment for type T.
  alignas(T) char storage_[sizeof(T)];
};

}  // namespace base

#endif
