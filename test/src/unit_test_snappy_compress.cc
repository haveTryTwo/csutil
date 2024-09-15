// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/snappy_compress.h"

#include <stdio.h>

#include "base/common.h"
#include "base/compress.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST_D(SnappyCompress, Test_Compress_Normal, "测试Snappy compress") { /*{{{*/
  using namespace base;

  std::string default_source_data;
  Code ret = GetRandStr(1024, &default_source_data);
  EXPECT_EQ(kOk, ret);

  std::string sources[] = {"",
                           "test havetrytwo",
                           "null",
                           "check good",
                           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                           "CCCCCCCCCCXXXXBBBBBBBBBBB",
                           default_source_data};

  std::string compressed;
  SnappyCompress snappy_compress;

  for (size_t i = 0; i < sizeof(sources) / sizeof(sources[0]); ++i) {
    Code ret = snappy_compress.Compress(sources[i], &compressed);
    EXPECT_EQ(ret, base::kOk);

    std::string uncompressed;
    ret = snappy_compress.Uncompress(compressed, &uncompressed);
    EXPECT_EQ(ret, kOk);

    fprintf(stderr, "uncompressed:%zu, compressed:%zu, ratio:%f\n", uncompressed.size(), compressed.size(),
            (double)compressed.size() / uncompressed.size());
    EXPECT_EQ(0, sources[i].compare(uncompressed));
  }
} /*}}}*/
