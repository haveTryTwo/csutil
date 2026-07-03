// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// HTTP 网关：把浏览器的 REST+JSON 请求转换为 sock_rpc(protobuf) 调用 Control 服务。
// 设计要点见 docs/BOOK_MGR_RPC_DESIGN.md 第 13 节。
//   - 自带最小 HTTP 收发（支持 GET/POST/PUT/DELETE，框架 HttpProto 仅支持 GET/POST，故不复用其分帧）；
//   - 复用 base::RpcChannel 调 Control（线程局部长连接复用）；
//   - JSON 解析/生成使用内置 rapidjson。

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <functional>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "base/config.h"
#include "base/log.h"
#include "base/status.h"
#include "sock/demo_book/book_common.h"
#include "sock/demo_book/proto/book.pb.h"
#include "sock/rpc_channel.h"

namespace {

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;

const uint32_t kMaxRequestBytes = 1024 * 1024;  // 单请求上限 1MB，超过则截断处理
const int kListenBacklog = 128;
const std::string kApiPrefix = "/api/books";

// 进程级全局：Control 地址 + 静态页面内容（启动时载入一次）
std::string g_control_ip = "127.0.0.1";
uint16_t g_control_port = 9101;
std::string g_index_html;

/**
 * @brief 将业务返回码映射为 HTTP 状态码
 * @param book_ret_code 业务返回码（book_mgr::kBookRet*）
 * @return 对应的 HTTP 状态码
 */
int MapHttpStatus(int32_t book_ret_code) { /*{{{*/
  switch (book_ret_code) {
    case book_mgr::kBookRetOk:
      return 200;
    case book_mgr::kBookRetInvalidReq:
      return 400;
    case book_mgr::kBookRetNotFound:
      return 404;
    default:
      return 500;
  }
} /*}}}*/

/**
 * @brief 对 URL 路径做百分号解码（%XX -> 字节）
 * @param src 原始路径片段
 * @return 解码后的字符串
 */
std::string UrlDecode(const std::string &src) { /*{{{*/
  std::string out;
  out.reserve(src.size());
  for (uint32_t i = 0; i < src.size(); ++i) {
    if (src[i] == '%' && i + 2 < src.size()) {
      int hi = 0;
      int lo = 0;
      if (sscanf(src.c_str() + i + 1, "%1x%1x", &hi, &lo) == 2) {
        out.push_back(static_cast<char>((hi << 4) | lo));
        i += 2;
        continue;
      }
    }
    out.push_back(src[i]);
  }
  return out;
} /*}}}*/

/**
 * @brief 从 JSON 取整型字段，兼容数字与字符串（表单提交可能为字符串）
 * @param doc rapidjson 文档
 * @param key 字段名
 * @return 解析到的整型，缺失或非法返回 0
 */
int32_t JsonGetInt(const rapidjson::Document &doc, const char *key) { /*{{{*/
  if (!doc.HasMember(key)) return 0;
  const rapidjson::Value &v = doc[key];
  if (v.IsInt()) return v.GetInt();
  if (v.IsString()) return atoi(v.GetString());
  return 0;
} /*}}}*/

/**
 * @brief 从 JSON 取字符串字段
 * @param doc rapidjson 文档
 * @param key 字段名
 * @param value 输出字符串
 */
void JsonGetString(const rapidjson::Document &doc, const char *key, std::string *value) { /*{{{*/
  if (doc.HasMember(key) && doc[key].IsString()) value->assign(doc[key].GetString());
} /*}}}*/

/**
 * @brief 把 JSON 请求体解析为 Book
 * @param body JSON 文本
 * @param book 输出图书（仅填充存在的字段）
 * @return kOk 成功；kInvalidParam JSON 非法
 */
base::Code ParseBook(const std::string &body, book_mgr::Book *book) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (doc.HasParseError() || !doc.IsObject()) return base::kInvalidParam;

  std::string str;
  JsonGetString(doc, "book_id", &str);
  if (!str.empty()) book->set_book_id(str);
  str.clear();
  JsonGetString(doc, "title", &str);
  book->set_title(str);
  str.clear();
  JsonGetString(doc, "author", &str);
  book->set_author(str);
  str.clear();
  JsonGetString(doc, "category", &str);
  book->set_category(str);
  book->set_price(JsonGetInt(doc, "price"));
  book->set_stock(JsonGetInt(doc, "stock"));
  return base::kOk;
} /*}}}*/

/**
 * @brief 将一个 Book 写入 JSON（作为 "book" 对象）
 * @param writer JSON writer
 * @param book 图书
 */
void WriteBook(JsonWriter *writer, const book_mgr::Book &book) { /*{{{*/
  writer->Key("book");
  writer->StartObject();
  writer->Key("book_id");
  writer->String(book.book_id().c_str());
  writer->Key("title");
  writer->String(book.title().c_str());
  writer->Key("author");
  writer->String(book.author().c_str());
  writer->Key("price");
  writer->Int(book.price());
  writer->Key("stock");
  writer->Int(book.stock());
  writer->Key("category");
  writer->String(book.category().c_str());
  writer->EndObject();
} /*}}}*/

/**
 * @brief 调用 Control 服务（经线程局部长连接复用）
 * @param req BookReq 请求
 * @param resp BookResp 响应（输出）
 * @return RpcChannel::SendAndRecv 的返回码
 */
base::Code CallControl(const book_mgr::BookReq &req, book_mgr::BookResp *resp) { /*{{{*/
  base::RpcChannel *channel = base::RpcChannel::Get(g_control_ip, g_control_port);
  return channel->SendAndRecv(req, resp);
} /*}}}*/

/**
 * @brief 处理 Create：解析 body -> Create RPC -> JSON 响应
 * @param body 请求体 JSON
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void HandleCreate(const std::string &body, int *http_status, std::string *out) { /*{{{*/
  book_mgr::Book book;
  if (ParseBook(body, &book) != base::kOk || book.book_id().empty()) {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"invalid json or missing book_id\"}";
    return;
  }

  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeCreateReq(book), &resp) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"call control failed\"}";
    return;
  }

  const book_mgr::BaseResp &base_resp = resp.create_resp().base();
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(base_resp.ret_code());
  writer.Key("ret_msg");
  writer.String(base_resp.ret_msg().c_str());
  writer.Key("data");
  writer.StartObject();
  writer.Key("book_id");
  writer.String(resp.create_resp().book_id().c_str());
  writer.EndObject();
  writer.EndObject();
  *http_status = MapHttpStatus(base_resp.ret_code());
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief 处理 Read：Get RPC -> JSON 响应（含 from_cache 与 book）
 * @param book_id 图书主键
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void HandleRead(const std::string &book_id, int *http_status, std::string *out) { /*{{{*/
  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeGetReq(book_id), &resp) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"call control failed\"}";
    return;
  }

  const book_mgr::GetBookResp &get_resp = resp.get_resp();
  const book_mgr::BaseResp &base_resp = get_resp.base();
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(base_resp.ret_code());
  writer.Key("ret_msg");
  writer.String(base_resp.ret_msg().c_str());
  writer.Key("data");
  writer.StartObject();
  writer.Key("from_cache");
  writer.Bool(get_resp.from_cache());
  if (base_resp.ret_code() == book_mgr::kBookRetOk && get_resp.has_book()) WriteBook(&writer, get_resp.book());
  writer.EndObject();
  writer.EndObject();
  *http_status = MapHttpStatus(base_resp.ret_code());
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief 构造仅含 BaseResp 的 JSON（用于 Update / Delete）
 * @param base_resp 业务基础响应
 * @return JSON 文本
 */
std::string BuildBaseEnvelope(const book_mgr::BaseResp &base_resp) { /*{{{*/
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(base_resp.ret_code());
  writer.Key("ret_msg");
  writer.String(base_resp.ret_msg().c_str());
  writer.Key("data");
  writer.StartObject();
  writer.EndObject();
  writer.EndObject();
  return buf.GetString();
} /*}}}*/

/**
 * @brief 处理 Update：解析 body（主键以路径为准）-> Update RPC -> JSON 响应
 * @param book_id 路径中的图书主键
 * @param body 请求体 JSON
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void HandleUpdate(const std::string &book_id, const std::string &body, int *http_status, std::string *out) { /*{{{*/
  book_mgr::Book book;
  if (ParseBook(body, &book) != base::kOk) {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"invalid json\"}";
    return;
  }
  book.set_book_id(book_id);  // 主键以路径为准

  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeUpdateReq(book), &resp) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"call control failed\"}";
    return;
  }
  const book_mgr::BaseResp &base_resp = resp.update_resp().base();
  *http_status = MapHttpStatus(base_resp.ret_code());
  *out = BuildBaseEnvelope(base_resp);
} /*}}}*/

/**
 * @brief 处理 Delete：Delete RPC -> JSON 响应
 * @param book_id 图书主键
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void HandleDelete(const std::string &book_id, int *http_status, std::string *out) { /*{{{*/
  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeDeleteReq(book_id), &resp) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"call control failed\"}";
    return;
  }
  const book_mgr::BaseResp &base_resp = resp.delete_resp().base();
  *http_status = MapHttpStatus(base_resp.ret_code());
  *out = BuildBaseEnvelope(base_resp);
} /*}}}*/

/**
 * @brief 从 "/api/books/{id}" 路径中提取并解码 book_id；"/api/books" 返回空串
 * @param path 不含 query 的请求路径
 * @return 解码后的 book_id
 */
std::string ExtractBookId(const std::string &path) { /*{{{*/
  if (path.size() <= kApiPrefix.size() + 1) return "";
  return UrlDecode(path.substr(kApiPrefix.size() + 1));  // 跳过前缀和其后的 '/'
} /*}}}*/

/**
 * @brief 从 query string 中取出指定参数并 URL 解码
 * @param query 形如 "prefix=book_&limit=20" 的查询串（不含前导 '?'）
 * @param key 参数名
 * @param value 输出参数值（缺失则为空）
 */
void GetQueryParam(const std::string &query, const std::string &key, std::string *value) { /*{{{*/
  value->clear();
  std::string needle = key + "=";
  size_t pos = 0;
  while (pos < query.size()) {
    size_t amp = query.find('&', pos);
    std::string pair = query.substr(pos, (amp == std::string::npos) ? std::string::npos : amp - pos);
    if (pair.compare(0, needle.size(), needle) == 0) {
      value->assign(UrlDecode(pair.substr(needle.size())));
      return;
    }
    if (amp == std::string::npos) break;
    pos = amp + 1;
  }
} /*}}}*/

/**
 * @brief 处理 List：解析 query -> List RPC -> JSON 响应（books 数组 + next_cursor + total）
 * @param query 请求的查询串（prefix/limit/start_after）
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void HandleList(const std::string &query, int *http_status, std::string *out) { /*{{{*/
  std::string prefix;
  std::string start_after;
  std::string limit_str;
  GetQueryParam(query, "prefix", &prefix);
  GetQueryParam(query, "start_after", &start_after);
  GetQueryParam(query, "limit", &limit_str);
  uint32_t limit = limit_str.empty() ? 0 : static_cast<uint32_t>(atoi(limit_str.c_str()));

  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeListReq(prefix, limit, start_after), &resp) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"call control failed\"}";
    return;
  }

  const book_mgr::ListBooksResp &list_resp = resp.list_resp();
  const book_mgr::BaseResp &base_resp = list_resp.base();
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(base_resp.ret_code());
  writer.Key("ret_msg");
  writer.String(base_resp.ret_msg().c_str());
  writer.Key("data");
  writer.StartObject();
  writer.Key("total");
  writer.Int(list_resp.total());
  writer.Key("next_cursor");
  writer.String(list_resp.next_cursor().c_str());
  writer.Key("books");
  writer.StartArray();
  for (int i = 0; i < list_resp.books_size(); ++i) {
    writer.StartObject();
    const book_mgr::Book &book = list_resp.books(i);
    writer.Key("book_id");
    writer.String(book.book_id().c_str());
    writer.Key("title");
    writer.String(book.title().c_str());
    writer.Key("author");
    writer.String(book.author().c_str());
    writer.Key("price");
    writer.Int(book.price());
    writer.Key("stock");
    writer.Int(book.stock());
    writer.Key("category");
    writer.String(book.category().c_str());
    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = MapHttpStatus(base_resp.ret_code());
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief API 路由分发：按 (method, path) 调用对应处理函数
 * @param method HTTP 方法
 * @param path 不含 query 的请求路径
 * @param query 请求的查询串（不含前导 '?'）
 * @param body 请求体
 * @param http_status 输出 HTTP 状态码
 * @param out 输出响应 JSON 体
 */
void RouteApi(const std::string &method, const std::string &path, const std::string &query, const std::string &body,
              int *http_status, std::string *out) { /*{{{*/
  std::string book_id = ExtractBookId(path);

  if (method == "POST" && path == kApiPrefix) {
    HandleCreate(body, http_status, out);
  } else if (method == "GET" && path == kApiPrefix) {
    HandleList(query, http_status, out);
  } else if (method == "GET" && !book_id.empty()) {
    HandleRead(book_id, http_status, out);
  } else if (method == "PUT" && !book_id.empty()) {
    HandleUpdate(book_id, body, http_status, out);
  } else if (method == "DELETE" && !book_id.empty()) {
    HandleDelete(book_id, http_status, out);
  } else {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"unsupported route\"}";
  }
} /*}}}*/

/**
 * @brief 组装 HTTP 响应报文（固定 Connection: close）
 * @param status HTTP 状态码
 * @param content_type 内容类型
 * @param body 响应体
 * @return 完整 HTTP 响应字符串
 */
std::string BuildHttpResponse(int status, const std::string &content_type, const std::string &body) { /*{{{*/
  const char *reason = (status == 200)   ? "OK"
                       : (status == 400) ? "Bad Request"
                       : (status == 404) ? "Not Found"
                                         : "Internal Server Error";
  // 附带 CORS 头：允许以 file:// 直接打开页面（跨源）调用本网关
  char header[512] = {0};
  snprintf(header, sizeof(header),
           "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n"
           "Access-Control-Allow-Headers: Content-Type\r\n"
           "Connection: close\r\n\r\n",
           status, reason, content_type.c_str(), body.size());
  return std::string(header) + body;
} /*}}}*/

/**
 * @brief 解析请求头中的 Content-Length（大小写不敏感）
 * @param header 请求头文本（含结尾 CRLFCRLF）
 * @return 内容长度；缺失返回 0
 */
int ParseContentLength(const std::string &header) { /*{{{*/
  std::string lower;
  lower.reserve(header.size());
  for (uint32_t i = 0; i < header.size(); ++i) lower.push_back(tolower(header[i]));

  size_t pos = lower.find("content-length:");
  if (pos == std::string::npos) return 0;
  return atoi(header.c_str() + pos + strlen("content-length:"));
} /*}}}*/

/**
 * @brief 从连接读取一条完整 HTTP 请求（读到头部结束后按 Content-Length 续读 body）
 * @param fd 连接套接字
 * @param request 输出完整请求报文
 * @return true 读到完整请求；false 连接关闭或出错
 */
bool RecvHttpRequest(int fd, std::string *request) { /*{{{*/
  char buf[4096];
  size_t header_end = std::string::npos;
  int content_len = 0;

  while (true) {
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n <= 0) return !request->empty();
    request->append(buf, n);

    if (header_end == std::string::npos) {
      size_t pos = request->find("\r\n\r\n");
      if (pos != std::string::npos) {
        header_end = pos + 4;
        content_len = ParseContentLength(request->substr(0, header_end));
      }
    }
    if (header_end != std::string::npos) {
      if (content_len <= 0) return true;
      if (request->size() >= header_end + static_cast<size_t>(content_len)) return true;
    }
    if (request->size() > kMaxRequestBytes) return true;
  }
} /*}}}*/

/**
 * @brief 处理单条连接：读取请求 -> 路由（静态页 / API）-> 写回响应 -> 关闭
 * @param fd 连接套接字
 */
void HandleConn(int fd) { /*{{{*/
  std::string request;
  if (!RecvHttpRequest(fd, &request)) {
    close(fd);
    return;
  }

  // 解析请求行：METHOD SP PATH SP VERSION
  size_t sp1 = request.find(' ');
  size_t sp2 = (sp1 == std::string::npos) ? std::string::npos : request.find(' ', sp1 + 1);
  if (sp1 == std::string::npos || sp2 == std::string::npos) {
    std::string resp = BuildHttpResponse(400, "application/json", "{\"ret_code\":1,\"ret_msg\":\"bad request\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  std::string method = request.substr(0, sp1);
  std::string raw_path = request.substr(sp1 + 1, sp2 - sp1 - 1);
  size_t qpos = raw_path.find('?');
  std::string path = raw_path.substr(0, qpos);
  std::string query = (qpos == std::string::npos) ? "" : raw_path.substr(qpos + 1);

  size_t body_pos = request.find("\r\n\r\n");
  std::string body = (body_pos == std::string::npos) ? "" : request.substr(body_pos + 4);

  // CORS 预检请求：浏览器在跨源 PUT/DELETE/JSON-POST 前会先发 OPTIONS
  if (method == "OPTIONS") {
    std::string resp = BuildHttpResponse(200, "text/plain", "");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  std::string resp;
  if (method == "GET" && (path == "/" || path == "/index.html")) {
    resp = BuildHttpResponse(200, "text/html; charset=UTF-8", g_index_html);
  } else if (path.compare(0, kApiPrefix.size(), kApiPrefix) == 0) {
    int http_status = 200;
    std::string out;
    RouteApi(method, path, query, body, &http_status, &out);
    resp = BuildHttpResponse(http_status, "application/json; charset=UTF-8", out);
  } else {
    resp = BuildHttpResponse(404, "application/json", "{\"ret_code\":2,\"ret_msg\":\"not found\"}");
  }

  send(fd, resp.data(), resp.size(), 0);
  close(fd);
} /*}}}*/

/**
 * @brief 工作线程入口：在共享监听套接字上循环 accept 并处理连接
 * @param arg 监听套接字 fd（以指针传递）
 * @return NULL
 */
void *WorkerMain(void *arg) { /*{{{*/
  int listen_fd = static_cast<int>(reinterpret_cast<intptr_t>(arg));
  while (true) {
    int conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd < 0) {
      if (errno == EINTR) continue;
      break;
    }
    HandleConn(conn_fd);
  }
  return NULL;
} /*}}}*/

/**
 * @brief 读取本地静态页面到内存（启动时调用一次）
 * @param path 静态页面文件路径
 */
void LoadIndexHtml(const std::string &path) { /*{{{*/
  FILE *fp = fopen(path.c_str(), "rb");
  if (fp == NULL) {
    g_index_html = "<html><body><h3>book web gateway</h3><p>index.html not found</p></body></html>";
    LOG_ERR("Failed to open static page:%s, use fallback\n", path.c_str());
    return;
  }
  char buf[4096];
  size_t n = 0;
  while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) g_index_html.append(buf, n);
  fclose(fp);
} /*}}}*/

/**
 * @brief 创建并绑定监听套接字
 * @param port 监听端口
 * @return 监听 fd，失败返回 -1
 */
int CreateListenSocket(uint16_t port) { /*{{{*/
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) return -1;

  int opt = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(listen_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    close(listen_fd);
    return -1;
  }
  if (listen(listen_fd, kListenBacklog) < 0) {
    close(listen_fd);
    return -1;
  }
  return listen_fd;
} /*}}}*/

}  // namespace

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  signal(SIGPIPE, SIG_IGN);  // 避免向已关闭连接写数据时进程被 SIGPIPE 杀死

  std::string conf_path = "./conf/web_server.conf";
  std::string user_conf_path = "./conf/web_user_info.conf";
  int opt = 0;
  while ((opt = getopt(argc, argv, "s:u:h")) != -1) {
    switch (opt) {
      case 's':
        conf_path = optarg;
        break;
      case 'u':
        user_conf_path = optarg;
        break;
      case 'h':
      default:
        fprintf(stderr, "Usage: %s [-s server_conf] [-u user_conf]\n", argv[0]);
        return 0;
    }
  }

  Config config;
  if (config.LoadFile(conf_path) != kOk) fprintf(stderr, "Failed to load %s, use defaults\n", conf_path.c_str());

  int port = 8080;
  config.GetInt32Value("port", 8080, &port);
  int worker_num = 4;
  config.GetInt32Value("real_worker_thread_num", 4, &worker_num);
  if (worker_num <= 0) worker_num = 1;

  Config user_conf;
  std::string static_path = "./web/index.html";
  if (user_conf.LoadFile(user_conf_path) == kOk) {
    user_conf.GetValue("control_server_ip", "127.0.0.1", &g_control_ip);
    int control_port = 9101;
    user_conf.GetInt32Value("control_server_port", 9101, &control_port);
    g_control_port = static_cast<uint16_t>(control_port);
    user_conf.GetValue("static_index_path", "./web/index.html", &static_path);
  } else {
    fprintf(stderr, "Failed to load %s, use defaults\n", user_conf_path.c_str());
  }

  LoadIndexHtml(static_path);

  int listen_fd = CreateListenSocket(static_cast<uint16_t>(port));
  if (listen_fd < 0) {
    LOG_ERR("Failed to listen on port:%d, errno:%d, err:%s\n", port, errno, strerror(errno));
    return EXIT_FAILURE;
  }

  fprintf(stderr, "book web gateway listening on :%d -> control %s:%d (workers=%d)\n", port, g_control_ip.c_str(),
          g_control_port, worker_num);

  for (int i = 1; i < worker_num; ++i) {
    pthread_t tid;
    pthread_create(&tid, NULL, WorkerMain, reinterpret_cast<void *>(static_cast<intptr_t>(listen_fd)));
    pthread_detach(tid);
  }
  WorkerMain(reinterpret_cast<void *>(static_cast<intptr_t>(listen_fd)));  // 主线程也作为一个 worker

  close(listen_fd);
  return 0;
} /*}}}*/
