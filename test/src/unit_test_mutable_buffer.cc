// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/coding.h"
#include "base/mutable_buffer.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(MutableBuffer, Test_Normal_Append_Data) { /*{{{*/
  using namespace base;
  MutableBuffer buf(8);

  // 1. Add "abcd"
  std::string dest_buf = "abcd";
  Code ret = buf.Append(dest_buf.data(), dest_buf.size());
  EXPECT_EQ(kOk, ret);

  const char* data1 = buf.RealDataPtr();
  uint32_t data1_size = buf.RealDataSize();
  EXPECT_EQ(dest_buf.size(), data1_size);
  EXPECT_EQ(0, memcmp(dest_buf.data(), data1, dest_buf.size()));

  // 2. Append "defg"
  ret = buf.Append("defg", 4);
  EXPECT_EQ(kOk, ret);

  const char* data2 = buf.RealDataPtr();
  uint32_t data2_size = buf.RealDataSize();

  EXPECT_EQ(8, data2_size);
  EXPECT_EQ(0, memcmp("defg", data2 + 4, 4));
  EXPECT_EQ(data1, data2);

  // 3. Append "mnih";
  ret = buf.Append("mnih", 4);
  EXPECT_EQ(kOk, ret);

  const char* data3 = buf.RealDataPtr();
  uint32_t data3_size = buf.RealDataSize();

  EXPECT_EQ(12, data3_size);
  EXPECT_EQ(0, memcmp("mnih", data3 + 8, 4));
  EXPECT_NEQ(data1, data3);
} /*}}}*/

TEST(MutableBuffer, Test_Normal_Skip_Data) { /*{{{*/
  using namespace base;
  MutableBuffer buf(8);

  // 1. Add "abcd"
  std::string dest_buf = "abcd";
  Code ret = buf.Append(dest_buf.data(), dest_buf.size());
  EXPECT_EQ(kOk, ret);

  const char* data1 = buf.RealDataPtr();
  uint32_t data1_size = buf.RealDataSize();
  EXPECT_EQ(dest_buf.size(), data1_size);
  EXPECT_EQ(0, memcmp(dest_buf.data(), data1, dest_buf.size()));

  // 2. Append "defg"
  ret = buf.Append("defg", 4);
  EXPECT_EQ(kOk, ret);

  const char* data2 = buf.RealDataPtr();
  uint32_t data2_size = buf.RealDataSize();

  EXPECT_EQ(8, data2_size);
  EXPECT_EQ(0, memcmp("defg", data2 + 4, 4));
  EXPECT_EQ(data1, data2);

  // 3. Skip "abcd"
  ret = buf.Skip(data1_size);
  EXPECT_EQ(kOk, ret);
  data2 = buf.RealDataPtr();
  data2_size = buf.RealDataSize();
  EXPECT_EQ(data2_size, 4);
  EXPECT_EQ(0, memcmp("defg", data2, 4));
  EXPECT_NEQ(data1, data2);

  // 4. Skip "defg"
  ret = buf.Skip(4);
  EXPECT_EQ(kOk, ret);
  data2 = buf.RealDataPtr();
  data2_size = buf.RealDataSize();
  EXPECT_EQ(data2_size, 0);
  EXPECT_EQ(data1, data2);
} /*}}}*/

TEST(MutableBuffer, Test_Normal_Skip_To_Null) { /*{{{*/
  using namespace base;
  MutableBuffer buf(8);

  // 1. Add "abcd"
  std::string dest_buf = "abcd";
  Code ret = buf.Append(dest_buf.data(), dest_buf.size());
  EXPECT_EQ(kOk, ret);

  const char* data1 = buf.RealDataPtr();
  uint32_t data1_size = buf.RealDataSize();
  EXPECT_EQ(dest_buf.size(), data1_size);
  EXPECT_EQ(0, memcmp(dest_buf.data(), data1, dest_buf.size()));

  // 2. Append "defg"
  ret = buf.Append("defg", 4);
  EXPECT_EQ(kOk, ret);

  const char* data2 = buf.RealDataPtr();
  uint32_t data2_size = buf.RealDataSize();

  EXPECT_EQ(8, data2_size);
  EXPECT_EQ(0, memcmp("defg", data2 + 4, 4));
  EXPECT_EQ(data1, data2);

  // 3. Append "mnih";
  ret = buf.Append("mnih", 4);
  EXPECT_EQ(kOk, ret);

  const char* data3 = buf.RealDataPtr();
  uint32_t data3_size = buf.RealDataSize();

  EXPECT_EQ(12, data3_size);
  EXPECT_EQ(0, memcmp("mnih", data3 + 8, 4));
  EXPECT_NEQ(data1, data3);

  // 4. Skip "abcd"
  ret = buf.Skip(data1_size);
  EXPECT_EQ(kOk, ret);
  data2 = buf.RealDataPtr();
  data2_size = buf.RealDataSize();
  EXPECT_EQ(data2_size, 8);
  EXPECT_EQ(0, memcmp("defg", data2, 4));
  EXPECT_NEQ(data1, data2);

  // 5. Skip "defg"
  ret = buf.Skip(4);
  EXPECT_EQ(kOk, ret);
  data3 = buf.RealDataPtr();
  data3_size = buf.RealDataSize();
  EXPECT_EQ(data3_size, 4);
  EXPECT_EQ(0, memcmp("mnih", data3, 4));
  EXPECT_NEQ(data1, data3);

  // 6. Skip "mnih"
  ret = buf.Skip(4);
  EXPECT_EQ(kOk, ret);
  data3 = buf.RealDataPtr();
  data3_size = buf.RealDataSize();
  EXPECT_EQ(data3_size, 0);
  EXPECT_EQ(NULL, data3);
} /*}}}*/

TEST(MutableBuffer, Test_Normal_Move_Head) { /*{{{*/
  using namespace base;
  MutableBuffer buf(8);

  // 1. Add "abcd"
  std::string dest_buf = "ab";
  Code ret = buf.Append(dest_buf.data(), dest_buf.size());
  EXPECT_EQ(kOk, ret);

  const char* data1 = buf.RealDataPtr();
  uint32_t data1_size = buf.RealDataSize();
  EXPECT_EQ(dest_buf.size(), data1_size);
  EXPECT_EQ(0, memcmp(dest_buf.data(), data1, dest_buf.size()));

  // 2. Append "defg"
  ret = buf.Append("defg", 4);
  EXPECT_EQ(kOk, ret);

  const char* data2 = buf.RealDataPtr();
  uint32_t data2_size = buf.RealDataSize();

  EXPECT_EQ(6, data2_size);
  EXPECT_EQ(0, memcmp("defg", data2 + 2, 4));
  EXPECT_EQ(data1, data2);

  // 3. Skip "abcd"
  ret = buf.Skip(data1_size);
  EXPECT_EQ(kOk, ret);
  data2 = buf.RealDataPtr();
  data2_size = buf.RealDataSize();
  EXPECT_EQ(data2_size, 4);
  EXPECT_EQ(0, memcmp("defg", data2, 4));
  EXPECT_EQ(data1 + 2, data2);

  // 3. Append "mnih";
  ret = buf.Append("mni", 3);
  EXPECT_EQ(kOk, ret);

  const char* data3 = buf.RealDataPtr();
  uint32_t data3_size = buf.RealDataSize();

  EXPECT_EQ(7, data3_size);
  EXPECT_EQ(data1, data3);
  EXPECT_EQ(0, memcmp("defgmni", data3, 7));
} /*}}}*/

TEST(MutableBuffer, Test_Normal_Press_OneMillion) { /*{{{*/
  using namespace base;
  MutableBuffer buf;

  std::string tmp_str;
  for (int i = 0; i < 101; ++i) {
    tmp_str.append(1, 'a' + i % 26);
  }
  EXPECT_EQ(101, tmp_str.size());

  for (int i = 0; i < 1000; ++i) {
    for (int j = 0; j < 1000; ++j) {
      Code ret = buf.Append(tmp_str.data(), tmp_str.size());
      EXPECT_EQ(kOk, ret);
    }

    for (int j = 0; j < 999; ++j) {
      Code ret = buf.Skip(tmp_str.size());
      EXPECT_EQ(kOk, ret);
    }
  }
} /*}}}*/

TEST(MutableBuffer, Test_Exception_Extend_Max) { /*{{{*/
  using namespace base;
  MutableBuffer buf(0);

  std::string tmp_str;
  for (int i = 0; i < 100; ++i) {
    tmp_str.append(1, 'a' + i % 26);
  }
  EXPECT_EQ(100, tmp_str.size());

  for (int i = 0; i < 10485; ++i) {
    Code ret = buf.Append(tmp_str.data(), tmp_str.size());
    EXPECT_EQ(kOk, ret);
  }
  Code ret = buf.Append(tmp_str.data(), tmp_str.size());
  EXPECT_EQ(kInvalidLength, ret);
} /*}}}*/
