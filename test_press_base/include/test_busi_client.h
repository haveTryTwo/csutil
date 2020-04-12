// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BUSI_CLIENT_H_
#define TEST_BUSI_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/time.h"
#include "base/status.h"
#include "http/http_proto.h"
#include "http/http_client.h"
#include "sock/tcp_client.h"
#include "strategy/singleton.h"

namespace test
{

class BusiClient
{
    public:
        BusiClient(const std::string &name) : client_name_(name), dst_port_(0)
        {/*{{{*/
        }/*}}}*/

        virtual ~BusiClient()
        {/*{{{*/
        }/*}}}*/

    public:
        virtual base::Code Init(const std::string& dst_ip_port_proto) = 0;
        virtual BusiClient* Create() = 0;

        // Note: Common method for rpc
        virtual base::Code SendAndRecv(const std::string &req, std::string *resp)
        {/*{{{*/
            return base::kInvalidParam;
        }/*}}}*/

        // Note: Common method for http
        virtual base::Code SendAndRecv(const std::string &req_relative_url, const std::string &post_params, std::string *resp)
        {/*{{{*/
            return base::kInvalidParam;
        }/*}}}*/

        // Note: Common method for asynchronous request
        // cmd: the number mark of request
        virtual base::Code SendAndRecv(uint16_t cmd, const std::string &req, std::string *resp)
        {/*{{{*/
            return base::kInvalidParam;
        }/*}}}*/

    public:
        const std::string& GetDstIpPort()
        {/*{{{*/
            return dst_ip_port_;
        }/*}}}*/

        const std::string& GetName()
        {/*{{{*/
            return client_name_;
        }/*}}}*/

    protected:
        std::string client_name_;
        std::string dst_ip_;
        uint16_t dst_port_;
        std::string dst_ip_port_;
};


}

#endif
