// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/file_util.h"
#include "base/status.h"
#include "base/util.h"

#include "test_base/include/test_base.h"

TEST(GetFileSystemSize, RootPath) { /*{{{*/
  using namespace base;

  std::string root_sys_path = "/";
  uint64_t total_size = 0;
  uint64_t free_size = 0;
  float total_size_output = 0;
  std::string total_size_suffix;
  float free_size_output = 0;
  std::string free_size_suffix;

  Code ret = GetFileSystemSize(root_sys_path, &total_size, &free_size);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(total_size, &total_size_output, &total_size_suffix);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(free_size, &free_size_output, &free_size_suffix);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "sys_path:%s, total_size:%.1f%s, free_size:%.1f%s\n", root_sys_path.c_str(),
          total_size_output, total_size_suffix.c_str(), free_size_output, free_size_suffix.c_str());
} /*}}}*/

TEST(GetFileSystemSize, HomePath) { /*{{{*/
  using namespace base;

  std::string home_path = "/home";
  uint64_t total_size = 0;
  uint64_t free_size = 0;

  float total_size_output = 0;
  std::string total_size_suffix;
  float free_size_output = 0;
  std::string free_size_suffix;

  Code ret = GetFileSystemSize(home_path, &total_size, &free_size);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(total_size, &total_size_output, &total_size_suffix);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(free_size, &free_size_output, &free_size_suffix);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "sys_path:%s, total_size:%.1f%s, free_size:%.1f%s\n", home_path.c_str(),
          total_size_output, total_size_suffix.c_str(), free_size_output, free_size_suffix.c_str());
} /*}}}*/

TEST(GetFileSystemSize, DataPath) { /*{{{*/
  using namespace base;

  std::string data_path = "/tmp";
  uint64_t total_size = 0;
  uint64_t free_size = 0;

  float total_size_output = 0;
  std::string total_size_suffix;
  float free_size_output = 0;
  std::string free_size_suffix;

  Code ret = GetFileSystemSize(data_path, &total_size, &free_size);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(total_size, &total_size_output, &total_size_suffix);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(free_size, &free_size_output, &free_size_suffix);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "sys_path:%s, total_size:%.1f%s, free_size:%.1f%s\n", data_path.c_str(),
          total_size_output, total_size_suffix.c_str(), free_size_output, free_size_suffix.c_str());
} /*}}}*/

TEST(GetFileSystemSize, VolumesPath) { /*{{{*/
  using namespace base;

  std::string volumes_path = "/Volumes/MacRTX";
  uint64_t total_size = 0;
  uint64_t free_size = 0;

  float total_size_output = 0;
  std::string total_size_suffix;
  float free_size_output = 0;
  std::string free_size_suffix;

  Code ret = GetFileSystemSize(volumes_path, &total_size, &free_size);

  if (ret == kOk) {
    ret = GetOutputSuffix(total_size, &total_size_output, &total_size_suffix);
    EXPECT_EQ(kOk, ret);

    ret = GetOutputSuffix(free_size, &free_size_output, &free_size_suffix);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "sys_path:%s, total_size:%.1f%s, free_size:%.1f%s\n", volumes_path.c_str(),
            total_size_output, total_size_suffix.c_str(), free_size_output,
            free_size_suffix.c_str());
  }
} /*}}}*/

TEST(GetFileSystemSize, RootPathOneMillonTest) { /*{{{*/
  using namespace base;

  std::string root_sys_path = "/";
  uint64_t total_size = 0;
  uint64_t free_size = 0;
  float total_size_output = 0;
  std::string total_size_suffix;
  float free_size_output = 0;
  std::string free_size_suffix;

  Code ret = kOk;
  for (uint32_t i = 0; i < 1000000; ++i) {
    ret = GetFileSystemSize(root_sys_path, &total_size, &free_size);
    EXPECT_EQ(kOk, ret);
  }

  ret = GetOutputSuffix(total_size, &total_size_output, &total_size_suffix);
  EXPECT_EQ(kOk, ret);

  ret = GetOutputSuffix(free_size, &free_size_output, &free_size_suffix);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "sys_path:%s, total_size:%.1f%s, free_size:%.1f%s\n", root_sys_path.c_str(),
          total_size_output, total_size_suffix.c_str(), free_size_output, free_size_suffix.c_str());
} /*}}}*/

TEST(GetNormalFilesPathRecurWithOutSort, TestDir) { /*{{{*/
  using namespace base;

  std::string test_dir = "../../test";
  std::vector<std::string> files_path;
  Code ret = GetNormalFilesPathRecurWithOutSort(test_dir, &files_path);
  EXPECT_EQ(kOk, ret);
  std::vector<std::string>::iterator it;
  for (it = files_path.begin(); it != files_path.end(); ++it) {
    fprintf(stderr, "%s\n", it->c_str());
  }
} /*}}}*/

TEST(GetNormalFilesPathRecurWithOutSort, EXP_InvalidDir) { /*{{{*/
  using namespace base;

  std::string test_dir = "../../a";
  std::vector<std::string> files_path;
  Code ret = GetNormalFilesPathRecurWithOutSort(test_dir, &files_path);
  EXPECT_EQ(kStatFailed, ret);
} /*}}}*/
