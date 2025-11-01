// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <chrono>

#include <utility>

#include "base/algo.h"
#include "base/status.h"
#include "base/utf8.h"

#include "test_base/include/test_base.h"

namespace {
TEST_D(IsUtf8, Test_Normal_Utf8, "utf8检测") { /*{{{*/
  using namespace base;
  std::string str = "hello world";
  EXPECT_TRUE(IsUtf8(str));
  str = "你好";
  EXPECT_TRUE(IsUtf8(str));
} /*}}}*/

TEST_D(IsUtf8, Test_Empty_String, "空字符串测试") { /*{{{*/
  using namespace base;
  std::string empty_str = "";
  EXPECT_TRUE(IsUtf8(empty_str));
} /*}}}*/

TEST_D(IsUtf8, Test_ASCII_Characters, "ASCII字符测试") { /*{{{*/
  using namespace base;

  // 基本ASCII字符
  EXPECT_TRUE(IsUtf8("Hello World!"));
  EXPECT_TRUE(IsUtf8("0123456789"));
  EXPECT_TRUE(IsUtf8("abcdefghijklmnopqrstuvwxyz"));
  EXPECT_TRUE(IsUtf8("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
  EXPECT_TRUE(IsUtf8("!@#$%^&*()_+-=[]{}|;':\",./<>?"));

  // 控制字符
  std::string control_chars = "\x00\x01\x02\x03\x1F\x7F";
  EXPECT_TRUE(IsUtf8(control_chars));
} /*}}}*/

TEST_D(IsUtf8, Test_Two_Byte_UTF8, "2字节UTF-8测试") { /*{{{*/
  using namespace base;

  // 拉丁字符
  EXPECT_TRUE(IsUtf8("café"));
  EXPECT_TRUE(IsUtf8("résumé"));
  EXPECT_TRUE(IsUtf8("naïve"));

  // 边界值测试
  std::string min_2byte = "\xC2\x80";  // U+0080
  EXPECT_TRUE(IsUtf8(min_2byte));

  std::string max_2byte = "\xDF\xBF";  // U+07FF
  EXPECT_TRUE(IsUtf8(max_2byte));
} /*}}}*/

TEST_D(IsUtf8, Test_Three_Byte_UTF8, "3字节UTF-8测试") { /*{{{*/
  using namespace base;

  // 中文字符
  EXPECT_TRUE(IsUtf8("你好世界"));
  EXPECT_TRUE(IsUtf8("测试"));
  EXPECT_TRUE(IsUtf8("编程"));

  // 日文字符
  EXPECT_TRUE(IsUtf8("こんにちは"));
  EXPECT_TRUE(IsUtf8("ひらがな"));

  // 韩文字符
  EXPECT_TRUE(IsUtf8("안녕하세요"));

  // 边界值测试
  std::string min_3byte = "\xE0\xA0\x80";  // U+0800
  EXPECT_TRUE(IsUtf8(min_3byte));

  std::string max_3byte = "\xEF\xBF\xBF";  // U+FFFF
  EXPECT_TRUE(IsUtf8(max_3byte));
} /*}}}*/

TEST_D(IsUtf8, Test_Four_Byte_UTF8, "4字节UTF-8测试") { /*{{{*/
  using namespace base;

  // 表情符号
  EXPECT_TRUE(IsUtf8("😀😃😄😁"));
  EXPECT_TRUE(IsUtf8("🌍🌎🌏"));
  EXPECT_TRUE(IsUtf8("👍👎👌"));
  EXPECT_TRUE(IsUtf8("❤️💙💚"));

  // 边界值测试
  std::string min_4byte = "\xF0\x90\x80\x80";  // U+10000
  EXPECT_TRUE(IsUtf8(min_4byte));

  std::string max_4byte = "\xF4\x8F\xBF\xBF";  // U+10FFFF
  EXPECT_TRUE(IsUtf8(max_4byte));
} /*}}}*/

TEST_D(IsUtf8, Test_Mixed_Characters, "混合字符测试") { /*{{{*/
  using namespace base;

  // ASCII + 中文
  EXPECT_TRUE(IsUtf8("Hello 世界"));
  EXPECT_TRUE(IsUtf8("Test 测试 123"));

  // 多种语言混合
  EXPECT_TRUE(IsUtf8("Hello 你好 こんにちは 안녕하세요"));

  // 包含表情符号的混合文本
  EXPECT_TRUE(IsUtf8("Hello 世界 😀"));
  EXPECT_TRUE(IsUtf8("编程很有趣 👨‍💻"));
} /*}}}*/

TEST_D(IsUtf8, Test_Invalid_Start_Bytes, "无效起始字节测试") { /*{{{*/
  using namespace base;

  // 无效的起始字节
  std::string invalid1 = "\xFF";
  EXPECT_FALSE(IsUtf8(invalid1));

  std::string invalid2 = "\xFE";
  EXPECT_FALSE(IsUtf8(invalid2));

  std::string invalid3 = "\x80";  // 单独的续字节
  EXPECT_FALSE(IsUtf8(invalid3));

  std::string invalid4 = "\xBF";  // 单独的续字节
  EXPECT_FALSE(IsUtf8(invalid4));
} /*}}}*/

TEST_D(IsUtf8, Test_Incomplete_Sequences, "不完整序列测试") { /*{{{*/
  using namespace base;

  // 2字节序列缺少续字节
  std::string incomplete1 = "\xC2";
  EXPECT_FALSE(IsUtf8(incomplete1));

  // 3字节序列缺少续字节
  std::string incomplete2 = "\xE0";
  EXPECT_FALSE(IsUtf8(incomplete2));

  std::string incomplete3 = "\xE0\xA0";
  EXPECT_FALSE(IsUtf8(incomplete3));

  // 4字节序列缺少续字节
  std::string incomplete4 = "\xF0";
  EXPECT_FALSE(IsUtf8(incomplete4));

  std::string incomplete5 = "\xF0\x90";
  EXPECT_FALSE(IsUtf8(incomplete5));

  std::string incomplete6 = "\xF0\x90\x80";
  EXPECT_FALSE(IsUtf8(incomplete6));
} /*}}}*/

TEST_D(IsUtf8, Test_Invalid_Continuation_Bytes, "无效续字节测试") { /*{{{*/
  using namespace base;

  // 2字节序列中的无效续字节
  std::string invalid1 = "\xC2\xFF";
  EXPECT_FALSE(IsUtf8(invalid1));

  std::string invalid2 = "\xC2\x00";
  EXPECT_FALSE(IsUtf8(invalid2));

  // 3字节序列中的无效续字节
  std::string invalid3 = "\xE0\xFF\x80";
  EXPECT_FALSE(IsUtf8(invalid3));

  std::string invalid4 = "\xE0\xA0\xFF";
  EXPECT_FALSE(IsUtf8(invalid4));

  // 4字节序列中的无效续字节
  std::string invalid5 = "\xF0\xFF\x80\x80";
  EXPECT_FALSE(IsUtf8(invalid5));

  std::string invalid6 = "\xF0\x90\xFF\x80";
  EXPECT_FALSE(IsUtf8(invalid6));

  std::string invalid7 = "\xF0\x90\x80\xFF";
  EXPECT_FALSE(IsUtf8(invalid7));
} /*}}}*/

TEST_D(IsUtf8, Test_Overlong_Encoding, "过长编码测试") { /*{{{*/
  using namespace base;

  // 用2字节编码ASCII字符 (应该用1字节)
  std::string overlong1 = "\xC0\x80";  // 编码 U+0000
  EXPECT_FALSE(IsUtf8(overlong1));

  std::string overlong2 = "\xC1\xBF";  // 编码 U+007F
  EXPECT_FALSE(IsUtf8(overlong2));

  // 用3字节编码2字节字符
  std::string overlong3 = "\xE0\x80\x80";  // 编码 U+0000
  EXPECT_FALSE(IsUtf8(overlong3));

  std::string overlong4 = "\xE0\x9F\xBF";  // 编码 U+07FF
  EXPECT_FALSE(IsUtf8(overlong4));

  // 用4字节编码3字节字符
  std::string overlong5 = "\xF0\x80\x80\x80";  // 编码 U+0000
  EXPECT_FALSE(IsUtf8(overlong5));

  std::string overlong6 = "\xF0\x8F\xBF\xBF";  // 编码 U+FFFF
  EXPECT_FALSE(IsUtf8(overlong6));
} /*}}}*/

TEST_D(IsUtf8, Test_Surrogate_Pairs, "代理对测试") { /*{{{*/
  using namespace base;

  // UTF-8中不允许代理对 (U+D800-U+DFFF)
  std::string surrogate1 = "\xED\xA0\x80";  // U+D800
  EXPECT_FALSE(IsUtf8(surrogate1));

  std::string surrogate2 = "\xED\xAF\xBF";  // U+DBFF
  EXPECT_FALSE(IsUtf8(surrogate2));

  std::string surrogate3 = "\xED\xB0\x80";  // U+DC00
  EXPECT_FALSE(IsUtf8(surrogate3));

  std::string surrogate4 = "\xED\xBF\xBF";  // U+DFFF
  EXPECT_FALSE(IsUtf8(surrogate4));
} /*}}}*/

TEST_D(IsUtf8, Test_Out_Of_Range, "超出范围测试") { /*{{{*/
  using namespace base;

  // 超出Unicode范围 (> U+10FFFF)
  std::string out_of_range1 = "\xF4\x90\x80\x80";  // U+110000
  EXPECT_FALSE(IsUtf8(out_of_range1));

  std::string out_of_range2 = "\xF7\xBF\xBF\xBF";  // 更大的值
  EXPECT_FALSE(IsUtf8(out_of_range2));
} /*}}}*/

TEST_D(IsUtf8, Test_Boundary_Cases, "边界情况测试") { /*{{{*/
  using namespace base;

  // 最大有效ASCII字符
  std::string max_ascii = "\x7F";
  EXPECT_TRUE(IsUtf8(max_ascii));

  // 最小2字节字符
  std::string min_2byte = "\xC2\x80";  // U+0080
  EXPECT_TRUE(IsUtf8(min_2byte));

  // 最大2字节字符
  std::string max_2byte = "\xDF\xBF";  // U+07FF
  EXPECT_TRUE(IsUtf8(max_2byte));

  // 最小3字节字符
  std::string min_3byte = "\xE0\xA0\x80";  // U+0800
  EXPECT_TRUE(IsUtf8(min_3byte));

  // 最大3字节字符 (不是代理对)
  std::string max_3byte_valid = "\xED\x9F\xBF";  // U+D7FF
  EXPECT_TRUE(IsUtf8(max_3byte_valid));

  std::string max_3byte_valid2 = "\xEE\x80\x80";  // U+E000
  EXPECT_TRUE(IsUtf8(max_3byte_valid2));

  std::string max_3byte = "\xEF\xBF\xBF";  // U+FFFF
  EXPECT_TRUE(IsUtf8(max_3byte));

  // 最小4字节字符
  std::string min_4byte = "\xF0\x90\x80\x80";  // U+10000
  EXPECT_TRUE(IsUtf8(min_4byte));

  // 最大4字节字符
  std::string max_4byte = "\xF4\x8F\xBF\xBF";  // U+10FFFF
  EXPECT_TRUE(IsUtf8(max_4byte));
} /*}}}*/

TEST_D(IsUtf8, Test_Real_World_Examples, "真实世界示例测试") { /*{{{*/
  using namespace base;

  // 常见的多语言文本
  EXPECT_TRUE(IsUtf8("UTF-8编码测试"));
  EXPECT_TRUE(IsUtf8("Iñtërnâtiônàlizætiøn"));
  EXPECT_TRUE(IsUtf8("Ελληνικά"));  // 希腊文
  EXPECT_TRUE(IsUtf8("العربية"));   // 阿拉伯文
  EXPECT_TRUE(IsUtf8("עברית"));     // 希伯来文
  EXPECT_TRUE(IsUtf8("русский"));   // 俄文

  // 包含特殊符号的文本
  EXPECT_TRUE(IsUtf8("Price: €100 ¥500 $50"));
  EXPECT_TRUE(IsUtf8("Math: α + β = γ"));
  EXPECT_TRUE(IsUtf8("Copyright © 2023"));

  // JSON字符串示例
  EXPECT_TRUE(IsUtf8("{\"name\": \"张三\", \"age\": 25}"));

  // URL编码前的文本
  EXPECT_TRUE(IsUtf8("搜索?q=UTF-8测试&lang=zh"));
} /*}}}*/

TEST_D(IsUtf8, Test_Performance, "性能测试") { /*{{{*/
  using namespace base;
  
  // 测试小字符串性能 (ASCII)
  std::string small_ascii(100, 'A');
  auto start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; ++i) {
    EXPECT_TRUE(IsUtf8(small_ascii));
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("小ASCII字符串(100字节) 10000次验证耗时: %ld μs\n", duration.count());
  
  // 测试中等字符串性能 (UTF-8中文)
  std::string medium_utf8;
  for (int i = 0; i < 500; ++i) {
    medium_utf8 += "测试";  // 每个中文字符3字节
  }
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 1000; ++i) {
    EXPECT_TRUE(IsUtf8(medium_utf8));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("中等UTF-8字符串(3000字节) 1000次验证耗时: %ld μs\n", duration.count());
  
  // 测试大字符串性能 (混合内容)
  std::string large_mixed;
  for (int i = 0; i < 1000; ++i) {
    large_mixed += "Hello世界😀Test测试";  // ASCII + 中文 + 表情符号
  }
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(IsUtf8(large_mixed));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("大混合字符串(%zu字节) 100次验证耗时: %ld μs\n", large_mixed.size(), duration.count());
  
  // 测试无效UTF-8性能 (早期失败)
  std::string invalid_early = "\xFF" + std::string(1000, 'A');
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; ++i) {
    EXPECT_FALSE(IsUtf8(invalid_early));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("无效UTF-8(早期失败) 10000次验证耗时: %ld μs\n", duration.count());
  
  // 测试无效UTF-8性能 (后期失败)
  std::string invalid_late = std::string(1000, 'A') + "\xFF";
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 1000; ++i) {
    EXPECT_FALSE(IsUtf8(invalid_late));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("无效UTF-8(后期失败) 1000次验证耗时: %ld μs\n", duration.count());
  
  // 测试极长字符串性能
  std::string very_large_ascii(100000, 'X');  // 100KB ASCII
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(IsUtf8(very_large_ascii));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("极长ASCII字符串(100KB) 10次验证耗时: %ld μs\n", duration.count());
  
  // 测试多字节字符密集型性能
  std::string dense_multibyte;
  for (int i = 0; i < 10000; ++i) {
    dense_multibyte += "你";  // 每个字符3字节
  }
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(IsUtf8(dense_multibyte));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("多字节密集字符串(30KB) 100次验证耗时: %ld μs\n", duration.count());
  
  // 测试4字节字符性能
  std::string emoji_string;
  for (int i = 0; i < 1000; ++i) {
    emoji_string += "😀😃😄😁";  // 每个表情符号4字节
  }
  start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(IsUtf8(emoji_string));
  }
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  printf("4字节字符串(16KB) 100次验证耗时: %ld μs\n", duration.count());
} /*}}}*/
}  // namespace