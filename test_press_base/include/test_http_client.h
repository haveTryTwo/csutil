// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_HTTP_BUSI_CLIENT_H_
#define TEST_HTTP_BUSI_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/time.h"
#include "base/status.h"
#include "http/http_proto.h"
#include "http/http_client.h"
#include "sock/tcp_client.h"
#include "strategy/singleton.h"

#include "test_press_base/include/test_busi_client.h"

namespace test
{

const std::string kHttpStr = "http";

const std::string kHttpGet = "GET";
const std::string kHttpPost = "POST";

class HttpBusiClient : public BusiClient
{
    public:
        HttpBusiClient(const std::string &client_name);
        virtual ~HttpBusiClient();

    public:
        virtual base::Code Init(const std::string& dst_ip_port_proto);
        virtual BusiClient* Create();

    public:
        virtual base::Code SendAndRecv(const std::string &req_relative_url, const std::string &post_params, std::string *resp);

    private:
        bool is_init_;
        base::HttpType http_type_; // GET or POST
        base::HttpClient http_client_;
};

}

#endif

