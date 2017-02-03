// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CONFIG_H_
#define BASE_CONFIG_H_

#include <map>
#include <string>

#include "base/status.h"

namespace base
{

class Config
{
    public:
        Config();
        ~Config();

    public:
        Code LoadFile(const std::string &path);

    public:
        Code GetValue(const std::string &key, std::string *value);
        Code GetInt32Value(const std::string &key, int *value);
        Code GetInt64Value(const std::string &key, int64_t *value);

        Code GetValue(const std::string &key, std::string *value) const;
        Code GetInt32Value(const std::string &key, int *value) const;
        Code GetInt64Value(const std::string &key, int64_t *value) const;

    public:
        void Print();
        
    private:
        Code SetConf(const std::string &cnt);

    private:
        std::map<std::string, std::string> confs_;
};

}

#endif
