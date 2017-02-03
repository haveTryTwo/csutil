// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/reg.h"
#include "base/log.h"
#include "base/int.h"
#include "base/util.h"
#include "base/common.h"

#include "http/http_proto.h"

namespace base
{

Code HttpRespProtoFunc(const char *src_data, int src_data_len, int *real_len)
{/*{{{*/
    Code ret = kOk;

    if (src_data_len < 4) return kDataNotEnough;
    
    ret = HttpProto::CheckRespHeader(std::string(src_data, src_data_len));
    if (ret != kOk) return ret;
    
    std::string end_str = kCRLF + kCRLF;
    const char *end_pos = strstr(src_data, end_str.c_str());
    if (end_pos == NULL) return kDataNotEnough;

    std::string upper_header_resp;
    ret = ToUpper(std::string(src_data, end_pos-src_data+end_str.size()), &upper_header_resp);
    if (ret != kOk) return ret;
    
    std::string tmp;
    ret = HttpProto::GetMessageHeader(upper_header_resp, kTransferEncoding, &tmp);
    if (ret == kOk && strcasecmp(tmp.c_str(), kChunked.c_str()) == 0)
    {
        uint32_t real_chunked_len = 0;
        ret = HttpProto::GetChunkedMsg(std::string(src_data+upper_header_resp.size(), src_data_len-upper_header_resp.size()), &real_chunked_len, NULL);
        if (ret != kOk) return ret;

        *real_len = upper_header_resp.size() + real_chunked_len;
    }
    else
    {
        ret = HttpProto::GetMessageHeader(upper_header_resp, kContentLength, &tmp);
        if (ret != kOk) return ret;

        int content_len = atoi(tmp.c_str());
        if (int(src_data_len - (end_pos-src_data+end_str.size())) < content_len) return kDataNotEnough;

        *real_len = end_pos-src_data + end_str.size() + atoi(tmp.c_str());
    }

    // LOG_ERR("real_len:%d\n", *real_len);

    return ret;
}/*}}}*/

Code HttpProto::GetMessageHeader(const std::string &upper_header_resp, const std::string &msg_key, std::string *msg_value)
{/*{{{*/
    if (upper_header_resp.empty() || msg_key.empty() || msg_value == NULL) return kInvalidParam;
    msg_value->clear();

    std::string msg_key_tmp;
    Code ret = ToUpper(msg_key, &msg_key_tmp);
    if (ret != kOk) return ret;
    msg_key_tmp += ":";

    uint32_t pos = upper_header_resp.find(msg_key_tmp);
    if (pos == (uint32_t)std::string::npos) return kNotFound;

    uint32_t crlf_pos = upper_header_resp.find(kCRLF, pos+msg_key_tmp.size());
    if (crlf_pos == (uint32_t)std::string::npos) crlf_pos = upper_header_resp.size();

    std::string msg_value_tmp;
    msg_value_tmp.assign(upper_header_resp, pos+msg_key_tmp.size(), crlf_pos-(pos+msg_key_tmp.size()));

    ret = Trim(msg_value_tmp, kDefaultDelim, msg_value);

    return ret;
}/*}}}*/

Code HttpProto::GetChunkedMsg(const std::string &body, uint32_t *real_body_len, std::string *chunked_msg)
{/*{{{*/
    const char *body_tmp = body.c_str();
    if (chunked_msg != NULL) chunked_msg->clear();

    Code ret = kOk;
    while (true)
    {
        const char *pos = strstr(body_tmp, kCRLF.c_str());
        if (pos == NULL) return kDataNotEnough;
        
        int content_len = 0;
        ret = GetInt32ByHex(std::string(body_tmp, pos-body_tmp), &content_len);
        if (ret != kOk) return ret;

        // 0\r\n\r\n is the end
        if (content_len == 0)
        {   
            std::string end_str = kCRLF+kCRLF;
            const char *end_pos = strstr(body_tmp, end_str.c_str()); 
            if (end_pos == NULL) return kDataNotEnough;

            if (real_body_len != NULL)
            {
                *real_body_len = end_pos+end_str.size()-body.data();
            }
            return ret;
        }

        if (int(body.size()-(pos+kCRLF.size()-body.data())) < content_len) return kDataNotEnough;

        if (chunked_msg != NULL)
        {
            chunked_msg->append(pos+kCRLF.size(), content_len);
        }
        
        if (memcmp(pos+kCRLF.size()+content_len, kCRLF.data(), kCRLF.size()) != 0) 
            return kDataNotEnough;

        body_tmp = pos+kCRLF.size()+content_len+kCRLF.size();
    }

    return ret;
}/*}}}*/

HttpProto::HttpProto()
{/*{{{*/
    http_type_ = GET;           // use GET as default

    proto_ = "http://";         // use http:// as default
    proto_version_ = "HTTP/1.1";
    host_ = "";
    port_ = 80;
    relative_url_ = "";
    params_ = "";
    user_agent_ = "Mozilla/5.0";
    content_type_ = "multipart/form-data";
    redirect_url_ = "";
}/*}}}*/

HttpProto::~HttpProto()
{/*{{{*/
}/*}}}*/

Code HttpProto::SetHttpType(HttpType http_type)
{/*{{{*/
    http_type_ = http_type;

    return kOk;
}/*}}}*/

Code HttpProto::ParseUrl(const std::string &url)
{/*{{{*/
    std::string tmp_url(url);
    if(0 == strncasecmp(tmp_url.c_str(), "http://", 7)) 
    {
        proto_ = "http://";
        port_ = 80;
    }
    else if (0 == strncasecmp(tmp_url.c_str(), "https://", 8)) 
    {
        proto_ = "https://";
        port_ = 443;
    }
    else
    {
        proto_ = "http://";
        port_ = 443;
        tmp_url = std::string("http://") + tmp_url;
    }

    uint32_t index = tmp_url.find(proto_) + proto_.size();
    uint32_t index2 = tmp_url.find(":", index);
    uint32_t index3 = tmp_url.find("/", index);

    // url: http://host [:port] [/ relative_path [? query]]
    if (index3 == (uint32_t)std::string::npos)
    {/*{{{*/
        host_ = tmp_url.substr(index);
        if (index2 != (uint32_t)std::string::npos)
        {
            port_ = atoi(tmp_url.substr(index2+1).c_str());
        }
        relative_url_ = "/";
        uint32_t param_pos = tmp_url.find( "?" );
        if(param_pos != (uint32_t)std::string::npos)
        {
            params_ = std::string(tmp_url, param_pos+1, tmp_url.size()-(param_pos+1));
        }

        if (host_.find("?") != (uint32_t)std::string::npos)
        {
            host_ = std::string(host_, 0, host_.find("?"));
        }

        if (host_.find(":") != (uint32_t)std::string::npos)
        {
            host_ = std::string(host_, 0, host_.find(":"));
        }
    }/*}}}*/
    else
    {/*{{{*/
        uint32_t pos = index2<index3 ? index2 : index3;
        host_ = tmp_url.substr(index, pos-index);

        if ((index2 != (uint32_t)std::string::npos) && (index2<index3))
        {
            port_ = atoi(tmp_url.substr(index2+1).c_str());
        }

        uint32_t param_pos = tmp_url.find( "?" );
        if(param_pos != (uint32_t)std::string::npos && (index3 < param_pos))
        {
            relative_url_ = std::string(tmp_url, index3, param_pos-index3);
            params_ = std::string(tmp_url, param_pos+1, tmp_url.size()-(param_pos+1));
        }
        else
        {
            relative_url_ = std::string(tmp_url, index3);
        }
    }/*}}}*/

    return kOk;
}/*}}}*/

Code HttpProto::GetHeader(std::string *header)
{/*{{{*/
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%d", port_);

    std::string tmp_url;
    if (params_.size() > 0)
    {
        tmp_url = relative_url_ + "?" + params_;
    }
    else
    {
        tmp_url = relative_url_;
    }

    *header = "GET " + tmp_url + " " + proto_version_ + "\r\n"
        "User-Agent: " + user_agent_ + "\r\n"
        "Accept: */*\r\n"
        "Host: " + host_ + ":" + buf + "\r\n"
        "Pragma: no-cache\r\n"
        "Connection: Keep-Alive\r\n";

    // TODO: set If-Modefied-Since:

    *header += "\r\n";

    return kOk; 
}/*}}}*/

Code HttpProto::PostHeader(std::string *header)
{/*{{{*/
    uint32_t params_len = params_.size();
	char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", params_len);

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%d", port_);

	*header = "POST " + relative_url_ + " " + proto_version_ + "\r\n"
			        "User-Agent: " + user_agent_ + "\r\n"
			        "Accept: */*\r\n"
			        "Host: " + host_ + ":" + buf + "\r\n"
			        "Content-Type: " + content_type_ + "\r\n"
			        "Content-Length: " + tmp + "\r\n"
			        "Pragma: no-cache\r\n"
			        "Connection: Keep-Alive\r\n";

	*header += "\r\n";

    return kOk;
}/*}}}*/

Code HttpProto::EncodeToReq(const std::string &url, const std::string &post_params, std::string *stream_data, std::string *host, uint16_t *port)
{/*{{{*/
    if (url.empty() || stream_data == NULL) return kInvalidParam;
    stream_data->clear();

    Code ret = ParseUrl(url);
    if (ret != kOk) return ret;

    params_.clear();
    params_.append(post_params);

    switch (http_type_)
    {
        case GET:
            ret = GetHeader(stream_data);
            break;
        case POST:
            ret = PostHeader(stream_data);
            stream_data->append(params_);
            break;
        case DELETE:
            break;
        default:
            return kInvalidHttpType;
    }

    if (host != NULL) host->assign(host_);
    if (port != NULL) *port = port_;

    return ret;
}/*}}}*/

Code HttpProto::EncodeToReq(const std::string &url, std::string *stream_data, std::string *host, uint16_t *port)
{/*{{{*/
    if (url.empty() || stream_data == NULL) return kInvalidParam;
    stream_data->clear();

    // Note: Get params from url
    Code ret = ParseUrl(url);
    if (ret != kOk) return ret;

    switch (http_type_)
    {
        case GET:
            ret = GetHeader(stream_data);
            break;
        case POST:
            ret = PostHeader(stream_data);
            stream_data->append(params_);
            break;
        case DELETE:
            break;
        default:
            return kInvalidHttpType;
    }

    if (host != NULL) host->assign(host_);
    if (port != NULL) *port = port_;

    return ret;
}/*}}}*/

Code HttpProto::DecodeFromResponse(const std::string &stream_data, std::string *user_data)
{/*{{{*/
    if (stream_data.empty() || user_data == NULL) return kInvalidParam;

    uint16_t ret_code;
    std::string ret_msg;
    Code ret = GetRespStatus(stream_data, &ret_code, &ret_msg);
    if (ret != kOk) return ret;

    // find the \r\n\r\n as the end of http response header
    std::string end_str = kCRLF + kCRLF;
    const char *end_pos = strstr(stream_data.c_str(), end_str.c_str());
    if (end_pos == NULL) return kHttpNoCRLFCRLF;

    std::string upper_header_resp;
    ret = ToUpper(std::string(stream_data.data(), end_pos-stream_data.data()+end_str.size()), &upper_header_resp);
    if (ret != kOk) return ret;

    switch (ret_code)
    {
        case kHttpStatusOk:
        case kHttpStatusNotModified:
            break;
        case kHttpStatusMoved:
        case kHttpStatusRedirect:
        case kHttpStatusRedirect_method:
        case kHttpStatusRedirectKeepVerb:
            ret = GetMessageHeader(upper_header_resp, kLocation, &redirect_url_);
            if (ret == kOk)
            {
                ret = kHttpRedirect;
            }
            break;
        default:
            ret = kInvalidHttpRetStatus;
            break;
    }

    std::string tmp;
    Code r = GetMessageHeader(upper_header_resp, kTransferEncoding, &tmp);

    if (r == kOk && strcasecmp(tmp.c_str(), kChunked.c_str()) == 0)
    {
        r = HttpProto::GetChunkedMsg(std::string(stream_data.data()+upper_header_resp.size(), stream_data.size()-upper_header_resp.size()), NULL, user_data);
        if (r != kOk) return ret;
    }
    else
    {
        uint32_t content_len = 0;
        r = GetMessageHeader(upper_header_resp, kContentLength, &tmp);
        if (r == kOk)
        {
            content_len = atoi(tmp.c_str());
            if (content_len == 0) return ret;
        }

        user_data->assign(stream_data.data()+upper_header_resp.size(), content_len);
    }

    return ret;
}/*}}}*/

Code HttpProto::GetRespStatus(const std::string &stream_data, uint16_t *ret_code, std::string *ret_msg)
{/*{{{*/
    if (stream_data.empty() || ret_code == NULL || ret_msg == NULL) return kInvalidParam;

    if (int(stream_data.size()) < kHttpRspHeaderMinLen) return kDataNotEnough;

    std::string http_reg_str = "^[hH][tT]{2}[pP]/[0-9]\\.[0-9] [0-9]{3} .*";
    Reg reg(http_reg_str);
    Code ret = reg.Init();
    if (ret != kOk) return ret;

    ret = reg.Check(stream_data);
    if (ret != kOk) return ret;

    *ret_code = atoi(stream_data.data()+9); 

    uint32_t pos = stream_data.find(kCRLF);
    if (pos == (uint32_t)std::string::npos) pos = stream_data.size();
    ret_msg->assign(stream_data.data()+13, pos-13);

    return kOk;
}/*}}}*/

Code HttpProto::CheckRespHeader(const std::string &stream_data)
{/*{{{*/
    if (int(stream_data.size()) < kHttpRspHeaderMinLen) return kDataNotEnough;

    std::string http_reg_str = "^[hH][tT]{2}[pP]/[0-9]\\.[0-9] [0-9]{3} .*";
    Reg reg(http_reg_str);
    Code ret = reg.Init();
    if (ret != kOk) return ret;

    ret = reg.Check(stream_data);
    if (ret != kOk) return ret;

    return kOk;
}/*}}}*/

}
