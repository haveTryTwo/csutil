// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/time.h"

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

TEST(Time, GetDate_NormalDate)
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
