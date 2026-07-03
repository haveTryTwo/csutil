// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/llm_client.h"

#include <string.h>

#include <curl/curl.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "base/log.h"

namespace book_agent {

namespace {

const int64_t kConnectTimeoutSec = 10;
const int64_t kTotalTimeoutSec = 180;

/**
 * @brief 从一条 SSE data 行的 JSON 中抽取 choices[0].delta.content
 * @param json_text data: 后面的 JSON 文本
 * @param delta 输出增量文本
 * @return true 抽到非空增量
 */
bool ExtractStreamDelta(const std::string &json_text, std::string *delta) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json_text.c_str(), json_text.size());
  if (doc.HasParseError() || !doc.IsObject()) return false;
  if (!doc.HasMember("choices") || !doc["choices"].IsArray() || doc["choices"].Empty()) return false;

  const rapidjson::Value &choice = doc["choices"][0];
  if (!choice.IsObject() || !choice.HasMember("delta") || !choice["delta"].IsObject()) return false;

  const rapidjson::Value &d = choice["delta"];
  if (!d.HasMember("content") || !d["content"].IsString()) return false;

  delta->assign(d["content"].GetString(), d["content"].GetStringLength());
  return !delta->empty();
} /*}}}*/

/**
 * @brief 从非流式响应 JSON 抽取 choices[0].message.content
 */
bool ExtractMessageContent(const std::string &json_text, std::string *content) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json_text.c_str(), json_text.size());
  if (doc.HasParseError() || !doc.IsObject()) return false;
  if (!doc.HasMember("choices") || !doc["choices"].IsArray() || doc["choices"].Empty()) return false;

  const rapidjson::Value &choice = doc["choices"][0];
  if (!choice.IsObject() || !choice.HasMember("message") || !choice["message"].IsObject()) return false;

  const rapidjson::Value &msg = choice["message"];
  if (!msg.HasMember("content") || !msg["content"].IsString()) return false;

  content->assign(msg["content"].GetString(), msg["content"].GetStringLength());
  return true;
} /*}}}*/

// 流式写回调的上下文
struct StreamContext { /*{{{*/
  const LlmClient::DeltaCallback *on_delta;
  std::string buffer;  // 跨 chunk 的残行缓存
  bool aborted;
}; /*}}}*/

/**
 * @brief libcurl 流式写回调：按行解析 SSE，逐段回调 on_delta
 */
size_t StreamWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) { /*{{{*/
  StreamContext *ctx = reinterpret_cast<StreamContext *>(userdata);
  size_t total = size * nmemb;
  ctx->buffer.append(ptr, total);

  std::string::size_type nl = std::string::npos;
  while ((nl = ctx->buffer.find('\n')) != std::string::npos) {
    std::string line = ctx->buffer.substr(0, nl);
    ctx->buffer.erase(0, nl + 1);
    if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
    if (line.empty()) continue;
    if (line.compare(0, 5, "data:") != 0) continue;

    std::string data = line.substr(5);
    while (!data.empty() && data[0] == ' ') data.erase(0, 1);
    if (data == "[DONE]") continue;

    std::string delta;
    if (ExtractStreamDelta(data, &delta)) {
      bool go_on = (*ctx->on_delta)(delta);
      if (!go_on) {
        ctx->aborted = true;
        return 0;  // 返回 0 通知 libcurl 中止
      }
    }
  }
  return total;
} /*}}}*/

/**
 * @brief libcurl 普通写回调：累积到字符串
 */
size_t AccumulateWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) { /*{{{*/
  std::string *out = reinterpret_cast<std::string *>(userdata);
  size_t total = size * nmemb;
  out->append(ptr, total);
  return total;
} /*}}}*/

}  // namespace

LlmClient::LlmClient() {}

LlmClient::~LlmClient() {}

void LlmClient::BuildRequestJson(const ModelProvider &provider, const std::vector<ChatMessage> &messages, bool stream,
                                 const std::string &tools_json, std::string *out) { /*{{{*/
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartObject();
  writer.Key("model");
  writer.String(provider.model.c_str());
  writer.Key("stream");
  writer.Bool(stream);
  writer.Key("messages");
  writer.StartArray();
  for (uint32_t i = 0; i < messages.size(); ++i) {
    const ChatMessage &m = messages[i];
    writer.StartObject();
    writer.Key("role");
    writer.String(m.role.c_str());
    writer.Key("content");
    writer.String(m.content.c_str(), static_cast<rapidjson::SizeType>(m.content.size()));
    if (m.role == "tool") {
      writer.Key("tool_call_id");
      writer.String(m.tool_call_id.c_str());
      if (!m.name.empty()) {
        writer.Key("name");
        writer.String(m.name.c_str());
      }
    }
    // assistant 发起过工具调用：把原样 tool_calls 数组回灌，模型才能把后续 tool 结果对应起来
    if (m.role == "assistant" && !m.tool_calls_json.empty()) {
      rapidjson::Document tc;
      tc.Parse(m.tool_calls_json.c_str(), m.tool_calls_json.size());
      if (!tc.HasParseError() && tc.IsArray()) {
        writer.Key("tool_calls");
        tc.Accept(writer);
      }
    }
    writer.EndObject();
  }
  writer.EndArray();
  // tools schema（原样透传数组）
  if (!tools_json.empty()) {
    rapidjson::Document td;
    td.Parse(tools_json.c_str(), tools_json.size());
    if (!td.HasParseError() && td.IsArray() && !td.Empty()) {
      writer.Key("tools");
      td.Accept(writer);
    }
  }
  writer.EndObject();
  out->assign(sb.GetString());
} /*}}}*/

/**
 * @brief 从非流式响应抽取 choices[0].message 的 content 与 tool_calls
 * @param json_text 响应体
 * @param out_content 输出文本（可空）
 * @param out_tool_calls 输出工具调用列表
 * @return true 解析成功
 */
static bool ExtractMessageWithTools(const std::string &json_text, std::string *out_content,
                                    std::vector<ToolCall> *out_tool_calls) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json_text.c_str(), json_text.size());
  if (doc.HasParseError() || !doc.IsObject()) return false;
  if (!doc.HasMember("choices") || !doc["choices"].IsArray() || doc["choices"].Empty()) return false;

  const rapidjson::Value &choice = doc["choices"][0];
  if (!choice.IsObject() || !choice.HasMember("message") || !choice["message"].IsObject()) return false;
  const rapidjson::Value &msg = choice["message"];

  if (msg.HasMember("content") && msg["content"].IsString()) {
    out_content->assign(msg["content"].GetString(), msg["content"].GetStringLength());
  }
  if (msg.HasMember("tool_calls") && msg["tool_calls"].IsArray()) {
    const rapidjson::Value &calls = msg["tool_calls"];
    for (rapidjson::SizeType i = 0; i < calls.Size(); ++i) {
      const rapidjson::Value &c = calls[i];
      if (!c.IsObject() || !c.HasMember("function") || !c["function"].IsObject()) continue;
      const rapidjson::Value &fn = c["function"];
      ToolCall tc;
      if (c.HasMember("id") && c["id"].IsString()) tc.id = c["id"].GetString();
      if (fn.HasMember("name") && fn["name"].IsString()) tc.name = fn["name"].GetString();
      if (fn.HasMember("arguments") && fn["arguments"].IsString()) {
        tc.arguments.assign(fn["arguments"].GetString(), fn["arguments"].GetStringLength());
      }
      if (tc.id.empty()) tc.id = "call_" + std::to_string(static_cast<long long>(i));
      if (!tc.name.empty()) out_tool_calls->push_back(tc);
    }
  }
  return true;
} /*}}}*/

base::Code LlmClient::ChatStream(const ModelProvider &provider, const std::string &api_key,
                                 const std::vector<ChatMessage> &messages, const DeltaCallback &on_delta) { /*{{{*/
  if (provider.base_url.empty()) return base::kInvalidParam;

  std::string payload;
  BuildRequestJson(provider, messages, true, "", &payload);

  CURL *curl = curl_easy_init();
  if (curl == NULL) return base::kConnectError;

  std::string url = provider.base_url + "/chat/completions";
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "Accept: text/event-stream");
  std::string auth;
  if (!api_key.empty()) {
    auth = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth.c_str());
  }

  StreamContext ctx;
  ctx.on_delta = &on_delta;
  ctx.aborted = false;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, static_cast<long>(kConnectTimeoutSec));
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, static_cast<long>(kTotalTimeoutSec));

  CURLcode rc = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (ctx.aborted) return base::kOk;  // 调用方主动中止（浏览器断开）
  if (rc != CURLE_OK) {
    LOG_ERR("llm stream failed, provider:%s, curl:%d(%s)\n", provider.name.c_str(), rc, curl_easy_strerror(rc));
    return base::kConnectError;
  }
  if (http_code >= 400) {
    LOG_ERR("llm stream http error, provider:%s, http_code:%ld\n", provider.name.c_str(), http_code);
    return base::kConnectError;
  }
  return base::kOk;
} /*}}}*/

base::Code LlmClient::Chat(const ModelProvider &provider, const std::string &api_key,
                           const std::vector<ChatMessage> &messages, std::string *content) { /*{{{*/
  if (provider.base_url.empty() || content == NULL) return base::kInvalidParam;

  std::string payload;
  BuildRequestJson(provider, messages, false, "", &payload);

  CURL *curl = curl_easy_init();
  if (curl == NULL) return base::kConnectError;

  std::string url = provider.base_url + "/chat/completions";
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  std::string auth;
  if (!api_key.empty()) {
    auth = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth.c_str());
  }

  std::string resp_body;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, AccumulateWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_body);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, static_cast<long>(kConnectTimeoutSec));
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, static_cast<long>(kTotalTimeoutSec));

  CURLcode rc = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (rc != CURLE_OK) return base::kConnectError;
  if (http_code >= 400) return base::kConnectError;
  if (!ExtractMessageContent(resp_body, content)) return base::kConnectError;
  return base::kOk;
} /*}}}*/

base::Code LlmClient::ChatWithTools(const ModelProvider &provider, const std::string &api_key,
                                    const std::vector<ChatMessage> &messages, const std::string &tools_json,
                                    std::string *out_content, std::vector<ToolCall> *out_tool_calls) { /*{{{*/
  if (provider.base_url.empty() || out_content == NULL || out_tool_calls == NULL) return base::kInvalidParam;

  std::string payload;
  BuildRequestJson(provider, messages, false, tools_json, &payload);

  CURL *curl = curl_easy_init();
  if (curl == NULL) return base::kConnectError;

  std::string url = provider.base_url + "/chat/completions";
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  std::string auth;
  if (!api_key.empty()) {
    auth = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth.c_str());
  }

  std::string resp_body;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, AccumulateWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_body);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, static_cast<long>(kConnectTimeoutSec));
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, static_cast<long>(kTotalTimeoutSec));

  CURLcode rc = curl_easy_perform(curl);
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (rc != CURLE_OK) {
    LOG_ERR("llm tool-chat failed, provider:%s, curl:%d(%s)\n", provider.name.c_str(), rc, curl_easy_strerror(rc));
    return base::kConnectError;
  }
  if (http_code >= 400) {
    LOG_ERR("llm tool-chat http error, provider:%s, http_code:%ld\n", provider.name.c_str(), http_code);
    return base::kConnectError;
  }
  if (!ExtractMessageWithTools(resp_body, out_content, out_tool_calls)) return base::kConnectError;
  return base::kOk;
} /*}}}*/

}  // namespace book_agent
