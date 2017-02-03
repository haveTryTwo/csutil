// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "base/msg.h"
#include "base/util.h"
#include "base/coding.h"
#include "base/common.h"
#include "sock/tcp_client.h"

namespace base
{

Code DefaultProtoFunc(const char *src_data, int src_data_len, int *real_len)
{/*{{{*/
    if (src_data == NULL || src_data_len < 0 || real_len == NULL) return kInvalidParam;

    if (src_data_len < kHeadLen) return kDataNotEnough;

    uint32_t len = 0;
    Code r = DecodeFixed32(std::string(src_data, src_data_len), &len);
    if (r != kOk) return r;

    if (src_data_len < (kHeadLen+len)) return kDataNotEnough;

    *real_len = kHeadLen+len;

    return kOk;
}/*}}}*/

TcpClient::TcpClient() : ev_(NULL), client_fd_(-1), data_proto_func_(NULL), serv_ip_(""), serv_port_(0),
    data_buf_(NULL), start_pos_(0), end_pos_(0), total_size_(0)
{
}

TcpClient::~TcpClient()
{/*{{{*/
    CloseConnect();

    if (ev_ != NULL)
    {
        delete ev_;
        ev_ = NULL;
    }

    if (data_buf_ != NULL)
    {
        delete data_buf_;
        data_buf_ = NULL;
    }
}/*}}}*/

Code TcpClient::Init(EventType evt_type, DataProtoFunc data_proto_func)
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

    data_proto_func_ = data_proto_func;

    data_buf_ = new char[kMaxStreamBufLen];
    total_size_ = kMaxStreamBufLen;

    return kOk;
}/*}}}*/

Code TcpClient::Connect(const std::string &ip, uint16_t port)
{/*{{{*/
    if (ip.size() == 0) return kInvalidParam;

    serv_ip_ = ip;
    serv_port_ = port;

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

Code TcpClient::ReConnect()
{/*{{{*/
    if (serv_ip_.empty() || serv_port_ == 0) return kIpOrPortNotInit;

    CloseConnect();

    return Connect(serv_ip_, serv_port_);
}/*}}}*/

Code TcpClient::SendNative(const std::string &data)
{/*{{{*/
    if (data.empty()) return kInvalidParam;

    Code r = ev_->Mod(client_fd_, EV_OUT|EV_ERR|EV_HUP);

    int32_t left_len = data.size();
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

            int ret = write(client_fd_, data.data()+data.size()-left_len, left_len);
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

Code TcpClient::Recv(std::string *data)
{/*{{{*/
    if (data == NULL) return kInvalidParam;

    Code r = kOk;
    int real_len = 0;

    while (true)
    {
        r = data_proto_func_(data_buf_+start_pos_, end_pos_-start_pos_, &real_len);

        if (r == kDataNotEnough)
        {
            r = RecvInternal();
            if (r != kOk) return r;

            continue;
        }
        else if (r != kOk)
        {
            return r;
        }

        data->assign(data_buf_+start_pos_, real_len);
        start_pos_ += real_len;
        break;
    }

    return r;
}/*}}}*/

void TcpClient::CloseConnect()
{/*{{{*/
    if (client_fd_ != -1)
    {
        close(client_fd_);
        client_fd_ = -1;
    }
}/*}}}*/

Code TcpClient::RecvInternal()
{/*{{{*/
    if (start_pos_ == end_pos_)
        start_pos_ = end_pos_ = 0;

    if (start_pos_ != 0)
    {
        memmove(data_buf_, data_buf_+start_pos_, end_pos_-start_pos_);
        end_pos_ -= start_pos_;
        start_pos_ = 0;
    }

    if (end_pos_ == total_size_) return kDataBufFull;

    Code r = ev_->Mod(client_fd_, EV_IN);
    while (true)
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

            int ret = read(client_fd_, data_buf_+end_pos_, total_size_-end_pos_);
            if (ret == 0 || (ret == -1 && errno != EAGAIN))
                goto err;
            if (ret == -1 && errno == EAGAIN)
                continue;

            end_pos_ += ret;
            break;
        }
        else 
            goto err;
    }
    return kOk;

err:
    CloseConnect();
    return kSocketError;
}/*}}}*/

}

#ifdef _TCP_CLIENT_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;

    TcpClient tcp_client;
    base::EventType event_type = base::kPoll;
#if defined(__linux__)
    event_type = base::kEPoll;
#endif
    base::Code ret = tcp_client.Init(event_type, DefaultProtoFunc);
    assert(ret == base::kOk);

    std::string ip("127.0.0.1");
    uint16_t port = 9090;
    ret = tcp_client.Connect(ip, port);
    assert(ret == base::kOk);
    
    // Encode the data and send
    std::string buf_out("hello world");
    std::string encode_buf_out;
    ret = EncodeToMsg(buf_out, &encode_buf_out);
    assert(ret == kOk);

    fprintf(stderr, "buf out:%s\n", buf_out.c_str());
    ret = tcp_client.SendNative(encode_buf_out);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to send native! ret:%d\n", (int)ret);
        return ret;
    }

    fprintf(stderr, "buf out:%s\n", buf_out.c_str());
    ret = tcp_client.SendNative(encode_buf_out);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to send native! ret:%d\n", (int)ret);
        return ret;
    }

    // Receive the data and decode 
    std::string buf_in;
    std::string decode_buf_in;

    ret = tcp_client.Recv(&buf_in);
    assert(ret == base::kOk);
    ret = DecodeFromMsg(buf_in, &decode_buf_in);
    assert(ret == kOk);
    fprintf(stderr, "buf in:%s\n", decode_buf_in.c_str());

    ret = tcp_client.Recv(&buf_in);
    assert(ret == base::kOk);
    ret = DecodeFromMsg(buf_in, &decode_buf_in);
    assert(ret == kOk);
    fprintf(stderr, "buf in:%s\n", decode_buf_in.c_str());

    return 0;
}
#endif
