// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <dlfcn.h>

#include "base/dynamic_lib.h"

namespace base {

DynamicLib::DynamicLib() : handle_(NULL) {}

DynamicLib::~DynamicLib() {
  if (handle_ != NULL) {
    dlclose(handle_);
    handle_ = NULL;
  }
}

Code DynamicLib::Open(const std::string &path) {
  if (path.empty()) return kInvalidParam;
  handle_ = dlopen(path.c_str(), RTLD_LAZY);
  if (handle_ == NULL) return kDlopenFailed;

  return kOk;
}

Code DynamicLib::GetFunc(const std::string &func_name, void **func) {
  if (func_name.empty() || func == NULL) return kInvalidParam;
  dlerror();
  *func = dlsym(handle_, func_name.c_str());
  if (dlerror() != NULL) return kDlsymFailed;

  return kOk;
}

Code DynamicLib::Close() {
  if (handle_ != NULL) {
    dlclose(handle_);
    handle_ = NULL;
  }

  return kOk;
}

}  // namespace base

#ifdef _DYNAMIC_LIB_MAIN_TEST_
typedef int (*Printf)(const char *format, ...);

int main(int argc, char *argv[]) {
  using namespace base;

  DynamicLib dlib;
  std::string path = "libc.so.6";
  Code ret = dlib.Open(path);
  if (ret != kOk) return ret;

  std::string func_name = "printf";
  void *addr = NULL;
  ret = dlib.GetFunc(func_name, &addr);
  if (ret != kOk) return ret;
  Printf print = reinterpret_cast<Printf>(addr);
  print("yes, load in\n");

  dlib.Close();

  return 0;
}
#endif
