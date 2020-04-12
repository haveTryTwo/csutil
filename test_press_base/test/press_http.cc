// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "base/log.h"
#include "base/status.h"
#include "base/common.h"

#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_busi_client.h"

class PressHttpObject : public test::PressObject
{
    public:
        PressHttpObject(const std::string &test_name) : test::PressObject(test_name)
        {
            num_ = 0;
        }

        virtual ~PressHttpObject() 
        {
        }

    public:
        virtual test::PressObject* Create();

        virtual base::Code Init(const std::string &dst_ip);
        virtual base::Code ExecBody(test::ResultInfo *res_info);

    private:
        int num_ ;
};

// NOTE: this is vary import! So register a press object
Register(PressHttp, PressHttpObject);

test::PressObject* PressHttpObject::Create()
{/*{{{*/
    return new PressHttpObject(*this);
}/*}}}*/

base::Code PressHttpObject::Init(const std::string &dst_ip_port_protos)
{/*{{{*/
    base::Code ret = test::PressObject::Init(dst_ip_port_protos);
    if (ret != base::kOk) return ret;

    return base::kOk;
}/*}}}*/

base::Code PressHttpObject::ExecBody(test::ResultInfo *res_info)
{/*{{{*/
    if (num_ > 100000)
    {
        return base::kExitOk;
    }
    struct timeval cases_begin;
    struct timeval cases_end;
    gettimeofday(&cases_begin, NULL);

    std::string ip_port = "127.0.0.1:9123";
    std::string relative_url = "/";

    test::BusiClient *busi_client = GetBusiClient(ip_port);
    if (busi_client == NULL) return base::kInvalidParam;
    std::string resp;
    base::Code ret = busi_client->SendAndRecv(relative_url, "", &resp);
    // LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);

    gettimeofday(&cases_end, NULL);
    int64_t diff_all_time = (cases_end.tv_sec-cases_begin.tv_sec)*base::kUnitConvOfMicrosconds +
        (cases_end.tv_usec-cases_begin.tv_usec);

    res_info->total_num_ = 1;
    if (ret == base::kOk)
        res_info->succ_num_ = 1;
    else
        res_info->fail_num_ = 1;
    res_info->max_req_ms_ = diff_all_time/base::kThousand;

    num_++;

    return base::kOk;
}/*}}}*/
