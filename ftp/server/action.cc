// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include "ftp/pb/src/ftp_rpc.pb.h"
#include "ftp/pb/src/ftp_model.pb.h"

#include "ftp/server/action.h"

namespace ftp
{

static base::Code DownloadAction(const base::Config &conf, const ftp_model::DownloadReq &download_req, std::string *out);
static base::Code UploadAction(const base::Config &conf, const ftp_model::UploadReq &upload_req, std::string *out);

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
            return DownloadAction(conf, msg.detail_message().download_req(), out);
        case ftp_rpc::DetailMessage::kUploadReq:
            return UploadAction(conf, msg.detail_message().upload_req(), out);
            break;
        default:
            return base::kInvalidMessageType;
    }

    return base::kOk;
}/*}}}*/

static base::Code DownloadAction(const base::Config &conf, const ftp_model::DownloadReq &download_req, std::string *out)
{
    if (out == NULL) return base::kInvalidParam;
    return base::kOk;
}

static base::Code UploadAction(const base::Config &conf, const ftp_model::UploadReq &upload_req, std::string *out)
{
    if (out == NULL) return base::kInvalidParam;
    return base::kOk;
}

}
