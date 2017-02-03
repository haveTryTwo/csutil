// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>

#include <google/protobuf/message.h>

#include "base/mutex.h"
#include "base/status.h"
#include "base/common.h"

#include "cs/client.h"

#include "ftp/pb/src/ftp_rpc.pb.h"
#include "ftp/pb/src/ftp_model.pb.h"

namespace ftp
{

int FtpClient()
{
    base::Client client;
    base::EventType event_type = base::kPoll;
#if defined(__linux__)
    event_type = base::kEPoll;
#endif
    base::Code ret = client.Init(event_type);
    assert(ret == base::kOk);

    std::string ip("127.0.0.1");
    uint16_t port = 9095;
    ret = client.Connect(ip, port);
    assert(ret == base::kOk);

    ftp_rpc::Message msg;
    msg.set_id(0x1);
    struct timeval tm;
    gettimeofday(&tm, NULL);
    msg.set_req_sec(tm.tv_sec);
    msg.set_req_usec(tm.tv_usec);

    ftp_model::DownloadReq *download_req = msg.mutable_detail_message()->mutable_download_req();
    std::string req_file_name = "a.txt";
    download_req->set_file_name(req_file_name);
    int64_t start = 0;
    int64_t len = 100;
    download_req->set_start(start);
    download_req->set_len(len);

    std::string buf_out;
    msg.SerializeToString(&buf_out);
    ret = client.Write(buf_out);
    fprintf(stderr, "ret:%d\n", ret);
    assert(ret == base::kOk);

    std::string buf_in;
    ret = client.Read(&buf_in);
    assert(ret == base::kOk);

    ftp_rpc::Message msg_resp;
    msg_resp.ParseFromString(buf_in);
    assert(msg.id() == msg_resp.id());
    assert(msg_resp.detail_message().choice_case() == ftp_rpc::DetailMessage::kDownloadResp);
    const ftp_model::DownloadResp &download_resp = msg_resp.detail_message().download_resp();
    if (download_resp.ret_code() != 0)
    {
        fprintf(stderr, "Failed to download! ret:%d, err:%s\n", 
                download_resp.ret_code(), download_resp.ret_msg().c_str());
        return -1;
    }
    fprintf(stderr, "file:%s, total_size:%lld\n", req_file_name.c_str(), download_resp.total_file_size());

    int fd = open(req_file_name.c_str(), O_WRONLY|O_CREAT, base::kDefaultRegularFileMode);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open file:%s\n", req_file_name.c_str());
        return -1;
    }

    int ret_size = write(fd, download_resp.content().c_str(), download_resp.content().size());
    assert(ret_size == (int)download_resp.content().size());
    close(fd);

    return base::kOk;
}

}

int main(int argc, char *argv[])
{
    ftp::FtpClient();

    return 0;
}
