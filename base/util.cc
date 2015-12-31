// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "base/log.h"
#include "base/util.h"
#include "base/common.h"

namespace base
{

Code Trim(const std::string &in_cnt, char delim, std::string *out_cnt)
{/*{{{*/
    std::string buf;
    Code ret = TrimLeft(in_cnt, delim, &buf);
    if (ret != kOk) return kOk;

    ret = TrimRight(buf, delim, out_cnt);
    return ret;
}/*}}}*/

Code TrimLeft(const std::string &in_cnt, char delim, std::string *out_cnt)
{/*{{{*/
    if (out_cnt == NULL) return kInvalidParam;

    int i = 0;
    for (; i < (int)in_cnt.size(); ++i)
    {
        if (in_cnt.data()[i] != delim) break;
    }

    out_cnt->assign(in_cnt.data()+i, in_cnt.size()-i);
    return kOk;
}/*}}}*/

Code TrimRight(const std::string &in_cnt, char delim, std::string *out_cnt)
{/*{{{*/
    if (out_cnt == NULL) return kInvalidParam;

    int i = in_cnt.size() - 1;
    for (; i >= 0; --i)
    {
        if (in_cnt.data()[i] != delim) break;
    }

    out_cnt->assign(in_cnt.data(), i+1);
    return kOk;
}/*}}}*/

Code Separate(const std::string &in_cnt, const std::string &delims,
        std::string *left_cnt, std::string *right_cnt)
{/*{{{*/
    std::size_t pos = in_cnt.find(delims);
    if (pos == std::string::npos) return kNotFound;

    left_cnt->assign(in_cnt.data(), pos);
    right_cnt->assign(in_cnt.data()+pos+delims.size(), in_cnt.size()-pos-delims.size());
    return kOk;
}/*}}}*/

Code Strtok(const std::string &cnt, char delim, std::deque<std::string> *words)
{/*{{{*/
    if (words == NULL) return kInvalidParam;

    Code ret = kOk;
    const char *start = cnt.c_str();
    
    while (true)
    {
        const char *pos = strchr(start, delim);
        if (pos == NULL)
        {
            words->push_back(start);
            break;
        }

        words->push_back(std::string(start, pos-start));
        start = pos+1;
    }

    return ret; 
}/*}}}*/

Code ToUpper(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    dst->clear();
    for (int i = 0; i < (int)src.size(); ++i)
    {
        if (src.data()[i] >= 'a' && src.data()[i] <= 'z')
            dst->append(1, src.data()[i]-32);
        else
            dst->append(src.data()+i, 1);
    }

    return kOk;
}/*}}}*/

Code GetAndSetMaxFileNo()
{/*{{{*/
    Code ret = kOk;

    struct rlimit rlim = {0};
    int r = getrlimit(RLIMIT_NOFILE, &rlim);
    if (r == -1)
    {
        LOG_ERR("Failed to get rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
        return kGetRlimitFailed;
    }
    LOG_INFO("before set! cur number of file descriptors:%d, max number of file descriptors:%d",
            (int)(rlim.rlim_cur), (int)(rlim.rlim_max));

    if (rlim.rlim_cur < rlim.rlim_max)
    {
        rlim.rlim_cur = rlim.rlim_max;

        r = setrlimit(RLIMIT_NOFILE, &rlim);
        if (r == -1)
        {
            LOG_ERR("Failed to set rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
            return kSetRlimitFailed;
        }
    }

    r = getrlimit(RLIMIT_NOFILE, &rlim);
    if (r == -1)
    {
        LOG_ERR("Failed to get rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
        return kGetRlimitFailed;
    }
    LOG_INFO("after set! cur number of file descriptors:%d, max number of file descriptors:%d",
            (int)(rlim.rlim_cur), (int)(rlim.rlim_max));

    return ret;
}/*}}}*/

}

#ifdef _UTIL_MAIN_TEST_
#include <assert.h>

int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

    std::string in_cnt("###zhang san###");
    std::string out_cnt;
    char delim = '#';

    Code ret = Trim(in_cnt, delim, &out_cnt);
    assert(ret == kOk);
    fprintf(stderr, "size:%lu, in_cnt:%s\n", in_cnt.size(), in_cnt.c_str());
    fprintf(stderr, "size:%lu, out_cnt:%s\n", out_cnt.size(), out_cnt.c_str());

    // Test Separete
    in_cnt.assign("name === wangwu");
    std::string left_cnt;
    std::string right_cnt;
    std::string delims = "===";
    ret = Separate(in_cnt, delims, &left_cnt, &right_cnt);
    assert(ret == kOk);
    fprintf(stderr, "in_cnt:%s, left:%s, right:%s\n", in_cnt.c_str(), left_cnt.c_str(), right_cnt.c_str());

    // Test strtok
    in_cnt.assign("aaa,bbb,cc,d");
    std::deque<std::string> columns;
    ret = Strtok(in_cnt, ',', &columns);
    assert(ret == kOk);
    std::deque<std::string>::iterator it; 
    fprintf(stderr, "size of columns:%zu\n", columns.size());
    for (it= columns.begin(); it != columns.end(); ++it)
        fprintf(stderr, "%s\n", it->c_str());
    fprintf(stderr, "\n");

    columns.clear();
    in_cnt.assign(",a,,b,");
    ret = Strtok(in_cnt, ',', &columns);
    assert(ret == kOk);
    fprintf(stderr, "size of columns:%zu\n", columns.size());
    for (it = columns.begin(); it != columns.end(); ++it)
        fprintf(stderr, "%s\n", it->c_str());
    fprintf(stderr, "\n");
    
    return 0;
}/*}}}*/
#endif
