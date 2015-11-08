// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_UTIL_H_
#define BASE_UTIL_H_

#include <string>

#include <fcntl.h>
#include <sys/socket.h>

#include "base/status.h"

namespace base
{

inline Code SetFdNonblock(int fd)
{
    int flags = fcntl(fd, F_GETFD);
    int ret = fcntl(fd, F_SETFD, flags | O_NONBLOCK);
    if (ret == -1) return kFcntlFailed;

    return kOk;
}

inline Code SetFdReused(int fd)
{
    int opt = 1;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(int));
    if (ret == -1) return kSetsockoptFailed;
    return kOk;
}

Code Trim(const std::string &in_cnt, char delim, std::string *out_cnt);
Code TrimLeft(const std::string &in_cnt, char delim, std::string *out_cnt);
Code TrimRight(const std::string &in_cnt, char delim, std::string *out_cnt);

Code Separate(const std::string &in_cnt, const std::string &delims,
        std::string *left_cnt, std::string *right_cnt);


}

#endif
