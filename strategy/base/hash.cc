// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/hash.h"
#include "base/common.h"

namespace base
{

uint32_t BKDRHash(const char *str)
{
    uint32_t seed = 131;
    uint32_t hash = 0;

    while (*str != '\0')
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t BKDRHash(uint32_t num)
{
    char buf[kMaxLenOfInt+1];
    snprintf(buf, sizeof(buf), "%010u", num);
    return BKDRHash(buf);
}

}

#ifdef _HASH_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;

    uint32_t num = 0xffffffff;
    uint32_t hash = BKDRHash(num);
    fprintf(stderr, "hash:%u\n", hash);

    return 0;
}
#endif
