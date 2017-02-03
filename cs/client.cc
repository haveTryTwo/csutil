// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "base/msg.h"
#include "base/util.h"
#include "base/common.h"
#include "base/coding.h"

#include "cs/client.h"
#include "cs/version.h"

namespace base
{

Client::Client() : client_fd_(-1), ev_(NULL)
{}

Client::~Client()
{/*{{{*/
    CloseConnect();

    if (ev_ != NULL)
    {
        delete ev_;
        ev_ = NULL;
    }
}/*}}}*/

Code Client::Init(EventType evt_type)
{/*{{{*/
    Code ret = kOk;
    switch (evt_type)
    {
        case kSelect:
            break;
        case kPoll:
            ev_ = new EventPoll();
            ret = ev_->Create(kDefaultSizeOfFds);
            break;
        case kEPoll:
#if defined(__linux__)
            ev_ = new EventEpoll();
#else
            ev_ = new EventPoll();
#endif
            ret = ev_->Create(kDefaultSizeOfFds);
            break;
        default:
            ev_ = new EventPoll();
            ret = ev_->Create(kDefaultSizeOfFds);
            break;
    }

    return kOk;
}/*}}}*/

Code Client::Connect(const std::string &ip, uint16_t port)
{/*{{{*/
    if (ip.size() == 0) return kInvalidParam;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip.c_str(), &(serv_addr.sin_addr));
    assert(ret == 1);

    CloseConnect();

    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd_ == -1) return kSocketError;
    Code r = SetFdNonblock(client_fd_);
    if (r != kOk) return r;

    ev_->Add(client_fd_, EV_OUT);

    while (true)
    {
        ret = connect(client_fd_, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
        if (ret == -1 && errno == EINTR)
            continue;
        else
            break;
    }
    if (ret == 0) return kOk;

    if ((errno != EAGAIN) && (errno != EINPROGRESS)) 
        return kConnectError;

    while (true)
    {
        r = ev_->Wait(kDefaultWaitTimeMs);
        if (r == kOk) break;
        if (r == kTimeOut) continue;

        CloseConnect();
        return r;
    }

    int sock_err = 0;
    socklen_t sock_err_len;
    ret = getsockopt(client_fd_, SOL_SOCKET, SO_ERROR, (char*)&sock_err, &sock_err_len);
    if (ret == -1)
    {
        CloseConnect();
        return kSocketError;
    }

    if (sock_err != 0)
    {
        CloseConnect();
        return kSocketError;
    }
    
    return kOk;
}/*}}}*/

Code Client::Write(const std::string &data)
{/*{{{*/
    if (data.empty()) return kInvalidParam;

    std::string msg;
    Code r = EncodeToMsg(data, &msg);
    assert(r == kOk);

    ev_->Mod(client_fd_, EV_OUT);
    int32_t left_len = msg.size();
    while (left_len > 0)
    {
        r = ev_->Wait(kDefaultWaitTimeMs);
        if (r == kTimeOut)
            continue;

        if (r == kOk)
        {
            int fd, event;
            r = ev_->GetEvents(&fd, &event);
            assert(r == kOk && fd == client_fd_);
            if ((event & EV_ERR) || (event & EV_HUP))
                goto err;

            int ret = write(client_fd_, msg.data()+msg.size()-left_len, left_len);
            if (ret == 0 || (ret == -1 && errno != EAGAIN))
                goto err;
            if (ret == -1 && errno == EAGAIN)
                continue;
            left_len -= ret;
        }
        else
            goto err;
    }

    return kOk;

err:
    CloseConnect();
    return kSocketError;
}/*}}}*/

Code Client::Read(std::string *data)
{/*{{{*/
    if (data == NULL) return kInvalidParam;
    
    data->resize(kHeadLen);
    char *tmp = const_cast<char*>(data->data());
    Code r = ReadInternal(tmp, data->size());
    if (r != kOk) return r;

    int left_len = 0;
    r = DecodeFixed32(*data, (uint32_t*)&left_len);
    assert(r == kOk);
    data->resize(left_len);
    tmp = const_cast<char*>(data->data());
    r = ReadInternal(tmp, data->size());
    if (r != kOk) return r;

    return kOk;
}/*}}}*/

Code Client::ReadInternal(char *buf, int buf_len)
{/*{{{*/
    Code r = ev_->Mod(client_fd_, EV_IN);
    int left_len = buf_len;
    while (left_len > 0)
    {
        r = ev_->Wait(kDefaultWaitTimeMs);
        if (r == kTimeOut)
            continue;

        if (r == kOk)
        {
            int fd, event;
            r = ev_->GetEvents(&fd, &event);
            assert(r == kOk && fd == client_fd_);
            if ((event & EV_ERR) || (event & EV_HUP))
                goto err;

            int ret = read(client_fd_, buf+buf_len-left_len, left_len);
            if (ret == 0 || (ret == -1 && errno != EAGAIN))
                goto err;
            if (ret == -1 && errno == EAGAIN)
                continue;
            left_len -= ret;
        }
        else 
            goto err;
    }
    return kOk;

err:
    CloseConnect();
    return kSocketError;
}/*}}}*/

void Client::CloseConnect()
{/*{{{*/
    if (client_fd_ != -1)
    {
        close(client_fd_);
        client_fd_ = -1;
    }
}/*}}}*/

}

#ifdef _CLIENT_MAIN_TEST_
int main(int argc, char *argv[])
{
    return 0;
}
#endif
