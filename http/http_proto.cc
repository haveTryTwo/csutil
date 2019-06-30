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
    const void *end_pos = memmem((const void*)src_data, src_data_len, (const void*)end_str.c_str(), end_str.size());
    if (end_pos == NULL) return kDataNotEnough;

    std::string upper_header_resp;
    ret = ToUpper(std::string(src_data, (const char*)end_pos-src_data+end_str.size()), &upper_header_resp);
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
        if ((int)upper_header_resp.size()+content_len > src_data_len) return kDataNotEnough;

        *real_len = upper_header_resp.size()+content_len;
    }

    // LOG_ERR("real_len:%d\n", *real_len);

    return ret;
}/*}}}*/

Code HttpReqProtoFunc(const char *src_data, int src_data_len, int *real_len)
{/*{{{*/
    Code ret = HttpProto::CheckReqHeader(std::string(src_data, src_data_len));
    if (ret != kOk) return ret;

    // find the \r\n\r\n as the end of http request header
    std::string end_str = kCRLF + kCRLF;
    const void *end_pos = memmem((const void*)src_data, src_data_len, (const void*)end_str.c_str(), end_str.size());
    if (end_pos == NULL) return kDataNotEnough;

    std::string upper_header_req;
    ret = ToUpper(std::string(src_data, (const char*)end_pos-src_data+end_str.size()), &upper_header_req);

    if (upper_header_req.find("GET") != std::string::npos)
    {
        *real_len = upper_header_req.size();
        return kOk;
    }
    else if (upper_header_req.find("POST") != std::string::npos)
    {
        std::string tmp;
        ret = HttpProto::GetMessageHeader(upper_header_req, kContentLength, &tmp);
        if (ret != kOk) return ret;
        int content_len = atoi(tmp.c_str());
        if ((int)upper_header_req.size()+content_len > src_data_len) return kDataNotEnough;
        *real_len = upper_header_req.size()+content_len;
        return kOk;
    }
    else
    {
        return kInvalidHttpType;
    }

    return ret;
}/*}}}*/

Code ToUpperWithOutValue(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    dst->clear();
    bool need_upper = true;
    for (int i = 0; i < (int)src.size(); ++i)
    {
        if (need_upper && src.data()[i] == kColon)
        {
            need_upper = false; // No need for value to be upper
        }

        if (!need_upper && src.data()[i] == '\r') 
        {
            need_upper = true; // New line
        }

        if (need_upper && src.data()[i] >= 'a' && src.data()[i] <= 'z')
            dst->append(1, src.data()[i]-32);
        else
            dst->append(src.data()+i, 1);
    }

    return kOk;
}/*}}}*/

Code HttpProto::GetMessageHeader(const std::string &upper_header, const std::string &msg_key, std::string *msg_value)
{/*{{{*/
    if (upper_header.empty() || msg_key.empty() || msg_value == NULL) return kInvalidParam;
    msg_value->clear();

    std::string msg_key_tmp;
    Code ret = ToUpper(msg_key, &msg_key_tmp);
    if (ret != kOk) return ret;
    msg_key_tmp += ":";

    uint32_t pos = upper_header.find(msg_key_tmp);
    if (pos == (uint32_t)std::string::npos) return kNotFound;

    uint32_t crlf_pos = upper_header.find(kCRLF, pos+msg_key_tmp.size());
    if (crlf_pos == (uint32_t)std::string::npos) crlf_pos = upper_header.size();

    std::string msg_value_tmp;
    msg_value_tmp.assign(upper_header, pos+msg_key_tmp.size(), crlf_pos-(pos+msg_key_tmp.size()));

    ret = Trim(msg_value_tmp, kWhiteDelim, msg_value);

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
    Clear();
}/*}}}*/

HttpProto::~HttpProto()
{/*{{{*/
}/*}}}*/

Code HttpProto::SetHttpType(HttpType http_type)
{/*{{{*/
    http_type_ = http_type;

    return kOk;
}/*}}}*/

/**
 * Url contains 
 *    1. protocol: "http:" or "https:" is the protocol, and "//" is a delimiter
 *    2. domain name: ip or domain name that identify the host
 *    3. port: the port is after ip, and ":" is a delimiter
 *    4. virtaul directory and file: part between first "/" and the last "/" is the virtual directory,
 *         part between last "/" and "?" or between last "/" and "#" or between "/" and the end is file
 *    5. parameters: part between "?" and "#", and "&" is delimiter between parameters
 *    6. anchor: part from "#" to end 
 *
 * Examples: http://www.books.com:80/books/history/index.php?time=1900&name=direnjie#place
 */
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
    // Ex:  http://www.books.com:80/books/history/index.php?time=1900&name=direnjie#place
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
            get_params_ = std::string(tmp_url, param_pos+1, tmp_url.size()-(param_pos+1));
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
            get_params_ = std::string(tmp_url, param_pos+1, tmp_url.size()-(param_pos+1));
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
    if (get_params_.size() > 0)
    {
        tmp_url = relative_url_ + "?" + get_params_;
    }
    else
    {
        tmp_url = relative_url_;
    }

    *header = "GET " + tmp_url + " " + proto_version_ + "\r\n"
        "User-Agent: " + user_agent_ + "\r\n"
        "Accept: */*\r\n"
        "Host: " + host_ + ":" + buf + "\r\n"
        "Content-Type: " + content_type_ + "\r\n"
        "Pragma: no-cache\r\n"
        "Connection: Keep-Alive\r\n";

    // TODO: set If-Modefied-Since:

    *header += "\r\n";

    return kOk; 
}/*}}}*/

Code HttpProto::PostHeader(std::string *header)
{/*{{{*/
    uint32_t params_len = post_params_.size();
	char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", params_len);

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%d", port_);

    std::string tmp_url;
    if (get_params_.size() > 0)
    {
        tmp_url = relative_url_ + "?" + get_params_;
    }
    else
    {
        tmp_url = relative_url_;
    }

	*header = "POST " + tmp_url + " " + proto_version_ + "\r\n"
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

Code HttpProto::EncodeToReq(const std::string &url, const std::string &post_params, std::string *post_stream_data, std::string *host, uint16_t *port)
{/*{{{*/
    if (url.empty() || post_stream_data == NULL) return kInvalidParam;
    post_stream_data->clear();

    Code ret = ParseUrl(url);
    if (ret != kOk) return ret;

    post_params_ = post_params;

    switch (http_type_)
    {
        case POST:
            ret = PostHeader(post_stream_data);
            post_stream_data->append(post_params_);
            break;
        case GET:
        case DELETE:
        default:
            return kInvalidHttpType;
    }

    if (host != NULL) host->assign(host_);
    if (port != NULL) *port = port_;

    return ret;
}/*}}}*/

Code HttpProto::EncodeToReq(const std::string &url, std::string *get_stream_data, std::string *host, uint16_t *port)
{/*{{{*/
    if (url.empty() || get_stream_data == NULL) return kInvalidParam;
    get_stream_data->clear();

    // Note: Get params from url
    post_params_.clear();
    Code ret = ParseUrl(url);
    if (ret != kOk) return ret;

    switch (http_type_)
    {
        case GET:
            ret = GetHeader(get_stream_data);
            break;
        case POST:
        case DELETE:
        default:
            return kInvalidHttpType;
    }

    if (host != NULL) host->assign(host_);
    if (port != NULL) *port = port_;

    return ret;
}/*}}}*/

Code HttpProto::DecodeFromReq(const std::string &stream_data)
{/*{{{*/
    Clear();

    Code ret = CheckReqHeader(stream_data);
    if (ret != kOk) return ret;

    if (stream_data.find("GET") != std::string::npos)
    {
       http_type_ = GET;
    }
    else if (stream_data.find("POST") != std::string::npos)
    {
        http_type_ = POST;
    }
    else
    {
        return kInvalidHttpType;
    }

    // EX: GET /book/index.php?name=lisi http/1.1
    size_t pos1 = stream_data.find(kWhiteDelim);
    if (pos1 == std::string::npos) return kInvalidParam;
    size_t pos2 = stream_data.find(kWhiteDelim, pos1+1);
    if (pos2 == std::string::npos) return kInvalidParam;
    size_t crlf_pos = stream_data.find(kCRLF, pos2+1);
    if (crlf_pos == std::string::npos) return kInvalidParam;

    // Relative URL and get params
    relative_url_.assign(stream_data.data()+pos1+1, pos2-pos1-1);
    proto_version_.assign(stream_data.data()+pos2+1, crlf_pos-pos2-1);

    size_t get_param_pos = relative_url_.find("?");
    if (get_param_pos != std::string::npos)
    {
        get_params_.assign(relative_url_.data()+get_param_pos+1, relative_url_.size()-get_param_pos-1);
        relative_url_.resize(get_param_pos);
    }

    // find the \r\n\r\n as the end of http request header
    std::string end_str = kCRLF + kCRLF;
    const char *end_pos = strstr(stream_data.c_str(), end_str.c_str());
    if (end_pos == NULL) return kHttpNoCRLFCRLF;

    std::string upper_header_req;
    ret = ToUpperWithOutValue(std::string(stream_data.data(), end_pos-stream_data.data()+end_str.size()), &upper_header_req);
    if (ret != kOk) return ret;

    // Host and port
    ret = GetMessageHeader(upper_header_req, kHost, &host_);
    if (ret != kOk) return ret;
    size_t port_pos = host_.find(":");
    if (port_pos != std::string::npos)
    {
        port_ = atoi(host_.c_str()+port_pos+1);
        host_.resize(port_pos);
    }

    if (http_type_ == GET) return kOk;

    std::string tmp;
    ret = HttpProto::GetMessageHeader(upper_header_req, kContentLength, &tmp);
    if (ret != kOk) return ret;
    int content_len = atoi(tmp.c_str());
    if (upper_header_req.size()+content_len > stream_data.size()) return kDataNotEnough;

    post_params_.assign(stream_data.data()+upper_header_req.size(), content_len);

    return ret;
}/*}}}*/

Code HttpProto::EncodeToResponse(int ret_code, const std::string &user_data, std::string *response)
{/*{{{*/
    if (response == NULL) return kInvalidParam;

    response->clear();

    std::string ret_msg;
    Code ret = GetRetMsg(ret_code, &ret_msg);
    if (ret != kOk) return ret;

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%d", ret_code);
    char tmp[32] = {0};
    snprintf(tmp, sizeof(tmp), "%zu", user_data.size());

    *response = "HTTP/1.1 " + std::string(buf) + " " + ret_msg + "\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n" 
        "Connection: Keep-Alive\r\n"
        "Content-Length: " + tmp + "\r\n"
        "\r\n" + user_data;

    return kOk;
}/*}}}*/

Code HttpProto::DecodeFromResponse(const std::string &stream_data, std::string *user_data)
{/*{{{*/
    if (stream_data.empty() || user_data == NULL) return kInvalidParam;

    Clear();

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

Code HttpProto::Clear()
{/*{{{*/
    http_type_ = GET;           // use GET as default

    proto_ = "http://";         // use http:// as default
    proto_version_ = "HTTP/1.1";
    host_ = "";
    port_ = 80;
    relative_url_ = "";
    get_params_ = "";
    post_params_ = "";
    user_agent_ = "Mozilla/5.0";
    // content_type_ = "multipart/form-data";
    content_type_ = "application/json";
    redirect_url_ = "";

    return kOk;
}/*}}}*/

Code HttpProto::CheckReqHeader(const std::string &stream_data)
{/*{{{*/
    if (stream_data.size() < kHttpReqHeaderMinLen || 
        ((stream_data.find(kCRLF) == std::string::npos) && (int(stream_data.size()) < kHttpReqHeaderMaxLen)))
        return kDataNotEnough;

    std::string http_req_reg_str = "(^GET|^POST) /.* [hH][tT]{2}[pP]/[0-9]\\.[0-9]";
    Reg reg(http_req_reg_str);
    Code ret = reg.Init();
    if (ret != kOk) return ret;

    ret = reg.Check(stream_data);
    if (ret != kOk) return ret;

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

Code HttpProto::GetRetMsg(int ret_code, std::string *ret_msg)
{/*{{{*/
    if (ret_msg == NULL) return kInvalidParam;

    switch (ret_code)
    {
        case kHttpStatusOk:
            ret_msg->assign("OK");
            break;
        case kHttpStatusMoved:
            ret_msg->assign("Moved Permanently");
            break;
        case kHttpStatusRedirect:
            ret_msg->assign("Move temporarily");
            break;
        case kHttpStatusRedirect_method:
            ret_msg->assign("See Other");
            break;
        case kHttpStatusNotModified:
            ret_msg->assign("Not Modified");
            break;
        case kHttpStatusRedirectKeepVerb:
            ret_msg->assign("Temporary Redirect");
            break;
        case kHttpStatusBadRequest:
            ret_msg->assign("Bad Redirect");
            break;
        case kHttpStatusUnauthorized:
            ret_msg->assign("Unauthorized");
            break;
        case kHttpStatusForbidden:
            ret_msg->assign("Forbidden");
            break;
        case kHttpStatusNotFound:
            ret_msg->assign("Not Found");
            break;
        case kHttpStatusInternalServerError:
            ret_msg->assign("Internal Server Error");
            break;
        case kHttpStatusServiceUnavail:
            ret_msg->assign("Server Unavailable");
            break;
        default:
            return kInvalidParam;
    }

    return kOk;
}/*}}}*/

const HttpType& HttpProto::GetHttpType()
{/*{{{*/
    return http_type_;
}/*}}}*/

const std::string& HttpProto::GetProtoVersion()
{/*{{{*/
    return proto_version_;
}/*}}}*/

const std::string& HttpProto::GetHost()
{/*{{{*/
    return host_;
}/*}}}*/

const uint16_t HttpProto::GetPort()
{/*{{{*/
    return port_;
}/*}}}*/

const std::string& HttpProto::GetRelativeUrl()
{/*{{{*/
    return relative_url_;
}/*}}}*/

const std::string& HttpProto::GetGetParams()
{/*{{{*/
    return get_params_;
}/*}}}*/

const std::string& HttpProto::GetPostParams()
{/*{{{*/
    return post_params_;
}/*}}}*/

}
