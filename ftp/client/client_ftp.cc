#include <iostream>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include <google/protobuf/message.h>

#include "base/mutex.h"
#include "base/status.h"

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
    int64_t len = 10;
    download_req->set_start(start);
    download_req->set_len(len);


    std::string buf_out("hello world");
    ret = client.Write(buf_out);
    fprintf(stderr, "buf_out:%s\n", buf_out.c_str());
    assert(ret == base::kOk);

    std::string buf_in;
    ret = client.Read(&buf_in);
    assert(ret == base::kOk);
    fprintf(stderr, "buf_in:%s\n", buf_in.c_str());

    return base::kOk;
}

}

int main(int argc, char *argv[])
{
    ftp::FtpClient();

    return 0;
}
