// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

/**
 * date format need to be "YYYY-mm-dd HH:MM:SS"
 */
Code Time::GetSecond(const std::string &date, time_t *time)
{/*{{{*/
    struct tm tm = {0};

    sscanf(date.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &(tm.tm_year), &(tm.tm_mon), 
            &(tm.tm_mday), &(tm.tm_hour), &(tm.tm_min), &(tm.tm_sec));

    tm.tm_year -= 1900;
    tm.tm_mon -=1;
    time_t r = mktime(&tm);
    if (r == -1) return kMkTimeFailed;

    *time = r;

    return kOk; 
}/*}}}*/

Code Time::GetDate(time_t second, std::string *date)
{/*{{{*/
    struct tm tm = {0};
    
    localtime_r(&second, &tm);

    char buf[kBufLen] = {0};
    snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d", 
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    date->assign(buf);

    return kOk;
}/*}}}*/

Code Time::GetDate(time_t second, const std::string &format, std::string *date)
{/*{{{*/
    struct tm tm = {0};
    
    localtime_r(&second, &tm);

    char buf[kBufLen] = {0};
    strftime(buf, sizeof(buf)-1, format.c_str(), &tm);

    date->assign(buf);

    return kOk;
}/*}}}*/


Code Time::GetYear(time_t second, uint32_t *year)
{/*{{{*/
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *year = ptm->tm_year+1900;

    return kOk;
}/*}}}*/

Code Time::GetHourOfDay(time_t second, uint32_t *hour)
{/*{{{*/
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *hour = ptm->tm_hour;

    return kOk;
}/*}}}*/

Code Time::GetDayOfMonth(time_t second, uint32_t *day)
{/*{{{*/
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *day = ptm->tm_mday;

    return kOk;
}/*}}}*/

Code Time::GetTime(struct timeval *tm)
{/*{{{*/
    if (tm == NULL) return kInvalidParam;

#if defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    tm->tv_sec = tp.tv_sec;
    tm->tv_usec = tp.tv_nsec/kThousand;
#else
    gettimeofday(tm, NULL);
#endif

    return kOk;
}/*}}}*/

void Time::Begin()
{/*{{{*/
#if defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    begin_.tv_sec = tp.tv_sec;
    begin_.tv_usec = tp.tv_nsec/kThousand;
#else
    gettimeofday(&begin_, NULL);
#endif
}/*}}}*/

void Time::End()
{/*{{{*/
#if defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    end_.tv_sec = tp.tv_sec;
    end_.tv_usec = tp.tv_nsec/kThousand;
#else
    gettimeofday(&end_, NULL);
#endif
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

void Time::PrintDiffTime() const
{/*{{{*/
    if (end_.tv_sec < begin_.tv_sec) return;

    int64_t diff_time = (end_.tv_sec-begin_.tv_sec)*kUnitConvOfMicrosconds +
                        (end_.tv_usec-begin_.tv_usec);

    fprintf(stderr, "run time: %lld seconds, %lld microseconds\n",
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

    time_t now_sec = time(NULL);
    fprintf(stderr, "time:%d\n", (int)now_sec);

    std::string date;
    base::Time::GetDate(now_sec, &date);
    fprintf(stderr, "date:%s\n", date.c_str());

    std::string format = "%Y%m%d%H%M%S";
    base::Time::GetDate(now_sec, format, &date);
    fprintf(stderr, "date:%s\n", date.c_str());

    time_t date_t;
    base::Time::GetSecond(date, &date_t);
    fprintf(stderr, "time:%d\n", (int)date_t);

    now_sec = 1562031999;
    now_sec = 1704029500;
    uint32_t year = 0;
    base::Time::GetYear(now_sec, &year);
    fprintf(stderr, "year:%u\n", year);

    uint32_t hour = 0;
    base::Time::GetHourOfDay(now_sec, &hour);
    fprintf(stderr, "hour:%u\n", hour);

    uint32_t day = 0;
    base::Time::GetDayOfMonth(now_sec, &day);
    fprintf(stderr, "day:%u\n", day);

    struct timeval now_tm = {0, 0};
    base::Time::GetTime(&now_tm);
#if defined(__linux__) || defined(__unix__)
    fprintf(stderr, "[begin] sec:%ld, microsecond:%ld\n", now_tm.tv_sec, now_tm.tv_usec);
#elif defined(__APPLE__)
    fprintf(stderr, "[begin] sec:%ld, microsecond:%d\n", now_tm.tv_sec, now_tm.tv_usec);
#endif


    return 0;
}/*}}}*/
#endif
