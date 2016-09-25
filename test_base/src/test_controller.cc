// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/time.h>

#include "base/common.h"

#include "test_controller.h"

namespace test
{

TestController::TestController() :
    test_case_num_(0), test_num_(0),
    succ_test_num_(0), fail_test_num_(0)
{/*{{{*/
}/*}}}*/

TestController::~TestController()
{/*{{{*/
    ClearTestCases();
}/*}}}*/

void TestController::Run()
{/*{{{*/
    fprintf(stderr, "[==========] Run %d tests in %d test cases\n", test_num_, test_case_num_);
    struct timeval all_cases_begin;
    struct timeval all_cases_end;

    gettimeofday(&all_cases_begin, NULL);

    std::vector<std::pair<std::string, std::vector<Test*> > >::iterator it = test_cases_.begin();
    for (; it != test_cases_.end(); ++it)
    {/*{{{*/
        fprintf(stderr, "\n[----------] %zu tests for %s\n", it->second.size(), it->first.c_str());

        struct timeval test_case_begin;
        struct timeval test_case_end;
        gettimeofday(&test_case_begin, NULL);

        std::vector<Test*>::iterator test_it = it->second.begin();
        for (; test_it != it->second.end(); ++test_it)
        {/*{{{*/
            Test *real_test = *test_it;
            fprintf(stderr, "[ RUN      ] %s.%s\n", real_test->GetTestCaseName().c_str(), 
                    real_test->GetTestName().c_str());
            struct timeval test_begin;
            struct timeval test_end;
            gettimeofday(&test_begin, NULL);

            real_test->Begin();

            real_test->ExecBody();

            real_test->End();

            gettimeofday(&test_end, NULL);
            int64_t diff_time = (test_end.tv_sec-test_begin.tv_sec)*base::kUnitConvOfMicrosconds +
                            (test_end.tv_usec-test_begin.tv_usec);

            if (real_test->GetIsSucc())
            {
                fprintf(stderr, "[      OK  ] %s.%s (%lld s, %lld ms)\n",
                        real_test->GetTestCaseName().c_str(),
                        real_test->GetTestName().c_str(),
                        diff_time/base::kUnitConvOfMicrosconds, diff_time%base::kUnitConvOfMicrosconds);
                ++succ_test_num_;
            }
            else
            {
                fprintf(stderr, "[     FAIL ] %s.%s (%lld s, %lld ms)\n",
                        real_test->GetTestCaseName().c_str(),
                        real_test->GetTestName().c_str(),
                        diff_time/base::kUnitConvOfMicrosconds, diff_time%base::kUnitConvOfMicrosconds);

                ++fail_test_num_;

                std::string fail_name = real_test->GetTestCaseName() + "." + real_test->GetTestName();
                fail_tests_.push_back(fail_name);
            }
        }/*}}}*/

        gettimeofday(&test_case_end, NULL);
        int64_t diff_case_time = (test_case_end.tv_sec-test_case_begin.tv_sec)*base::kUnitConvOfMicrosconds +
                        (test_case_end.tv_usec-test_case_begin.tv_usec);

        fprintf(stderr, "[----------] %zu tests for %s (case total %lld s, %lld ms)\n\n", 
                it->second.size(), it->first.c_str(),
                diff_case_time/base::kUnitConvOfMicrosconds, diff_case_time%base::kUnitConvOfMicrosconds);
    }/*}}}*/

    gettimeofday(&all_cases_end, NULL);
    int64_t diff_all_time = (all_cases_end.tv_sec-all_cases_begin.tv_sec)*base::kUnitConvOfMicrosconds +
                        (all_cases_end.tv_usec-all_cases_begin.tv_usec);

    fprintf(stderr, "[==========] Run %d tests in %d test cases (total %lld s, %lld ms)\n", 
            test_num_, test_case_num_,
            diff_all_time/base::kUnitConvOfMicrosconds, diff_all_time%base::kUnitConvOfMicrosconds);

    fprintf(stderr, "[   PASS   ] %d tests\n", succ_test_num_);
    fprintf(stderr, "[   FAIL   ] %d tests, as follow:\n", fail_test_num_);

    std::vector<std::string>::iterator fail_it = fail_tests_.begin();
    for (; fail_it != fail_tests_.end(); ++fail_it)
    {
        fprintf(stderr, "[   FAIL   ] %s\n", fail_it->c_str());
    }
}/*}}}*/

void TestController::Register(Test *test)
{/*{{{*/
    std::vector<std::pair<std::string, std::vector<Test*> > >::iterator it = test_cases_.begin();
    for (; it != test_cases_.end(); ++it)
    {
        if (it->first == test->GetTestCaseName())
        {
            break;
        }
    }

    if (it != test_cases_.end())
    {
        it->second.push_back(test);
        ++test_num_;
    }
    else
    {
        test_cases_.push_back(std::pair<std::string, std::vector<Test*> >(test->GetTestCaseName(), 
                    std::vector<Test*>()));
        ++test_case_num_;

        std::vector<std::pair<std::string, std::vector<Test*> > >::reverse_iterator rit = 
            test_cases_.rbegin();
        rit->second.push_back(test);
        ++test_num_;
    }
}/*}}}*/

void TestController::ClearTestCases()
{/*{{{*/
    std::vector<std::pair<std::string, std::vector<Test*> > >::iterator it = test_cases_.begin();
    for (; it != test_cases_.end(); ++it)
    {
        std::vector<Test*>::iterator test_it = it->second.begin();
        for (; test_it != it->second.end(); ++test_it)
        {
            delete *test_it;
            *test_it = NULL;
        }
        it->second.clear();
    }
    test_cases_.clear();
}/*}}}*/

}

TEST(FILE_UTIL, CreateDir)
{
    EXPECT_EQ(10, 10);
}

TEST(FILE_UTIL, CreateFile)
{
    EXPECT_EQ(10, 11);
}

int main(int argc, char *argv[])
{
    strategy::Singleton<test::TestController>::instance()->Run();;

    return 0;
}
