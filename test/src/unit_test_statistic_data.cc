// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/statistic_data.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(DataStatistic, TestCreate) {
  using namespace base;

  std::string path = "../log/data_stat_report.log";
  DataStatistic data_stat(path);
  Code ret = kOk;

  // Original Data
  std::string data_type = "OriData";
  uint64_t size = 530;
  ret = data_stat.AddStat(data_type, size);
  EXPECT_EQ(kOk, ret);

  size = 630;
  ret = data_stat.AddStat(data_type, size);
  EXPECT_EQ(kOk, ret);

  // Compres Data
  data_type = "ComData";
  size = 430;
  ret = data_stat.AddStat(data_type, size);
  EXPECT_EQ(kOk, ret);

  size = 330;
  ret = data_stat.AddStat(data_type, size);
  EXPECT_EQ(kOk, ret);

  // Dump Data
  ret = data_stat.DumpStat();
  EXPECT_EQ(kOk, ret);
}
