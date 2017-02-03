#include <stdint.h>

#inclCSUTILase/curl_http.h"

namespace base
{

static std::string g_http_data_buf;

size_t RespHttpData( char *ptr, size_t size, size_t nmemb, void *userdata)
{/*{{{*/
    if (ptr == NULL) return 0;
    uint32_t real_size = size * nmemb;

    g_http_data_buf.append(ptr, real_size);

    return real_size;
}/*}}}*/

CurlHttp::CurlHttp() : curl_(NULL)
{
}

CurlHttp::~CurlHttp()
{/*{{{*/
    if (curl_ != NULL)
    {
        curl_easy_cleanup(curl_);
        curl_ = NULL;
    }
}/*}}}*/

Code CurlHttp::Init()
{/*{{{*/
    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
    if (ret != 0) return kCurlGlobalInitFailed;

    curl_ = curl_easy_init();
    if (curl_ == NULL) return kCurlEasyInitFailed;

    curl_easy_setopt(curl_, CURLOPT_POST, 1);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, RespHttpData);

    return kOk;
}/*}}}*/

Code CurlHttp::Perform(const std::string &url, const std::string &content, std::string *result)
{/*{{{*/
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, content.c_str());

    CURLcode ret = curl_easy_perform(curl_);
    if (ret != 0) return kCurlEasyPerformFailed;

    result->assign(g_http_data_buf);
    g_http_data_buf.clear();

    return kOk;
}/*}}}*/


}

#ifdef _CURL_HTTP_MAIN_TEST_

#include <stdio.h>
int main(int argc, char *argv[])
{
    using namespace base;

    CurlHttp curl_http;
    curl_http.Init();

//    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string content;
    std::string result;

    Code r = curl_http.Perform(url, content, &result);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, content:%s, ret:%d\n",
                url.c_str(), content.c_str(), r);
        return r;
    }

    fprintf(stderr, "%s\n", result.c_str());

    return 0;
}


#endif
