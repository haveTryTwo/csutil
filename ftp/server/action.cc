// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "base/log.h"
#include "base/common.h"

#include "ftp/pb/src/ftp_rpc.pb.h"
#include "ftp/pb/src/ftp_model.pb.h"

#include "ftp/server/action.h"

namespace ftp
{

static base::Code DownloadAction(const base::Config &conf, const ftp_rpc::Message &msg_req, std::string *out);
static base::Code UploadAction(const base::Config &conf, const ftp_rpc::Message &msg_req, std::string *out);

base::Code FtpAction(const base::Config &conf, const std::string &in, std::string *out)
{/*{{{*/
    if (out == NULL) return base::kInvalidParam;

    ftp_rpc::Message msg;
    if (!msg.ParseFromString(in))
    {
        LOG_ERR("Failed to parse string:%s\n", in.c_str());
        return base::kInvalidPbMessage;
    }

    switch (msg.detail_message().choice_case())
    {
        case ftp_rpc::DetailMessage::kDownloadReq:
            return DownloadAction(conf, msg, out);
        case ftp_rpc::DetailMessage::kUploadReq:
            return UploadAction(conf, msg, out);
            break;
        default:
            return base::kInvalidMessageType;
    }

    return base::kOk;
}/*}}}*/

static base::Code DownloadAction(const base::Config &conf, const ftp_rpc::Message &msg_req, std::string *out)
{/*{{{*/
    if (out == NULL) return base::kInvalidParam;

    ftp_rpc::Message msg_resp;
    msg_resp.set_id(msg_req.id());
    msg_resp.set_req_sec(msg_req.req_sec());
    msg_resp.set_req_usec(msg_req.req_usec());

    ftp_model::DownloadResp *download_resp = msg_resp.mutable_detail_message()->mutable_download_resp();
    ftp_model::DownloadReq download_req = msg_req.detail_message().download_req();

    std::string store_dir;
    base::Code r = conf.GetValue(base::kFtpStoreDirKey, &store_dir);
    if (r != base::kOk) store_dir = base::kDefaultFtpStoreDir;
   
    if (download_req.file_name().empty())
    {
        download_resp->set_ret_code(base::kFileNameNotSet);
        download_resp->set_ret_msg("File name not set");
        msg_resp.SerializeToString(out);
        return base::kOk;
    }

    std::string file_path = store_dir + "/" + download_req.file_name();
    struct stat st;
    int ret = stat(file_path.c_str(), &st);
    if (ret != 0 || !S_ISREG(st.st_mode))
    {
        download_resp->set_ret_code(base::kFileNotExist);
        download_resp->set_ret_msg("File not exist");
        msg_resp.SerializeToString(out);
        return base::kOk;
    }

    int64_t file_size = st.st_size;
    if (download_req.start() < 0)
    {
        download_resp->set_ret_code(base::kInvaliedFileStartPos);
        download_resp->set_ret_msg("Invalid start pos");
        msg_resp.SerializeToString(out);
        return base::kOk;
    }

    if (download_req.start() >= file_size)
    {
        download_resp->set_ret_code(base::kOk);
        download_resp->set_content("");
        download_resp->set_total_file_size(file_size);
        msg_resp.SerializeToString(out);
        return base::kOk;
    }

    int max_buf_len = 0;
    r = conf.GetInt32Value(base::kBufLenKey, &max_buf_len);
    if (r != base::kOk) max_buf_len = base::kDefaultBufLen;
    int buf_len = 0;
    if (download_req.len() <= 0 || download_req.len() >= max_buf_len)
        buf_len = max_buf_len;
    else
        buf_len = download_req.len();
    buf_len = (download_req.start()+buf_len <= file_size) ? buf_len : file_size-download_req.start();

    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        download_resp->set_ret_code(base::kOpenFileFailed);
        download_resp->set_ret_msg("Open File Failed!");
        msg_resp.SerializeToString(out);
        return base::kOk;
    }

    char *buf = new char[buf_len+1];
    lseek(fd, download_req.start(), SEEK_SET);
    ret = read(fd, buf, buf_len); 
    download_resp->set_content(buf, ret);
    download_resp->set_ret_code(base::kOk);
    download_resp->set_total_file_size(file_size);
    msg_resp.SerializeToString(out);

    close(fd);

    return base::kOk;
}/*}}}*/

static base::Code UploadAction(const base::Config &conf, const ftp_rpc::Message &msg_req, std::string *out)
{
    if (out == NULL) return base::kInvalidParam;
    return base::kOk;
}

}
