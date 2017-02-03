// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
