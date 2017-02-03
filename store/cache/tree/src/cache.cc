// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cache.h"

namespace store
{

Cache::Cache()
{
}

Cache::~Cache()
{
}

base::Code Cache::Init(std::string dump_dir)
{/*{{{*/
    dump_dir_ = dump_dir;

    return Load();
}/*}}}*/

base::Code Cache::Load()
{/*{{{*/
    return base::kOk;
}/*}}}*/

base::Code Cache::Dump()
{/*{{{*/
    return base::kOk;
}/*}}}*/

}

