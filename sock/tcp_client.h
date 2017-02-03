// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_TCP_CLIENT_H_
#define SOCK_TCP_CLIENT_H_

#include <string>

#include <stdint.h>

#include "base/event.h"
#include "base/status.h"

namespace base
{

/**
 * Note: This function is used to get data real length when reading data from stream.
 *       And user should define this data proto function.
 *
 * @return:
 *       0 : success, and real len is set
 *       1 : data in data_buf_ is not enough, and need to read stream again
 *       other : failed code
 */
typedef Code (*DataProtoFunc)(const char *src_data, int src_data_len, int *real_len);

Code DefaultProtoFunc(const char *src_data, int src_data_len, int *real_len);

class TcpClient
{/*{{{*/
    public:
        TcpClient();
        ~TcpClient();

        Code Init(EventType evt_type, DataProtoFunc data_proto_func);

        Code Connect(const std::string &ip, uint16_t port);
        Code ReConnect();

        // Send the data without modifying.
        Code SendNative(const std::string &data);

        // Receive data occarding to DataProtoFunc
        Code Recv(std::string *data); 

        void CloseConnect();

    private:
        Code RecvInternal();

    private:
        Event *ev_;
        int client_fd_;
        DataProtoFunc data_proto_func_;
        std::string serv_ip_;
        uint16_t serv_port_;

        char *data_buf_;
        int start_pos_;
        int end_pos_;
        int total_size_;
};/*}}}*/

}
#endif
