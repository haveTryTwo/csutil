// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AGENT_SRC_AGENT_SERVICE_H_
#define AGENT_SRC_AGENT_SERVICE_H_

#include <functional>
#include <string>
#include <vector>

#include "base/status.h"
#include "agent/src/agent_config.h"
#include "agent/src/knowledge_retriever.h"
#include "agent/src/llm_client.h"
#include "agent/src/skill_registry.h"
#include "agent/src/tool_executor.h"

namespace book_agent {

/**
 * @brief 记忆抽取候选（LLM 从一轮对话中提炼出的稳定事实/偏好）
 */
struct MemoryCandidate { /*{{{*/
  std::string kind;  // fact | preference
  std::string text;
}; /*}}}*/

/**
 * @brief Agent 编排：加载配置/技能，做领域路由，组装 messages 并调用 LLM
 *
 * 初始化后内部数据只读，可被多个 worker 线程并发调用；LlmClient 无状态。
 */
class AgentService { /*{{{*/
 public:
  // 路由确定后、开始拉流前回调一次，告知最终 model/domain（用于 SSE meta）
  typedef std::function<void(const std::string &model, const std::string &domain)> MetaCallback;
  // 工具过程事件回调：phase ∈ {"tool_call","tool_result"}，name 工具名，detail 参数/结果摘要
  typedef std::function<void(const std::string &phase, const std::string &name, const std::string &detail)>
      ToolEventCallback;

  AgentService();
  ~AgentService();

  /**
   * @brief 加载 provider 注册表与领域技能
   * @param agent_conf agent.conf 路径
   * @param skills_dir skills 目录（其下每个子目录含 SKILL.md）
   * @param knowledge_root 知识库根目录（SKILL.knowledge_globs 相对该目录解析）；空则禁用源码检索
   * @return kOk 成功；否则返回子步骤错误码（agent 功能不可用，网关其余功能不受影响）
   */
  base::Code Init(const std::string &agent_conf, const std::string &skills_dir,
                  const std::string &knowledge_root = "");

  /**
   * @brief 是否已就绪（Init 成功）
   */
  bool IsReady() const;

  /**
   * @brief 只读访问：provider 注册表 / 技能表（供网关构造列表接口）
   */
  const AgentConfig &Config() const;
  const SkillRegistry &Skills() const;

  /**
   * @brief 注入业务库工具回调（list_books/get_book 内部走 Control RPC，由网关提供）
   * @param fn 回调
   */
  void SetBookToolFn(const ToolExecutor::BookToolFn &fn);

  /**
   * @brief 工具能力是否可用（root 已配置）
   */
  bool ToolsEnabled() const;

  /**
   * @brief 领域路由：显式 domain 命中则用之；auto/未知则按关键词选，兜底 general
   * @param domain 请求指定的领域（"auto" 表示自动）
   * @param question 用户最新问题（用于关键词匹配）
   * @return 选定的领域名（保证 Skills().Get() 命中或为 general）
   */
  std::string Route(const std::string &domain, const std::string &question) const;

  /**
   * @brief 流式问答：路由 -> 取 skill 正文作 system -> 注入检索/记忆/摘要 -> 选模型 -> 调 LLM 流式
   * @param req_model 请求模型名（空则用 skill 偏好/默认）
   * @param domain 请求领域（"auto" 自动路由）
   * @param user_msgs 用户侧消息（历史 + 最新问题，均为 user/assistant）
   * @param extra_systems 额外 system 块（如长期记忆/滚动摘要），注入在 SKILL/检索之后、history 之前
   * @param on_meta 路由后、拉流前回调（model/domain）
   * @param on_delta 增量回调（返回 false 中止）
   * @return kOk 成功；kNotFound 无可用模型/技能；kConnectError LLM 调用失败
   */
  base::Code ChatStream(const std::string &req_model, const std::string &domain,
                        const std::vector<ChatMessage> &user_msgs, const std::vector<std::string> &extra_systems,
                        const MetaCallback &on_meta, const LlmClient::DeltaCallback &on_delta);

  /**
   * @brief 带工具的流式问答（agentic loop / ReAct，§16.6）：
   *        装配同 ChatStream，再带 tools schema 进入「非流式工具循环」，
   *        期间的工具调用/结果经 on_tool_event 上报；最终答案经 on_delta 分块下发。
   * @param req_model 请求模型名（空则用 skill 偏好/默认）
   * @param domain 请求领域（"auto" 自动路由）
   * @param user_msgs 用户侧消息（历史 + 最新问题）
   * @param extra_systems 额外 system 块（长期记忆/滚动摘要）
   * @param max_iters 工具循环最大迭代次数
   * @param on_meta 路由后、开始前回调（model/domain）
   * @param on_tool_event 工具过程事件回调（可空）
   * @param on_delta 最终答案增量回调（返回 false 中止）
   * @return kOk 成功；kNotFound 无可用模型/技能；kConnectError LLM 调用失败
   */
  base::Code ChatStreamAgentic(const std::string &req_model, const std::string &domain,
                               const std::vector<ChatMessage> &user_msgs,
                               const std::vector<std::string> &extra_systems, uint32_t max_iters,
                               const MetaCallback &on_meta, const ToolEventCallback &on_tool_event,
                               const LlmClient::DeltaCallback &on_delta);

  /**
   * @brief 生成/更新滚动摘要（L2）：把「已有摘要 + 待覆盖消息」浓缩成一段（非流式，一次调用）
   * @param req_model 请求模型名（空则用默认）
   * @param prior_summary 已有摘要（可空）
   * @param to_cover 待浓缩进摘要的消息（通常是保留窗口之前的旧消息）
   * @param out_summary 输出新的摘要文本
   * @return kOk 成功；kNotFound 无可用模型；kConnectError 调用失败
   */
  base::Code Summarize(const std::string &req_model, const std::string &prior_summary,
                       const std::vector<ChatMessage> &to_cover, std::string *out_summary);

  /**
   * @brief 从一轮对话中抽取长期记忆候选（L3）：LLM 输出严格 JSON 数组，解析为候选列表
   * @param req_model 请求模型名（空则用默认）
   * @param user_text 用户提问
   * @param assistant_text 助手回答
   * @param out 输出候选（无可记忆时为空数组）
   * @return kOk 成功（含空结果）；kNotFound 无可用模型；kConnectError 调用失败
   */
  base::Code ExtractMemories(const std::string &req_model, const std::string &user_text,
                             const std::string &assistant_text, std::vector<MemoryCandidate> *out);

 private:
  AgentService(const AgentService &s);
  AgentService &operator=(const AgentService &s);

  /**
   * @brief 依 req_model/skill 偏好/默认，解析出可用 provider
   * @param req_model 请求模型名
   * @param skill 选定技能（可为 NULL）
   * @return 命中的 provider；无则 NULL
   */
  const ModelProvider *ResolveProvider(const std::string &req_model, const Skill *skill) const;

 private:
  /**
   * @brief 组装基础消息序列：SKILL 正文 + 源码检索 + extra_systems + 用户历史；并输出选定 skill/provider
   * @param req_model 请求模型名
   * @param domain 请求领域
   * @param user_msgs 用户侧消息
   * @param extra_systems 额外 system 块
   * @param out_messages 输出组装后的消息
   * @param out_provider 输出选定 provider
   * @return kOk 成功；kNotFound 无可用模型
   */
  base::Code AssembleMessages(const std::string &req_model, const std::string &domain,
                              const std::vector<ChatMessage> &user_msgs, const std::vector<std::string> &extra_systems,
                              std::vector<ChatMessage> *out_messages, const ModelProvider **out_provider,
                              std::string *out_domain);

 private:
  bool ready_;
  AgentConfig config_;
  SkillRegistry skills_;
  LlmClient llm_;
  KnowledgeRetriever retriever_;
  ToolExecutor tools_;
}; /*}}}*/

}  // namespace book_agent

#endif
