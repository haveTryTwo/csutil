// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TIME_H_
#define BASE_TIME_H_

#include <string>

#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#include "base/status.h"

namespace base
{

class Time
{/*{{{*/
    public:
        Time();
        ~Time();
        Time(const Time &t);
        Time& operator= (const Time &t);

    public:
        /**
         * date format need to be "YYYY-mm-dd HH:MM:SS"
         */
        static Code GetSecond(const std::string &date, time_t *time);

    public:
        static Code GetDate(time_t second, std::string *date);
        static Code GetDate(time_t second, const std::string &format, std::string *date);
        static Code GetDate(time_t second, uint32_t *year, uint32_t *mon, uint32_t *mday);

    public:
        static Code GetCompilerDate(std::string *date);
        static Code GetCompilerDate(uint32_t *year, uint32_t *mon, uint32_t *mday, uint32_t *hour, uint32_t *min, uint32_t *second);
        static Code GetNumericMonth(const std::string &str_month, uint32_t *numeric_month);
        
    public:
        static Code GetYear(time_t second, uint32_t *year);
        static Code GetHourOfDay(time_t second, uint32_t *hour);
        static Code GetDayOfMonth(time_t second, uint32_t *mday);
        static Code GetMonthOfYear(time_t second, uint32_t *mon);

        static Code GetMinSecOfYear(uint32_t year, uint32_t *second);
        static Code GetMaxSecOfYear(uint32_t year, uint32_t *second);
        static Code GetMinSecOfMonth(uint32_t year, uint32_t month, uint32_t *second);
        static Code GetMaxSecOfMonth(uint32_t year, uint32_t month, uint32_t *second);

        static Code GetRealDate(uint32_t year, int month, uint32_t *real_year, int *real_month);

    public:
        static Code GetPreMonth(uint32_t year, uint32_t month, uint32_t *pre_year, uint32_t *pre_month);
        static Code GetNextMonth(uint32_t year, uint32_t month, uint32_t *next_year, uint32_t *next_month);

        static Code GetPreDay(uint32_t year, uint32_t month, uint32_t day, uint32_t *pre_year, uint32_t *pre_month, uint32_t *pre_day);
        static Code GetNextDay(uint32_t year, uint32_t month, uint32_t day, uint32_t *next_year, uint32_t *next_month, uint32_t *next_day);
        
    public:
        static Code GetTime(struct timeval *tm);
        static Code GetTime(uint32_t *sec, uint32_t *nsec);
        static Code GetAbsTime(uint32_t escape_msec, struct timespec *abs_ts);

    public:
        static Code IsLeapYear(uint32_t year, bool *leap_year_flag);
        static Code GetDayNumOfMonth(uint32_t year, uint32_t month, uint32_t *day_num);

    public:
        void Begin();
        void End();
        void Print() const;
        void PrintDiffTime() const;

    private:
        struct timeval begin_;
        struct timeval end_;
};/*}}}*/

Code GetWeekIndex(time_t second, int *index, int *year);

}

#endif
