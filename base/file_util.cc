// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "base/common.h"
#include "base/file_util.h"

namespace base
{

Code CreateDir(const std::string &dir_path)
{/*{{{*/
    if (dir_path.empty()) return kInvalidParam;

    int ret = access(dir_path.c_str(), F_OK);
    if (ret == 0) return kOk;
        
    ret = mkdir(dir_path.c_str(), kDefaultDirMode);
    if (ret != 0) return kMkdirFailed;

    return kOk;
}/*}}}*/

Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg)
{/*{{{*/
    int fd = open(path.c_str(), O_RDWR|O_CREAT, kDefaultRegularFileMode);
    if (fd == -1) return kOpenFileFailed;

    char buf[kBufLen] = {0};
    int ret = read(fd, buf, sizeof(buf));
    if (ret == -1) return kReadError;

    if (ret == (int)msg.size() && (memcmp(buf, msg.c_str(), ret) == 0))
    {
        close(fd);
        return kOk;
    }

    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);   // truncate the size of to 0 as the size may be larger than message
    ret = write(fd, msg.c_str(), msg.size());
    if (ret != (int)msg.size())
    {
        close(fd);
        return kWriteError;
    }

    close(fd);
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

    const std::string test_log_path = dir_path + "/test.log";
    CompareAndWriteWholeFile(test_log_path, "cc");

    return 0;
}
#endif
