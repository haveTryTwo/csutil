// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/tcp_client.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>

#include "base/coding.h"
#include "base/common.h"
#include "base/log.h"
#include "base/util.h"

namespace base {

TcpClient::TcpClient()
    : ev_(NULL),
      client_fd_(-1),
      data_proto_func_(NULL),
      serv_ip_(""),
      serv_port_(0),
      data_buf_(NULL),
      start_pos_(0),
      end_pos_(0),
      total_size_(0),
      is_init_(false) { /*{{{*/ } /*}}}*/

TcpClient::TcpClient(const std::string &ip, uint16_t port)
    : ev_(NULL),
      client_fd_(-1),
      data_proto_func_(NULL),
      serv_ip_(ip),
      serv_port_(port),
      data_buf_(NULL),
      start_pos_(0),
      end_pos_(0),
      total_size_(0),
      is_init_(false) { /*{{{*/ } /*}}}*/

TcpClient::~TcpClient() { /*{{{*/
  CloseConnect();

  if (ev_ != NULL) {
    delete ev_;
    ev_ = NULL;
  }

  if (data_buf_ != NULL) {
    delete data_buf_;
    data_buf_ = NULL;
  }
} /*}}}*/

Code TcpClient::Init(EventType evt_type, DataProtoFunc data_proto_func) { /*{{{*/
  if (is_init_) return kOk;

  Code ret = kOk;
  switch (evt_type) {
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
  if (ret != kOk) {
    delete ev_;
    ev_ = NULL;
    return ret;
  }

  data_proto_func_ = data_proto_func;

  data_buf_ = new char[kMaxStreamBufLen];
  total_size_ = kMaxStreamBufLen;

  is_init_ = true;
  return ret;
} /*}}}*/

Code TcpClient::Connect() { /*{{{*/
  if (!is_init_) return kNotInit;
  // NOTE: if ip or port not init, then return
  if (serv_ip_.empty() || serv_port_ == 0) return kIpOrPortNotInit;
  return Connect(serv_ip_, serv_port_);
} /*}}}*/

Code TcpClient::Connect(const std::string &ip, uint16_t port) { /*{{{*/
  if (!is_init_) return kNotInit;
  if (ip.size() == 0) return kInvalidParam;

  // NOTE: if conntection is alive, then just return instead of reconnect
  if (serv_ip_ == ip && serv_port_ == port && client_fd_ != -1) return kOk;

  serv_ip_ = ip;
  serv_port_ = port;

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  int ret = inet_pton(AF_INET, ip.c_str(), &(serv_addr.sin_addr));
  if (ret != 1) return kSocketError;

  CloseConnect();

  client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd_ == -1) return kSocketError;
  Code r = SetFdNonblock(client_fd_);
  if (r != kOk) return r;

  ev_->Add(client_fd_, EV_OUT);

  while (true) {
    ret = connect(client_fd_, (struct sockaddr *)(&serv_addr), sizeof(serv_addr));
    if (ret == -1 && errno == EINTR)
      continue;
    else
      break;
  }
  if (ret == 0) return kOk;

  if ((errno != EAGAIN) && (errno != EINPROGRESS)) return kConnectError;

  int time_waits = 0;
  while (true) {
    r = ev_->Wait(kDefaultWaitTimeMs);
    // fprintf(stderr, "(%s:%d) now time_waits:%d, r:%d\n", __FILE__, __LINE__, time_waits, r);
    if (r == kOk) break;
    if (r == kTimeOut) {
      time_waits += kDefaultWaitTimeMs;
      if (time_waits >= kDefaultMaxWaitTimeMs) {  // Note: if waiting time exceed max time, then just quit
        CloseConnect();
        return kTimeOut;
      }

      continue;
    }

    // Note: Other error, then close
    CloseConnect();
    return r;
  }

  int sock_err = 0;
  socklen_t sock_err_len;
  ret = getsockopt(client_fd_, SOL_SOCKET, SO_ERROR, (char *)&sock_err, &sock_err_len);
  if (ret == -1) {
    CloseConnect();
    return kSocketError;
  }

  if (sock_err != 0) {
    CloseConnect();
    return kSocketError;
  }

  start_pos_ = 0;
  end_pos_ = 0;

  return kOk;
} /*}}}*/

Code TcpClient::ReConnect() { /*{{{*/
  if (serv_ip_.empty() || serv_port_ == 0) return kIpOrPortNotInit;

  CloseConnect();

  return Connect(serv_ip_, serv_port_);
} /*}}}*/

Code TcpClient::SendNative(const std::string &data) { /*{{{*/
  if (!is_init_) return kNotInit;
  if (data.empty()) return kInvalidParam;

  // Note: if connection is not alive, then reronnect
  if (client_fd_ == -1) {
    Code ret = ReConnect();
    if (ret != kOk) return ret;
  }

  Code r = ev_->Mod(client_fd_, EV_OUT | EV_ERR | EV_HUP);

  int32_t left_len = data.size();
  int time_waits = 0;
  while (left_len > 0) {
    r = ev_->Wait(kDefaultWaitTimeMs);
    // fprintf(stderr, "(%s:%d) now time_waits:%d, r:%d\n", __FILE__, __LINE__, time_waits, r);
    if (r == kTimeOut) {
      time_waits += kDefaultWaitTimeMs;
      if (time_waits >= kDefaultMaxWaitTimeMs) {  // Note: if waiting time exceed max time, then just quit
        CloseConnect();
        return kTimeOut;
      }

      continue;
    }
    time_waits = 0;

    if (r == kOk) {
      int fd, event;
      r = ev_->GetEvents(&fd, &event);
      assert(r == kOk && fd == client_fd_);
      if ((event & EV_ERR) || (event & EV_HUP)) goto err;

      int ret = write(client_fd_, data.data() + data.size() - left_len, left_len);
      if (ret == 0 || (ret == -1 && errno != EAGAIN)) goto err;
      if (ret == -1 && errno == EAGAIN) continue;
      left_len -= ret;
    } else
      goto err;
  }

  return kOk;

err:
  CloseConnect();
  return kSocketError;
} /*}}}*/

Code TcpClient::Recv(std::string *data) { /*{{{*/
  if (!is_init_) return kNotInit;
  if (data == NULL) return kInvalidParam;

  Code r = kOk;
  int real_len = 0;

  while (true) {
    r = data_proto_func_(data_buf_ + start_pos_, end_pos_ - start_pos_, &real_len);

    if (r == kDataNotEnough) {
      r = RecvInternal();
      if (r != kOk) return r;

      continue;
    } else if (r != kOk) {
      return r;
    }

    data->assign(data_buf_ + start_pos_, real_len);
    start_pos_ += real_len;
    break;
  }

  return r;
} /*}}}*/

void TcpClient::CloseConnect() { /*{{{*/
  if (client_fd_ != -1) {
    ev_->Del(client_fd_);
    close(client_fd_);
    client_fd_ = -1;

    start_pos_ = 0;
    end_pos_ = 0;
  }
} /*}}}*/

Code TcpClient::RecvInternal() { /*{{{*/
  if (start_pos_ == end_pos_) start_pos_ = end_pos_ = 0;

  if (start_pos_ != 0) {
    memmove(data_buf_, data_buf_ + start_pos_, end_pos_ - start_pos_);
    end_pos_ -= start_pos_;
    start_pos_ = 0;
  }

  if (end_pos_ == total_size_) return kDataBufFull;

  Code r = ev_->Mod(client_fd_, EV_IN);
  int time_waits = 0;
  while (true) {
    r = ev_->Wait(kDefaultWaitTimeMs);
    // fprintf(stderr, "(%s:%d) now time_waits:%d, r:%d\n", __FILE__, __LINE__, time_waits, r);
    if (r == kTimeOut) {
      time_waits += kDefaultWaitTimeMs;
      if (time_waits >= kDefaultMaxWaitTimeMs) {  // Note: if waiting time exceed max time, then just quit
        CloseConnect();
        return kTimeOut;
      }

      continue;
    }
    time_waits = 0;

    if (r != kOk) goto err;

    int fd, event;
    r = ev_->GetEvents(&fd, &event);
    assert(r == kOk && fd == client_fd_);
    if ((event & EV_ERR) || (event & EV_HUP)) goto err;

    int ret = read(client_fd_, data_buf_ + end_pos_, total_size_ - end_pos_);
    if (ret == 0 || (ret == -1 && errno != EAGAIN)) {
      LOG_ERR("Failed to read fd:%d, ret:%d\n", client_fd_, ret);
      goto err;
    }
    if (ret == -1 && errno == EAGAIN) continue;

    if (end_pos_ + ret > total_size_) {
      LOG_ERR("end_pos_:%d + ret:%d > total_size_:%d", end_pos_, ret, total_size_);
      goto err;
    }

    end_pos_ += ret;
    break;
  }
  return kOk;

err:
  CloseConnect();
  return kSocketError;
} /*}}}*/

}  // namespace base
