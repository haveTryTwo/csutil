// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTTP_HTTP_PROTO_H_
#define HTTP_HTTP_PROTO_H_

#include <string>

#include <stdint.h>

#include "base/status.h"

namespace base
{

const int kHttpRspHeaderMinLen = 13; // EX: HTTP/1.1 200 
const int kHttpReqHeaderMinLen = 14; // EX: GET / HTTP/1.1
const int kHttpReqHeaderMaxLen = 2083; // EX: max size of request line 
const std::string kCRLF = "\r\n";

const std::string kTransferEncoding = "Transfer-Encoding";
const std::string kChunked = "Chunked";
const std::string kContentLength = "Content-Length";
const std::string kLocation = "Location";
const std::string kHost = "Host";

enum HttpStatus
{
    /**
     *  1xx: Inforamtion Response – the request was received, continuing process
     */ 
    kContinue                       = 100, // server has received requset headers, client may proceed to send request body
    kSwitchingProtocols             = 101, // server has agreed to switch protocols
    
    /**
     * 2xx: Success – the request was successfully received, understood and accepted
     */
    kHttpStatusOk                   = 200, // OK

    /**
     * 3xx: Redirection – further action needs to be taken in order to complete the request
     */
    kHttpStatusMoved                = 301, // Moved Permanently
    kHttpStatusRedirect             = 302, // Move temporarily
    kHttpStatusRedirect_method      = 303, // See Other
    kHttpStatusNotModified          = 304, // Not Modified
    kHttpStatusRedirectKeepVerb     = 307, // Temporary Redirect

    /**
     * 4xx: Client errors – the request contains bad syntax or cannot be fulfilled
     */
    kHttpStatusBadRequest           = 400, // Bad Requst
    kHttpStatusUnauthorized         = 401, // Unauthorized
    kHttpStatusForbidden            = 403, // Forbidden
    kHttpStatusNotFound             = 404, // Not Found

    /**
     * 5xx: Server errors – the server failed to fulfill an apparently valid request
     */
    kHttpStatusInternalServerError  = 500, // Internal Server Error 
    kHttpStatusServiceUnavail       = 503, // Server Unavailable
};

enum HttpType
{
    GET,
    POST,
    PUT,
    DELETE,
};


Code HttpReqProtoFunc(const char *src_data, int src_data_len, int *real_len);
Code HttpRespProtoFunc(const char *src_data, int src_data_len, int *real_len);


class HttpProto
{
    public:
        HttpProto();
        ~HttpProto();

    public:
        Code SetHttpType(enum HttpType http_type);

        Code ParseUrl(const std::string &url);

        Code GetHeader(std::string *header);
        Code PostHeader(std::string *header);

        Code EncodeToReq(const std::string &url, const std::string &post_params, std::string *post_stream_data, std::string *host, uint16_t *port);
        Code EncodeToReq(const std::string &url, std::string *get_stream_data, std::string *host, uint16_t *port);
        Code DecodeFromReq(const std::string &stream_data);


        Code EncodeToResponse(int ret_code, const std::string &user_data, std::string *response);
        Code DecodeFromResponse(const std::string &stream_data, std::string *user_data);
        Code GetRespStatus(const std::string &stream_data, uint16_t *ret_code, std::string *ret_msg);

        Code Clear();

        Code SetUserNameAndPwd(const std::string &user_name, const std::string &pwd);
        Code ClearUserNameAndPwd();

    public:
        static Code GetMessageHeader(const std::string &upper_header, const std::string &msg_key, std::string *msg_value);
        static Code GetChunkedMsg(const std::string &body, uint32_t *real_body_len, std::string *chunked_msg);
        static Code CheckReqHeader(const std::string &stream_data);
        static Code CheckRespHeader(const std::string &stream_data);

        static Code GetRetMsg(int ret_code, std::string *ret_msg);

    public:
        const HttpType& GetHttpType();
        const std::string& GetProtoVersion();
        const std::string& GetHost();
        const uint16_t GetPort();
        const std::string& GetRelativeUrl();
        const std::string& GetGetParams();
        const std::string& GetPostParams();

    private:
        HttpType http_type_;

        std::string proto_;         // http://
        std::string proto_version_; // HTTP/1.1
        std::string host_;
        uint16_t port_;
        std::string relative_url_;
        std::string get_params_;
        std::string post_params_;
        std::string user_agent_;
        std::string content_type_;
        std::string redirect_url_;

        bool is_using_user_name_pwd_;
        std::string user_name_;
        std::string password_;
};

}

#endif
