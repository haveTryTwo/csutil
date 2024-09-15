// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Base64Encode, Test_Normal_Str) { /*{{{*/
  using namespace base;

  std::map<std::string, std::string> maps;

  maps[""] = "";
  maps[" "] = "IA==";
  maps["  "] = "ICA=";
  maps["abc"] = "YWJj";
  maps["abcd"] = "YWJjZA==";
  maps["abcde"] = "YWJjZGU=";
  maps["abcdef"] = "YWJjZGVm";
  maps["azAZ09+/"] = "YXpBWjA5Ky8=";
  maps["   "] = "ICAg";
  maps["abcdefEG m%+@@#!%#% ABVC"] = "YWJjZGVmRUcgbSUrQEAjISUjJSBBQlZD";
  maps["nice you and here and there, good meet you!"] = "bmljZSB5b3UgYW5kIGhlcmUgYW5kIHRoZXJlLCBnb29kIG1lZXQgeW91IQ==";
  maps["ab12340012392345009123"] = "YWIxMjM0MDAxMjM5MjM0NTAwOTEyMw==";

  std::string tmp_src;
  std::string tmp_dst;
  std::map<std::string, std::string>::iterator it;
  for (it = maps.begin(); it != maps.end(); ++it) {
    Code ret = Base64Encode(it->first, &tmp_dst);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(it->second, tmp_dst);
    fprintf(stderr, "value[%s:%s]\n", it->second.c_str(), tmp_dst.c_str());

    ret = Base64Decode(tmp_dst, &tmp_src);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(it->first, tmp_src);
    fprintf(stderr, "key[%s:%s]\n", it->first.c_str(), tmp_src.c_str());
  }

} /*}}}*/

TEST(Base64Encode, Test_Normal_Full_Char) { /*{{{*/
  using namespace base;

  std::string src;
  for (uint32_t i = 0; i < 256; ++i) {
    src.append(1, i);
  }

  std::string tmp_src;
  std::string tmp_dst;

  Code ret = Base64Encode(src, &tmp_dst);
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "%s\n", tmp_dst.c_str());

  ret = Base64Decode(tmp_dst, &tmp_src);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(src, tmp_src);
} /*}}}*/

TEST(Base64Decode, Test_Exception_Wrong_Str) { /*{{{*/
  using namespace base;

  std::string encode_str;
  for (uint32_t i = 211; i < 256; ++i) {
    encode_str.append(1, i);
  }

  std::string tmp_src;
  Code ret = Base64Decode(encode_str, &tmp_src);
  EXPECT_EQ(kInvalidParam, ret);

  encode_str = "abce=";
  ret = Base64Decode(encode_str, &tmp_src);
  EXPECT_EQ(kInvalidParam, ret);

  encode_str = "ab=e";
  ret = Base64Decode(encode_str, &tmp_src);
  EXPECT_EQ(kInvalidParam, ret);

  encode_str = "a=ce";
  ret = Base64Decode(encode_str, &tmp_src);
  EXPECT_EQ(kInvalidParam, ret);

  encode_str = "=ace";
  ret = Base64Decode(encode_str, &tmp_src);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

// TEST(Base64Encode, Test_Press_Encode_10_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 10;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Encode(default_source_data, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//     }
//
//     fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     ret = Base64Decode(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Encode_100_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 100;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Encode(default_source_data, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//     }
//
// //    fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     ret = Base64Decode(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Encode_1000_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 1000;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Encode(default_source_data, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//     }
//
//  //   fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     ret = Base64Decode(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Decode_10_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 10;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     ret = Base64Encode(default_source_data, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Decode(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//     }
//
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Decode_100_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 100;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     ret = Base64Encode(default_source_data, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
// //    fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Decode(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//     }
//
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Decode_1000_Len_Ten_Thousand)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     uint32_t source_data_len = 1000;
//     std::string default_source_data;
//     Code ret = GetRandBinStr(source_data_len, &default_source_data);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(source_data_len, default_source_data.size());
//
//     std::string tmp_dst;
//
//     ret = Base64Encode(default_source_data, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
// //    fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     std::string tmp_src;
//     for (int i = 0; i < 10000; ++i)
//     {
//         ret = Base64Decode(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//     }
//
//     EXPECT_EQ(default_source_data, tmp_src);
// }/*}}}*/
//
// TEST(Base64Encode, Test_Press_Encode_4_Len_Check)
// {/*{{{*/
//     using namespace base;
//
//     // source data
//     std::string tmp_dst;
//     uint32_t start_num = 0;
//
//     for (int64_t i = start_num; i < start_num+1000000000; ++i)
//     {
//         std::string default_source_data(reinterpret_cast<char*>(&i), 4);
//         Code ret = Base64Encode(default_source_data, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(8, tmp_dst.size());
//         EXPECT_EQ('=', tmp_dst.data()[6]);
//         EXPECT_EQ('=', tmp_dst.data()[7]);
//     }
//
//     fprintf(stderr, "%s\n", tmp_dst.c_str());
// }/*}}}*/
//

// #if (BYTE_ORDER == LITTLE_ENDIAN)
// TEST(EncodeFixed32, Test_Normal_UInt)
// {/*{{{*/
//     using namespace base;
//
//     uint32_t src = 0x11223344;
//     std::string dst;
//     dst.append(1, 0x44).append(1, 0x33).append(1, 0x22).append(1, 0x11);
//
//     // EncodeFixed32
//     std::string tmp_dst;
//     Code ret = EncodeFixed32(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst.size(), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst.data(), tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeFixed32
//     uint32_t tmp_src = 0;
//     ret = DecodeFixed32(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#x\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Normal_UInts)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint32_t, std::string> var_map;
//     var_map[0x7f].append(1, 0x7f).append(1, 0x0).append(1, 0x0).append(1, 0x0);
//     var_map[0x3fff].append(1, 0xff).append(1, 0x3f).append(1, 0x0).append(1, 0x0);
//     var_map[0x1fffff].append(1, 0xff).append(1, 0xff).append(1, 0x1f).append(1, 0x0);
//     var_map[0xfffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0xf);
//     var_map[0xffffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0xff);
//
//     std::map<uint32_t, std::string>::iterator it = var_map.begin();
//     for (it = var_map.begin(); it != var_map.end(); ++it)
//     {
//         // EncodeFixed32
//         std::string tmp_dst;
//         Code ret = EncodeFixed32(it->first, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->second.size(), tmp_dst.size());
//         EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//         // DecodeFixed32
//         uint32_t tmp_src = 0;
//         ret = DecodeFixed32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->first, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Loop_First_UInts)
// {/*{{{*/
//     using namespace base;
//
//     for (uint32_t i = 0; i < 10000000; ++i)
//     {
//         // EncodeFixed32
//         std::string tmp_dst;
//         Code ret = EncodeFixed32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeFixed32
//         uint32_t tmp_src = 0;
//         ret = DecodeFixed32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Loop_Last_UInts)
// {/*{{{*/
//     using namespace base;
//
//     for (uint32_t i = UINT_MAX; i > UINT_MAX-10000000; --i)
//     {
//         // EncodeFixed32
//         std::string tmp_dst;
//         Code ret = EncodeFixed32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeFixed32
//         uint32_t tmp_src = 0;
//         ret = DecodeFixed32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Press_OneMillion_UInt)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint32_t, std::string> var_map;
//     var_map[0xfffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0xf);
//
//     for (int i = 0; i < 1000000; ++i)
//     {
//         std::map<uint32_t, std::string>::iterator it = var_map.begin();
//         for (it = var_map.begin(); it != var_map.end(); ++it)
//         {
//             // EncodeFixed32
//             std::string tmp_dst;
//             Code ret = EncodeFixed32(it->first, &tmp_dst);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->second.size(), tmp_dst.size());
//             EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//             // DecodeFixed32
//             uint32_t tmp_src = 0;
//             ret = DecodeFixed32(tmp_dst, &tmp_src);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->first, tmp_src);
//         }
//     }
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Exception_MoreLength)
// {/*{{{*/
//     using namespace base;
//
//     uint32_t src = 0x11223344;
//     std::string dst;
//     dst.append(1, 0x44).append(1, 0x33).append(1, 0x22).append(1, 0x11);
//
//     // EncodeFixed32
//     std::string tmp_dst;
//     Code ret = EncodeFixed32(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst.size(), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst.data(), tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeFixed32
//     uint32_t tmp_src = 0;
//     tmp_dst.append(1, 0x11).append(1, 0x22).append(1, 0xcc);
//     ret = DecodeFixed32(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#x\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeFixed32, Test_Exception_UInt)
// {/*{{{*/
//     using namespace base;
//
//     // DecodeFixed32
//     std::string tmp_dst;
//     tmp_dst.append(1, 0x01).append(1, 0x02);
//     uint32_t tmp_src = 0;
//     Code ret = DecodeFixed32(tmp_dst, &tmp_src);
//     EXPECT_NEQ(kOk, ret);
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Normal_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     uint64_t src = 0x1122334455667788;
//     std::string dst;
//     dst.append(1, 0x88).append(1, 0x77).append(1, 0x66).append(1, 0x55).append(1, 0x44).append(1,
//     0x33).append(1, 0x22).append(1, 0x11);
//
//     // EncodeFixed64
//     std::string tmp_dst;
//     Code ret = EncodeFixed64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst.size(), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst.data(), tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeFixed64
//     uint64_t tmp_src = 0;
//     ret = DecodeFixed64(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Normal_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint64_t, std::string> var_map;
//     var_map[0x7f].append(1, 0x7f).append(1, 0x0).append(1, 0x0).append(1,
//     0x0).append(1,0x0).append(1,0x0).append(1,0x0).append(1,0x0); var_map[0x3fff].append(1,
//     0xff).append(1, 0x3f).append(1, 0x0).append(1,
//     0x0).append(1,0x0).append(1,0x0).append(1,0x0).append(1,0x0); var_map[0x1fffff].append(1,
//     0xff).append(1, 0xff).append(1, 0x1f).append(1,
//     0x0).append(1,0x0).append(1,0x0).append(1,0x0).append(1,0x0); var_map[0xfffffff].append(1,
//     0xff).append(1, 0xff).append(1, 0xff).append(1,
//     0xf).append(1,0x0).append(1,0x0).append(1,0x0).append(1,0x0); var_map[0xffffffff].append(1,
//     0xff).append(1, 0xff).append(1, 0xff).append(1,
//     0xff).append(1,0x0).append(1,0x0).append(1,0x0).append(1,0x0);
//     var_map[0xffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x0).append(1,0x0).append(1,0x0);
//     var_map[0x3ffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x3).append(1,0x0).append(1,0x0);
//     var_map[0x1ffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x1).append(1,0x0);
//     var_map[0xffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x0);
//     var_map[0x7fffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//     var_map[0xffffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff);
//
//
//     std::map<uint64_t, std::string>::iterator it = var_map.begin();
//     for (it = var_map.begin(); it != var_map.end(); ++it)
//     {
//         // EncodeFixed64
//         std::string tmp_dst;
//         Code ret = EncodeFixed64(it->first, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->second.size(), tmp_dst.size());
//         EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//         // DecodeFixed64
//         uint64_t tmp_src = 0;
//         ret = DecodeFixed64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->first, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Loop_First_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     for (uint64_t i = 0; i < 10000000; ++i)
//     {
//         // EncodeFixed64
//         std::string tmp_dst;
//         Code ret = EncodeFixed64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeFixed64
//         uint64_t tmp_src = 0;
//         ret = DecodeFixed64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Loop_Last_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     for (uint64_t i = UINT64_MAX; i > UINT64_MAX-10000000; --i)
//     {
//         // EncodeFixed64
//         std::string tmp_dst;
//         Code ret = EncodeFixed64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeFixed64
//         uint64_t tmp_src = 0;
//         ret = DecodeFixed64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Press_OneMillion_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint64_t, std::string> var_map;
//     var_map[0xffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,
//     0x0);
//
//     for (int i = 0; i < 1000000; ++i)
//     {
//         std::map<uint64_t, std::string>::iterator it = var_map.begin();
//         for (it = var_map.begin(); it != var_map.end(); ++it)
//         {
//             // EncodeFixed64
//             std::string tmp_dst;
//             Code ret = EncodeFixed64(it->first, &tmp_dst);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->second.size(), tmp_dst.size());
//             EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//             // DecodeFixed64
//             uint64_t tmp_src = 0;
//             ret = DecodeFixed64(tmp_dst, &tmp_src);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->first, tmp_src);
//         }
//     }
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Exception_MoreLength)
// {/*{{{*/
//     using namespace base;
//
//     uint64_t src = 0x1122334455667788;
//     std::string dst;
//     dst.append(1, 0x88).append(1, 0x77).append(1, 0x66).append(1, 0x55).append(1, 0x44).append(1,
//     0x33).append(1, 0x22).append(1, 0x11);
//
//     // EncodeFixed64
//     std::string tmp_dst;
//     Code ret = EncodeFixed64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst.size(), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst.data(), tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeFixed64
//     uint64_t tmp_src = 0;
//     tmp_dst.append(1, 0x11).append(1, 0x22);
//     ret = DecodeFixed64(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(EncodeFixed64, Test_Exception_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     // DecodeFixed64
//     std::string tmp_dst;
//     tmp_dst.append(1, 0x01).append(1, 0x02).append(1, 0x03).append(1, 0x04).append(1, 0x05);
//     uint64_t tmp_src = 0;
//     Code ret = DecodeFixed64(tmp_dst, &tmp_src);
//     EXPECT_NEQ(kOk, ret);
// }/*}}}*/
// #endif
//
// TEST(EncodeVar32, Test_Normal_UInt)
// {/*{{{*/
//     using namespace base;
//
//     uint32_t src = 0x11223344;
//     const char dst[5] = {0xC4, 0xE6, 0x88, 0x89, 0x01};
//
//     // EncodeVar32
//     std::string tmp_dst;
//     Code ret = EncodeVar32(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(sizeof(dst)/sizeof(dst[0]), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst, tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeVar32
//     uint32_t tmp_src = 0;
//     ret = DecodeVar32(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#x\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Normal_UInts)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint32_t, std::string> var_map;
//     var_map[0x7f].append(1, 0x7f);
//     var_map[0x3fff].append(1, 0xff).append(1, 0x7f);
//     var_map[0x1fffff].append(1, 0xff).append(1, 0xff).append(1, 0x7f);
//     var_map[0xfffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0x7f);
//     var_map[0xffffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1,
//     0xf);
//
//     std::map<uint32_t, std::string>::iterator it = var_map.begin();
//     for (it = var_map.begin(); it != var_map.end(); ++it)
//     {
//         // EncodeVar32
//         std::string tmp_dst;
//         Code ret = EncodeVar32(it->first, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->second.size(), tmp_dst.size());
//         EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//         // DecodeVar32
//         uint32_t tmp_src = 0;
//         ret = DecodeVar32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->first, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Loop_First_UInts)
// {/*{{{*/
//     using namespace base;
//
//     for (uint32_t i = 0; i < 10000000; ++i)
//     {
//         // EncodeVar32
//         std::string tmp_dst;
//         Code ret = EncodeVar32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeVar32
//         uint32_t tmp_src = 0;
//         ret = DecodeVar32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Loop_Last_UInts)
// {/*{{{*/
//     using namespace base;
//
//     for (uint32_t i = UINT_MAX; i > UINT_MAX-10000000; --i)
//     {
//         // EncodeVar32
//         std::string tmp_dst;
//         Code ret = EncodeVar32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeVar32
//         uint32_t tmp_src = 0;
//         ret = DecodeVar32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Press_OneMillion_UInt)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint32_t, std::string> var_map;
//     var_map[0xfffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0x7f);
//
//     for (int i = 0; i < 1000000; ++i)
//     {
//         std::map<uint32_t, std::string>::iterator it = var_map.begin();
//         for (it = var_map.begin(); it != var_map.end(); ++it)
//         {
//             // EncodeVar32
//             std::string tmp_dst;
//             Code ret = EncodeVar32(it->first, &tmp_dst);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->second.size(), tmp_dst.size());
//             EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//             // DecodeVar32
//             uint32_t tmp_src = 0;
//             ret = DecodeVar32(tmp_dst, &tmp_src);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->first, tmp_src);
//         }
//     }
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Exception_MoreLength)
// {/*{{{*/
//     using namespace base;
//
//     uint32_t src = 0x11223344;
//     const char dst[5] = {0xC4, 0xE6, 0x88, 0x89, 0x01};
//
//     // EncodeVar32
//     std::string tmp_dst;
//     Code ret = EncodeVar32(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(sizeof(dst)/sizeof(dst[0]), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst, tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeVar32
//     uint32_t tmp_src = 0;
//     tmp_dst.append(1, 0xcc).append(1, 0xdd);
//     ret = DecodeVar32(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#x\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeVar32, Test_Exception_UInt)
// {/*{{{*/
//     using namespace base;
//
//     // DecodeVar32
//     std::string tmp_dst;
//     tmp_dst.append(1, 0xff).append(1, 0xf2);
//     uint32_t tmp_src = 0;
//     Code ret = DecodeVar32(tmp_dst, &tmp_src);
//     EXPECT_NEQ(kOk, ret);
// }/*}}}*/
//
//
// TEST(EncodeVar64, Test_Normal_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     uint64_t src = 0x1122334455667788;
//     const char dst[9] = {0x88, 0xef, 0x99, 0xab, 0xc5, 0xe8, 0x8c, 0x91, 0x11};
//
//     // EncodeVar32
//     std::string tmp_dst;
//     Code ret = EncodeVar64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(sizeof(dst)/sizeof(dst[0]), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst, tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeVar64
//     uint64_t tmp_src = 0;
//     ret = DecodeVar64(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#llx\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Normal_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint64_t, std::string> var_map;
//     var_map[0x7f].append(1, 0x7f);
//     var_map[0x3fff].append(1, 0xff).append(1, 0x7f);
//     var_map[0x1fffff].append(1, 0xff).append(1, 0xff).append(1, 0x7f);
//     var_map[0xfffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1, 0x7f);
//     var_map[0x7ffffffff].append(1, 0xff).append(1, 0xff).append(1, 0xff).append(1,
//     0xff).append(1, 0x7f);
//     var_map[0x3ffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//     var_map[0x1ffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//     var_map[0xffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//     var_map[0x7fffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//     var_map[0xffffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x1);
//
//     std::map<uint64_t, std::string>::iterator it = var_map.begin();
//     for (it = var_map.begin(); it != var_map.end(); ++it)
//     {
//         // EncodeVar64
//         std::string tmp_dst;
//         Code ret = EncodeVar64(it->first, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->second.size(), tmp_dst.size());
//         EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//         // DecodeVar64
//         uint64_t tmp_src = 0;
//         ret = DecodeVar64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->first, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Loop_First_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     for (uint64_t i = 0; i < 10000000; ++i)
//     {
//         // EncodeVar64
//         std::string tmp_dst;
//         Code ret = EncodeVar64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeVar64
//         uint64_t tmp_src = 0;
//         ret = DecodeVar64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Loop_Last_UInt64s)
// {/*{{{*/
//     using namespace base;
//
//     for (uint64_t i = UINT64_MAX; i > UINT64_MAX-10000000; --i)
//     {
//         // EncodeVar64
//         std::string tmp_dst;
//         Code ret = EncodeVar64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//
//         // DecodeVar64
//         uint64_t tmp_src = 0;
//         ret = DecodeVar64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Press_OneMillion_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     std::map<uint64_t, std::string> var_map;
//     var_map[0xffffffffffffff].append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0xff).append(1,0x7f);
//
//     for (int i = 0; i < 1000000; ++i)
//     {
//         std::map<uint64_t, std::string>::iterator it = var_map.begin();
//         for (it = var_map.begin(); it != var_map.end(); ++it)
//         {
//             // EncodeVar64
//             std::string tmp_dst;
//             Code ret = EncodeVar64(it->first, &tmp_dst);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->second.size(), tmp_dst.size());
//             EXPECT_EQ(0, memcmp(it->second.data(), tmp_dst.data(), tmp_dst.size()));
//
//             // DecodeVar64
//             uint64_t tmp_src = 0;
//             ret = DecodeVar64(tmp_dst, &tmp_src);
//             EXPECT_EQ(kOk, ret);
//             EXPECT_EQ(it->first, tmp_src);
//         }
//     }
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Exception_MoreLength)
// {/*{{{*/
//     using namespace base;
//
//     uint64_t src = 0x1122334455667788;
//     const char dst[9] = {0x88, 0xef, 0x99, 0xab, 0xc5, 0xe8, 0x8c, 0x91, 0x11};
//
//     // EncodeVar32
//     std::string tmp_dst;
//     Code ret = EncodeVar64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(sizeof(dst)/sizeof(dst[0]), tmp_dst.size());
//     EXPECT_EQ(0, memcmp(dst, tmp_dst.data(), tmp_dst.size()));
//     for (int i = 0; i < (int)tmp_dst.size(); ++i)
//     {
//         fprintf(stderr, "%02x ", (unsigned char)tmp_dst.data()[i]);
//     }
//     fprintf(stderr, "\n");
//
//     // DecodeVar64
//     uint64_t tmp_src = 0;
//     tmp_dst.append(1, 0xaa).append(1, 0xbb);
//     ret = DecodeVar64(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//     fprintf(stderr, "%#llx\n", tmp_src);
// }/*}}}*/
//
// TEST(EncodeVar64, Test_Exception_UInt64)
// {/*{{{*/
//     using namespace base;
//
//     // DecodeVar64
//     std::string tmp_dst;
//     tmp_dst.append(1, 0xff).append(1,
//     0xf2).append(1,0xf3).append(1,0xf4).append(1,0xf5).append(1,0xf6); uint64_t tmp_src = 0; Code
//     ret = DecodeVar64(tmp_dst, &tmp_src); EXPECT_NEQ(kOk, ret);
// }/*}}}*/
//
// TEST(EncodeZigZag32, Test_Normal_Int)
// {/*{{{*/
//     using namespace base;
//
//     int src = -2;
//     uint32_t dst = 3;
//
//     // EncodeZigZag32
//     uint32_t tmp_dst = 0;
//     Code ret = EncodeZigZag32(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst, tmp_dst);
//
//     // DecodeZigZag32
//     int tmp_src = 0;
//     ret = DecodeZigZag32(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(EncodeZigZag32, Test_Normal_Ints)
// {/*{{{*/
//     using namespace base;
//
//     for (int i = 0; i < 1000000; ++i)
//     {/*{{{*/
//         uint32_t dst = i*2;
//
//         // EncodeZigZag32
//         uint32_t tmp_dst = 0;
//         Code ret = EncodeZigZag32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag32
//         int tmp_src = 0;
//         ret = DecodeZigZag32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int i = INT_MAX-1000000; i < INT_MAX; ++i)
//     {/*{{{*/
//         uint32_t dst = i*2;
//
//         // EncodeZigZag32
//         uint32_t tmp_dst = 0;
//         Code ret = EncodeZigZag32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag32
//         int tmp_src = 0;
//         ret = DecodeZigZag32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int i = -1; i > -1000000; --i)
//     {/*{{{*/
//         uint32_t dst = -i*2 - 1;
//
//         // EncodeZigZag32
//         uint32_t tmp_dst = 0;
//         Code ret = EncodeZigZag32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag32
//         int tmp_src = 0;
//         ret = DecodeZigZag32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int i = INT_MIN+1000000; i > INT_MIN; --i)
//     {/*{{{*/
//         uint32_t dst = -i*2 - 1;
//
//         // EncodeZigZag32
//         uint32_t tmp_dst = 0;
//         Code ret = EncodeZigZag32(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag32
//         int tmp_src = 0;
//         ret = DecodeZigZag32(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//         if (i == INT_MIN +10 )
//         {
//             fprintf(stderr, "i:%d, tmp_src:%d, tmp_dst:%u\n", i, tmp_src, tmp_dst);
//         }
//     }/*}}}*/
// }/*}}}*/
//
// TEST(EncodeZigZag64, Test_Normal_Int64)
// {/*{{{*/
//     using namespace base;
//
//     int64_t src = -2;
//     uint64_t dst = 3;
//
//     // EncodeZigZag64
//     uint64_t tmp_dst = 0;
//     Code ret = EncodeZigZag64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst, tmp_dst);
//
//     // DecodeZigZag64
//     int64_t tmp_src = 0;
//     ret = DecodeZigZag64(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(EncodeZigZag64, Test_Normal_Ints)
// {/*{{{*/
//     using namespace base;
//
//     for (int64_t i = 0; i < 1000000; ++i)
//     {/*{{{*/
//         uint64_t dst = i*2;
//
//         // EncodeZigZag64
//         uint64_t tmp_dst = 0;
//         Code ret = EncodeZigZag64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag64
//         int64_t tmp_src = 0;
//         ret = DecodeZigZag64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int64_t i = INT64_MAX-1000000; i < INT64_MAX; ++i)
//     {/*{{{*/
//         uint64_t dst = i*2;
//
//         // EncodeZigZag64
//         uint64_t tmp_dst = 0;
//         Code ret = EncodeZigZag64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag64
//         int64_t tmp_src = 0;
//         ret = DecodeZigZag64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int64_t i = -1; i > -1000000; --i)
//     {/*{{{*/
//         uint64_t dst = -i*2 - 1;
//
//         // EncodeZigZag64
//         uint64_t tmp_dst = 0;
//         Code ret = EncodeZigZag64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag64
//         int64_t tmp_src = 0;
//         ret = DecodeZigZag64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//     }/*}}}*/
//
//     for (int64_t i = INT64_MIN+1000000; i > INT64_MIN; --i)
//     {/*{{{*/
//         uint64_t dst = -i*2 - 1;
//
//         // EncodeZigZag64
//         uint64_t tmp_dst = 0;
//         Code ret = EncodeZigZag64(i, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(dst, tmp_dst);
//
//         // DecodeZigZag64
//         int64_t tmp_src = 0;
//         ret = DecodeZigZag64(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(i, tmp_src);
//         if (i == INT64_MIN +10)
//         {
//             fprintf(stderr, "i:%lld, tmp_src:%lld, tmp_dst:%llu\n", i, tmp_src, tmp_dst);
//         }
//     }/*}}}*/
// }/*}}}*/
//
// TEST(EncodeZigZag64, Test_Compare_Var_and_ZigZag_Int64)
// {/*{{{*/
//     using namespace base;
//
//     int64_t src = -1000;
//     uint64_t dst = 1999;
//
//     std::string only_dst;
//     Code ret = EncodeVar64(src, &only_dst);
//     EXPECT_EQ(kOk, ret);
//     fprintf(stderr, "only_dst size:%zu\n", only_dst.size());
//
//     uint64_t only_src = 0;
//     ret = DecodeVar64(only_dst, &only_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, (int64_t)only_src);
//
//     // EncodeZigZag64
//     uint64_t tmp_dst = 0;
//     ret = EncodeZigZag64(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(dst, tmp_dst);
//
//     std::string zigzag_dst;
//     ret = EncodeVar64(tmp_dst, &zigzag_dst);
//     EXPECT_EQ(kOk, ret);
//     fprintf(stderr, "zigzag_dst size:%zu\n", zigzag_dst.size());
//
//     uint64_t zigzag_src = 0;
//     ret = DecodeVar64(zigzag_dst, &zigzag_src);
//     EXPECT_EQ(0, ret);
//     EXPECT_EQ(tmp_dst, zigzag_src);
//
//     // DecodeZigZag64
//     int64_t tmp_src = 0;
//     ret = DecodeZigZag64(zigzag_src, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
//
// }/*}}}*/
//
// TEST(Base16Encode, Test_Normal_SimpleStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     bin_str.append(1, 0x00);
//     bin_str.append(1, 0x08);
//     bin_str.append(1, 0x0f);
//     bin_str.append(1, 0x3f);
//     bin_str.append(1, 0x7f);
//     bin_str.append(1, 0xed);
//     bin_str.append(1, 0xf2);
//
//
//     char buf[32] = {'0', '0', '0', '8', '0', 'f', '3', 'f', '7', 'f', 'e', 'd', 'f', '2'};
//     std::string expected_readable_str(buf);
//
//     std::string readable_str;
//     Code ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Encode, Test_Normal_AllBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     for (int i = 0; i < 256; ++i)
//     {
//         bin_str.append(1, i);
//     }
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (int i = 0; i < 256; ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)i);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     Code ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Encode, Test_Normal_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 1024;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
// //    fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Encode, Test_Press_10_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 10;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Encode(bin_str, &readable_str);
//         EXPECT_EQ(kOk, ret);
//     }
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     // fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Encode, Test_Press_100_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 100;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Encode(bin_str, &readable_str);
//         EXPECT_EQ(kOk, ret);
//     }
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     //fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Encode, Test_Press_1000_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 1000;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Encode(bin_str, &readable_str);
//         EXPECT_EQ(kOk, ret);
//     }
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     //fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base16Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Decode, Test_Normal_SimpleStr)
// {/*{{{*/
//     using namespace base;
//     std::string src_str = "0baf18ad030f1b3c9108765423dacbeffd";
//     std::string bin_str;
//
//     Code ret = Base16Decode(src_str, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src_str.size(), bin_str.size()*2);
//
//     std::string tmp_str;
//     ret = Base16Encode(bin_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//
//     EXPECT_EQ(src_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Decode, Test_Exception_WrongLen)
// {/*{{{*/
//     using namespace base;
//     std::string src_str = "0baf18ad030f1b3c9108765423dacbeff";
//     std::string bin_str;
//
//     Code ret = Base16Decode(src_str, &bin_str);
//     EXPECT_EQ(kInvalidParam, ret);
// }/*}}}*/
//
// TEST(Base16Decode, Test_Press_10_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 10;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     // fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Decode(readable_str, &tmp_str);
//     }
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Decode, Test_Press_100_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 100;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     // fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Decode(readable_str, &tmp_str);
//     }
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base16Decode, Test_Press_1000_Len_Ten_Thousand_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 1000;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     char buf[3] = {0};
//     std::string expected_readable_str;
//     for (size_t i = 0; i < bin_str.size(); ++i)
//     {
//         snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
//         expected_readable_str.append(buf);
//     }
//
//     std::string readable_str;
//     ret = Base16Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(readable_str.size(), bin_str.size()*2);
//     EXPECT_EQ(expected_readable_str, readable_str);
//     // fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     for (uint32_t i = 0; i < 10000; ++i)
//     {
//         ret = Base16Decode(readable_str, &tmp_str);
//     }
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base32Encode, Test_Normal_Str)
// {/*{{{*/
//     using namespace base;
//
//     std::map<std::string, std::string> maps;
//
//     maps["a"] = "ME======";
//     maps["ab"] = "MFRA====";
//     maps["abc"] = "MFRGG===";
//     maps["abcd"] = "MFRGGZA=";
//     maps["abcde"] = "MFRGGZDF";
//     maps["abcdef"] = "MFRGGZDFMY======";
//     maps["azAZ09+/"] = "MF5ECWRQHEVS6===";
//     maps["   "] = "EAQCA===";
//     maps["abcdefEG m%+@@##% ABVC"] = "MFRGGZDFMZCUOIDNEUVUAQBDEMSSAQKCKZBQ====";
//     maps["nice you and here and there, good meet you!"] =
//     "NZUWGZJAPFXXKIDBNZSCA2DFOJSSAYLOMQQHI2DFOJSSYIDHN5XWIIDNMVSXIIDZN52SC===";
//     maps["ab12340012392345009123"] = "MFRDCMRTGQYDAMJSGM4TEMZUGUYDAOJRGIZQ====";
//
//     std::string tmp_src;
//     std::string tmp_dst;
//     std::map<std::string, std::string>::iterator it;
//     for (it = maps.begin(); it != maps.end(); ++it)
//     {
//         Code ret = Base32Encode(it->first, &tmp_dst);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->second, tmp_dst);
//         fprintf(stderr, "%s : %s\n", it->second.c_str(), tmp_dst.c_str());
//
//         ret = Base32Decode(tmp_dst, &tmp_src);
//         EXPECT_EQ(kOk, ret);
//         EXPECT_EQ(it->first, tmp_src);
//         fprintf(stderr, "%s : %s\n", it->first.c_str(), tmp_src.c_str());
//     }
//
// }/*}}}*/
//
// TEST(Base32Encode, Test_Normal_Full_Char)
// {/*{{{*/
//     using namespace base;
//
//     std::string src;
//     for (uint32_t i = 0; i < 256; ++i)
//     {
//         src.append(1, i);
//     }
//
//     std::string tmp_src;
//     std::string tmp_dst;
//
//     Code ret = Base32Encode(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     ret = Base32Decode(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(Base32Encode, Test_Normal_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 1024;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     std::string readable_str;
//     ret = Base32Encode(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
// //    fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base32Decode(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
//
// TEST(Base32EncodeForGeoHash, Test_Normal_Full_Char)
// {/*{{{*/
//     using namespace base;
//
//     std::string src;
//     for (uint32_t i = 0; i < 256; ++i)
//     {
//         src.append(1, i);
//     }
//
//     std::string tmp_src;
//     std::string tmp_dst;
//
//     Code ret = Base32EncodeForGeoHash(src, &tmp_dst);
//     EXPECT_EQ(kOk, ret);
//     fprintf(stderr, "%s\n", tmp_dst.c_str());
//
//     ret = Base32DecodeForGeoHash(tmp_dst, &tmp_src);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(src, tmp_src);
// }/*}}}*/
//
// TEST(Base32EncodeForGeoHash, Test_Normal_RandomBinStr)
// {/*{{{*/
//     using namespace base;
//     std::string bin_str;
//     uint32_t len = 1024;
//     Code ret = GetRandBinStr(len, &bin_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(len, bin_str.size());
//
//     std::string readable_str;
//     ret = Base32EncodeForGeoHash(bin_str, &readable_str);
//     EXPECT_EQ(kOk, ret);
// //    fprintf(stderr, "%s\n", readable_str.c_str());
//
//     std::string tmp_str;
//     ret = Base32DecodeForGeoHash(readable_str, &tmp_str);
//     EXPECT_EQ(kOk, ret);
//     EXPECT_EQ(bin_str, tmp_str);
// }/*}}}*/
