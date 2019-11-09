// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_HASH_H_
#define BASE_HASH_H_

#include <string>

#include <stdint.h>

namespace base
{

/**
 * BKDR Hash 
 */
uint32_t BKDRHash(const char *str);

uint32_t BKDRHash(uint32_t num);

uint32_t CRC32(const char *str, int len);

/**
 * Note: https://zh.wikipedia.org/wiki/Murmur哈希
 */
uint32_t Murmur32(const std::string &key, uint32_t seed);

size_t HashString(const char* s);

template <typename Container, typename Key>
typename Container::const_iterator GetEqualOrUpperBound(const Container &container, const Key &key)
{/*{{{*/
    typename Container::const_iterator it;
    it = container.find(key);
    if (it != container.end())
    {
        return it;
    }

    it = container.upper_bound(key);
    return it;
}/*}}}*/

}

#endif
