// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include "test_base/include/test_base.h"

namespace test
{

//Test::Test(const std::string &test_case_name, const std::string &test_name) :
Test::Test() : test_case_name_(""), test_name_(""), is_succ_(true)
{/*{{{*/
}/*}}}*/

Test::~Test()
{/*{{{*/
}/*}}}*/

void Test::InitTest(const std::string &test_case_name, const std::string &test_name)
{/*{{{*/
    test_case_name_ = test_case_name;
    test_name_ = test_name;
}/*}}}*/

void Test::Begin()
{/*{{{*/
    Init();
}/*}}}*/

void Test::End()
{/*{{{*/
    Destroy();
}/*}}}*/

void Test::Init()
{/*{{{*/
}/*}}}*/

void Test::Destroy()
{/*{{{*/
}/*}}}*/

void Test::ExecBody()
{/*{{{*/
}/*}}}*/

const std::string& Test::GetTestCaseName()
{/*{{{*/
    return test_case_name_;
}/*}}}*/

void Test::SetTestCaseName(const std::string &test_case_name)
{/*{{{*/
    test_case_name_ = test_case_name;
}/*}}}*/

const std::string& Test::GetTestName()
{/*{{{*/
    return test_name_;
}/*}}}*/

void Test::SetTestName(const std::string &test_name)
{/*{{{*/
    test_name_ = test_name;
}/*}}}*/

bool Test::GetIsSucc()
{/*{{{*/
    return is_succ_;
}/*}}}*/

void Test::SetIsSucc(bool is_succ)
{/*{{{*/
    is_succ_ = is_succ;
}/*}}}*/

}

test::Test* MakeRegister(const std::string &test_case_name, const std::string &test_name, test::Test *test_obj)
{/*{{{*/
    test_obj->InitTest(test_case_name, test_name);

    strategy::Singleton<test::TestController>::Instance()->Register(test_obj);

    return test_obj;
}/*}}}*/
