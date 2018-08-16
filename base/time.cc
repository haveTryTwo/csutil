// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <time.h>
#include <stdio.h>
#include <limits.h>
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

Code Time::GetDate(time_t second, uint32_t *year, uint32_t *mon, uint32_t *mday)
{/*{{{*/
    if (year == NULL && mon == NULL && mday == NULL) return kInvalidParam;

    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *year = ptm->tm_year+1900;
    *mon = ptm->tm_mon+1;
    *mday = ptm->tm_mday;

    return kOk;
}/*}}}*/

Code Time::GetCompilerDate(std::string *date)
{/*{{{*/
    if (date == NULL) return kInvalidParam;

    char tmp_buf[64] = {0};
    uint32_t tmp_day = 0;
    uint32_t tmp_year = 0;
    sscanf(__DATE__, "%s %u %u", tmp_buf, (unsigned int*)&tmp_day, (unsigned int*)&tmp_year);
    uint32_t tmp_month = 0;
    Code ret = GetNumericMonth(tmp_buf, &tmp_month);
    if (ret != kOk) return ret;

    snprintf(tmp_buf, sizeof(tmp_buf)-1, "%04d-%02d-%02d %s", 
            tmp_year, tmp_month, tmp_day, __TIME__);

    date->assign(tmp_buf);

    return kOk;
}/*}}}*/

Code Time::GetCompilerDate(uint32_t *year, uint32_t *mon, uint32_t *mday, uint32_t *hour, uint32_t *min, uint32_t *second)
{/*{{{*/
    if (year == NULL || mon == NULL || mday == NULL || hour == NULL || min == NULL || second == NULL) 
        return kInvalidParam;

    char tmp_buf[64] = {0};
    uint32_t tmp_day = 0;
    uint32_t tmp_year = 0;
    sscanf(__DATE__, "%s %u %u", tmp_buf, (unsigned int*)&tmp_day, (unsigned int*)&tmp_year);
    uint32_t tmp_month = 0;
    Code ret = GetNumericMonth(tmp_buf, &tmp_month);
    if (ret != kOk) return ret;

    uint32_t tmp_hour = 0;
    uint32_t tmp_min = 0;
    uint32_t tmp_second = 0;
    sscanf(__TIME__, "%u:%u:%u", (unsigned int*)&tmp_hour, (unsigned int*)&tmp_min, (unsigned int*)&tmp_second);

    *year = tmp_year;
    *mon = tmp_month;
    *mday = tmp_day;
    *hour = tmp_hour;
    *min = tmp_min;
    *second = tmp_second;

    return kOk;
}/*}}}*/

Code Time::GetNumericMonth(const std::string &str_month, uint32_t *numeric_month)
{/*{{{*/
    if (numeric_month == NULL) return kInvalidParam;

    std::map<std::string, uint32_t> month_map;
    month_map.insert(std::pair<std::string, uint32_t>(kJan, 1));
    month_map.insert(std::pair<std::string, uint32_t>(kFeb, 2));
    month_map.insert(std::pair<std::string, uint32_t>(kMar, 3));
    month_map.insert(std::pair<std::string, uint32_t>(kApr, 4));
    month_map.insert(std::pair<std::string, uint32_t>(kMay, 5));
    month_map.insert(std::pair<std::string, uint32_t>(kJun, 6));
    month_map.insert(std::pair<std::string, uint32_t>(kJul, 7));
    month_map.insert(std::pair<std::string, uint32_t>(kAug, 8));
    month_map.insert(std::pair<std::string, uint32_t>(kSep, 9));
    month_map.insert(std::pair<std::string, uint32_t>(kOct, 10));
    month_map.insert(std::pair<std::string, uint32_t>(kNov, 11));
    month_map.insert(std::pair<std::string, uint32_t>(kDec, 12));

    std::map<std::string, uint32_t>::iterator month_it = month_map.find(str_month);
    if (month_it == month_map.end()) return kInvalidParam;

    *numeric_month = month_it->second;

    return kOk;
}/*}}}*/


Code Time::GetYear(time_t second, uint32_t *year)
{/*{{{*/
    if (year == NULL) return kInvalidParam;
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *year = ptm->tm_year+1900;

    return kOk;
}/*}}}*/

Code Time::GetMonthOfYear(time_t second, uint32_t *mon)
{/*{{{*/
    if (mon == NULL) return kInvalidParam;
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *mon = ptm->tm_mon+1;

    return kOk;
}/*}}}*/

Code Time::GetDayOfMonth(time_t second, uint32_t *day)
{/*{{{*/
    if (day == NULL) return kInvalidParam;
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *day = ptm->tm_mday;

    return kOk;
}/*}}}*/

Code Time::GetHourOfDay(time_t second, uint32_t *hour)
{/*{{{*/
    if (hour == NULL) return kInvalidParam;
    struct tm result_tm;
    struct tm *ptm = localtime_r(&second, &result_tm);
    if (ptm == NULL) return kLocalTimeFailed;

    *hour = ptm->tm_hour;

    return kOk;
}/*}}}*/

Code Time::GetMinSecOfYear(uint32_t year, uint32_t* second)
{/*{{{*/
    if (second == NULL) return kInvalidParam;

    struct tm tm = {0};

    tm.tm_year = year - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 1;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    time_t r = mktime(&tm);
    if (r == -1) return kMkTimeFailed;

    *second = r;

    return kOk;
}/*}}}*/

Code Time::GetMaxSecOfYear(uint32_t year, uint32_t* second)
{/*{{{*/
    if (second == NULL || year >= UINT_MAX) return kInvalidParam;

    uint32_t new_year = year + 1;
    Code ret = GetMinSecOfYear(new_year, second);
    if (ret != kOk) return ret;

    *second -= 1;

    return kOk;
}/*}}}*/

Code Time::GetMinSecOfMonth(uint32_t year, uint32_t month, uint32_t *second)
{/*{{{*/
    if (second == NULL || month <= 0 || month > 12) return kInvalidParam;

    struct tm tm = {0};

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = 1;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    time_t r = mktime(&tm);
    if (r == -1) return kMkTimeFailed;

    *second = r;

    return kOk;
}/*}}}*/

Code Time::GetMaxSecOfMonth(uint32_t year, uint32_t month, uint32_t *second)
{/*{{{*/
    if (second == NULL || month <= 0 || month > 12) return kInvalidParam;
    
    uint32_t new_year = year;
    uint32_t new_month = month;
    if (month == 12)
    {
        new_year = year+1;
        new_month = 1;
    }
    else
    {
        new_month += 1;
    }

    Code ret = GetMinSecOfMonth(new_year, new_month, second);
    if (ret != kOk) return ret;

    *second -= 1;

    return kOk;
}/*}}}*/

Code Time::GetRealDate(uint32_t year, int month, uint32_t *real_year, int *real_month)
{/*{{{*/
    if (real_year == NULL || real_month == NULL) return kInvalidParam;

    if (month <= 0)
    {
        uint32_t tmp_month = -month;
        uint32_t multiplue = tmp_month / kMonthNumOfYear;
        int tmp_year = year - (multiplue+1);
        if (tmp_year < 0) return kInvalidParam;

        *real_year = tmp_year;
        *real_month = month + (multiplue+1)*kMonthNumOfYear;

        return kOk;
    }
    else
    {
        uint32_t multiplue = (month-1) / kMonthNumOfYear;
        uint64_t tmp_real_year = year + multiplue;
        if (tmp_real_year > UINT_MAX) return kInvalidParam;

        *real_year = year + multiplue;
        *real_month = month - multiplue*kMonthNumOfYear;

        return kOk;
    }
    
    return kOtherFailed;
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

Code Time::GetTime(uint32_t *sec, uint32_t *nsec)
{/*{{{*/
    if (sec == NULL || nsec == NULL) return kInvalidParam;

#if defined(__linux__)
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    *sec = tp.tv_sec;
    *nsec = tp.tv_nsec;
#else
    struct timeval tm;
    gettimeofday(&tm, NULL);
    *sec = tm.tv_sec;
    *nsec = tm.tv_usec*kThousand;
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
            (long long int)diff_time/kUnitConvOfMicrosconds, 
            (long long int)diff_time%kUnitConvOfMicrosconds);
}/*}}}*/

void Time::PrintDiffTime() const
{/*{{{*/
    if (end_.tv_sec < begin_.tv_sec) return;

    int64_t diff_time = (end_.tv_sec-begin_.tv_sec)*kUnitConvOfMicrosconds +
                        (end_.tv_usec-begin_.tv_usec);

    fprintf(stderr, "run time: %lld seconds, %lld microseconds\n",
            (long long int)diff_time/kUnitConvOfMicrosconds, 
            (long long int)diff_time%kUnitConvOfMicrosconds);
}/*}}}*/

Code GetWeekIndex(time_t second, int *index, int *year)
{/*{{{*/
    if (index == NULL || year == NULL) return kInvalidParam;

    struct tm cur_tm;
    localtime_r(&second, &cur_tm);
    int diff = 0;
    if (cur_tm.tm_wday > cur_tm.tm_yday)
    {
        diff = (cur_tm.tm_yday+kWeek - cur_tm.tm_wday)%kWeek;
    }
    else
    {
        diff = (cur_tm.tm_yday-cur_tm.tm_wday)%kWeek;
    }

    // Note: new year day (01-01) if monday, then get week index directly
    if (diff == 6)
    {
        *index = cur_tm.tm_yday/kWeek + 1;
        *year = cur_tm.tm_year + 1900;
        return kOk;
    }

    // The Week which cross new year is treated as old week index in last year
    if (cur_tm.tm_yday < diff + 1)
    {
        struct tm tmp_tm;
        memset(&tmp_tm, 0, sizeof(tmp_tm));
        tmp_tm.tm_year = cur_tm.tm_year;
        tmp_tm.tm_mon = 0;
        tmp_tm.tm_mday = 1;
        time_t tmp_time = mktime(&tmp_tm);
        tmp_time -= 1; // last day of last year
        return GetWeekIndex(tmp_time, index, year);
    }

    *index = (cur_tm.tm_yday-diff-1)/kWeek + 1;
    *year = cur_tm.tm_year + 1900;

    return kOk;
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
