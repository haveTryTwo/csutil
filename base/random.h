// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_RANDOM_H_
#define BASE_RANDOM_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base
{
    
Code Itoa(uint32_t in, std::string *out);

Code InitRand();
Code CheckIsSatisfied(float ratio, bool *is_statisfied);

Code GetRandStr(uint32_t rand_str_len, std::string *rand_str);

class RangeRandom
{/*{{{*/
    public:
        RangeRandom(uint32_t begin, uint32_t end);
        ~RangeRandom();

        Code GetRandStr(std::string *out);

    private:
        RangeRandom(const RangeRandom &rr);
        RangeRandom& operator= (const RangeRandom &rr);

    private:
        uint32_t begin_;
        uint32_t end_;
        uint32_t cur_;
};/*}}}*/

}

#endif
