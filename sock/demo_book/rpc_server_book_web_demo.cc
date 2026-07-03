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

#include <curl/curl.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "base/config.h"
#include "base/log.h"
#include "base/status.h"
#include "sock/demo_book/agent/agent_service.h"
#include "sock/demo_book/agent/memory_store.h"
#include "sock/demo_book/agent/session_store.h"
#include "sock/demo_book/book_common.h"
#include "sock/demo_book/proto/book.pb.h"
#include "sock/rpc_channel.h"

namespace {

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;

const uint32_t kMaxRequestBytes = 1024 * 1024;  // 单请求上限 1MB，超过则截断处理
const int kListenBacklog = 128;
const std::string kApiPrefix = "/api/books";
const std::string kSessPrefix = "/api/agent/sessions";
const std::string kMemPrefix = "/api/agent/memories";
const uint32_t kMaxSessions = 200;        // 列表最多返回的会话数
const uint32_t kAutoTitleMaxBytes = 30;   // 自动标题最大字节数
const uint32_t kMemInjectMax = 20;         // 单次注入到上下文的长期记忆条数上限（L3）
const uint32_t kMemListMax = 200;          // 记忆列表接口最大返回条数
const uint32_t kSummaryTriggerMsgs = 12;   // 会话消息数超过该值才触发滚动摘要（L2）
const uint32_t kKeepRecentMsgs = 6;        // 触发摘要后仍以原文保留的最近消息条数（L1 窗口）
const uint32_t kToolMaxItersDefault = 4;   // agentic 工具循环默认最大迭代次数（§16.6）
const int kBookToolListDefault = 20;       // list_books 工具默认返回条数
const int kBookToolListMax = 50;           // list_books 工具最大返回条数
const size_t kToolEventBriefMax = 600;     // SSE 工具事件 detail 展示截断长度

// 进程级全局：Control 地址 + 静态页面内容（启动时载入一次）
std::string g_control_ip = "127.0.0.1";
uint16_t g_control_port = 9101;
std::string g_index_html;

// Agent 编排服务（启动时 Init 一次，之后只读，可多线程共享）
book_agent::AgentService g_agent;

// Agent 会话/消息持久化（LevelDB，进程内单实例，内部加锁）
book_agent::SessionStore g_store;

// 长期记忆存储（独立 LevelDB，跨会话全局记忆；见 §16.5 L3）
book_agent::MemoryStore g_mem;

// 记忆机制总开关（关闭则不注入/不抽取/不摘要，但记忆管理接口仍可用）
bool g_memory_enable = true;

// 工具调用（agentic loop）总开关与最大迭代次数（§16.6）
bool g_tools_enable = true;
uint32_t g_tool_max_iters = kToolMaxItersDefault;

// 知识版本：本进程 Agent Init 成功的时刻。SKILL.md/路由表升级只在重启后生效，
// 用它标记「当前生效的技能与检索规则」，供陈旧会话提示对比。
int64_t g_knowledge_version_ms = 0;

/**
 * @brief 当前时间（毫秒）
 */
int64_t NowMs() { /*{{{*/
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<int64_t>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
} /*}}}*/

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
 * @brief Agent 工具回调：list_books / get_book（内部走 Control RPC），返回给模型的可读文本
 * @param name 工具名（list_books / get_book）
 * @param args_json 工具参数 JSON 串
 * @return 结果文本
 */
std::string BookToolQuery(const std::string &name, const std::string &args_json) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(args_json.c_str(), args_json.size());

  if (name == "get_book") {
    std::string book_id;
    if (!doc.HasParseError() && doc.IsObject() && doc.HasMember("book_id") && doc["book_id"].IsString()) {
      book_id = doc["book_id"].GetString();
    }
    if (book_id.empty()) return "get_book 缺少 book_id";
    book_mgr::BookResp resp;
    if (CallControl(book_mgr::MakeGetReq(book_id), &resp) != base::kOk) return "get_book 调用 Control 失败";
    const book_mgr::GetBookResp &g = resp.get_resp();
    if (g.base().ret_code() != book_mgr::kBookRetOk || !g.has_book()) return "get_book 未找到图书：" + book_id;
    const book_mgr::Book &b = g.book();
    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "book_id=%s title=%s author=%s price=%d stock=%d category=%s", b.book_id().c_str(),
             b.title().c_str(), b.author().c_str(), b.price(), b.stock(), b.category().c_str());
    return buf;
  }

  // list_books
  std::string prefix;
  int limit = 0;
  if (!doc.HasParseError() && doc.IsObject()) {
    if (doc.HasMember("prefix") && doc["prefix"].IsString()) prefix = doc["prefix"].GetString();
    if (doc.HasMember("limit") && doc["limit"].IsInt()) {
      limit = doc["limit"].GetInt();
    } else if (doc.HasMember("limit") && doc["limit"].IsString()) {
      limit = atoi(doc["limit"].GetString());
    }
  }
  if (limit <= 0 || limit > kBookToolListMax) limit = kBookToolListDefault;
  book_mgr::BookResp resp;
  if (CallControl(book_mgr::MakeListReq(prefix, static_cast<uint32_t>(limit), ""), &resp) != base::kOk) {
    return "list_books 调用 Control 失败";
  }
  const book_mgr::ListBooksResp &l = resp.list_resp();
  char head[128] = {0};
  snprintf(head, sizeof(head), "共 %d 本，返回 %d 本：\n", l.total(), l.books_size());
  std::string out = head;
  for (int i = 0; i < l.books_size(); ++i) {
    const book_mgr::Book &b = l.books(i);
    out += "- " + b.book_id() + " 《" + b.title() + "》 " + b.author() + "\n";
  }
  return out;
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

// 前置声明：BuildHttpResponse 定义在文件后段，Agent 处理函数先用到
std::string BuildHttpResponse(int status, const std::string &content_type, const std::string &body);

/**
 * @brief GET /api/agent/models：列出可用模型（不含密钥/base_url）
 */
void HandleAgentModels(int *http_status, std::string *out) { /*{{{*/
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  writer.StartObject();
  writer.Key("default");
  writer.String(g_agent.Config().DefaultModel().c_str());
  writer.Key("models");
  writer.StartArray();
  std::vector<const book_agent::ModelProvider *> providers;
  g_agent.Config().List(&providers);
  for (uint32_t i = 0; i < providers.size(); ++i) {
    writer.StartObject();
    writer.Key("name");
    writer.String(providers[i]->name.c_str());
    writer.Key("model");
    writer.String(providers[i]->model.c_str());
    writer.Key("available");
    writer.Bool(book_agent::AgentConfig::IsAvailable(*providers[i]));
    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief GET /api/agent/agents：列出领域 Agent（含 auto 与各 SKILL 的 name/description）
 */
void HandleAgentAgents(int *http_status, std::string *out) { /*{{{*/
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  writer.StartObject();
  writer.Key("agents");
  writer.StartArray();
  writer.StartObject();
  writer.Key("name");
  writer.String("auto");
  writer.Key("desc");
  writer.String("自动路由到最合适的领域");
  writer.EndObject();
  std::vector<const book_agent::Skill *> skills;
  g_agent.Skills().List(&skills);
  for (uint32_t i = 0; i < skills.size(); ++i) {
    writer.StartObject();
    writer.Key("name");
    writer.String(skills[i]->name.c_str());
    writer.Key("desc");
    writer.String(skills[i]->description.c_str(), static_cast<rapidjson::SizeType>(skills[i]->description.size()));
    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief 解析 /api/agent/chat 请求体（model/domain/messages）
 * @return true 解析成功且 messages 非空
 */
bool ParseChatRequest(const std::string &body, std::string *model, std::string *domain,
                      std::vector<book_agent::ChatMessage> *msgs) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (doc.HasParseError() || !doc.IsObject()) return false;

  if (doc.HasMember("model") && doc["model"].IsString()) model->assign(doc["model"].GetString());
  if (doc.HasMember("domain") && doc["domain"].IsString()) {
    domain->assign(doc["domain"].GetString());
  } else {
    *domain = "auto";
  }
  if (!doc.HasMember("messages") || !doc["messages"].IsArray()) return false;

  const rapidjson::Value &arr = doc["messages"];
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const rapidjson::Value &m = arr[i];
    if (!m.IsObject() || !m.HasMember("role") || !m.HasMember("content")) continue;
    if (!m["role"].IsString() || !m["content"].IsString()) continue;
    book_agent::ChatMessage cm;
    cm.role.assign(m["role"].GetString());
    cm.content.assign(m["content"].GetString(), m["content"].GetStringLength());
    msgs->push_back(cm);
  }
  return !msgs->empty();
} /*}}}*/

/**
 * @brief 直接向连接 fd 写一段 SSE data（delta 经 JSON 转义）
 * @return send 成功返回 true；对端断开返回 false（用于中止拉流）
 */
bool SendSseDelta(int fd, const std::string &delta) { /*{{{*/
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("delta");
  writer.String(delta.c_str(), static_cast<rapidjson::SizeType>(delta.size()));
  writer.EndObject();
  std::string line = "data: ";
  line += buf.GetString();
  line += "\n\n";
  return send(fd, line.data(), line.size(), 0) >= 0;
} /*}}}*/

/**
 * @brief POST /api/agent/chat：流式问答。自行发送 SSE 头/增量/[DONE] 并关闭连接。
 * @param fd 连接套接字
 * @param body 请求体
 */
void HandleAgentChat(int fd, const std::string &body) { /*{{{*/
  if (!g_agent.IsReady()) {
    std::string resp = BuildHttpResponse(503, "application/json", "{\"ret_code\":4,\"ret_msg\":\"agent not ready\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  std::string model;
  std::string domain;
  std::vector<book_agent::ChatMessage> msgs;
  if (!ParseChatRequest(body, &model, &domain, &msgs)) {
    std::string resp = BuildHttpResponse(400, "application/json", "{\"ret_code\":1,\"ret_msg\":\"invalid chat req\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  // SSE 响应头：不带 Content-Length，逐段写回
  const char *sse_header =
      "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream; charset=UTF-8\r\n"
      "Cache-Control: no-cache\r\nConnection: close\r\n"
      "Access-Control-Allow-Origin: *\r\n"
      "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n"
      "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
  send(fd, sse_header, strlen(sse_header), 0);

  book_agent::AgentService::MetaCallback on_meta = [fd](const std::string &m, const std::string &d) {
    rapidjson::StringBuffer buf;
    JsonWriter writer(buf);
    writer.StartObject();
    writer.Key("model");
    writer.String(m.c_str());
    writer.Key("domain");
    writer.String(d.c_str());
    writer.EndObject();
    std::string line = "event: meta\ndata: ";
    line += buf.GetString();
    line += "\n\n";
    send(fd, line.data(), line.size(), 0);
  };
  book_agent::LlmClient::DeltaCallback on_delta = [fd](const std::string &delta) -> bool {
    return SendSseDelta(fd, delta);
  };

  std::vector<std::string> no_extra;  // 无状态 /api/agent/chat 不注入记忆/摘要
  base::Code ret = g_agent.ChatStream(model, domain, msgs, no_extra, on_meta, on_delta);
  if (ret != base::kOk) {
    rapidjson::StringBuffer buf;
    JsonWriter writer(buf);
    writer.StartObject();
    writer.Key("error");
    writer.String("llm call failed");
    writer.Key("code");
    writer.Int(ret);
    writer.EndObject();
    std::string line = "data: ";
    line += buf.GetString();
    line += "\n\n";
    send(fd, line.data(), line.size(), 0);
  }
  const char *done = "data: [DONE]\n\n";
  send(fd, done, strlen(done), 0);
  close(fd);
} /*}}}*/

// ================= Agent 会话（多会话 + 持久化，阶段一） =================

/**
 * @brief 解析会话路径：/api/agent/sessions[/{id}[/{action}]]
 * @param path 请求路径
 * @param id 输出会话 id（集合路径为空）
 * @param action 输出子动作（chat/regenerate；无则空）
 * @return true 表示命中会话路径族
 */
bool ParseSessionPath(const std::string &path, std::string *id, std::string *action) { /*{{{*/
  id->clear();
  action->clear();
  if (path == kSessPrefix) return true;  // 集合：列表/新建
  std::string prefix = kSessPrefix + "/";
  if (path.compare(0, prefix.size(), prefix) != 0) return false;
  std::string rest = path.substr(prefix.size());
  std::string::size_type slash = rest.find('/');
  if (slash == std::string::npos) {
    *id = UrlDecode(rest);
  } else {
    *id = UrlDecode(rest.substr(0, slash));
    *action = rest.substr(slash + 1);
  }
  return true;
} /*}}}*/

/**
 * @brief 由首个问题生成自动标题：取首行、UTF-8 安全地截断到 kAutoTitleMaxBytes
 * @param seed 用户问题
 * @return 标题
 */
std::string MakeAutoTitle(const std::string &seed) { /*{{{*/
  std::string line = seed;
  std::string::size_type nl = line.find('\n');
  if (nl != std::string::npos) line = line.substr(0, nl);
  uint32_t begin = 0;
  uint32_t end = static_cast<uint32_t>(line.size());
  while (begin < end && (line[begin] == ' ' || line[begin] == '\t')) ++begin;
  while (end > begin && (line[end - 1] == ' ' || line[end - 1] == '\t' || line[end - 1] == '\r')) --end;
  line = line.substr(begin, end - begin);
  if (line.size() <= kAutoTitleMaxBytes) return line;

  uint32_t cut = kAutoTitleMaxBytes;
  while (cut > 0 && (static_cast<unsigned char>(line[cut]) & 0xC0) == 0x80) --cut;  // 回退到 UTF-8 起始字节
  return line.substr(0, cut) + "…";
} /*}}}*/

/**
 * @brief 把 SessionMeta 写成 JSON 对象（写在当前 writer 位置）
 *
 * 额外附带 stale：该会话最近一条 assistant 回复所用的知识版本是否不同于当前进程的
 * 知识版本（即 SKILL.md/路由表在此之后升级过并重启，或该会话早于本特性上线、版本未知）。
 * 前端据此提示「建议新建会话」，避免用户误信重启/升级前生成的过期回答。
 */
void WriteSessionObj(JsonWriter *writer, const book_agent::SessionMeta &meta) { /*{{{*/
  bool stale = meta.msg_count > 0 && meta.know_ver_ms != g_knowledge_version_ms;
  writer->StartObject();
  writer->Key("session_id");
  writer->String(meta.session_id.c_str());
  writer->Key("title");
  writer->String(meta.title.c_str(), static_cast<rapidjson::SizeType>(meta.title.size()));
  writer->Key("created_ms");
  writer->Int64(meta.created_ms);
  writer->Key("updated_ms");
  writer->Int64(meta.updated_ms);
  writer->Key("model");
  writer->String(meta.model.c_str());
  writer->Key("domain");
  writer->String(meta.domain.c_str());
  writer->Key("msg_count");
  writer->Uint(meta.msg_count);
  writer->Key("stale");
  writer->Bool(stale);
  writer->EndObject();
} /*}}}*/

/**
 * @brief GET /api/agent/sessions：按最近更新倒序列出会话
 */
void HandleSessionList(int *http_status, std::string *out) { /*{{{*/
  std::vector<book_agent::SessionMeta> sessions;
  g_store.ListSessions(kMaxSessions, &sessions);
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  writer.StartObject();
  writer.Key("sessions");
  writer.StartArray();
  for (uint32_t i = 0; i < sessions.size(); ++i) WriteSessionObj(&writer, sessions[i]);
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief POST /api/agent/sessions：新建会话（可带 title/model/domain）
 */
void HandleSessionCreate(const std::string &body, int *http_status, std::string *out) { /*{{{*/
  std::string title;
  std::string model;
  std::string domain;
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (!doc.HasParseError() && doc.IsObject()) {
    JsonGetString(doc, "title", &title);
    JsonGetString(doc, "model", &model);
    JsonGetString(doc, "domain", &domain);
  }

  book_agent::SessionMeta meta;
  if (g_store.CreateSession(title, model, domain, g_knowledge_version_ms, &meta) != base::kOk) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"create session failed\"}";
    return;
  }
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  WriteSessionObj(&writer, meta);
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief GET /api/agent/sessions/{id}：返回会话元信息 + 全量消息（重开恢复）
 */
void HandleSessionGet(const std::string &id, int *http_status, std::string *out) { /*{{{*/
  book_agent::SessionMeta meta;
  if (g_store.GetSession(id, &meta) != base::kOk) {
    *http_status = 404;
    *out = "{\"ret_code\":2,\"ret_msg\":\"session not found\"}";
    return;
  }
  std::vector<book_agent::StoredMessage> msgs;
  g_store.GetMessages(id, &msgs);

  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  writer.StartObject();
  writer.Key("session");
  WriteSessionObj(&writer, meta);
  writer.Key("messages");
  writer.StartArray();
  for (uint32_t i = 0; i < msgs.size(); ++i) {
    writer.StartObject();
    writer.Key("seq");
    writer.Uint(msgs[i].seq);
    writer.Key("role");
    writer.String(msgs[i].role.c_str());
    writer.Key("content");
    writer.String(msgs[i].content.c_str(), static_cast<rapidjson::SizeType>(msgs[i].content.size()));
    writer.Key("model");
    writer.String(msgs[i].model.c_str());
    writer.Key("domain");
    writer.String(msgs[i].domain.c_str());
    writer.Key("ts_ms");
    writer.Int64(msgs[i].ts_ms);
    writer.Key("partial");
    writer.Bool(msgs[i].partial);
    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief PATCH /api/agent/sessions/{id}：改标题/模型/领域
 */
void HandleSessionPatch(const std::string &id, const std::string &body, int *http_status, std::string *out) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (doc.HasParseError() || !doc.IsObject()) {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"invalid json\"}";
    return;
  }
  std::string title;
  std::string model;
  std::string domain;
  bool has_title = doc.HasMember("title") && doc["title"].IsString();
  bool has_model = doc.HasMember("model") && doc["model"].IsString();
  bool has_domain = doc.HasMember("domain") && doc["domain"].IsString();
  if (has_title) title = doc["title"].GetString();
  if (has_model) model = doc["model"].GetString();
  if (has_domain) domain = doc["domain"].GetString();

  base::Code ret = g_store.PatchSession(id, has_title ? &title : NULL, has_model ? &model : NULL,
                                        has_domain ? &domain : NULL);
  if (ret == base::kNotFound) {
    *http_status = 404;
    *out = "{\"ret_code\":2,\"ret_msg\":\"session not found\"}";
    return;
  }
  *http_status = (ret == base::kOk) ? 200 : 500;
  *out = (ret == base::kOk) ? "{\"ret_code\":0,\"ret_msg\":\"success\",\"data\":{}}"
                            : "{\"ret_code\":4,\"ret_msg\":\"patch failed\"}";
} /*}}}*/

/**
 * @brief DELETE /api/agent/sessions/{id}：删除会话及其消息
 */
void HandleSessionDelete(const std::string &id, int *http_status, std::string *out) { /*{{{*/
  g_store.DeleteSession(id);
  *http_status = 200;
  *out = "{\"ret_code\":0,\"ret_msg\":\"success\",\"data\":{}}";
} /*}}}*/

/**
 * @brief 非流式会话路由（列表/新建/取详情/改/删）
 */
void RouteAgentSession(const std::string &method, const std::string &path, const std::string &body, int *http_status,
                       std::string *out) { /*{{{*/
  if (!g_store.IsReady()) {
    *http_status = 503;
    *out = "{\"ret_code\":4,\"ret_msg\":\"session store not ready\"}";
    return;
  }
  std::string id;
  std::string action;
  ParseSessionPath(path, &id, &action);

  if (id.empty()) {
    if (method == "GET") {
      HandleSessionList(http_status, out);
    } else if (method == "POST") {
      HandleSessionCreate(body, http_status, out);
    } else {
      *http_status = 400;
      *out = "{\"ret_code\":1,\"ret_msg\":\"unsupported route\"}";
    }
  } else if (action.empty()) {
    if (method == "GET") {
      HandleSessionGet(id, http_status, out);
    } else if (method == "PATCH") {
      HandleSessionPatch(id, body, http_status, out);
    } else if (method == "DELETE") {
      HandleSessionDelete(id, http_status, out);
    } else {
      *http_status = 400;
      *out = "{\"ret_code\":1,\"ret_msg\":\"unsupported route\"}";
    }
  } else {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"use POST for chat/regenerate\"}";
  }
} /*}}}*/

/**
 * @brief 解析记忆路径：/api/agent/memories[/{id}]
 * @param path 请求路径
 * @param id 输出记忆 id（集合路径为空）
 * @return true 命中记忆路径族
 */
bool ParseMemoryPath(const std::string &path, std::string *id) { /*{{{*/
  id->clear();
  if (path == kMemPrefix) return true;
  std::string prefix = kMemPrefix + "/";
  if (path.compare(0, prefix.size(), prefix) != 0) return false;
  *id = UrlDecode(path.substr(prefix.size()));
  return true;
} /*}}}*/

/**
 * @brief 把 MemoryItem 写成 JSON 对象（写在当前 writer 位置）
 */
void WriteMemoryObj(JsonWriter *writer, const book_agent::MemoryItem &m) { /*{{{*/
  writer->StartObject();
  writer->Key("mem_id");
  writer->String(m.mem_id.c_str());
  writer->Key("kind");
  writer->String(m.kind.c_str());
  writer->Key("text");
  writer->String(m.text.c_str(), static_cast<rapidjson::SizeType>(m.text.size()));
  writer->Key("source_session");
  writer->String(m.source_session.c_str());
  writer->Key("created_ms");
  writer->Int64(m.created_ms);
  writer->Key("updated_ms");
  writer->Int64(m.updated_ms);
  writer->EndObject();
} /*}}}*/

/**
 * @brief GET /api/agent/memories：列出长期记忆（按创建倒序）
 */
void HandleMemoryList(int *http_status, std::string *out) { /*{{{*/
  std::vector<book_agent::MemoryItem> items;
  g_mem.List("", kMemListMax, &items);
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String("success");
  writer.Key("data");
  writer.StartObject();
  writer.Key("enabled");
  writer.Bool(g_memory_enable);
  writer.Key("memories");
  writer.StartArray();
  for (uint32_t i = 0; i < items.size(); ++i) WriteMemoryObj(&writer, items[i]);
  writer.EndArray();
  writer.EndObject();
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief POST /api/agent/memories：手动新增一条记忆 {kind?,text}
 */
void HandleMemoryCreate(const std::string &body, int *http_status, std::string *out) { /*{{{*/
  std::string kind;
  std::string text;
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (!doc.HasParseError() && doc.IsObject()) {
    JsonGetString(doc, "kind", &kind);
    JsonGetString(doc, "text", &text);
  }
  if (text.empty()) {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"empty text\"}";
    return;
  }
  book_agent::MemoryItem item;
  base::Code ret = g_mem.Create(kind, text, "", &item);  // kOk 新增 / kExist 命中去重
  if (ret != base::kOk && ret != base::kExist) {
    *http_status = 500;
    *out = "{\"ret_code\":4,\"ret_msg\":\"create memory failed\"}";
    return;
  }
  rapidjson::StringBuffer buf;
  JsonWriter writer(buf);
  writer.StartObject();
  writer.Key("ret_code");
  writer.Int(0);
  writer.Key("ret_msg");
  writer.String(ret == base::kExist ? "exist" : "success");
  writer.Key("data");
  WriteMemoryObj(&writer, item);
  writer.EndObject();
  *http_status = 200;
  *out = buf.GetString();
} /*}}}*/

/**
 * @brief PATCH /api/agent/memories/{id}：修改记忆 {kind?,text?}
 */
void HandleMemoryPatch(const std::string &id, const std::string &body, int *http_status, std::string *out) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  if (doc.HasParseError() || !doc.IsObject()) {
    *http_status = 400;
    *out = "{\"ret_code\":1,\"ret_msg\":\"invalid json\"}";
    return;
  }
  std::string kind;
  std::string text;
  bool has_kind = doc.HasMember("kind") && doc["kind"].IsString();
  bool has_text = doc.HasMember("text") && doc["text"].IsString();
  if (has_kind) kind = doc["kind"].GetString();
  if (has_text) text.assign(doc["text"].GetString(), doc["text"].GetStringLength());

  base::Code ret = g_mem.Patch(id, has_kind ? &kind : NULL, has_text ? &text : NULL);
  if (ret == base::kNotFound) {
    *http_status = 404;
    *out = "{\"ret_code\":2,\"ret_msg\":\"memory not found\"}";
    return;
  }
  *http_status = (ret == base::kOk) ? 200 : 500;
  *out = (ret == base::kOk) ? "{\"ret_code\":0,\"ret_msg\":\"success\",\"data\":{}}"
                            : "{\"ret_code\":4,\"ret_msg\":\"patch failed\"}";
} /*}}}*/

/**
 * @brief DELETE /api/agent/memories/{id}：删除一条记忆
 */
void HandleMemoryDelete(const std::string &id, int *http_status, std::string *out) { /*{{{*/
  g_mem.Delete(id);
  *http_status = 200;
  *out = "{\"ret_code\":0,\"ret_msg\":\"success\",\"data\":{}}";
} /*}}}*/

/**
 * @brief 长期记忆路由（列表/新增/改/删）
 */
void RouteAgentMemory(const std::string &method, const std::string &path, const std::string &body, int *http_status,
                      std::string *out) { /*{{{*/
  if (!g_mem.IsReady()) {
    *http_status = 503;
    *out = "{\"ret_code\":4,\"ret_msg\":\"memory store not ready\"}";
    return;
  }
  std::string id;
  ParseMemoryPath(path, &id);

  if (id.empty()) {
    if (method == "GET") {
      HandleMemoryList(http_status, out);
    } else if (method == "POST") {
      HandleMemoryCreate(body, http_status, out);
    } else {
      *http_status = 400;
      *out = "{\"ret_code\":1,\"ret_msg\":\"unsupported route\"}";
    }
  } else {
    if (method == "PATCH") {
      HandleMemoryPatch(id, body, http_status, out);
    } else if (method == "DELETE") {
      HandleMemoryDelete(id, http_status, out);
    } else {
      *http_status = 400;
      *out = "{\"ret_code\":1,\"ret_msg\":\"unsupported route\"}";
    }
  }
} /*}}}*/

/**
 * @brief 组装长期记忆注入块（L3）：列出全局记忆，格式化为一段 system 文本
 * @return 记忆块文本；无记忆/未启用则空串
 */
std::string BuildMemoryBlock() { /*{{{*/
  if (!g_memory_enable || !g_mem.IsReady()) return "";
  std::vector<book_agent::MemoryItem> items;
  g_mem.List("", kMemInjectMax, &items);
  if (items.empty()) return "";
  std::string block = "关于用户/项目的已知信息（长期记忆，若与当前问题无关可忽略）：\n";
  for (uint32_t i = 0; i < items.size(); ++i) {
    block += "- [" + items[i].kind + "] " + items[i].text + "\n";
  }
  return block;
} /*}}}*/

/**
 * @brief 组装滚动摘要注入块（L2）
 * @param summary 会话摘要
 * @return 摘要块文本；无摘要则空串
 */
std::string BuildSummaryBlock(const book_agent::SummaryData &summary) { /*{{{*/
  if (summary.text.empty()) return "";
  return "此前较早的对话已浓缩为如下摘要（仅供参考，最近若干轮为原文）：\n" + summary.text;
} /*}}}*/

/**
 * @brief 后置滚动摘要（L2）：会话过长时，把「保留窗口之前的旧消息」并入摘要，推进 covered_seq
 * @param session_id 会话 id
 * @param req_model 摘要所用模型（沿用本轮模型）
 *
 * 触发条件：总消息数 > kSummaryTriggerMsgs，且新的覆盖上界（total-kKeepRecentMsgs）超过既有 covered_seq。
 * 最佳努力：任何一步失败都静默跳过，不影响主流程。
 */
void RunRollingSummary(const std::string &session_id, const std::string &req_model) { /*{{{*/
  book_agent::SessionMeta meta;
  if (g_store.GetSession(session_id, &meta) != base::kOk) return;
  if (meta.msg_count <= kSummaryTriggerMsgs) return;

  book_agent::SummaryData summary;
  g_store.GetSummary(session_id, &summary);
  uint32_t new_covered = meta.msg_count - kKeepRecentMsgs;
  if (new_covered <= summary.covered_seq) return;  // 无新增可覆盖

  std::vector<book_agent::StoredMessage> msgs;
  g_store.GetMessages(session_id, &msgs);
  std::vector<book_agent::ChatMessage> to_cover;
  for (uint32_t i = 0; i < msgs.size(); ++i) {
    if (msgs[i].seq >= summary.covered_seq && msgs[i].seq < new_covered) {
      book_agent::ChatMessage cm;
      cm.role = msgs[i].role;
      cm.content = msgs[i].content;
      to_cover.push_back(cm);
    }
  }
  if (to_cover.empty()) return;

  std::string new_summary;
  if (g_agent.Summarize(req_model, summary.text, to_cover, &new_summary) == base::kOk && !new_summary.empty()) {
    g_store.PutSummary(session_id, new_covered, new_summary);
    fprintf(stderr, "[memory] rolling summary updated: session=%s covered_seq=%u->%u summary_bytes=%zu\n",
            session_id.c_str(), summary.covered_seq, new_covered, new_summary.size());
  }
} /*}}}*/

/**
 * @brief 一轮回答结束后的记忆维护（在响应已发完、连接关闭后执行，不阻塞用户）：
 *        1) 从（末条 user + 本轮 assistant）抽取长期记忆并去重落库；2) 触发滚动摘要。
 * @param session_id 会话 id
 * @param req_model 本轮所用模型
 * @param history 本轮请求历史（用于取末条 user）
 * @param answer 本轮 assistant 全文
 */
void RunMemoryMaintenance(const std::string &session_id, const std::string &req_model,
                          const std::vector<book_agent::ChatMessage> &history, const std::string &answer) { /*{{{*/
  if (!g_memory_enable || answer.empty()) return;

  std::string last_user;
  for (uint32_t i = static_cast<uint32_t>(history.size()); i > 0; --i) {
    if (history[i - 1].role == "user") {
      last_user = history[i - 1].content;
      break;
    }
  }

  if (!last_user.empty() && g_mem.IsReady()) {
    std::vector<book_agent::MemoryCandidate> cands;
    if (g_agent.ExtractMemories(req_model, last_user, answer, &cands) == base::kOk) {
      for (uint32_t i = 0; i < cands.size(); ++i) {
        g_mem.Create(cands[i].kind, cands[i].text, session_id, NULL);  // 内部去重
      }
    }
  }

  RunRollingSummary(session_id, req_model);
} /*}}}*/

/**
 * @brief 会话内一次流式作答的公共核心：SSE 头 -> meta -> delta -> 落库 assistant -> [DONE]
 * @param fd 连接
 * @param session_id 会话 id
 * @param req_model 请求模型（可空）
 * @param req_domain 请求领域（可空/auto）
 * @param history 历史窗口（含本轮 user；摘要生效时为 covered_seq 之后的原文窗口）
 * @param extra_systems 额外 system 块（长期记忆/滚动摘要），注入在 SKILL/检索之后、history 之前
 * @param set_title 是否在结束后写自动标题
 * @param title_seed 自动标题来源（首个问题）
 */
void StreamSessionReply(int fd, const std::string &session_id, const std::string &req_model,
                        const std::string &req_domain, const std::vector<book_agent::ChatMessage> &history,
                        const std::vector<std::string> &extra_systems, bool set_title,
                        const std::string &title_seed) { /*{{{*/
  const char *sse_header =
      "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream; charset=UTF-8\r\n"
      "Cache-Control: no-cache\r\nConnection: close\r\n"
      "Access-Control-Allow-Origin: *\r\n"
      "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n"
      "Access-Control-Allow-Headers: Content-Type\r\n\r\n";
  send(fd, sse_header, strlen(sse_header), 0);

  std::string answer;
  std::string used_model = req_model;
  std::string used_domain = req_domain;

  book_agent::AgentService::MetaCallback on_meta = [fd, &used_model, &used_domain,
                                                    &session_id](const std::string &m, const std::string &d) {
    used_model = m;
    used_domain = d;
    rapidjson::StringBuffer buf;
    JsonWriter writer(buf);
    writer.StartObject();
    writer.Key("model");
    writer.String(m.c_str());
    writer.Key("domain");
    writer.String(d.c_str());
    writer.Key("session_id");
    writer.String(session_id.c_str());
    writer.EndObject();
    std::string line = "event: meta\ndata: ";
    line += buf.GetString();
    line += "\n\n";
    send(fd, line.data(), line.size(), 0);
  };
  book_agent::LlmClient::DeltaCallback on_delta = [fd, &answer](const std::string &delta) -> bool {
    answer += delta;
    return SendSseDelta(fd, delta);
  };
  // 工具过程事件：以 SSE 自定义事件下发，前端渲染为可折叠「工具卡片」
  book_agent::AgentService::ToolEventCallback on_tool_event = [fd](const std::string &phase, const std::string &name,
                                                                   const std::string &detail) {
    rapidjson::StringBuffer buf;
    JsonWriter writer(buf);
    writer.StartObject();
    writer.Key("name");
    writer.String(name.c_str());
    writer.Key("detail");
    // 结果可能较大，截断到合理长度供前端展示（模型侧仍拿到完整结果）
    std::string brief = detail.size() > kToolEventBriefMax ? detail.substr(0, kToolEventBriefMax) + " …" : detail;
    writer.String(brief.c_str(), static_cast<rapidjson::SizeType>(brief.size()));
    writer.EndObject();
    std::string line = "event: " + phase + "\ndata: ";
    line += buf.GetString();
    line += "\n\n";
    send(fd, line.data(), line.size(), 0);
  };

  base::Code ret;
  if (g_tools_enable && g_agent.ToolsEnabled()) {
    ret = g_agent.ChatStreamAgentic(req_model, req_domain, history, extra_systems, g_tool_max_iters, on_meta,
                                    on_tool_event, on_delta);
  } else {
    ret = g_agent.ChatStream(req_model, req_domain, history, extra_systems, on_meta, on_delta);
  }
  if (ret != base::kOk && answer.empty()) {
    rapidjson::StringBuffer buf;
    JsonWriter writer(buf);
    writer.StartObject();
    writer.Key("error");
    writer.String("llm call failed");
    writer.Key("code");
    writer.Int(ret);
    writer.EndObject();
    std::string line = "data: ";
    line += buf.GetString();
    line += "\n\n";
    send(fd, line.data(), line.size(), 0);
  }

  if (!answer.empty()) {
    g_store.AppendMessage(session_id, "assistant", answer, used_model, used_domain, false, g_knowledge_version_ms,
                          NULL);
  }
  if (set_title && !title_seed.empty()) {
    std::string title = MakeAutoTitle(title_seed);
    g_store.PatchSession(session_id, &title, NULL, NULL);
  }

  const char *done = "data: [DONE]\n\n";
  send(fd, done, strlen(done), 0);
  close(fd);

  // 连接已关闭、用户已拿到回答；此后做记忆抽取与滚动摘要（LLM 调用较慢，但不再阻塞用户）
  RunMemoryMaintenance(session_id, used_model, history, answer);
} /*}}}*/

/**
 * @brief POST /api/agent/sessions/{id}/chat：会话内提问，服务端落库并流式回答
 */
void HandleSessionChat(int fd, const std::string &id, const std::string &body) { /*{{{*/
  if (!g_agent.IsReady() || !g_store.IsReady()) {
    std::string resp = BuildHttpResponse(503, "application/json", "{\"ret_code\":4,\"ret_msg\":\"agent not ready\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  rapidjson::Document doc;
  doc.Parse(body.c_str(), body.size());
  std::string content;
  std::string model;
  std::string domain;
  if (!doc.HasParseError() && doc.IsObject()) {
    JsonGetString(doc, "content", &content);
    JsonGetString(doc, "model", &model);
    JsonGetString(doc, "domain", &domain);
  }
  if (content.empty()) {
    std::string resp = BuildHttpResponse(400, "application/json", "{\"ret_code\":1,\"ret_msg\":\"empty content\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  book_agent::SessionMeta meta;
  if (g_store.GetSession(id, &meta) != base::kOk) {
    std::string resp = BuildHttpResponse(404, "application/json", "{\"ret_code\":2,\"ret_msg\":\"session not found\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  bool first_turn = (meta.msg_count == 0);
  if (model.empty()) model = meta.model;
  if (domain.empty()) domain = meta.domain.empty() ? "auto" : meta.domain;
  // 记住会话默认 model/domain，便于重开与重生成沿用
  g_store.PatchSession(id, NULL, model.empty() ? NULL : &model, &domain);

  g_store.AppendMessage(id, "user", content, "", "", false, 0, NULL);

  // L2 摘要生效时只带 covered_seq 之后的原文窗口；否则带全部历史（covered_seq=0）
  book_agent::SummaryData summary;
  g_store.GetSummary(id, &summary);
  std::vector<book_agent::StoredMessage> msgs;
  g_store.GetMessages(id, &msgs);
  std::vector<book_agent::ChatMessage> history;
  for (uint32_t i = 0; i < msgs.size(); ++i) {
    if (msgs[i].seq < summary.covered_seq) continue;
    book_agent::ChatMessage cm;
    cm.role = msgs[i].role;
    cm.content = msgs[i].content;
    history.push_back(cm);
  }

  std::vector<std::string> extra_systems;
  extra_systems.push_back(BuildMemoryBlock());     // L3 长期记忆
  extra_systems.push_back(BuildSummaryBlock(summary));  // L2 滚动摘要

  StreamSessionReply(fd, id, model, domain, history, extra_systems, first_turn, content);
} /*}}}*/

/**
 * @brief POST /api/agent/sessions/{id}/regenerate：删除末条 assistant 后按末条 user 重跑
 */
void HandleSessionRegenerate(int fd, const std::string &id) { /*{{{*/
  if (!g_agent.IsReady() || !g_store.IsReady()) {
    std::string resp = BuildHttpResponse(503, "application/json", "{\"ret_code\":4,\"ret_msg\":\"agent not ready\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }
  book_agent::SessionMeta meta;
  if (g_store.GetSession(id, &meta) != base::kOk) {
    std::string resp = BuildHttpResponse(404, "application/json", "{\"ret_code\":2,\"ret_msg\":\"session not found\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }

  g_store.DeleteLastAssistant(id);  // 无 assistant 也继续（相当于对末条 user 重答）

  book_agent::SummaryData summary;
  g_store.GetSummary(id, &summary);
  std::vector<book_agent::StoredMessage> msgs;
  g_store.GetMessages(id, &msgs);
  if (msgs.empty()) {
    std::string resp = BuildHttpResponse(400, "application/json", "{\"ret_code\":1,\"ret_msg\":\"nothing to regen\"}");
    send(fd, resp.data(), resp.size(), 0);
    close(fd);
    return;
  }
  std::vector<book_agent::ChatMessage> history;
  for (uint32_t i = 0; i < msgs.size(); ++i) {
    if (msgs[i].seq < summary.covered_seq) continue;
    book_agent::ChatMessage cm;
    cm.role = msgs[i].role;
    cm.content = msgs[i].content;
    history.push_back(cm);
  }
  std::vector<std::string> extra_systems;
  extra_systems.push_back(BuildMemoryBlock());
  extra_systems.push_back(BuildSummaryBlock(summary));
  std::string domain = meta.domain.empty() ? "auto" : meta.domain;
  StreamSessionReply(fd, id, meta.model, domain, history, extra_systems, false, "");
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

  // Agent 流式问答：自行收发并关闭连接（SSE），不走统一的一次性响应
  if (method == "POST" && path == "/api/agent/chat") {
    HandleAgentChat(fd, body);
    return;
  }
  // 会话内流式问答 / 重生成（同样自行收发并关闭）
  if (method == "POST") {
    std::string sid;
    std::string action;
    if (ParseSessionPath(path, &sid, &action) && !sid.empty()) {
      if (action == "chat") {
        HandleSessionChat(fd, sid, body);
        return;
      }
      if (action == "regenerate") {
        HandleSessionRegenerate(fd, sid);
        return;
      }
    }
  }

  std::string resp;
  if (method == "GET" && (path == "/" || path == "/index.html")) {
    resp = BuildHttpResponse(200, "text/html; charset=UTF-8", g_index_html);
  } else if (path.compare(0, kMemPrefix.size(), kMemPrefix) == 0) {
    int http_status = 200;
    std::string out;
    RouteAgentMemory(method, path, body, &http_status, &out);
    resp = BuildHttpResponse(http_status, "application/json; charset=UTF-8", out);
  } else if (path.compare(0, kSessPrefix.size(), kSessPrefix) == 0) {
    int http_status = 200;
    std::string out;
    RouteAgentSession(method, path, body, &http_status, &out);
    resp = BuildHttpResponse(http_status, "application/json; charset=UTF-8", out);
  } else if (method == "GET" && path == "/api/agent/models") {
    int http_status = 200;
    std::string out;
    HandleAgentModels(&http_status, &out);
    resp = BuildHttpResponse(http_status, "application/json; charset=UTF-8", out);
  } else if (method == "GET" && path == "/api/agent/agents") {
    int http_status = 200;
    std::string out;
    HandleAgentAgents(&http_status, &out);
    resp = BuildHttpResponse(http_status, "application/json; charset=UTF-8", out);
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
  std::string agent_conf_path = "./conf/agent.conf";
  std::string agent_skills_dir = "./agent/skills";
  std::string agent_db_path = "./data/agent_db";
  std::string agent_mem_db_path = "./data/agent_mem_db";
  std::string agent_knowledge_root = "../..";
  int memory_enable = 1;
  int tools_enable = 1;
  int tool_max_iters = static_cast<int>(kToolMaxItersDefault);
  if (user_conf.LoadFile(user_conf_path) == kOk) {
    user_conf.GetValue("control_server_ip", "127.0.0.1", &g_control_ip);
    int control_port = 9101;
    user_conf.GetInt32Value("control_server_port", 9101, &control_port);
    g_control_port = static_cast<uint16_t>(control_port);
    user_conf.GetValue("static_index_path", "./web/index.html", &static_path);
    user_conf.GetValue("agent_conf_path", "./conf/agent.conf", &agent_conf_path);
    user_conf.GetValue("agent_skills_dir", "./agent/skills", &agent_skills_dir);
    user_conf.GetValue("agent_db_path", "./data/agent_db", &agent_db_path);
    user_conf.GetValue("agent_mem_db_path", "./data/agent_mem_db", &agent_mem_db_path);
    user_conf.GetValue("agent_knowledge_root", "../..", &agent_knowledge_root);
    user_conf.GetInt32Value("agent_memory_enable", 1, &memory_enable);
    user_conf.GetInt32Value("agent_tools_enable", 1, &tools_enable);
    user_conf.GetInt32Value("agent_tool_max_iters", static_cast<int>(kToolMaxItersDefault), &tool_max_iters);
  } else {
    fprintf(stderr, "Failed to load %s, use defaults\n", user_conf_path.c_str());
  }
  g_memory_enable = (memory_enable != 0);
  g_tools_enable = (tools_enable != 0);
  if (tool_max_iters > 0) g_tool_max_iters = static_cast<uint32_t>(tool_max_iters);

  LoadIndexHtml(static_path);

  // 初始化 Agent（provider 注册表 + 领域技能 + 源码知识检索）；失败不影响 CRUD/列表功能
  curl_global_init(CURL_GLOBAL_ALL);
  Code agent_ret = g_agent.Init(agent_conf_path, agent_skills_dir, agent_knowledge_root);
  if (agent_ret == kOk) {
    g_knowledge_version_ms = NowMs();  // 本次生效的知识版本：SKILL/路由表升级需重启方可反映在此
    g_agent.SetBookToolFn(BookToolQuery);  // 注入业务库工具（list_books/get_book -> Control RPC）
    std::vector<const book_agent::Skill *> skills;
    g_agent.Skills().List(&skills);
    fprintf(stderr,
            "agent ready: conf=%s, skills_dir=%s, skills=%zu, default_model=%s, knowledge_version_ms=%lld, "
            "tools_enable=%d, tools_available=%d, tool_max_iters=%u\n",
            agent_conf_path.c_str(), agent_skills_dir.c_str(), skills.size(),
            g_agent.Config().DefaultModel().c_str(), static_cast<long long>(g_knowledge_version_ms), g_tools_enable,
            g_agent.ToolsEnabled(), g_tool_max_iters);
  } else {
    fprintf(stderr, "agent disabled (init ret:%d); CRUD/list still available\n", agent_ret);
  }

  // 初始化会话持久化存储（LevelDB）；失败则会话相关接口返回 503，其余功能不受影响
  Code store_ret = g_store.Init(agent_db_path);
  if (store_ret == kOk) {
    fprintf(stderr, "session store ready: db=%s\n", agent_db_path.c_str());
  } else {
    fprintf(stderr, "session store disabled (init ret:%d)\n", store_ret);
  }

  // 初始化长期记忆存储（独立 LevelDB）；失败则记忆接口返回 503，其余功能不受影响
  Code mem_ret = g_mem.Init(agent_mem_db_path);
  if (mem_ret == kOk) {
    fprintf(stderr, "memory store ready: db=%s, memory_enable=%d\n", agent_mem_db_path.c_str(), g_memory_enable);
  } else {
    fprintf(stderr, "memory store disabled (init ret:%d)\n", mem_ret);
  }

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
