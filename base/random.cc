// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <assert.h>

#include "random.h"
#include "common.h"

namespace base
{

Code Itoa(uint32_t in, std::string *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;

    char buf[kMaxLenOfInt+1];
    uint32_t ret = snprintf(buf, sizeof(buf), "%010u", in);
    assert(ret < (int)sizeof(buf) && ret >= 0);
    out->append(buf, ret);
    
    return kOk;
}/*}}}*/

RangeRandom::RangeRandom(uint32_t begin, uint32_t end) : 
                         begin_(begin), end_(end), cur_(begin-1)
{/*{{{*/
}/*}}}*/

RangeRandom::~RangeRandom()
{/*{{{*/
}/*}}}*/

Code RangeRandom::GetRandStr(std::string *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;
    if (++cur_ >= end_) cur_ = begin_;

    return Itoa(cur_, out);
}/*}}}*/

}

#ifdef _RANDOM_MAIN_TEST_
int main(int agrc, char *argv[])
{/*{{{*/
    using namespace base;

    int begin = 400000000;
    int end =   400000011;
    RangeRandom rr(begin, end);

    std::string num;
    for (int i = 0; i < end - begin; ++i)
    {
        rr.GetRandStr(&num);
        fprintf(stderr, "%s\n", num.c_str());
        num.resize(0);
    }

    return 0;
}/*}}}*/
#endif