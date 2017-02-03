// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_CACHE_TREE_CACHE_H_
#define STORE_CACHE_TREE_CACHE_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

#include "cache.h"

namespace store
{

class TreeCache : public Cache
{
    public:
        TreeCache();
        virtual ~TreeCache();

        virtual base::Code Load();
        virtual base::Code Dump();

    public:


    private:
};

}

#endif
