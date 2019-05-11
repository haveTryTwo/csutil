// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>

#include "base/ip.h"
#include "base/status.h"
#include "base/smart_ptr.h"

#include "test_base/include/test_base.h"

TEST(SmartPtr, Test_Normal_One_Ptr)
{/*{{{*/
    using namespace base;

    SmartPtr<std::string> smart_1(new std::string("aa"));
    fprintf(stderr, "smart_1 object:%s\n", smart_1->c_str());
}/*}}}*/

TEST(SmartPtr, Test_Normal_Two_Ptr)
{/*{{{*/
    using namespace base;

    SmartPtr<std::string> smart_1(new std::string("aa"));
    fprintf(stderr, "smart_1 object:%s\n", smart_1->c_str());

    SmartPtr<std::string> smart_2(new std::string("bb"));
    SmartPtr<std::string> smart_2_1 = smart_2;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());

    smart_2_1 = smart_1;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());
}/*}}}*/

TEST(SmartPtr, Test_Normal_Three_Ptr)
{/*{{{*/
    using namespace base;

    SmartPtr<std::string> smart_1(new std::string("aa"));
    fprintf(stderr, "smart_1 object:%s\n", smart_1->c_str());

    SmartPtr<std::string> smart_2(new std::string("bb"));
    SmartPtr<std::string> smart_2_1 = smart_2;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());

    smart_2_1 = smart_1;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());


    SmartPtr<std::string> smart_3(new std::string("cc"));
    smart_3 = smart_1;
    fprintf(stderr, "smart_3 object:%s\n", smart_3->c_str());
}/*}}}*/

TEST(SmartPtr, Test_Normal_Four_Ptr)
{/*{{{*/
    using namespace base;

    SmartPtr<std::string> smart_1(new std::string("aa"));
    fprintf(stderr, "smart_1 object:%s\n", smart_1->c_str());

    SmartPtr<std::string> smart_2(new std::string("bb"));
    SmartPtr<std::string> smart_2_1 = smart_2;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());

    smart_2_1 = smart_1;
    fprintf(stderr, "smart_2_1 object:%s\n", smart_2_1->c_str());


    SmartPtr<std::string> smart_3(new std::string("cc"));
    smart_3 = smart_1;
    fprintf(stderr, "smart_3 object:%s\n", smart_3->c_str());

    SmartPtr<std::string> smart_4;
    smart_4 = smart_1;
    EXPECT_EQ(*smart_1, *smart_4);
    fprintf(stderr, "smart_4 object:%s\n", smart_4->c_str());
}/*}}}*/

TEST(SmartPtr, Test_Normal_Same_Ptr)
{/*{{{*/
    using namespace base;

    SmartPtr<std::string> smart_1(new std::string("aa"));
    smart_1 = smart_1;
    fprintf(stderr, "smart_1 object:%s\n", smart_1->c_str());
}/*}}}*/

