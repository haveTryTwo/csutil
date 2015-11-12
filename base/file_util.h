// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/status.h"

namespace base
{

Code CreateDir(const std::string &dir_path);
Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg);

}
