// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "base/msg.h"
#include "base/coding.h"
#include "base/common.h"

namespace base
{

Code EncodeToMsg(const std::string &data, std::string *msg)
{
    if (msg == NULL) return kInvalidParam;

    std::string buf;
    Code ret = EncodeFixed32(data.size(), &buf);
    assert(ret == kOk);
    msg->assign(buf);
    msg->append(data);

    return kOk;
}

Code DecodeFromMsg(const std::string &msg, std::string *data)
{
    if (data == NULL || (int)msg.size() < kHeadLen) return kInvalidParam;

    uint32_t len = 0;
    Code ret = DecodeFixed32(msg, &len);
    assert(ret == kOk);
    assert(len == (msg.size()-kHeadLen));
    data->assign(msg.data()+kHeadLen, len);

    return kOk;
}

void PrintBinMsg(const std::string &str)
{
    for (int i = 0; i < (int)str.size(); ++i)
    {
        if (i % 8 == 0) fprintf(stderr, "\n");
        fprintf(stderr, "%02x ", *(uint8_t*)(str.data()+i));
    }
}

}

#ifdef _MSG_MAIN_TEST_
#include <stdio.h>

int main(int argc, char *argv[])
{
    using namespace base;

    std::string data("i ok, thank you very much");
    std::string msg;
    Code ret = EncodeToMsg(data, &msg);
    assert(ret == kOk);

    fprintf(stderr, "msg:%s\n", msg.c_str()+4);

    std::string in_data;
    ret = DecodeFromMsg(msg, &in_data);
    fprintf(stderr, "in data:%s\n", in_data.c_str());

    return 0;
}
#endif
