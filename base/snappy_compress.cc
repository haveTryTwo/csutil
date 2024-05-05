// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/snappy_compress.h"

#include "snappy.h"

namespace base {

Code SnappyCompress::Compress(const std::string &uncompressed, std::string *compressed) {
  if (compressed == NULL) return kInvalidParam;

  snappy::Compress(uncompressed.data(), uncompressed.size(), compressed);
  return kOk;
}

Code SnappyCompress::Uncompress(const std::string &compressed, std::string *uncompressed) {
  if (uncompressed == NULL) return kInvalidParam;

  bool ret = snappy::Uncompress(compressed.data(), compressed.size(), uncompressed);
  if (!ret) return kFailedToUncompress;

  return kOk;
}

}  // namespace base
