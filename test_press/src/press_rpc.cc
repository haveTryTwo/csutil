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
#include "test_press_base/include/test_rpc_client.h"

class PressRpcObject : public test::PressObject
{
    public:
        PressRpcObject(const std::string &test_name) : test::PressObject(test_name)
        {
            num_ = 0;
        }

        virtual ~PressRpcObject() 
        {
        }

    public:
        virtual test::PressObject* Create();

        virtual base::Code Init(const std::string &dst_ip);
        virtual base::Code ExecBody();

    private:
        int num_ ;
};

// NOTE: this is vary import! So register a press object
Register(PressRpc, PressRpcObject);

test::PressObject* PressRpcObject::Create()
{/*{{{*/
    return new PressRpcObject(*this);
}/*}}}*/

base::Code PressRpcObject::Init(const std::string &dst_ip_port_protos)
{/*{{{*/
    base::Code ret = test::PressObject::Init(dst_ip_port_protos);
    if (ret != base::kOk) return ret;

    return base::kOk;
}/*}}}*/

base::Code PressRpcObject::ExecBody()
{/*{{{*/
    if (num_ > 100000)
    {
        return base::kExitOk;
    }

    test::BusiClient *busi_client = GetBusiClient(test::kRpcStr);
    if (busi_client == NULL) 
    {
        LOG_ERR("Failed to get %s client", test::kRpcStr.c_str());
        return base::kInvalidParam;
    }
    std::string resp;
    base::Code ret = busi_client->SendAndRecv("client!", &resp);
    // LOG_ERR("resp:%s, ret:%d", resp.c_str(), ret);

    num_++;

    return ret;
}/*}}}*/

