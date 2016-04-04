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


Code GetNormalFilesNameWithOutSort(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesNameWithOutSort(const std::string &dir_path, std::deque<std::string> *files);
Code GetNormalFilesName(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesName(const std::string &dir_path, std::deque<std::string> *files);


Code GetNormalFilesPathWithOutSort(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesPathWithOutSort(const std::string &dir_path, std::deque<std::string> *files);
Code GetNormalFilesPath(const std::string &dir_path, std::vector<std::string> *files);
Code GetNormalFilesPath(const std::string &dir_path, std::deque<std::string> *files);

Code GetNormalFilesPathWithOutSort(const std::vector<std::string> &dirs_path, std::vector<std::string> *files);
Code GetNormalFilesPath(const std::vector<std::string> &dirs_path, std::vector<std::string> *files);

Code GetNormalFilesPathWithOutSort(const std::string &src_dir_path, std::vector<std::string> *src_files, const std::string &dst_dir_path, std::vector<std::string> *dst_files);

Code GetLineContentAndRemoveNewLine(const std::string &path, std::vector<std::string> *contents);

Code GetFileSize(const std::string &file_path, uint64_t *file_size);
Code GetFilesSize(const std::vector<std::string> &files_path, uint64_t *files_size);
Code GetFilesSize(const std::string &dir_path, uint64_t *files_size);

Code MoveFile(const std::string &old_path, const std::string &new_path);

// NOTE: 1. empty data is not supported to be written
Code DumpBinData(const std::string &bin_str, FILE *fp);
Code PumpBinData(std::string *bin_str, FILE *fp);

// NOTE: 1. empty data is not supported to be written
Code DumpStringData(const std::string &str, FILE *fp);
Code PumpStringData(std::string *str, FILE *fp);

}
