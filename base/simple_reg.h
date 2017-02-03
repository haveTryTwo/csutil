// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SIMPLE_REG_H_
#define BASE_SIMPLE_REG_H_

namespace base
{

class RegExp
{
    public:
        RegExp(const std::string &reg_str_);
        ~RegExp();

    public:
        Code Init();
        Code Check(const std::string &str);

    private:
        std::string reg_str_;
};

}

#endif
