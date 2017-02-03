// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CS_CLIENT_H_
#define CS_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/status.h"
#include "base/event.h"

namespace base
{

class Client
{
    public:
        Client();
        ~Client();

    public:
        Code Init(EventType evt_type);

        Code Connect(const std::string &ip, uint16_t port);
        void CloseConnect();

        Code Write(const std::string &data);
        Code Read(std::string *data);
    private:
        Code ReadInternal(char *buf, int buf_len);

    private:
        int client_fd_;
        Event *ev_;
};

}

#endif
