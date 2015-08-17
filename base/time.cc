// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "base/time.h"
#include "base/common.h"

namespace base
{

Time::Time()
{/*{{{*/
    memset(&begin_, 0, sizeof(begin_));
    memset(&end_, 0, sizeof(end_));
}/*}}}*/

Time::~Time()
{/*{{{*/
}/*}}}*/

Time::Time(const Time &t)
{/*{{{*/
    memcpy(&begin_, &(t.begin_), sizeof(begin_));
    memcpy(&end_, &(t.end_), sizeof(end_));
}/*}}}*/

Time& Time::operator= (const Time &t)
{/*{{{*/
    memcpy(&begin_, &(t.begin_), sizeof(begin_));
    memcpy(&end_, &(t.end_), sizeof(end_));

    return *this;
}/*}}}*/

void Time::Begin()
{/*{{{*/
    gettimeofday(&begin_, NULL);
}/*}}}*/

void Time::End()
{/*{{{*/
    gettimeofday(&end_, NULL);
}/*}}}*/

void Time::Print() const
{/*{{{*/
    if (end_.tv_sec < begin_.tv_sec) return;

#if defined(__linux__) || defined(__unix__)
    fprintf(stderr, "[begin] sec:%ld, microsecond:%ld\n", begin_.tv_sec, begin_.tv_usec);
    fprintf(stderr, "[end  ] sec:%ld, microsecond:%ld\n", end_.tv_sec, end_.tv_usec);
#elif defined(__APPLE__)
    fprintf(stderr, "[begin] sec:%ld, microsecond:%d\n", begin_.tv_sec, begin_.tv_usec);
    fprintf(stderr, "[end  ] sec:%ld, microsecond:%d\n", end_.tv_sec, end_.tv_usec);
#endif

    int64_t diff_time = (end_.tv_sec-begin_.tv_sec)*kUnitConvOfMicrosconds +
                        (end_.tv_usec-begin_.tv_usec);

    fprintf(stderr, "diff time: %lld seconds, %lld microseconds\n",
            diff_time/kUnitConvOfMicrosconds, diff_time%kUnitConvOfMicrosconds);
}/*}}}*/

}

#ifdef _TIME_MAIN_TEST_
#include <string>
#include <vector>

#include <unistd.h>

struct Person
{/*{{{*/
    std::string name;
    int age;
    double weight;
    double height;
};/*}}}*/

int main(int argc, char *argv[])
{/*{{{*/
    base::Time t;
    t.Begin();

    for (int i = 0; i < base::kUnitConvOfMicrosconds; ++i);

    t.End();
    t.Print();

    {
        std::vector<Person> persons;
        for (int i = 0; i < 1000; ++i)
        {
            Person p = {std::string("zhangsan"), i, 124.23, 123.123 };
            persons.push_back(p);
        }
        fprintf(stderr, "Size of Persons:%zu\n", persons.size());
        typedef std::vector<Person>::iterator VecIter;
        base::Time t;

        t.Begin();
        for (int i = 0; i < 100000; ++i)
            for (VecIter it = persons.begin(); it != persons.end(); ++it);

        t.End();
        t.Print();
    }

    return 0;
}/*}}}*/
#endif
