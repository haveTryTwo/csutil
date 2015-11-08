// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DYNAMIC_LIB_H_
#define BASE_DYNAMIC_LIB_H_

#include <string>

#include "base/status.h"

namespace base
{
   
class DynamicLib
{
    public:
        DynamicLib();
        ~DynamicLib();

    private:
        DynamicLib(const DynamicLib &dlib);
        DynamicLib& operator =(const DynamicLib &dlib);

    public:
        Code Open(const std::string &path);
        Code GetFunc(const std::string &func_name, void **func);
        Code Close();

    private:
        void *handle_;
};

}

#endif
