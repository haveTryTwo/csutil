// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_BASE_H_
#define TEST_BASE_TEST_BASE_H_

#include "base/time.h"
#include "strategy/singleton.h"

#include "test_base/include/test_controller.h"

namespace test
{

class TestController;

class Test
{
    public:
        Test();
        virtual ~Test();

    public:
        void InitTest(const std::string &test_case_name, const std::string &test_name);

    public:
        void Begin();
        void End();
        
        virtual void Init();
        virtual void Destroy();

        virtual void ExecBody();

    public:
        const std::string& GetTestCaseName();
        void SetTestCaseName(const std::string &test_case_name);

        const std::string& GetTestName();
        void SetTestName(const std::string &test_name);

        bool GetIsSucc();
        void SetIsSucc(bool is_succ);

    private:
        std::string test_case_name_;
        std::string test_name_;

    private:
        bool is_succ_;
};

}

#define TEST_CLASS_NAME_(test_case_name_, test_name)\
    test_case_name##_##test_name

#define TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name)\
    test_case_name##_##test_name##_test_obj

test::Test* MakeRegister(const std::string &test_case_name, const std::string &test_name, test::Test *test_obj);

#define TEST_INTERNAL_(test_case_name, test_name, father_class) \
    class TEST_CLASS_NAME_(test_case_name, test_name) : public father_class\
    {\
    public:\
        TEST_CLASS_NAME_(test_case_name, test_name)() : father_class() {} \
        \
    public:\
           virtual void ExecBody();\
    };\
    test::Test *TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name) = \
    MakeRegister(#test_case_name, #test_name,\
            new TEST_CLASS_NAME_(test_case_name, test_name)());\
    void TEST_CLASS_NAME_(test_case_name, test_name)::ExecBody()\


#define TEST(test_case_name, test_name) \
        TEST_INTERNAL_(test_case_name, test_name, test::Test)

#define TEST_F(test_case_name, test_name) \
        TEST_INTERNAL_(test_case_name, test_name, test_case_name)

#define EXPECT_EQ(expect_val, real_val)\
    if (expect_val != real_val)\
    {\
        SetIsSucc(false);\
        fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__);\
    }\

#endif
