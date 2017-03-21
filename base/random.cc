// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
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

Code InitRand()
{/*{{{*/
    srand(time(NULL));
    return kOk;
}/*}}}*/

Code CheckIsSatisfied(float ratio, bool *is_statisfied)
{/*{{{*/
    if (ratio < 0 || ratio > 1 || is_statisfied == NULL) return kInvalidParam;

    *is_statisfied = false;

    int int_ratio = (int)(ratio * kHundred);
    int num = rand()%kHundred;
//    fprintf(stderr, "num:%d, int_ratio:%d\n", num, int_ratio);

    if (num < int_ratio)
        *is_statisfied = true;

    return kOk;
}/*}}}*/

Code GetRandStr(uint32_t rand_str_len, std::string *rand_str)
{/*{{{*/
    if (rand_str == NULL) return kInvalidParam;
    InitRand();

    std::string alnum = "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ0123456789";
    std::string tmp_rand_str;
    for (uint32_t i = 0; i < rand_str_len; ++i)
    {
        uint32_t index = rand()%alnum.size();
        tmp_rand_str.append(1, alnum.data()[index]);
    }
    *rand_str = tmp_rand_str;

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

    float ratio = 0.1;
    bool is_satisfied = false;
    InitRand();
    for (int i = 0; i < 1000000; ++i)
        CheckIsSatisfied(ratio, &is_satisfied);
    fprintf(stderr, "ratio:%f, satisfied:%d\n", ratio, is_satisfied);

    return 0;
}/*}}}*/
#endif
