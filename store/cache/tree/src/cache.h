// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_CACHE_CACHE_H_
#define STORE_CACHE_CACHE_H_

#include "base/status.h"

namespace store
{

class Cache
{
    public:
        Cache();
        virtual ~Cache();

        base::Code Init(std::string dump_dir);
        virtual base::Code Load();
        virtual base::Code Dump();

    private:
        std::string dump_dir_;
};

}

#endif
