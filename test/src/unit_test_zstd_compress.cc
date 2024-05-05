// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/zstd_compress.h"

#include <stdio.h>

#include "base/compress.h"
#include "base/common.h"
#include "base/status.h"
#include "base/random.h"

#include "test_base/include/test_base.h"

TEST_D(ZstdCompress, Test_Compress_Normal, "测试zstd compress") { /*{{{*/
  using namespace base;

  std::string default_source_data;
  Code ret = GetRandStr(1024, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::string sources[] = { "", "test havetrytwo", "null", "check good",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "CCCCCCCCCCXXXXBBBBBBBBBBB",
    default_source_data};
  
  std::string compressed;
  ZstdCompress zstd_compress;

  for (size_t i = 0; i < sizeof(sources)/sizeof(sources[0]); ++i) {
    Code ret = zstd_compress.Compress(sources[i], &compressed);
    EXPECT_EQ(ret, base::kOk);

    std::string uncompressed;
    ret = zstd_compress.Uncompress(compressed, &uncompressed);
    EXPECT_EQ(ret, kOk);

    fprintf(stderr, "uncompressed:%zu, compressed:%zu, ratio:%f\n",
        uncompressed.size(), compressed.size(), (double)compressed.size()/uncompressed.size());
    EXPECT_EQ(0, sources[i].compare(uncompressed));
  }
} /*}}}*/
