// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include <stdio.h>
#include <assert.h>

#include "base/util.h"
#include "base/file_util.h"

#include "file_content_replace.h"

namespace tools
{

base::Code ReplaceAllCCFileContent(const std::string &dir, uint64_t replace_pos, uint64_t replace_len, const std::string &replace_str)
{/*{{{*/
    if (dir.empty() || replace_str.empty()) return base::kInvalidParam;
    if (replace_len != replace_str.size()) return base::kInvalidParam;

    std::vector<std::string> files_path;
    base::Code ret = base::GetNormalFilesPathRecurWithOutSort(dir, &files_path);
    assert(ret == base::kOk);

    std::vector<std::string>::iterator vec_it;
    for (vec_it = files_path.begin(); vec_it != files_path.end(); ++vec_it)
    {
        bool is_cpp_flags = false;
        ret = base::CheckIsCplusplusFile(*vec_it, &is_cpp_flags);
        assert(ret == base::kOk);
        if (!is_cpp_flags)
            continue;

        ret = base::ReplaceFileContent(*vec_it, replace_pos, replace_len, replace_str);
        if (ret != base::kOk)
        {
            fprintf(stderr, "Failed to replace cplusplus file:%s at pos:%llu to str:%s\n",
                    vec_it->c_str(), (unsigned long long)replace_pos, replace_str.c_str());
            return ret;
        }
        fprintf(stderr, "File:%s of position:%llu has been changed to %s\n",
                vec_it->c_str(), (unsigned long long)replace_pos, replace_str.c_str());
    }

    return ret;
}/*}}}*/


}
