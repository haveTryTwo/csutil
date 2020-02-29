// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "base/int.h"
#include "base/common.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

// TEST(GetInt64, ZeroData)
// {/*{{{*/
//     using namespace base;
// 
//     int64_t zero_data = 0;
// 
//     std::string str_data = "0";
//     int64_t data = 0;
//     base::Code ret = GetInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(zero_data, data);
// }/*}}}*/
// 
// TEST(GetInt64, SmallData)
// {/*{{{*/
//     using namespace base;
// 
//     int64_t small_data = 1234567;
// 
//     std::string str_data = "1234567";
//     int64_t data = 0;
//     base::Code ret = GetInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(small_data, data);
// }/*}}}*/
// 
// TEST(GetInt64, BigData)
// {/*{{{*/
//     using namespace base;
// 
// //    uint64_t big_data = 18446744073709551616;
//     int64_t big_data = 0xffffffffffffffff;
//     fprintf(stdout, "%#llx, %llu\n", (unsigned long long)big_data, (unsigned long long)big_data);
// 
//     std::string str_data = "18446744073709551615";
// //    str_data = "9223372036854775808";
//     int64_t data = 0;
//     base::Code ret = GetInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(big_data, data);
//     fprintf(stdout, "%#llx, %llu\n", (unsigned long long)data, (unsigned long long)data);
// }/*}}}*/
// 
// TEST(GetInt64, ExceptionOverflow)
// {/*{{{*/
//     using namespace base;
// 
//     std::string str_data = "2184467440737095516151";
//     int64_t data = 0;
//     base::Code ret = GetInt64(str_data, &data);
//     EXPECT_EQ(kStrtollFailed, ret);
// }/*}}}*/
// 
// TEST(GetUInt64, ZeroData)
// {/*{{{*/
//     using namespace base;
// 
//     uint64_t zero_data = 0;
// 
//     std::string str_data = "0";
//     uint64_t data = 0;
//     base::Code ret = GetUInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(zero_data, data);
// }/*}}}*/
// 
// TEST(GetUInt64, SmallData)
// {/*{{{*/
//     using namespace base;
// 
//     uint64_t small_data = 1234567;
// 
//     std::string str_data = "1234567";
//     uint64_t data = 0;
//     base::Code ret = GetUInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(small_data, data);
// }/*}}}*/
// 
// TEST(GetUInt64, BigData)
// {/*{{{*/
//     using namespace base;
// 
//     uint64_t big_data = 0xffffffffffffffff;
//     fprintf(stdout, "%#llx, %llu\n", (unsigned long long)big_data, (unsigned long long)big_data);
// 
//     std::string str_data = "18446744073709551615";
//     uint64_t data = 0;
//     base::Code ret = GetUInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(big_data, data);
// }/*}}}*/
// 
// TEST(GetUInt64, ExceptionOverflow)
// {/*{{{*/
//     using namespace base;
// 
//     std::string str_data = "2184467440737095516151";
//     uint64_t data = 0;
//     base::Code ret = GetUInt64(str_data, &data);
//     EXPECT_EQ(kStrtollFailed, ret);
// }/*}}}*/
// 
// TEST(GetUInt64, ExceptionRepeatedData)
// {/*{{{*/
//     using namespace base;
// 
//     std::string str_data = "0000000000000012";
//     uint64_t data = 12;
//     base::Code ret = GetUInt64(str_data, &data);
//     EXPECT_EQ(kOk, ret);
// }/*}}}*/
// 
// /**
//  * BigAdd
//  */
// TEST(BigAdd, Test_Normal_EqualLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341234";
//     std::string rn          = "1234123412341234123412341234";
//     std::string real_sum    = "2468246824682468246824682468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenthWithCarry_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9768123412341234123412374634";
//     std::string rn          = " 3959123412341234123412399834";
//     std::string real_sum    = "13727246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_LeftLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9968123412341234123412374634";
//     std::string rn          = "   59123412341234123412399834";
//     std::string real_sum    = "10027246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_RightLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59123412341234123412399834";
//     std::string rn          = " 9968123412341234123412374634";
//     std::string real_sum    = "10027246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_StartWithZero_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "001234123412341234123412341234";
//     std::string rn          = "0001234123412341234123412341234";
//     std::string real_sum    = "2468246824682468246824682468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_AllZero_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          = "00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341234";
//     std::string real_sum    = "-2468246824682468246824682468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenthWithCarry_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9768123412341234123412374634";
//     std::string rn          = " -3959123412341234123412399834";
//     std::string real_sum    = "-13727246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_LeftLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234123412374634";
//     std::string rn          = "   -59123412341234123412399834";
//     std::string real_sum    = "-10027246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_RightLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = " -9968123412341234123412374634";
//     std::string real_sum    = "-10027246824682468246824774468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_StartWithZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          = "-0001234123412341234123412341234";
//     std::string real_sum    = "-2468246824682468246824682468";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_AllZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          = "-00000";
//     std::string real_sum    = "-0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_LeftBigNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341238";
//     std::string rn          =  "1234123412341234123412341237";
//     std::string real_sum    = "-1";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_LeftLittleNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          =  "1234123412341234123412341237";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_LeftEqualNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          =  "1234123412341234123412341234";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_RightBigNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          =  "1234123412341234123412341238";
//     std::string rn          = "-1234123412341234123412341239";
//     std::string real_sum    = "-1";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_RightLittleNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          =  "1234123412341234123412341237";
//     std::string rn          = "-1234123412341234123412341234";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_EqualLenth_RightEqualNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          =  "1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341234";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_LeftLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234033313374634";
//     std::string rn          = "    59123412341234123412399834";
//     std::string real_sum    =  "-9908999999999999909900974800";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_LeftLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "  9968123412341234033313374634";
//     std::string rn          = "   -59123412341234123412399834";
//     std::string real_sum    =   "9908999999999999909900974800";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_RightLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = "  9968123412341234033313374634";
//     std::string real_sum    =   "9908999999999999909900974800";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_RightLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "    59123412341234123412399834";
//     std::string rn          = " -9968123412341234033313374634";
//     std::string real_sum    =  "-9908999999999999909900974800";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_StartWithZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          = "0001234123412341234123412341237";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_AllZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          = "00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Normal_AllZero_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          = "-00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigAdd, Test_Exception_Invalid_Num) 
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59a123412341234123412399834";
//     std::string rn          = " 9968a123412341234123412374634";
//     std::string result;
//     Code ret = BigAdd(ln, rn, &result);
//     EXPECT_EQ(kNotAllDigits, ret);
// }/*}}}*/
// 
// 
// TEST(BigAdd, Test_Press_Check_Small_PostNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (uint32_t i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%u", i);
//         std::string ln = buf;
//         for (uint32_t j = 0; j < 1000; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%u", j);
//             std::string rn = buf;
// 
//             uint64_t sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%llu", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (uint64_t i = 0x12345678; i < 0x12345789; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%llu", i);
//         std::string ln = buf;
//         for (uint64_t j = 0x98765432; j < 0x98765543; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%llu", j);
//             std::string rn = buf;
// 
//             uint64_t sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%llu", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigAdd, Test_Press_Check_Small_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i > -1000; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = -0x12345678; i > -0x12345789; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = -0x98765432; j > -0x98765543; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigAdd, Test_Press_Check_Small_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345678; i < 0x12345789; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = -0x98765432; j > -0x98765543; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigAdd, Test_Press_Check_Small_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string rn = buf;
//         for (int j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string ln = buf;
// 
//             int sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345678; i < 0x12345789; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string rn = buf;
//         for (int64_t j = -0x98765432; j > -0x98765543; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string ln = buf;
// 
//             int64_t sum = i + j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigAdd(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// 
// /**
//  * BigSub
//  */
// TEST(BigSub, Test_Normal_EqualLenth_RightNegtivetNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341234";
//     std::string rn          ="-1234123412341234123412341234";
//     std::string real_sum    = "2468246824682468246824682468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenthWithCarry_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9768123412341234123412374634";
//     std::string rn          = "-3959123412341234123412399834";
//     std::string real_sum    = "13727246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_LeftLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9968123412341234123412374634";
//     std::string rn          = "  -59123412341234123412399834";
//     std::string real_sum    = "10027246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_RightLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59123412341234123412399834";
//     std::string rn          = "-9968123412341234123412374634";
//     std::string real_sum    = "10027246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_StartWithZero_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "001234123412341234123412341234";
//     std::string rn          ="-0001234123412341234123412341234";
//     std::string real_sum    = "2468246824682468246824682468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_AllZero_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          ="-00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          =  "1234123412341234123412341234";
//     std::string real_sum    = "-2468246824682468246824682468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenthWithCarry_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9768123412341234123412374634";
//     std::string rn          = "  3959123412341234123412399834";
//     std::string real_sum    = "-13727246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_LeftLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234123412374634";
//     std::string rn          = "    59123412341234123412399834";
//     std::string real_sum    = "-10027246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_RightLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = "  9968123412341234123412374634";
//     std::string real_sum    = "-10027246824682468246824774468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_StartWithZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          = "0001234123412341234123412341234";
//     std::string real_sum    = "-2468246824682468246824682468";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_AllZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          = "00000";
//     std::string real_sum    = "-0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_LeftBigTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341238";
//     std::string rn          = "-1234123412341234123412341237";
//     std::string real_sum    = "-1";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_LeftLittleTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341237";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_LeftEqualTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341234";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_RightBigTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341238";
//     std::string rn          = "1234123412341234123412341239";
//     std::string real_sum    = "-1";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_RightLittleTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341237";
//     std::string rn          = "1234123412341234123412341234";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_EqualLenth_RightEqualTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341234";
//     std::string rn          = "1234123412341234123412341234";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_LeftLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234033313374634";
//     std::string rn          = "   -59123412341234123412399834";
//     std::string real_sum    =  "-9908999999999999909900974800";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_LeftLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9968123412341234033313374634";
//     std::string rn          = "   59123412341234123412399834";
//     std::string real_sum    =  "9908999999999999909900974800";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_RightLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = " -9968123412341234033313374634";
//     std::string real_sum    =   "9908999999999999909900974800";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_RightLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59123412341234123412399834";
//     std::string rn          = " 9968123412341234033313374634";
//     std::string real_sum    = "-9908999999999999909900974800";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_StartWithZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          ="-0001234123412341234123412341237";
//     std::string real_sum    = "3";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_AllZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          ="-00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Normal_AllZero_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          = "00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigSub, Test_Exception_Invalid_Num) 
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59a123412341234123412399834";
//     std::string rn          = " 9968a123412341234123412374634";
//     std::string result;
//     Code ret = BigSub(ln, rn, &result);
//     EXPECT_EQ(kNotAllDigits, ret);
// }/*}}}*/
// 
// TEST(BigSub, Test_Press_Check_Small_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int32_t i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int32_t j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int64_t sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345678; i < 0x12345789; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = -0x98765432; j > -0x98765543; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigSub, Test_Press_Check_Small_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i > -1000; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j < 1000; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = -0x12345678; i > -0x12345789; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = 0x98765432; j < 0x98765543; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigSub, Test_Press_Check_Small_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j < 1000; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345678; i < 0x12345789; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = 0x98765432; j < 0x98765543; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i - j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigSub, Test_Press_Check_Small_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i > -1000; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string rn = buf;
//         for (int j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string ln = buf;
// 
//             int sum = j - i;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = -0x12345678; i > -0x12345789; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string rn = buf;
//         for (int64_t j = -0x98765432; j > -0x98765543; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string ln = buf;
// 
//             int64_t sum = j - i;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigSub(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// /**
//  * BigMultiply
//  */
// TEST(BigMultiply, Test_Normal_EqualLenth_RightNegtivetNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341234";
//     std::string rn          ="-1234123412341234123412341234";
//     std::string real_sum    = "-1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
//     fprintf(stderr, "real_sum:%s, result:%s\n", real_sum.c_str(), result.c_str());
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_LeftLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9968123412341234123412374634";
//     std::string rn          = "  -59123412341234123412399834";
//     std::string real_sum    = "-589349470776160524343849514765665137430834794607410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_RightLongLenth_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59123412341234123412399834";
//     std::string rn          = "-9968123412341234123412374634";
//     std::string real_sum    = "-589349470776160524343849514765665137430834794607410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_StartWithZero_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "0001234123412341234123412341234";
//     std::string rn          ="-0001234123412341234123412341234";
//     std::string real_sum    = "-1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_AllZero_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          ="-00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          =  "1234123412341234123412341234";
//     std::string real_sum    = "-1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_LeftLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234123412374634";
//     std::string rn          = "    59123412341234123412399834";
//     std::string real_sum    = "-589349470776160524343849514765665137430834794607410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_RightLongLenth_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = "  9968123412341234123412374634";
//     std::string real_sum    = "-589349470776160524343849514765665137430834794607410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_StartWithZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          = "0001234123412341234123412341234";
//     std::string real_sum    = "-1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_AllZero_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          = "00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_LeftBigTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341238";
//     std::string rn          = "-1234123412341234123412341237";
//     std::string real_sum    = "1523060596888771785469376028844593630280119721143031406";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_LeftLittleTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341237";
//     std::string real_sum    = "1523060596888771785469376023908099980915183227493666458";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_LeftEqualTwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-1234123412341234123412341234";
//     std::string rn          = "-1234123412341234123412341234";
//     std::string real_sum    = "1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_RightBigTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341238";
//     std::string rn          = "1234123412341234123412341239";
//     std::string real_sum    = "1523060596888771785469376031312840454962587967967713882";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_RightLittleTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341237";
//     std::string rn          = "1234123412341234123412341234";
//     std::string real_sum    = "1523060596888771785469376023908099980915183227493666458";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_EqualLenth_RightEqualTwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "1234123412341234123412341234";
//     std::string rn          = "1234123412341234123412341234";
//     std::string real_sum    = "1523060596888771785469376020205729743891480857256642756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_LeftLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " -9968123412341234033313374634";
//     std::string rn          = "   -59123412341234123412399834";
//     std::string real_sum    =  "589349470776160519016889186232811852097022151041410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_LeftLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = " 9968123412341234033313374634";
//     std::string rn          = "   59123412341234123412399834";
//     std::string real_sum    =  "589349470776160519016889186232811852097022151041410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_RightLongLenth_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   -59123412341234123412399834";
//     std::string rn          = " -9968123412341234033313374634";
//     std::string real_sum    =  "589349470776160519016889186232811852097022151041410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_RightLongLenth_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59123412341234123412399834";
//     std::string rn          = " 9968123412341234033313374634";
//     std::string real_sum    =  "589349470776160519016889186232811852097022151041410756";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_StartWithZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-001234123412341234123412341234";
//     std::string rn          ="-0001234123412341234123412341237";
//     std::string real_sum    = "1523060596888771785469376023908099980915183227493666458";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_AllZero_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "-00";
//     std::string rn          ="-00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_AllZero_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "00";
//     std::string rn          = "00000";
//     std::string real_sum    = "0";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Exception_Invalid_Num) 
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "   59a123412341234123412399834";
//     std::string rn          = " 9968a123412341234123412374634";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kNotAllDigits, ret);
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Press_Check_Small_RightNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int32_t i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int32_t j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int64_t sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//             if (real_sum != result)
//                 fprintf(stderr, "%s:%s\n", real_sum.c_str(), result.c_str());
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345700; i < 0x12345723; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = -0x98765500; j > -0x98765523; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Press_Check_Small_LeftNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i > -1000; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j < 1000; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = -0x12345700; i > -0x12345723; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = 0x98765500; j < 0x98765523; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Press_Check_Small_TwoPostNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i < 1000; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string ln = buf;
//         for (int j = 0; j < 1000; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string rn = buf;
// 
//             int sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = 0x12345700; i < 0x12345723; ++i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string ln = buf;
//         for (int64_t j = 0x98765500; j < 0x98765523; ++j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string rn = buf;
// 
//             int64_t sum = i * j;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Press_Check_Small_TwoNegtiveNum)
// {/*{{{*/
//     using namespace base;
// 
//     char buf[kSmallBufLen] = {0};
//     for (int i = 0; i > -1000; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%d", i);
//         std::string rn = buf;
//         for (int j = 0; j > -1000; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%d", j);
//             std::string ln = buf;
// 
//             int sum = j * i;
//             snprintf(buf, sizeof(buf)-1, "%d", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// 
//     for (int64_t i = -0x12345700; i > -0x12345723; --i)
//     {/*{{{*/
//         snprintf(buf, sizeof(buf)-1, "%lld", i);
//         std::string rn = buf;
//         for (int64_t j = -0x98765500; j > -0x98765523; --j)
//         {
//             snprintf(buf, sizeof(buf)-1, "%lld", j);
//             std::string ln = buf;
// 
//             int64_t sum = j * i;
//             snprintf(buf, sizeof(buf)-1, "%lld", sum);
//             std::string real_sum = buf;
// 
//             std::string result;
//             Code ret = BigMultiply(ln, rn, &result);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(real_sum, result);
//         }
//     }/*}}}*/
// }/*}}}*/
// 
// TEST(BigMultiply, Test_Normal_AllNice)
// {/*{{{*/
//     using namespace base;
// 
//     std::string ln          = "9999999999999999999999999999999";
//     std::string rn          ="98765432199999999999999999999999";
//     std::string real_sum    = "987654321999999999999999999999891234567800000000000000000000001";
//     std::string result;
//     Code ret = BigMultiply(ln, rn, &result);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(real_sum, result);
// }/*}}}*/
// 
// 
// // 42 = (-80538738812075974)³ + 80435758145817515³ + 12602123297335631³
// TEST(BigMultiply, Test_Normal_42)
// {/*{{{*/
//     using namespace base;
// 
//     std::string big_num1 = "-80538738812075974";
//     std::string big_num2 = "80435758145817515";
//     std::string big_num3 = "12602123297335631";
// 
//     std::string tmp_num;
//     std::string last_result;
//     std::string real_sum = "42";
// 
//     std::string first_result;
//     Code ret = BigMultiply(big_num1, big_num1, &tmp_num);
//     EXPECT_EQ(kOk, ret);
//     ret = BigMultiply(big_num1, tmp_num, &first_result);
//     EXPECT_EQ(kOk, ret);
// 
//     std::string second_result;
//     ret = BigMultiply(big_num2, big_num2, &tmp_num);
//     EXPECT_EQ(kOk, ret);
//     ret = BigMultiply(big_num2, tmp_num, &second_result);
//     EXPECT_EQ(kOk, ret);
// 
//     std::string third_result;
//     ret = BigMultiply(big_num3, big_num3, &tmp_num);
//     EXPECT_EQ(kOk, ret);
//     ret = BigMultiply(big_num3, tmp_num, &third_result);
//     EXPECT_EQ(kOk, ret);
// 
//     ret = BigAdd(first_result, second_result, &tmp_num);
//     EXPECT_EQ(kOk, ret);
//     ret = BigAdd(third_result, tmp_num, &last_result);
//     EXPECT_EQ(kOk, ret);
// 
//     EXPECT_EQ(real_sum, last_result);
//     fprintf(stderr, "real_sum:%s, last_result:%s\n", real_sum.c_str(), last_result.c_str());
// }/*}}}*/
// 
/**
 * GCD: greast common divisor
 */
TEST(GCD, NormalCommDivisorOne) 
{/*{{{*/
    using namespace base;

    uint64_t first = 10;
    uint64_t second = 3;
    uint64_t max_comm_divisor = 1;
    uint64_t tmp_comm_divisor = 0;
    Code ret = GCD(first, second, &tmp_comm_divisor);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(max_comm_divisor, tmp_comm_divisor);
}/*}}}*/

TEST(GCD, NormalCommDivisorTwo) 
{/*{{{*/
    using namespace base;

    uint64_t first = 10;
    uint64_t second = 8;
    uint64_t max_comm_divisor = 2;
    uint64_t tmp_comm_divisor = 0;
    Code ret = GCD(first, second, &tmp_comm_divisor);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(max_comm_divisor, tmp_comm_divisor);
}/*}}}*/

TEST(GCD, NormalCommDivisorThree) 
{/*{{{*/
    using namespace base;

    uint64_t first_arr[] =    {12, 16, 30, 50, 80, 100, 120};
    uint64_t second_arr[] =   {8,  20, 27, 40, 100, 80, 119};
    uint64_t max_comm_arr[] = {4,  4,  3,  10, 20, 20,  1};
    for (size_t i = 0; i < sizeof(first_arr)/sizeof(first_arr[0]); ++i)
    {
        uint64_t tmp_comm_divisor = 0;
        Code ret = GCD(first_arr[i], second_arr[i], &tmp_comm_divisor);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(max_comm_arr[i], tmp_comm_divisor);
    }
}/*}}}*/

TEST(GCD, BigCommDivisorOne) 
{/*{{{*/
    using namespace base;

    uint64_t first = 10000;
    uint64_t second = 1251;
    uint64_t max_comm_divisor = 1;
    uint64_t tmp_comm_divisor = 0;
    Code ret = GCD(first, second, &tmp_comm_divisor);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(max_comm_divisor, tmp_comm_divisor);
}/*}}}*/

TEST(GCD, ExceptionCommDivisorOne) 
{/*{{{*/
    using namespace base;

    uint64_t first = 10000;
    uint64_t second = 0;
    uint64_t max_comm_divisor = 1;
    uint64_t tmp_comm_divisor = 0;
    Code ret = GCD(first, second, &tmp_comm_divisor);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(max_comm_divisor, tmp_comm_divisor);
}/*}}}*/

/**
 * LCM: least common multiple
 */
TEST(LCM, NormalCommMulOne) 
{/*{{{*/
    using namespace base;

    uint32_t first = 10;
    uint32_t second = 3;
    uint32_t min_comm_mul = 30;
    uint64_t tmp_comm_mul = 0;
    Code ret = LCM(first, second, &tmp_comm_mul);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(min_comm_mul, tmp_comm_mul);
}/*}}}*/

TEST(LCM, NormalCommMulTwo) 
{/*{{{*/
    using namespace base;

    uint32_t first = 10;
    uint32_t second = 8;
    uint32_t min_comm_mul = 40;
    uint64_t tmp_comm_mul = 0;
    Code ret = LCM(first, second, &tmp_comm_mul);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(min_comm_mul, tmp_comm_mul);
}/*}}}*/

TEST(LCM, NormalCommMulThree) 
{/*{{{*/
    using namespace base;

    uint64_t first_arr[] =    {12, 16, 30, 50, 80, 100, 120};
    uint64_t second_arr[] =   {8,  20, 27, 40, 100, 80, 119};
    uint64_t min_comm_arr[] = {24, 80, 270, 200, 400, 400, 14280};
    for (size_t i = 0; i < sizeof(first_arr)/sizeof(first_arr[0]); ++i)
    {
        uint64_t tmp_comm_mul = 0;
        Code ret = LCM(first_arr[i], second_arr[i], &tmp_comm_mul);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(min_comm_arr[i], tmp_comm_mul);
    }
}/*}}}*/

TEST(LCM, BigCommMulOne) 
{/*{{{*/
    using namespace base;

    uint32_t first = 10000;
    uint32_t second = 1251;
    uint32_t min_comm_mul = 12510000;
    uint64_t tmp_comm_mul = 0;
    Code ret = LCM(first, second, &tmp_comm_mul);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(min_comm_mul, tmp_comm_mul);
}/*}}}*/

TEST(LCM, ExceptionCommMulOne) 
{/*{{{*/
    using namespace base;

    uint32_t first = 10000;
    uint32_t second = 0;
    uint32_t min_comm_mul = 0;
    uint64_t tmp_comm_mul = 0;
    Code ret = LCM(first, second, &tmp_comm_mul);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(min_comm_mul, tmp_comm_mul);
}/*}}}*/

TEST(ReverseBits, NormalDataOne) 
{/*{{{*/
    using namespace base;

    uint64_t source = 0xffeeddccbbaa9988;
    uint64_t dest = 0x119955dd33bb77ff;
    uint64_t tmp_dest = 0;
    Code ret = ReverseBits(source, &tmp_dest);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dest, tmp_dest);
    fprintf(stderr, "tmp_dest:%llx\n", (unsigned long long)tmp_dest);
}/*}}}*/

TEST(MaxContinuousSum, Test_Normal_Int_Arr) 
{/*{{{*/
    using namespace base;

    int64_t arr[] = {-1, -2, 1, 2, 3, -100, 1, 3, 8, -1, -3, -7, 10, 9, -20};
    int64_t expect_dst = 20;
    int64_t dst = 0;
    Code ret = MaxContinuousSum(arr, sizeof(arr)/sizeof(arr[0]), &dst);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(expect_dst, dst);
    fprintf(stderr, "dest:%lld\n", (long long)dst);
}/*}}}*/

TEST(MaxContinuousSum, Test_Normal_All_Negtive_Num) 
{/*{{{*/
    using namespace base;

    int64_t arr[] = {-31, -21, -7, -9, -3, -100, -3, -3, -8, -2, -3, -7, -10, -9, -20};
    int64_t expect_dst = -2;
    int64_t dst = 0;
    Code ret = MaxContinuousSum(arr, sizeof(arr)/sizeof(arr[0]), &dst);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(expect_dst, dst);
    fprintf(stderr, "dest:%lld\n", (long long)dst);
}/*}}}*/

TEST(MaxContinuousSum, Test_Normal_Empty_Arr) 
{/*{{{*/
    using namespace base;

    int64_t arr[] = {};
    int64_t expect_dst = LLONG_MIN;
    int64_t dst = 0;
    Code ret = MaxContinuousSum(arr, sizeof(arr)/sizeof(arr[0]), &dst);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(expect_dst, dst);
    fprintf(stderr, "dest:%lld\n", (long long)dst);
}/*}}}*/
