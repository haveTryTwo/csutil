// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/search.h"

namespace base
{
}

#ifdef _SEARCH_MAIN_TEST_
#include <deque>
#include <vector>
#include <utility>

#include <assert.h>

int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

    // descend search
    int arr_des_odd[] = {40, 35, 30, 25, 20, 15, 10};
    Print(arr_des_odd, arr_des_odd + sizeof(arr_des_odd)/sizeof(arr_des_odd[0]), arr_des_odd[0]);
    int pos = 0;
    int test_arr_odd[] = {43, 40, 37, 35, 32, 30, 28, 25, 21, 20, 16, 15, 13, 10, 2 };
    for (int i = 0; i < (int)(sizeof(test_arr_odd)/sizeof(test_arr_odd[0])); ++i)
    {
        Code ret = BinaryDescendSearch(arr_des_odd, sizeof(arr_des_odd)/sizeof(arr_des_odd[0]),
                                test_arr_odd[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_odd[i], pos, arr_des_odd[pos]);
    }
    fprintf(stderr, "\n");

    int arr_des_even[] = {40, 35, 30, 25, 20, 15};
    Print(arr_des_even, arr_des_even + sizeof(arr_des_even)/sizeof(arr_des_even[0]), arr_des_even[0]);
    int test_arr_even[] = {44, 40, 38, 35, 33, 30, 26, 25, 24, 20, 18, 15, 11};
    for (int i = 0; i < (int)(sizeof(test_arr_even)/sizeof(test_arr_even[0])); ++i)
    {
        Code ret = BinaryDescendSearch(arr_des_even, sizeof(arr_des_even)/sizeof(arr_des_even[0]),
                                test_arr_even[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_even[i], pos, arr_des_even[pos]);
    }
    fprintf(stderr, "\n");

    // ascend search
    int arr_asc_odd[] = {10, 15, 20, 25, 30, 35, 40};
    Print(arr_asc_odd, arr_asc_odd + sizeof(arr_asc_odd)/sizeof(arr_asc_odd[0]), arr_asc_odd[0]);
    for (int i = (int)(sizeof(test_arr_odd)/sizeof(test_arr_odd[0])) - 1; i >= 0; --i)
    {
        Code ret = BinaryAscendSearch(arr_asc_odd, sizeof(arr_asc_odd)/sizeof(arr_asc_odd[0]),
                                test_arr_odd[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_odd[i], pos, arr_asc_odd[pos]);
    }
    fprintf(stderr, "\n");

    int arr_asc_even[] = {15, 20, 25, 30, 35, 40};
    Print(arr_asc_even, arr_asc_even + sizeof(arr_asc_even)/sizeof(arr_asc_even[0]), arr_asc_even[0]);
    for (int i = (int)(sizeof(test_arr_even)/sizeof(test_arr_even[0])) -1; i >= 0; --i)
    {
        Code ret = BinaryAscendSearch(arr_asc_even, sizeof(arr_asc_even)/sizeof(arr_asc_even[0]),
                                test_arr_even[i], &pos, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, find pos:%d, value:%d\n", test_arr_even[i], pos, arr_asc_even[pos]);
    }
    fprintf(stderr, "\n");

    // descend search using RandomIterator
    std::vector<int> vec_des_odd(arr_des_odd, arr_des_odd+sizeof(arr_des_odd)/sizeof(arr_des_odd[0]));
    Print(vec_des_odd.begin(), vec_des_odd.end(), vec_des_odd[0]);
    std::vector<int>::iterator it;
    for (int i = 0; i < (int)(sizeof(test_arr_odd)/sizeof(test_arr_odd[0])); ++i)
    {
        Code ret = BinaryDescendSearch(vec_des_odd.begin(), vec_des_odd.end(),
                    test_arr_odd[i], &it, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, value:%d\n", test_arr_odd[i], *it);
    }
    fprintf(stderr, "\n");

    std::vector<int> vec_des_even(arr_des_even, arr_des_even+sizeof(arr_des_even)/sizeof(arr_des_even[0]));
    Print(vec_des_even.begin(), vec_des_even.end(), vec_des_even[0]);
    for (int i = 0; i < (int)(sizeof(test_arr_even)/sizeof(test_arr_even[0])); ++i)
    {
        Code ret = BinaryDescendSearch(vec_des_even.begin(), vec_des_even.end(),
                test_arr_even[i], &it, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, value:%d\n", test_arr_even[i], *it);
    }

    fprintf(stderr, "\n");
    // ascend search using RandomIterator
    std::deque<int> deq_asc_odd(arr_asc_odd, arr_asc_odd+sizeof(arr_asc_odd)/sizeof(arr_asc_odd[0]));
    Print(deq_asc_odd.begin(), deq_asc_odd.end(), deq_asc_odd[0]);
    std::deque<int>::iterator deq_it;
    for (int i = (int)(sizeof(test_arr_odd)/sizeof(test_arr_odd[0])) - 1; i >= 0; --i)
    {
        Code ret = BinaryAscendSearch(deq_asc_odd.begin(), deq_asc_odd.end(),
                    test_arr_odd[i], &deq_it, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, value:%d\n", test_arr_odd[i], *deq_it);
    }
    fprintf(stderr, "\n");

    std::deque<int> deq_asc_even(arr_asc_even, arr_asc_even+sizeof(arr_asc_even)/sizeof(arr_asc_even[0]));
    Print(deq_asc_even.begin(), deq_asc_even.end(), deq_asc_even[0]);
    for (int i = (int)(sizeof(test_arr_even)/sizeof(test_arr_even[0])) -1; i >= 0; --i)
    {
        Code ret = BinaryAscendSearch(deq_asc_even.begin(), deq_asc_even.end(),
                    test_arr_even[i], &deq_it, CompareNum<int>);
        assert(ret == kOk);
        fprintf(stderr, "test_value:%d, value:%d\n", test_arr_even[i], *deq_it);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "\n");
    // ascend search using RandomIterator and ComparePair
    std::vector<std::pair<std::string, std::string> > dir;
    dir.push_back(std::make_pair<std::string, std::string>("aa", "aa.txt"));
    dir.push_back(std::make_pair<std::string, std::string>("hh", "hh.txt"));
    dir.push_back(std::make_pair<std::string, std::string>("oo", "oo.txt"));
    dir.push_back(std::make_pair<std::string, std::string>("uu", "uu.txt"));

    std::pair<std::string, std::string> finds[] = {/*{{{*/
        std::pair<std::string, std::string>("aa", ""),
        std::pair<std::string, std::string>("bb", ""),
        std::pair<std::string, std::string>("cc", ""),
        std::pair<std::string, std::string>("hh", ""),
        std::pair<std::string, std::string>("ii", ""),
        std::pair<std::string, std::string>("jj", ""),
        std::pair<std::string, std::string>("oo", ""),
        std::pair<std::string, std::string>("pp", ""),
        std::pair<std::string, std::string>("qq", ""),
        std::pair<std::string, std::string>("uu", ""),
        std::pair<std::string, std::string>("vv", ""),
        std::pair<std::string, std::string>("xx", ""),
    };/*}}}*/
    fprintf(stderr, "finds num:%zu, single size:%zu\n", sizeof(finds)/sizeof(finds[0]), sizeof(finds[0]));

    std::vector<std::pair<std::string, std::string> >::iterator pair_it;
    for (int i = 0; i < (int)(sizeof(finds)/sizeof(finds[0])); ++i)
    {
        Code ret = BinaryAscendSearch(dir.begin(), dir.end(), finds[i], &pair_it,
                ComparePair<std::pair<std::string, std::string> >);
        assert(ret == kOk);
        fprintf(stderr, "finds key:%s, value:(%s, %s)\n", finds[i].first.c_str(), pair_it->first.c_str(), 
                pair_it->second.c_str());
    }
    fprintf(stderr, "\n");


    return 0;
}/*}}}*/
#endif
