// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test_base/include/test_base.h"

TEST(FILE_UTIL, CreateDir)
{
    EXPECT_EQ(10, 10);
}

TEST(FILE_UTIL, CreateFile)
{
    EXPECT_EQ(10, 11);
    EXPECT_EQ(10, 11);
    EXPECT_EQ(10, 11);
}

class TimeUtil : public test::Test
{
public:
    void Init()
    {
        time_ = 100;
    }

    void Destroy()
    {
        time_ = 0;
    }

protected:
    int time_;
};

TEST_F(TimeUtil, TimeCheck)
{
    EXPECT_EQ(100, time_);
    time_ = 10;
    EXPECT_EQ(10, time_);
}
