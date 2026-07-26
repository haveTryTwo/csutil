// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/common.h"
#include "base/config.h"
#include "base/random.h"
#include "base/status.h"
#include "base/util.h"

#include "test_base/include/test_base.h"

// ============================================================================
// 测试用例列表：
// 1. Test_LoadFile_Normal              - 基本功能测试：加载有效配置文件
// 2. Test_LoadFile_InvalidParam        - 异常情况测试：加载空路径
// 3. Test_LoadFile_NotExist            - 异常情况测试：加载不存在的文件
// 4. Test_GetValue_Normal              - 基本功能测试：获取字符串值
// 5. Test_GetValue_NotFound            - 异常情况测试：获取不存在的键
// 6. Test_GetValue_NullPointer         - 边界测试：传入空指针
// 7. Test_GetInt32Value_Normal         - 基本功能测试：获取int32值
// 8. Test_GetInt32Value_InvalidParam   - 异常情况测试：空指针参数
// 9. Test_GetInt32Value_NotFound       - 异常情况测试：键不存在
// 10. Test_GetInt32Value_InvalidFormat - 异常情况测试：非数字格式
// 11. Test_GetInt64Value_Normal        - 基本功能测试：获取int64值
// 12. Test_GetInt64Value_InvalidParam  - 异常情况测试：空指针参数
// 13. Test_GetInt64Value_NotFound      - 异常情况测试：键不存在
// 14. Test_GetInt64Value_Overflow      - 边界测试：数值溢出
// 15. Test_GetInt64Value_MaxMin        - 边界测试：最大最小值
// 16. Test_GetValue_WithDefault        - 基本功能测试：带默认值获取
// 17. Test_GetInt32Value_WithDefault   - 基本功能测试：带默认值获取int32
// 18. Test_GetInt64Value_WithDefault   - 基本功能测试：带默认值获取int64
// 19. Test_Config_Comment              - 基本功能测试：注释行处理
// 20. Test_Config_EmptyLine            - 基本功能测试：空行处理
// 21. Test_Config_Whitespace           - 基本功能测试：空白字符处理
// 22. Test_Config_SpecialChars         - 边界测试：特殊字符处理
// 23. Test_Config_DuplicateKeys        - 边界测试：重复键处理
// 24. Test_Config_ConstMethods         - 基本功能测试：const方法测试
// ============================================================================

TEST_D(Config, Test_LoadFile_Normal, "基本功能测试：加载有效配置文件") { /*{{{*/
  using namespace base;
  Config config;

  // 创建临时配置文件
  const char *tmp_file = "/tmp/test_config_normal.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "# This is a comment\n");
  fprintf(fp, "key1 = value1\n");
  fprintf(fp, "key2=value2\n");
  fprintf(fp, "port = 8080\n");
  fprintf(fp, "timeout = 30\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("key1", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value1");

  ret = config.GetValue("key2", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value2");

  int port = 0;
  ret = config.GetInt32Value("port", &port);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(port, 8080);

  // 清理临时文件
  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_LoadFile_InvalidParam, "异常情况测试：加载空路径") { /*{{{*/
  using namespace base;
  Config config;

  Code ret = config.LoadFile("");
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/

TEST_D(Config, Test_LoadFile_NotExist, "异常情况测试：加载不存在的文件") { /*{{{*/
  using namespace base;
  Config config;

  Code ret = config.LoadFile("/tmp/non_existent_config_file_12345.conf");
  EXPECT_EQ(ret, kOpenError);
} /*}}}*/

TEST_D(Config, Test_GetValue_Normal, "基本功能测试：获取字符串值") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_getvalue.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "string_key = hello world\n");
  fprintf(fp, "empty_value = \n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("string_key", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "hello world");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetValue_NotFound, "异常情况测试：获取不存在的键") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_notfound.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "existing_key = value\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("non_existing_key", &value);
  EXPECT_EQ(ret, kNotFound);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetValue_NullPointer, "边界测试：传入空指针") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_null.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "key = value\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  // GetValue允许传入NULL指针
  ret = config.GetValue("key", NULL);
  EXPECT_EQ(ret, kOk);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt32Value_Normal, "基本功能测试：获取int32值") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int32.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "positive_int = 12345\n");
  fprintf(fp, "negative_int = -9876\n");
  fprintf(fp, "zero = 0\n");
  fprintf(fp, "hex_value = 0x100\n");
  fprintf(fp, "octal_value = 0755\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int value = 0;
  ret = config.GetInt32Value("positive_int", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 12345);

  ret = config.GetInt32Value("negative_int", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, -9876);

  ret = config.GetInt32Value("zero", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 0);

  ret = config.GetInt32Value("hex_value", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 0x100);

  ret = config.GetInt32Value("octal_value", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 0755);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt32Value_InvalidParam, "异常情况测试：空指针参数") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int32_null.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "int_value = 123\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  ret = config.GetInt32Value("int_value", NULL);
  EXPECT_EQ(ret, kInvalidParam);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt32Value_NotFound, "异常情况测试：键不存在") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int32_notfound.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "existing_key = 123\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int value = 0;
  ret = config.GetInt32Value("non_existing_key", &value);
  EXPECT_EQ(ret, kNotFound);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt32Value_InvalidFormat, "异常情况测试：非数字格式") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_invalid_format.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "not_a_number = abc\n");
  fprintf(fp, "partial_number = 123abc\n");
  fprintf(fp, "empty_string = \n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int value = 0;
  ret = config.GetInt32Value("not_a_number", &value);
  EXPECT_NE(ret, kOk);
  EXPECT_TRUE(ret == kNoDigits || ret == kStrtollFailed);

  ret = config.GetInt32Value("partial_number", &value);
  EXPECT_EQ(ret, kNotAllDigits);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_Normal, "基本功能测试：获取int64值") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int64.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "large_positive = 9223372036854775807\n");   // INT64_MAX
  fprintf(fp, "large_negative = -9223372036854775808\n");  // INT64_MIN
  fprintf(fp, "normal_value = 1234567890\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int64_t value = 0;
  ret = config.GetInt64Value("large_positive", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, INT64_MAX);

  ret = config.GetInt64Value("large_negative", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, INT64_MIN);

  ret = config.GetInt64Value("normal_value", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 1234567890);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_InvalidParam, "异常情况测试：空指针参数") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int64_null.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "int64_value = 123456789012345\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  ret = config.GetInt64Value("int64_value", NULL);
  EXPECT_EQ(ret, kInvalidParam);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_NotFound, "异常情况测试：键不存在") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int64_notfound.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "existing = 999\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int64_t value = 0;
  ret = config.GetInt64Value("non_existing", &value);
  EXPECT_EQ(ret, kNotFound);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_Overflow, "边界测试：数值溢出") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_overflow.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  // 超过INT64_MAX的值
  fprintf(fp, "overflow_value = 99999999999999999999999999\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int64_t value = 0;
  ret = config.GetInt64Value("overflow_value", &value);
  EXPECT_EQ(ret, kStrtollFailed);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_MaxMin, "边界测试：最大最小值") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_maxmin.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "int32_max = 2147483647\n");
  fprintf(fp, "int32_min = -2147483648\n");
  fprintf(fp, "int64_max = 9223372036854775807\n");
  fprintf(fp, "int64_min = -9223372036854775808\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int int32_val = 0;
  ret = config.GetInt32Value("int32_max", &int32_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int32_val, INT32_MAX);

  ret = config.GetInt32Value("int32_min", &int32_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int32_val, INT32_MIN);

  int64_t int64_val = 0;
  ret = config.GetInt64Value("int64_max", &int64_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int64_val, INT64_MAX);

  ret = config.GetInt64Value("int64_min", &int64_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int64_val, INT64_MIN);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetValue_WithDefault, "基本功能测试：带默认值获取") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_default.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "existing_key = existing_value\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("existing_key", "default_value", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "existing_value");

  ret = config.GetValue("non_existing_key", "default_value", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "default_value");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt32Value_WithDefault, "基本功能测试：带默认值获取int32") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int32_default.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "port = 8080\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int value = 0;
  ret = config.GetInt32Value("port", 9000, &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 8080);

  ret = config.GetInt32Value("non_existing_port", 9000, &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 9000);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_GetInt64Value_WithDefault, "基本功能测试：带默认值获取int64") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_int64_default.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);
  fprintf(fp, "big_number = 9999999999\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  int64_t value = 0;
  ret = config.GetInt64Value("big_number", 1234567890, &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 9999999999);

  ret = config.GetInt64Value("non_existing_number", 1234567890, &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, 1234567890);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_Comment, "基本功能测试：注释行处理") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_comment.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "# This is a comment\n");
  fprintf(fp, "key1 = value1\n");
  fprintf(fp, "  # Indented comment\n");
  fprintf(fp, "key2 = value2\n");
  fprintf(fp, "#key3 = value3\n");  // 注释掉的配置
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("key1", &value);
  EXPECT_EQ(ret, kOk);

  ret = config.GetValue("key2", &value);
  EXPECT_EQ(ret, kOk);

  ret = config.GetValue("key3", &value);
  EXPECT_EQ(ret, kNotFound);

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_EmptyLine, "基本功能测试：空行处理") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_emptyline.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "\n");
  fprintf(fp, "key1 = value1\n");
  fprintf(fp, "\n");
  fprintf(fp, "\n");
  fprintf(fp, "key2 = value2\n");
  fprintf(fp, "   \n");  // 只有空格的行
  fprintf(fp, "key3 = value3\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("key1", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value1");

  ret = config.GetValue("key2", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value2");

  ret = config.GetValue("key3", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value3");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_Whitespace, "基本功能测试：空白字符处理") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_whitespace.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "  key1  =  value1  \n");
  fprintf(fp, "key2=value2\n");
  fprintf(fp, "\tkey3\t=\tvalue3\t\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("key1", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value1");

  ret = config.GetValue("key2", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value2");

  ret = config.GetValue("key3", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value3");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_SpecialChars, "边界测试：特殊字符处理") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_special.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "path = /usr/local/bin\n");
  fprintf(fp, "url = http://example.com:8080/path?key=value\n");
  fprintf(fp, "special = value_with-dash.and_dot\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("path", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "/usr/local/bin");

  ret = config.GetValue("url", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "http://example.com:8080/path?key=value");

  ret = config.GetValue("special", &value);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(value, "value_with-dash.and_dot");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_DuplicateKeys, "边界测试：重复键处理") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_duplicate.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "duplicate_key = first_value\n");
  fprintf(fp, "duplicate_key = second_value\n");
  fprintf(fp, "duplicate_key = third_value\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  std::string value;
  ret = config.GetValue("duplicate_key", &value);
  EXPECT_EQ(ret, kOk);
  // 由于使用map::insert，第一个值会被保留
  EXPECT_EQ(value, "first_value");

  remove(tmp_file);
} /*}}}*/

TEST_D(Config, Test_Config_ConstMethods, "基本功能测试：const方法测试") { /*{{{*/
  using namespace base;
  Config config;

  const char *tmp_file = "/tmp/test_config_const.conf";
  FILE *fp = fopen(tmp_file, "w");
  EXPECT_NE(fp, nullptr);

  fprintf(fp, "string_value = test_string\n");
  fprintf(fp, "int32_value = 12345\n");
  fprintf(fp, "int64_value = 9876543210\n");
  fclose(fp);

  Code ret = config.LoadFile(tmp_file);
  EXPECT_EQ(ret, kOk);

  const Config &const_config = config;

  std::string str_val;
  ret = const_config.GetValue("string_value", &str_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(str_val, "test_string");

  int int32_val = 0;
  ret = const_config.GetInt32Value("int32_value", &int32_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int32_val, 12345);

  int64_t int64_val = 0;
  ret = const_config.GetInt64Value("int64_value", &int64_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int64_val, 9876543210);

  // 测试const版本的带默认值方法
  ret = const_config.GetValue("non_exist", "default", &str_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(str_val, "default");

  ret = const_config.GetInt32Value("non_exist", 999, &int32_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int32_val, 999);

  ret = const_config.GetInt64Value("non_exist", 888, &int64_val);
  EXPECT_EQ(ret, kOk);
  EXPECT_EQ(int64_val, 888);

  remove(tmp_file);
} /*}}}*/
