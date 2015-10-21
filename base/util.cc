// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

}

#ifdef _UTIL_MAIN_TEST_
#include <assert.h>

int main(int argc, char *argv[])
{
    using namespace base;

    std::string in_cnt("###zhang san###");s
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

    return 0;
}
#endif