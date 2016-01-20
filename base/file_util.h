// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <vector>
#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base
{

Code CreateDir(const std::string &dir_path);
Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg);


Code GetNormalFilesName(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesName(const std::string &dir_path, std::deque<std::string> *files);

Code GetNormalFilesPath(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesPath(const std::string &dir_path, std::deque<std::string> *files);

Code GetLineContentAndRemoveNewLine(const std::string &path, std::vector<std::string> *contents);

Code GetFileSize(const std::string &file_path, uint64_t *file_size);
Code GetFilesSize(const std::vector<std::string> &files_path, uint64_t *files_size);
Code GetFilesSize(const std::string &dir_path, uint64_t *files_size);

Code MoveFile(const std::string &old_path, const std::string &new_path);

}
