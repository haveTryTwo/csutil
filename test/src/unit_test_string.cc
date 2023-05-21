// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>
#include <string>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/random.h"

#include "test_base/include/test_base.h"

TEST(String, Test_Press_10_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 10;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::string str;
  for (int i = 0; i < 10000; ++i) {
    str.clear();
    for (int j = 0; j < 10; ++j) {
      str += default_source_data;
    }
  }

  fprintf(stderr, "%s\n", str.c_str());
} /*}}}*/

TEST(String, Test_Press_100_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 100;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::string str;
  for (int i = 0; i < 10000; ++i) {
    str.clear();
    for (int j = 0; j < 10; ++j) {
      str += default_source_data;
    }
  }

  fprintf(stderr, "%s\n", str.c_str());
} /*}}}*/

TEST(String, Test_Press_1000_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 1000;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::string str;
  for (int i = 0; i < 10000; ++i) {
    str.clear();
    for (int j = 0; j < 10; ++j) {
      str += default_source_data;
    }
  }

  // fprintf(stderr, "%s\n", str.c_str());
} /*}}}*/

TEST(StringStream, Test_Press_10_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 10;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::ostringstream out;
  for (int i = 0; i < 10000; ++i) {
    out.seekp(std::ios_base::beg);
    for (int j = 0; j < 10; ++j) {
      out << default_source_data;
    }
  }

  fprintf(stderr, "%s\n", out.str().c_str());
} /*}}}*/

TEST(StringStream, Test_Press_100_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 100;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::ostringstream out;
  for (int i = 0; i < 10000; ++i) {
    out.seekp(std::ios_base::beg);
    for (int j = 0; j < 10; ++j) {
      out << default_source_data;
    }
  }

  fprintf(stderr, "%s\n", out.str().c_str());
} /*}}}*/

TEST(StringStream, Test_Press_1000_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  uint32_t source_data_len = 1000;
  std::string default_source_data;
  Code ret = GetRandStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::ostringstream out;
  for (int i = 0; i < 10000; ++i) {
    out.seekp(std::ios_base::beg);
    for (int j = 0; j < 10; ++j) {
      out << default_source_data;
    }
  }

  // fprintf(stderr, "%s\n", out.str().c_str());
} /*}}}*/

TEST(StringSplit, Test_Normal_SplitUTF8) { /*{{{*/
  using namespace base;

  std::vector<std::string> source_strs;
  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";
  source_strs.push_back(test_name);
  source_strs.push_back(check_str);
  source_strs.push_back("aa-bb+11_DD?mm*oo你是*(这)-=+我们me<gg");

  std::deque<std::string> dst_check_strs[3];
  { /*{{{*/
    dst_check_strs[0].push_back("到");
    dst_check_strs[0].push_back("词");

    dst_check_strs[1].push_back("腾");
    dst_check_strs[1].push_back("讯");
    dst_check_strs[1].push_back("科");
    dst_check_strs[1].push_back("技");
    dst_check_strs[1].push_back("（");
    dst_check_strs[1].push_back("深");
    dst_check_strs[1].push_back("圳");
    dst_check_strs[1].push_back("）");
    dst_check_strs[1].push_back("有");
    dst_check_strs[1].push_back("限");
    dst_check_strs[1].push_back("公");
    dst_check_strs[1].push_back("司");
    dst_check_strs[1].push_back("太");
    dst_check_strs[1].push_back("太");
    dst_check_strs[1].push_back("乐");
    dst_check_strs[1].push_back("的");
    dst_check_strs[1].push_back("公");
    dst_check_strs[1].push_back("式");
    dst_check_strs[1].push_back("拉");
    dst_check_strs[1].push_back("升");
    dst_check_strs[1].push_back("的");
    dst_check_strs[1].push_back("提");
    dst_check_strs[1].push_back("升");
    dst_check_strs[1].push_back("到");
    dst_check_strs[1].push_back("词");
    dst_check_strs[1].push_back("汇");
    dst_check_strs[1].push_back("和");
    dst_check_strs[1].push_back("到");
    dst_check_strs[1].push_back("词");

    dst_check_strs[2].push_back("aa-bb+11_DD");
    dst_check_strs[2].push_back("?");
    dst_check_strs[2].push_back("mm");
    dst_check_strs[2].push_back("*");
    dst_check_strs[2].push_back("oo");
    dst_check_strs[2].push_back("你");
    dst_check_strs[2].push_back("是");
    dst_check_strs[2].push_back("*");
    dst_check_strs[2].push_back("(");
    dst_check_strs[2].push_back("这");
    dst_check_strs[2].push_back(")");
    dst_check_strs[2].push_back("-");
    dst_check_strs[2].push_back("=");
    dst_check_strs[2].push_back("+");
    dst_check_strs[2].push_back("我");
    dst_check_strs[2].push_back("们");
    dst_check_strs[2].push_back("me");
    dst_check_strs[2].push_back("<");
    dst_check_strs[2].push_back("gg");
  } /*}}}*/

  for (size_t i = 0; i < source_strs.size(); ++i) {
    std::deque<std::string> out;
    Code ret = SplitUTF8(source_strs[i], &out);
    EXPECT_EQ(ret, kOk);

    fprintf(stderr, "source:%s\n", source_strs[i].c_str());
    // std::deque<std::string>::iterator it;
    // for (it = out.begin(); it != out.end(); ++it)
    // {
    //     fprintf(stderr, "%s\n", it->c_str());
    // }

    int r = CheckEqual(dst_check_strs[i], out);
    EXPECT_EQ(r, 0);
    fprintf(stderr, "\n");
  }
} /*}}}*/

TEST(StringFind, Test_Press_BruteForce_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  int pos = 0;
  Code ret = kOk;
  for (int i = 0; i < 1000000; ++i) {
    ret = BruteForce(check_str, test_name, &pos);
  }
  EXPECT_EQ(ret, kOk);
  if (ret == kOk) {
    fprintf(stderr, "pos:%d, chekc_str size:%zu\n", pos, check_str.size());

    std::string highlight_str;
    ret = GetHighlighting(check_str, test_name, pos, kDefaultPreTags, kDefaultPostTags,
                          &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  } else {
    fprintf(stderr, "Failed to exectue BruteForce(), ret:%d\n", ret);
  }
} /*}}}*/

TEST(StringFind, Test_Press_KMP_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  int pos = 0;
  Code ret = kOk;
  for (int i = 0; i < 1000000; ++i) {
    ret = KMP(check_str, test_name, &pos);
  }
  EXPECT_EQ(ret, kOk);
  if (ret == kOk) {
    fprintf(stderr, "pos:%d, chekc_str size:%zu\n", pos, check_str.size());

    std::string highlight_str;
    ret = GetHighlighting(check_str, test_name, pos, kDefaultPreTags, kDefaultPostTags,
                          &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  } else {
    fprintf(stderr, "Failed to exectue BruteForce(), ret:%d\n", ret);
  }
} /*}}}*/

TEST(StringFind, Test_Press_BM_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  int pos = 0;
  Code ret = kOk;
  for (int i = 0; i < 1000000; ++i) {
    ret = BM(check_str, test_name, &pos);
  }
  EXPECT_EQ(ret, kOk);
  if (ret == kOk) {
    fprintf(stderr, "pos:%d, chekc_str size:%zu\n", pos, check_str.size());

    std::string highlight_str;
    ret = GetHighlighting(check_str, test_name, pos, kDefaultPreTags, kDefaultPostTags,
                          &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  } else {
    fprintf(stderr, "Failed to exectue BruteForce(), ret:%d\n", ret);
  }
} /*}}}*/

TEST(StringFind, Test_Press_RK_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  int pos = 0;
  Code ret = kOk;
  for (int i = 0; i < 1000000; ++i) {
    ret = RK(check_str, test_name, &pos);
  }
  EXPECT_EQ(ret, kOk);
  if (ret == kOk) {
    fprintf(stderr, "pos:%d, chekc_str size:%zu\n", pos, check_str.size());

    std::string highlight_str;
    ret = GetHighlighting(check_str, test_name, pos, kDefaultPreTags, kDefaultPostTags,
                          &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  } else {
    fprintf(stderr, "Failed to exectue BruteForce(), ret:%d\n", ret);
  }
} /*}}}*/

TEST(StringFind, Test_Press_StringFind_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  size_t pos = 0;
  for (int i = 0; i < 1000000; ++i) {
    pos = check_str.find(test_name);
  }
  if (pos != std::string::npos) {
    std::string base_check_str;
    Code ret = Base64Encode(check_str, &base_check_str);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "pos:%zu, chekc_str size:%zu, base64_check_str size:%zu\n", pos,
            check_str.size(), base_check_str.size());

    std::string highlight_str;
    ret = GetHighlighting(check_str, test_name, pos, kDefaultPreTags, kDefaultPostTags,
                          &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  }
} /*}}}*/

TEST(StringFind, Test_Press_StrStr_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  const char *str_pos = NULL;
  for (int i = 0; i < 1000000; ++i) {
    str_pos = strstr(check_str.c_str(), test_name.c_str());
  }
  if (str_pos != NULL) {
    fprintf(stderr, "pos:%zu, check_str size:%zu\n", (str_pos - check_str.data()),
            check_str.size());
    std::string highlight_str;
    Code ret = GetHighlighting(check_str, test_name, (str_pos - check_str.data()), kDefaultPreTags,
                               kDefaultPostTags, &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  }
} /*}}}*/

TEST(StringFind, Test_Press_MemMem_One_Million) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  void *str_pos = NULL;
  for (int i = 0; i < 1000000; ++i) {
    str_pos = memmem(const_cast<char *>(check_str.data()), check_str.size(),
                     const_cast<char *>(test_name.data()), test_name.size());
  }
  if (str_pos != NULL) {
    fprintf(stderr, "pos:%zu, check_str size:%zu\n", ((char *)str_pos - check_str.data()),
            check_str.size());

    std::string highlight_str;
    Code ret = GetHighlighting(check_str, test_name, ((char *)str_pos - check_str.data()),
                               kDefaultPreTags, kDefaultPostTags, &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", test_name.c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  }
} /*}}}*/

TEST(StringFind, Test_Press_StrStr_One_Million_SplitUTF8) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  std::deque<std::string> out;
  Code ret = SplitUTF8(test_name, &out);
  EXPECT_EQ(ret, kOk);
  std::deque<const char *> str_pos_c(out.size(), NULL);

  for (int i = 0; i < 1000000; ++i) {
    for (int j = 0; j < (int)out.size(); ++j) {
      str_pos_c[j] = strstr(check_str.c_str(), out[j].c_str());
    }
  }

  if (str_pos_c[0] != NULL) {
    fprintf(stderr, "pos:%zu, check_str size:%zu\n", (str_pos_c[0] - check_str.data()),
            check_str.size());
    std::string highlight_str;
    Code ret = GetHighlighting(check_str, out[0], (str_pos_c[0] - check_str.data()),
                               kDefaultPreTags, kDefaultPostTags, &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", out[0].c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  }
} /*}}}*/

TEST(StringFind, Test_Press_MemMem_One_Million_SplitUTF8) { /*{{{*/
  using namespace base;

  std::string test_name = "到词";
  std::string check_str = "腾讯科技（深圳）有限公司太太乐的公式拉升的提升到词汇和到词";

  std::deque<std::string> out;
  Code ret = SplitUTF8(test_name, &out);
  EXPECT_EQ(ret, kOk);
  std::deque<void *> str_pos_c(out.size(), NULL);

  for (int i = 0; i < 1000000; ++i) {
    for (int j = 0; j < (int)out.size(); ++j) {
      str_pos_c[j] = memmem(const_cast<char *>(check_str.data()), check_str.size(),
                            const_cast<char *>(out[j].data()), out[j].size());
    }
  }
  if (str_pos_c[0] != NULL) {
    fprintf(stderr, "pos:%zu, check_str size:%zu\n", ((char *)str_pos_c[0] - check_str.data()),
            check_str.size());

    std::string highlight_str;
    Code ret = GetHighlighting(check_str, out[0], ((char *)str_pos_c[0] - check_str.data()),
                               kDefaultPreTags, kDefaultPostTags, &highlight_str);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "search string:    %s\n", out[0].c_str());
    fprintf(stderr, "source string:    %s\n", check_str.c_str());
    fprintf(stderr, "highlight string: %s\n", highlight_str.c_str());
  }
} /*}}}*/
