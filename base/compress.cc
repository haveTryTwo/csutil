// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/compress.h"

namespace base {

Code BaseCompress::Compress(const std::string &uncompressed, std::string *compressed) { return kOtherFailed; }

Code BaseCompress::Uncompress(const std::string &compressed, std::string *uncompressed) { return kOtherFailed; }

}  // namespace base
