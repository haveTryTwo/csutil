// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <stdio.h>
#include <stdint.h>

#include "base/hash.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Murmur32, Test_Normal)
{/*{{{*/
    using namespace base;

    std::string key = "10.12.13.14:2010:100";
    uint32_t seed = 0x12345678;
    uint32_t expect_num = 537653845;
    uint32_t num = Murmur32(key, seed);
    EXPECT_EQ(expect_num, num);
}/*}}}*/

TEST(Murmur32, Test_Normal_Empty_Str)
{/*{{{*/
    using namespace base;

    std::string key = "";
    uint32_t seed = 0x12345678;
    uint32_t expect_num = 3816608188;
    uint32_t num = Murmur32(key, seed);
    EXPECT_EQ(expect_num, num);
}/*}}}*/

TEST(GetEqualOrUpperBound, Test_Normal)
{/*{{{*/
    using namespace base;

    std::map<int, int> test_map;
    test_map.insert(std::make_pair<int, int>(10, 10));
    test_map.insert(std::make_pair<int, int>(20, 20));
    test_map.insert(std::make_pair<int, int>(30, 30));
    test_map.insert(std::make_pair<int, int>(40, 40));

    std::map<int, int>::const_iterator it;
    
    int num = 35;
    int upper_bound = 40;
    it = GetEqualOrUpperBound(test_map, num);
    EXPECT_NEQ(test_map.end(), it);
    EXPECT_EQ(upper_bound, it->first);

    num = 45;
    it = GetEqualOrUpperBound(test_map, num);
    EXPECT_EQ(test_map.end(), it);
}/*}}}*/

TEST(GetEqualOrUpperBound, Test_Equal)
{/*{{{*/
    using namespace base;

    std::map<int, int> test_map;
    test_map.insert(std::make_pair<int, int>(10, 10));
    test_map.insert(std::make_pair<int, int>(20, 20));
    test_map.insert(std::make_pair<int, int>(30, 30));
    test_map.insert(std::make_pair<int, int>(40, 40));

    std::map<int, int>::const_iterator it;
    
    int num = 20;
    int upper_bound = 20;
    it = GetEqualOrUpperBound(test_map, num);
    EXPECT_NEQ(test_map.end(), it);
    EXPECT_EQ(upper_bound, it->first);

    num = 40;
    upper_bound = 40;
    it = GetEqualOrUpperBound(test_map, num);
    EXPECT_NEQ(test_map.end(), it);
    EXPECT_EQ(upper_bound, it->first);
}/*}}}*/

TEST(GetEqualOrUpperBound, Test_To_LowerBound)
{/*{{{*/
    using namespace base;

    std::map<int, int> test_map;
    test_map.insert(std::make_pair<int, int>(10, 10));
    test_map.insert(std::make_pair<int, int>(20, 20));
    test_map.insert(std::make_pair<int, int>(30, 30));
    test_map.insert(std::make_pair<int, int>(40, 40));

    std::map<int, int>::const_iterator equal_or_upper_it;
    std::map<int, int>::const_iterator lower_bound_it;

    for (int i = 0; i < 50; ++i)
    {
        equal_or_upper_it = GetEqualOrUpperBound(test_map, i);
        lower_bound_it = test_map.lower_bound(i);
        
        EXPECT_EQ(equal_or_upper_it, lower_bound_it);
    }
}/*}}}*/
