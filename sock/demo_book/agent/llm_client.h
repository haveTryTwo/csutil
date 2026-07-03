// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_AGENT_LLM_CLIENT_H_
#define SOCK_DEMO_BOOK_AGENT_LLM_CLIENT_H_

#include <functional>
#include <string>
#include <vector>

#include "base/status.h"
#include "sock/demo_book/agent/agent_config.h"

namespace book_agent {

/**
 * @brief 一条对话消息（OpenAI 兼容：role ∈ {system,user,assistant,tool}）
 *
 * 工具调用相关字段仅在对应场景填写：
 * - role=="assistant" 且模型请求了工具时：tool_calls_json 存原样的 tool_calls 数组（回灌给模型）；
 * - role=="tool"（工具执行结果）时：tool_call_id 对应发起的调用，name 为工具名，content 为结果文本。
 */
struct ChatMessage { /*{{{*/
  std::string role;
  std::string content;
  std::string name;             // role==tool 时的工具名
  std::string tool_call_id;     // role==tool 时对应的调用 id
  std::string tool_calls_json;  // role==assistant 时原样的 tool_calls 数组 JSON
}; /*}}}*/

/**
 * @brief 模型请求的一次工具调用
 */
struct ToolCall { /*{{{*/
  std::string id;
  std::string name;
  std::string arguments;  // JSON 参数串（原样）
}; /*}}}*/

/**
 * @brief OpenAI 兼容 Chat Completions 客户端（libcurl 实现）
 *
 * 无状态：每次调用创建独立 easy handle，可被多线程并发使用。
 * 使用前需在进程启动时调用一次 curl_global_init。
 */
class LlmClient { /*{{{*/
 public:
  // 流式增量回调：返回 false 表示调用方要求中止拉流（如浏览器断开）
  typedef std::function<bool(const std::string &delta)> DeltaCallback;

  LlmClient();
  ~LlmClient();

  /**
   * @brief 非流式对话：一次拿到完整 content
   * @param provider 目标模型
   * @param api_key 密钥（可为空，本地模型）
   * @param messages 对话消息（含 system）
   * @param content 输出：assistant 完整回答
   * @return kOk 成功；kConnectError 网络/HTTP 失败；kInvalidParam 参数非法
   */
  base::Code Chat(const ModelProvider &provider, const std::string &api_key,
                  const std::vector<ChatMessage> &messages, std::string *content);

  /**
   * @brief 流式对话：解析 SSE，逐段回调 on_delta
   * @param provider 目标模型
   * @param api_key 密钥（可为空）
   * @param messages 对话消息（含 system）
   * @param on_delta 增量回调
   * @return kOk 成功（含被调用方主动中止）；kConnectError 网络/HTTP 失败
   */
  base::Code ChatStream(const ModelProvider &provider, const std::string &api_key,
                        const std::vector<ChatMessage> &messages, const DeltaCallback &on_delta);

  /**
   * @brief 带工具的非流式对话（OpenAI 兼容 function calling）
   * @param provider 目标模型
   * @param api_key 密钥（可为空）
   * @param messages 对话消息（可含 assistant.tool_calls 与 tool 结果消息）
   * @param tools_json tools schema（JSON 数组字符串）；空则不带工具
   * @param out_content 输出：本轮 assistant 文本（可能为空，尤其当有 tool_calls 时）
   * @param out_tool_calls 输出：模型请求的工具调用（可能为空）
   * @return kOk 成功；kConnectError 网络/HTTP 失败；kInvalidParam 参数非法
   */
  base::Code ChatWithTools(const ModelProvider &provider, const std::string &api_key,
                           const std::vector<ChatMessage> &messages, const std::string &tools_json,
                           std::string *out_content, std::vector<ToolCall> *out_tool_calls);

 private:
  LlmClient(const LlmClient &c);
  LlmClient &operator=(const LlmClient &c);

  /**
   * @brief 组装 chat/completions 请求 JSON
   * @param provider 目标模型
   * @param messages 对话消息
   * @param stream 是否流式
   * @param tools_json 可选 tools schema（JSON 数组串）；空则不带
   * @param out 输出 JSON 串
   */
  void BuildRequestJson(const ModelProvider &provider, const std::vector<ChatMessage> &messages, bool stream,
                        const std::string &tools_json, std::string *out);
}; /*}}}*/

}  // namespace book_agent

#endif
