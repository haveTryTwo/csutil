// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <vector>
#include <string>

#include "base/status.h"

namespace base
{

Code CreateDir(const std::string &dir_path);
Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg);


Code GetNormalFiles(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFiles(const std::string &dir_path, std::deque<std::string> *files);


Code GetLineContentAndRemoveNewLine(const std::string &path, std::vector<std::string> *contents);

}
