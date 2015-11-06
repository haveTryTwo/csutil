// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "base/common.h"
#include "base/file_util.h"

namespace base
{

Code CreateDir(const std::string &dir_path)
{/*{{{*/
    if (dir_path.empty()) return kInvalidParam;

    int ret = access(dir_path.c_str(), F_OK);
    if (ret == 0) return kOk;
        
    ret = mkdir(dir_path.c_str(), kDefaultMode);
    if (ret != 0) return kMkdirFailed;

    return kOk;
}/*}}}*/

}

#ifdef _FILE_UTIL_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;

    const std::string dir_path = "../demo/log";
    Code r = CreateDir(dir_path);
    if (r == kOk) 
        fprintf(stderr, "Dir:%s now exists\n", dir_path.c_str());
    else 
        fprintf(stderr, "Failed to create dir:%s\n", dir_path.c_str());

    return 0;
}
#endif
