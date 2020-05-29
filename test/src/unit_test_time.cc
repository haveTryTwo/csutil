// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/time.h"
#include "base/common.h"

#include "test_base/include/test_base.h"

TEST(Time, GetYear_NormalDate)
{/*{{{*/
    using namespace base;

    uint32_t time = 1481358320;
    uint32_t year = 2016;
    uint32_t tmp_year = 0;
    Code ret = Time::GetYear(time, &tmp_year);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(year, tmp_year);
}/*}}}*/

TEST(Time, GetMonthOfYear_NormalDate)
{/*{{{*/
    using namespace base;

    uint32_t time = 1481358320;
    uint32_t month = 12;
    uint32_t tmp_month = 0;
    Code ret = Time::GetMonthOfYear(time, &tmp_month);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(month, tmp_month);
}/*}}}*/

TEST(Time, GetDayOfMonth_NormalDate)
{/*{{{*/
    using namespace base;

    uint32_t time = 1481358320;
    uint32_t day = 10;
    uint32_t tmp_day = 0;
    Code ret = Time::GetDayOfMonth(time, &tmp_day);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(day, tmp_day);
}/*}}}*/

TEST(Time, GetHourOfDay_NormalDate)
{/*{{{*/
    using namespace base;

    uint32_t time = 1481358320;
    uint32_t hour = 16;
    uint32_t tmp_hour = 0;
    Code ret = Time::GetHourOfDay(time, &tmp_hour);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(hour, tmp_hour);
}/*}}}*/

TEST(Time, GetSecond_NormalDate)
{/*{{{*/
    using namespace base;

    uint32_t year = 2016;
    uint32_t month = 12;
    uint32_t day = 10;
    uint32_t hour = 16;
    uint32_t min = 25;
    uint32_t second = 20;
    char buf[64] = {0};
    snprintf(buf, sizeof(buf)-1, "%4d-%2d-%2d %2d:%2d:%2d", (int)year, (int)month, (int)day,
            (int)hour, (int)min, (int)second);

    uint32_t time = 1481358320;
    time_t tmp_time = 0;
    Code ret = Time::GetSecond(buf, &tmp_time);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(time, (uint32_t)tmp_time);
}/*}}}*/

TEST(Time, GetSecond_BeginDay)
{/*{{{*/
    using namespace base;

    uint32_t year = 2016;
    uint32_t month = 12;
    uint32_t day = 10;
    char buf[64] = {0};
    snprintf(buf, sizeof(buf)-1, "%4d-%2d-%2d 00:00:00", (int)year, (int)month, (int)day);

    uint32_t time = 1481299200;
    time_t tmp_time = 0;
    Code ret = Time::GetSecond(buf, &tmp_time);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(time, (uint32_t)tmp_time);
}/*}}}*/

TEST(Time, GetSecond_EndDay)
{/*{{{*/
    using namespace base;

    uint32_t year = 2016;
    uint32_t month = 12;
    uint32_t day = 10;
    char buf[64] = {0};
    snprintf(buf, sizeof(buf)-1, "%4d-%2d-%2d 23:59:59", (int)year, (int)month, (int)day);

    uint32_t time = 1481385599;
    time_t tmp_time = 0;
    Code ret = Time::GetSecond(buf, &tmp_time);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(time, (uint32_t)tmp_time);
}/*}}}*/

TEST(Time, GetDate_Normal_Date)
{/*{{{*/
    using namespace base;

    uint32_t time = 1481358320;
    uint32_t year = 2016;
    uint32_t mon = 12;
    uint32_t day = 10;

    uint32_t tmp_year = 0;
    uint32_t tmp_mon = 0;
    uint32_t tmp_day = 0;

    Code ret = Time::GetDate(time, &tmp_year, &tmp_mon, &tmp_day);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(year, tmp_year);
    EXPECT_EQ(mon, tmp_mon);
    EXPECT_EQ(day, tmp_day);
}/*}}}*/

TEST(Time, GetDate_EXP_ZERO)
{/*{{{*/
    using namespace base;

    uint32_t time = 0;
    uint32_t year = 1970;
    uint32_t mon = 1;
    uint32_t day = 1;

    uint32_t tmp_year = 0;
    uint32_t tmp_mon = 0;
    uint32_t tmp_day = 0;

    Code ret = Time::GetDate(time, &tmp_year, &tmp_mon, &tmp_day);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(year, tmp_year);
    EXPECT_EQ(mon, tmp_mon);
    EXPECT_EQ(day, tmp_day);

    std::string date = "1970-01-01 08:00:00";
    std::string tmp_date;
    ret = Time::GetDate(time, &tmp_date);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "time:%u, tmp_date:%s\n", (unsigned int)time, tmp_date.c_str());
    EXPECT_EQ(date, tmp_date);
}/*}}}*/

TEST(Time, GetSecond_Nomal_Zero)
{/*{{{*/
    using namespace base;

    std::string date = "1970-01-01 08:00:00";

    time_t time = 0;
    time_t tmp_time = 0;
    Code ret = Time::GetSecond(date, &tmp_time);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "date:%s, tmp_time:%d, ret:%d\n", date.c_str(), (int)tmp_time, ret);
    EXPECT_EQ(time, tmp_time);
}/*}}}*/

TEST(Time, GetSecond_EXP_ZERO)
{/*{{{*/
    using namespace base;

    std::string date = "1970-01-01 00:00:00";

    time_t time = -28800;
    time_t tmp_time = 0;
    Code ret = Time::GetSecond(date, &tmp_time);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "date:%s, tmp_time:%d, ret:%d\n", date.c_str(), (int)tmp_time, ret);
    EXPECT_EQ(time, tmp_time);
}/*}}}*/

TEST(Time, GetSecond_Press_OneMillionTimes)
{/*{{{*/
    using namespace base;

    std::string date = "2015-03-01 08:00:00";

    time_t time = 1425168000;
    time_t tmp_time = 0;
    int i = 0;
    for (i = 0; i < kMillion; ++i)
    {
        Code ret = Time::GetSecond(date, &tmp_time);
        EXPECT_EQ(kOk, ret);
    }
    fprintf(stderr, "date:%s, tmp_time:%d, i:%d\n", date.c_str(), (int)tmp_time, i);
    EXPECT_EQ(time, tmp_time);
}/*}}}*/

TEST(Time, GetCompilerDate_StrDate_Normal)
{/*{{{*/
    using namespace base;

    std::string comliper_date;
    Code ret = Time::GetCompilerDate(&comliper_date);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "date:%s\n", comliper_date.c_str());
}/*}}}*/

TEST(Time, GetCompilerDate_UintDate_Normal)
{/*{{{*/
    using namespace base;

    uint32_t year = 0;
    uint32_t mon = 0;
    uint32_t mday = 0;
    uint32_t hour = 0;
    uint32_t min = 0;
    uint32_t second = 0;
    Code ret = Time::GetCompilerDate(&year, &mon, &mday, &hour, &min, &second);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "date:%04u-%02u-%02u %02u:%02u:%02u\n", (unsigned)year, (unsigned)mon, (unsigned)mday,
            (unsigned)hour, (unsigned)min, (unsigned)second);
}/*}}}*/

TEST(Time, Normal_Week_Index)
{/*{{{*/
    using namespace base;

    time_t cur_time = 1419318135;
    int index = 0;
    int year = 0;
    for (int i = 0; i < 10000; ++i)
    {
        time_t tmp_time = cur_time + i*86400;
        Code ret = GetWeekIndex(tmp_time, &index, &year);
        EXPECT_EQ(kOk, ret);

        struct tm cur_tm;
        localtime_r(&tmp_time, &cur_tm);
        fprintf(stderr, "%ld %02d %d %d-%02d-%02d %02d:%02d:%02d\n",
            tmp_time, index, year, cur_tm.tm_year+1900, cur_tm.tm_mon+1, cur_tm.tm_mday,
            cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec);
    }
}/*}}}*/

TEST(Time, GetMinSecOfYear_Normal_Year)
{/*{{{*/
    using namespace base;

    uint32_t year = 2018;
    uint32_t dst_second = 1514736000;

    uint32_t second = 0;
    Code ret = Time::GetMinSecOfYear(year, &second);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_second, second);
    fprintf(stderr, "Min second of year:%u is %u\n", year, dst_second);
}/*}}}*/

TEST(Time, GetMaxSecOfYear_Normal_Year)
{/*{{{*/
    using namespace base;

    uint32_t year = 2018;
    uint32_t dst_second = 1546271999;

    uint32_t second = 0;
    Code ret = Time::GetMaxSecOfYear(year, &second);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_second, second);
    fprintf(stderr, "Max second of year:%u is %u\n", year, dst_second);
}/*}}}*/

TEST(Time, GetMinSecOfMonth_Normal_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2018;
    uint32_t month = 5;
    uint32_t dst_second = 1525104000;

    uint32_t second = 0;
    Code ret = Time::GetMinSecOfMonth(year, month, &second);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_second, second);
    fprintf(stderr, "Min second of year-month:%u-%u is %u\n", year, month, dst_second);
}/*}}}*/

TEST(Time, GetMaxSecOfMonth_Normal_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2018;
    uint32_t month = 5;
    uint32_t dst_second = 1527782399;

    uint32_t second = 0;
    Code ret = Time::GetMaxSecOfMonth(year, month, &second);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_second, second);
    fprintf(stderr, "Max second of year-month:%u-%u is %u\n", year, month, dst_second);
}/*}}}*/

TEST(Time, GetMaxSecOfMonth_Normal_December)
{/*{{{*/
    using namespace base;

    uint32_t year = 2018;
    uint32_t month = 12;
    uint32_t dst_second = 1546271999;

    uint32_t second = 0;
    Code ret = Time::GetMaxSecOfMonth(year, month, &second);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_second, second);
    fprintf(stderr, "Max second of year-month:%u-%u is %u\n", year, month, dst_second);
}/*}}}*/

TEST(Time, GetRealDate_Normal_Year_Positive_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2017;
    int month = 13;
    uint32_t dst_year = 2018;
    int dst_month = 1;

    uint32_t real_year = 0;
    int real_month = 0;
    Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_year, real_year);
    EXPECT_EQ(dst_month, real_month);
}/*}}}*/

TEST(Time, GetRealDate_Normal_Year_Zero_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2017;
    int month = 0;
    uint32_t dst_year = 2016;
    int dst_month = 12;

    uint32_t real_year = 0;
    int real_month = 0;
    Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_year, real_year);
    EXPECT_EQ(dst_month, real_month);
}/*}}}*/

TEST(Time, GetRealDate_Normal_Year_Negtive_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2017;
    int month = -12;
    uint32_t dst_year = 2015;
    int dst_month = 12;

    uint32_t real_year = 0;
    int real_month = 0;
    Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_year, real_year);
    EXPECT_EQ(dst_month, real_month);
}/*}}}*/

TEST(Time, GetRealDate_Normal_Year_Positive_Months)
{/*{{{*/
    using namespace base;

    for (int i = 1; i <= 100000; ++i)
    {
        uint32_t year = 0; // Note: start from 0 year
        int month = i;
        uint32_t dst_year = (i-1)/12;
        int dst_month = month - dst_year*12;

        uint32_t real_year = 0;
        int real_month = 0;
        Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(dst_year, real_year);
        EXPECT_EQ(dst_month, real_month);
    }
}/*}}}*/

TEST(Time, GetRealDate_Normal_Year_Negtive_Months)
{/*{{{*/
    using namespace base;

    for (int i = 0; i > -100000; --i)
    {
        uint32_t year = 10000; // Note: start from 10000 year
        int month = i;
        uint32_t dst_year = year - 1 - (-i)/12;
        int dst_month = month + ((-i)/12+1)*12;

        uint32_t real_year = 0;
        int real_month = 0;
        Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(dst_year, real_year);
        EXPECT_EQ(dst_month, real_month);
    }
}/*}}}*/

TEST(Time, GetRealDate_Exception_Year_Positive_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2017;
    int month = 1<<31;

    uint32_t real_year = 0;
    int real_month = 0;
    Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
    EXPECT_EQ(kInvalidParam, ret);
}/*}}}*/

TEST(Time, GetRealDate_Exception_Year_Negtive_Month)
{/*{{{*/
    using namespace base;

    uint32_t year = 2017;
    int month = -120000;

    uint32_t real_year = 0;
    int real_month = 0;
    Code ret = Time::GetRealDate(year, month, &real_year, &real_month);
    EXPECT_EQ(kInvalidParam, ret);
}/*}}}*/

TEST(Time, GetAbsTime_Normal_Less_OneSecond_Time)
{/*{{{*/
    using namespace base;

    struct timespec ts;
    uint32_t escape_msec = 10;
    Code ret = Time::GetAbsTime(escape_msec, &ts);
    EXPECT_EQ(kOk, ret);

    uint32_t now_sec = 0;
    uint32_t now_nsec = 0;
    ret = Time::GetTime(&now_sec, &now_nsec);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "now time is :%u, %u\n", now_sec, now_nsec);
    fprintf(stderr, "Escape is   :%u, %u after %u milliseconds\n", ts.tv_sec, ts.tv_nsec, escape_msec);
}/*}}}*/

TEST(Time, GetAbsTime_Normal_Time)
{/*{{{*/
    using namespace base;

    struct timespec ts;
    uint32_t escape_msec = 1000000100;
    Code ret = Time::GetAbsTime(escape_msec, &ts);
    EXPECT_EQ(kOk, ret);

    uint32_t now_sec = 0;
    uint32_t now_nsec = 0;
    ret = Time::GetTime(&now_sec, &now_nsec);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "now time is :%u, %u\n", now_sec, now_nsec);
    fprintf(stderr, "Escape is   :%u, %u after %u milliseconds\n", ts.tv_sec, ts.tv_nsec, escape_msec);
}/*}}}*/

TEST(Time, GetPreMonth_Normal_From_1_to_10000)
{/*{{{*/
    using namespace base;

    for (int i = 1; i < 10000; ++i)
    {
        for (int j = 1; j < 13; ++j)
        {
            uint32_t cur_year = i;
            uint32_t cur_month = j;
            uint32_t dst_pre_year = i;
            uint32_t dst_pre_month = j;
            if (cur_month == 1)
            {
                dst_pre_year--;
                dst_pre_month = 12;
            }
            else
            {
                dst_pre_month--;
            }

            uint32_t pre_year = 0;
            uint32_t pre_month = 0;
            Code ret = Time::GetPreMonth(cur_year, cur_month, &pre_year, &pre_month);
            EXPECT_EQ(kOk, ret);
            EXPECT_EQ(dst_pre_year, pre_year);
            EXPECT_EQ(dst_pre_month, pre_month);

//            fprintf(stderr, "cur:[%u:%u], dst_prex:[%u:%u], pre:[%u:%u]\n",
//                cur_year, cur_month, dst_pre_year, dst_pre_month, pre_year, pre_month);
        }
    }
}/*}}}*/

TEST(Time, GetNextMonth_Normal_From_1_to_10000)
{/*{{{*/
    using namespace base;

    for (int i = 1; i < 10000; ++i)
    {
        for (int j = 1; j < 13; ++j)
        {
            uint32_t cur_year = i;
            uint32_t cur_month = j;
            uint32_t dst_next_year = i;
            uint32_t dst_next_month = j;
            if (cur_month == 12)
            {
                dst_next_year++;
                dst_next_month = 1;
            }
            else
            {
                dst_next_month++;
            }

            uint32_t next_year = 0;
            uint32_t next_month = 0;
            Code ret = Time::GetNextMonth(cur_year, cur_month, &next_year, &next_month);
            EXPECT_EQ(kOk, ret);
            EXPECT_EQ(dst_next_year, next_year);
            EXPECT_EQ(dst_next_month, next_month);

//            fprintf(stderr, "cur:[%u:%u], dst_next:[%u:%u], next:[%u:%u]\n",
//                cur_year, cur_month, dst_next_year, dst_next_month, next_year, next_month);
        }
    }
}/*}}}*/

TEST(Time, GetDayNumOfMonth_Normal_From_1971_to_2105)
{/*{{{*/
    using namespace base;

    // Note: get the day number of month for year: [0, 10000]
#ifdef __x86_64__
    fprintf(stderr, "__x86_64__ sytem! Time could be deal with 2105 of year\n");
    for (int i = 1971; i < 2305; ++i)
    {
#elif __i386__
    fprintf(stderr, "__i386__ sytem! Time could be deal with 2038 of year\n");
    for (int i = 1971; i < 2038; ++i)
    {
#endif
        for (int j = 1; j < 13; ++j)
        {
            uint32_t cur_year = i;
            uint32_t cur_month = j;
            uint32_t next_year = 0;
            uint32_t next_month = 0;
            Code ret = Time::GetNextMonth(cur_year, cur_month, &next_year, &next_month);
            EXPECT_EQ(kOk, ret);

            char buf[kBufLen] = {0};
            snprintf(buf, sizeof(buf), "%04u-%02u-01 00:00:00", (unsigned)next_year, (unsigned)next_month);
            time_t second = 0;
            ret = Time::GetSecond(buf, &second);
            EXPECT_EQ(kOk, ret);
            --second;
            uint32_t dst_year = 0;
            uint32_t dst_month = 0;
            uint32_t dst_day = 0;
            ret = Time::GetDate(second, &dst_year, &dst_month, &dst_day);
            EXPECT_EQ(kOk, ret);

            uint32_t day_num = 0;
            ret = Time::GetDayNumOfMonth(cur_year, cur_month, &day_num);
            EXPECT_EQ(kOk, ret);

            EXPECT_EQ(dst_day, day_num);
            if (dst_day != day_num)
            {
                fprintf(stderr, "cur:[%u:%u], next:[%u,%u], buf:%s, second:%lld, dst:[%u:%u:%u], day_num:%u\n",
                    (unsigned)cur_year, (unsigned)cur_month, (unsigned)next_year, (unsigned)next_month,
                    buf, second,
                    (unsigned)dst_year, (unsigned)dst_month, (unsigned)dst_day, (unsigned)day_num);

                // struct tm oldtm = {0};
                // oldtm.tm_isdst = -1;
                // sscanf(buf, "%4d-%2d-%2d %2d:%2d:%2d", &(oldtm.tm_year), &(oldtm.tm_mon),
                //     &(oldtm.tm_mday), &(oldtm.tm_hour), &(oldtm.tm_min), &(oldtm.tm_sec));
                // fprintf(stderr, "%04u-%02u-%02u, %02d:%02d:%02d\n", oldtm.tm_year, oldtm.tm_mon, oldtm.tm_mday, oldtm.tm_hour, oldtm.tm_min, oldtm.tm_sec);
                // oldtm.tm_year -= 1900;
                // oldtm.tm_mon -=1;
                // time_t r = mktime(&oldtm);
                // fprintf(stderr, "buf:%s, r:%lld\n", buf, r);
            }
        }
    }
}/*}}}*/

TEST(Time, GetDiffInNatureDay_Normal_LessDiffTime)
{/*{{{*/
    using namespace base;

    time_t end_time = 1590632461; // 2020-05-28 10:21:01
    time_t start_time = 1590595200; // 2020-05-28 00:00:00
    for (time_t t = end_time; t > 1584547200; --t) // 2020-03-19 00:00:00
    {
        uint32_t expect_time = 0;
        if (t >= start_time)
        {
            expect_time = 0;
        }
        else
        {
            expect_time = (start_time-t)/86400 + ((start_time-t)%86400 == 0 ? 0:1);
        }

        uint32_t real_diff_time = 0;
        Code ret = base::Time::GetDiffInNatureDay(t, end_time, &real_diff_time);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(expect_time, real_diff_time);
    }
}/*}}}*/

TEST(Time, GetDiffInNatureDay_Normal_BigDiffTime)
{/*{{{*/
    using namespace base;

    time_t begin_time = 1590632461; // 2020-05-28 10:21:01
    time_t start_time = 1590681600-1; // 2020-05-28 59:59:59
    for (time_t t = begin_time; t < 1597766400; ++t) // 2020-08-19 00:00:00
    {
        uint32_t expect_time = 0;
        if (t <= start_time)
        {
            expect_time = 0;
        }
        else
        {
            expect_time = (t-start_time)/86400 + ((t-start_time)%86400 == 0 ? 0:1);
        }

        uint32_t real_diff_time = 0;
        Code ret = base::Time::GetDiffInNatureDay(t, begin_time, &real_diff_time);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(expect_time, real_diff_time);
    }
}/*}}}*/

TEST(Time, GetDiffInNatureDay_Normal_LessDiffTimeInNum)
{/*{{{*/
    using namespace base;

    time_t end_time = 1590632461; // 2020-05-28 10:21:01
    time_t start_time = 1590595200; // 2020-05-28 00:00:00
    time_t min_time = 1584547200; // 2020-03-19 00:00:00
    std::map<time_t, uint32_t> m;
    for (time_t i = start_time, j = 0; i >= min_time; i -= 86400, j++)
    {
        m[i] = j;
    }
    m[start_time+86400] = 0; // NOTE: add for anchor

    for (time_t t = end_time; t > 1584547200; --t) // 2020-03-19 00:00:00
    {
        std::map<time_t, uint32_t>::iterator it = m.upper_bound(t);
        EXPECT_NEQ(m.end(), it);
        it=m.find(it->first-86400);
        EXPECT_NEQ(m.end(), it);

        uint32_t real_diff_time = 0;
        Code ret = base::Time::GetDiffInNatureDay(t, end_time, &real_diff_time);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(it->second, real_diff_time);
        if (it->second != real_diff_time)
        {
            fprintf(stderr, "t:%u, real_diff_time:%u, expect_time:[%u, %u]\n",
                t, real_diff_time, it->first, it->second);
        }
    }
}/*}}}*/

TEST(Time, GetDiffInNatureDay_Normal_BigDiffTimeInNum)
{/*{{{*/
    using namespace base;

    time_t begin_time = 1590632461; // 2020-05-28 10:21:01
    time_t start_time = 1590681600-1; // 2020-05-28 59:59:59
    time_t largest_time = 1597766400; // 2020-08-19 00:00:00
    std::map<time_t, uint32_t> m;
    for (time_t i = start_time, j = 0; i < largest_time; i+=86400, j++)
    {
        m[i] = j;
    }

    for (time_t t = begin_time; t < largest_time; ++t) // 2020-08-19 00:00:00
    {
        std::map<time_t, uint32_t>::iterator it = m.lower_bound(t);
        EXPECT_NEQ(m.end(), it);

        uint32_t real_diff_time = 0;
        Code ret = base::Time::GetDiffInNatureDay(t, begin_time, &real_diff_time);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(it->second, real_diff_time);
    }
}/*}}}*/
