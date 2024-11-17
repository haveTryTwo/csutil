// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/status.h"

#include "test_base/include/test_base.h"

#include "store/db/hash_db/src/hash_db.h"

TEST(HashDB, NormalFileInit) { /*{{{*/
  using namespace base;
  using namespace store;

  std::string pre_path = "../data/hash_db";
  DBBase *db = new HashDB();
  Code ret = db->Init(pre_path);
  EXPECT_EQ(kOk, ret);

  delete db;
} /*}}}*/

TEST(HashDB, NormalDataGet) { /*{{{*/
  using namespace base;
  using namespace store;

  std::string pre_path = "../data/hash_db";
  DBBase *db = new HashDB();
  Code ret = db->Init(pre_path);
  EXPECT_EQ(kOk, ret);

  std::string key = "key1";
  std::string value = "value1";
  std::string tmp_value;
  int64_t tmp_version = 0;

  ret = db->Put(key, value, -1);
  EXPECT_EQ(kOk, ret);

  ret = db->Get(key, &tmp_value, &tmp_version);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(value, tmp_value);

  ret = db->Del(key, tmp_version);
  EXPECT_EQ(kOk, ret);

  delete db;
} /*}}}*/

TEST(HashDB, NormalDataCasPutAndDel) { /*{{{*/
  using namespace base;
  using namespace store;

  std::string pre_path = "../data/hash_db";
  DBBase *db = new HashDB();
  Code ret = db->Init(pre_path);
  EXPECT_EQ(kOk, ret);

  std::string key = "key1";
  std::string value = "value1";
  std::string tmp_value;
  int64_t version = 1;
  int64_t tmp_version = 0;

  ret = db->Del(key, -1);
  EXPECT_EQ(kNotFound, ret);

  ret = db->Get(key, &tmp_value, &tmp_version);
  EXPECT_EQ(kNotFound, ret);

  // Put new data with wrong version
  ret = db->Put(key, value, 1);
  EXPECT_EQ(kCASFailed, ret);

  // Put new data with right version
  ret = db->Put(key, value, 0);
  EXPECT_EQ(kOk, ret);

  // Get the data
  ret = db->Get(key, &tmp_value, &tmp_version);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(value, tmp_value);
  EXPECT_EQ(version, tmp_version);

  // Put with wrong version
  ret = db->Put(key, value, tmp_version + 1);
  EXPECT_EQ(kCASFailed, ret);

  // Put with right version
  ret = db->Put(key, value, tmp_version);
  EXPECT_EQ(kOk, ret);

  // Get the data
  ret = db->Get(key, &tmp_value, &tmp_version);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(value, tmp_value);
  EXPECT_EQ(version + 1, tmp_version);

  // Del with wrong version
  ret = db->Del(key, tmp_version + 1);
  EXPECT_EQ(kCASFailed, ret);

  // Del with right version
  ret = db->Del(key, tmp_version);
  EXPECT_EQ(kOk, ret);

  ret = db->Get(key, &tmp_value, &tmp_version);
  EXPECT_EQ(kNotFound, ret);

  delete db;
} /*}}}*/

TEST(HashDB, BigDataForGet) { /*{{{*/
  using namespace base;
  using namespace store;

  std::string pre_path = "../data/hash_db";
  DBBase *db = new HashDB();
  Code ret = db->Init(pre_path);
  EXPECT_EQ(kOk, ret);

  uint32_t max_num = 200;
  char buf[8] = "\0";
  for (uint32_t i = 0; i < max_num; ++i) {
    snprintf(buf, sizeof(buf), "%u", (unsigned int)i);
    std::string key = std::string("key_") + buf;
    std::string value = std::string("value_") + buf;

    Code ret = db->Put(key, value, 0);
    EXPECT_EQ(kOk, ret);
  }

  for (uint32_t i = 0; i < max_num; ++i) {
    snprintf(buf, sizeof(buf), "%u", (unsigned int)i);
    std::string key = std::string("key_") + buf;
    std::string value = std::string("value_") + buf;

    int64_t tmp_version = 0;
    std::string tmp_value;
    Code ret = db->Get(key, &tmp_value, &tmp_version);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value, tmp_value);
    EXPECT_EQ(1, tmp_version);
  }

  DBStatusInfo status_info;
  status_info.Clear();
  ret = db->GetStatus(&status_info);
  EXPECT_EQ(kOk, ret);
  std::string tmp_info;
  status_info.ToString(&tmp_info);
  fprintf(stderr, "%s", tmp_info.c_str());

  for (uint32_t i = 0; i < max_num; ++i) {
    snprintf(buf, sizeof(buf), "%u", (unsigned int)i);
    std::string key = std::string("key_") + buf;

    int64_t tmp_version = 1;
    Code ret = db->Del(key, tmp_version);
    EXPECT_EQ(kOk, ret);
  }

  status_info.Clear();
  ret = db->GetStatus(&status_info);
  EXPECT_EQ(kOk, ret);
  status_info.ToString(&tmp_info);
  fprintf(stderr, "%s", tmp_info.c_str());

  delete db;
} /*}}}*/
