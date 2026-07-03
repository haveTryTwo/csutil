// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/agent_service.h"

#include <ctype.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "base/log.h"

namespace book_agent {

namespace {

const char kGeneralDomain[] = "general";

const size_t kStreamChunkBytes = 60;  // 分块下发完整答案时的单块字节数（UTF-8 安全）

// 摘要/记忆抽取的 system 提示（要求模型据实、简洁、可机读）
const char kSummarySystem[] =
    "你是对话摘要器。请把【已有摘要】与【新增对话】融合成一段简洁中文摘要，"
    "只保留对后续对话有用的关键信息（用户意图、已确认的结论、待办、限制条件），"
    "去除寒暄与冗余。直接输出摘要正文，不要加前后缀或解释。";

// 工具使用引导：仅在 agentic 路径注入，显著提升小模型主动调用工具的比例
const char kToolGuidance[] =
    "你可以调用以下只读工具来查证事实，不要凭空猜测：\n"
    "- search_code(query, glob?)：在 csutil 仓库根目录下检索源码/文档关键字（找证据、定位实现）；\n"
    "- read_file(path, start?, end?)：读取仓库内某文件的具体片段（仅限仓库根目录内路径）；\n"
    "- list_books(prefix?, limit?) / get_book(book_id)：查询 demo 业务图书库的真实数据；\n"
    "- list_skills()：列出可用领域 Agent。\n"
    "当问题涉及本仓库的代码/文档实现细节、或业务库中的具体数据时，必须先调用相应工具核实，"
    "拿到工具结果后再据实作答，并可引用文件与行号。若无需工具即可回答，则直接回答。";

const char kExtractSystem[] =
    "你从一轮对话中抽取值得【长期记忆】的稳定信息（用户偏好、稳定事实），忽略一次性问答细节与寒暄。"
    "严格输出 JSON 数组，每项形如 {\"kind\":\"fact|preference\",\"text\":\"...\"}；"
    "text 用简洁中文陈述句。没有可记忆内容时输出 []。不要输出 JSON 以外的任何字符。";

// 领域关键词表：用于 domain=auto 时的轻量路由（英文小写；中文原样）
struct DomainKeywords {
  const char *domain;
  const char *keywords[32];
};

const DomainKeywords kRouteTable[] = {
    {"mystl", {"my_vector", "my_list", "my_deque", "my_map", "my_set", "my_hash", "rb_tree", "hashtable", "map", "set",
               "红黑树", "哈希", "hash", "unordered", "stl", "vector", "迭代器", "扩容", "rehash", "allocator", "配置器",
               NULL}},
    {"test", {"press", "压测", "单测", "测试", "qps", "并发", "时延", "benchmark", "覆盖", NULL}},
    {"sock", {"rpc", "连接复用", "服务发现", "负载均衡", "熔断", "time_wait", "网关", "socket", NULL}},
    {"store", {"leveldb", "lru", "缓存", "bitcask", "存储", "分页", "cache", "kv", NULL}},
    {"base", {"日志", "配置", "事件循环", "epoll", "协程", "bloom", "hyperloglog", "加密", "ann", "一致性哈希", NULL}},
};

/**
 * @brief 生成小写副本（仅影响 ASCII，UTF-8 中文字节不变）
 */
std::string ToLowerCopy(const std::string &in) { /*{{{*/
  std::string out = in;
  for (uint32_t i = 0; i < out.size(); ++i) out[i] = static_cast<char>(tolower(static_cast<unsigned char>(out[i])));
  return out;
} /*}}}*/

/**
 * @brief 容错解析记忆抽取结果：从模型输出里截取首个 '[' 到末个 ']' 作为 JSON 数组解析
 *        （兼容模型偶尔包裹的 ```json 代码块或前后解释文字）
 * @param content 模型原始输出
 * @param out 输出候选（追加）
 */
void ParseMemoryJson(const std::string &content, std::vector<MemoryCandidate> *out) { /*{{{*/
  std::string::size_type lb = content.find('[');
  std::string::size_type rb = content.rfind(']');
  if (lb == std::string::npos || rb == std::string::npos || rb <= lb) return;
  std::string arr = content.substr(lb, rb - lb + 1);

  rapidjson::Document doc;
  doc.Parse(arr.c_str(), arr.size());
  if (doc.HasParseError() || !doc.IsArray()) return;
  for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
    const rapidjson::Value &item = doc[i];
    if (!item.IsObject() || !item.HasMember("text") || !item["text"].IsString()) continue;
    MemoryCandidate cand;
    if (item.HasMember("kind") && item["kind"].IsString()) cand.kind = item["kind"].GetString();
    if (cand.kind != "fact" && cand.kind != "preference") cand.kind = "fact";
    cand.text.assign(item["text"].GetString(), item["text"].GetStringLength());
    if (!cand.text.empty()) out->push_back(cand);
  }
} /*}}}*/

/**
 * @brief 把工具调用列表序列化为 OpenAI tool_calls 数组 JSON（回灌给模型）
 */
std::string BuildToolCallsJson(const std::vector<ToolCall> &calls) { /*{{{*/
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> w(sb);
  w.StartArray();
  for (uint32_t i = 0; i < calls.size(); ++i) {
    w.StartObject();
    w.Key("id");
    w.String(calls[i].id.c_str());
    w.Key("type");
    w.String("function");
    w.Key("function");
    w.StartObject();
    w.Key("name");
    w.String(calls[i].name.c_str());
    w.Key("arguments");  // arguments 按字符串回灌（OpenAI 规范即为字符串）
    w.String(calls[i].arguments.c_str(), static_cast<rapidjson::SizeType>(calls[i].arguments.size()));
    w.EndObject();
    w.EndObject();
  }
  w.EndArray();
  return sb.GetString();
} /*}}}*/

/**
 * @brief 把完整答案按 UTF-8 安全边界分块，逐块回调 on_delta（保持前端流式观感）
 */
void StreamOut(const std::string &content, const LlmClient::DeltaCallback &on_delta) { /*{{{*/
  if (content.empty() || !on_delta) return;
  size_t pos = 0;
  while (pos < content.size()) {
    size_t len = kStreamChunkBytes;
    if (pos + len >= content.size()) {
      len = content.size() - pos;
    } else {
      // 回退到 UTF-8 字符边界，避免切碎多字节字符
      while (len > 0 && (static_cast<unsigned char>(content[pos + len]) & 0xC0) == 0x80) --len;
      if (len == 0) len = kStreamChunkBytes;  // 极端情况兜底
    }
    if (!on_delta(content.substr(pos, len))) return;  // 调用方要求中止
    pos += len;
  }
} /*}}}*/

}  // namespace

AgentService::AgentService() : ready_(false) {}

AgentService::~AgentService() {}

base::Code AgentService::Init(const std::string &agent_conf, const std::string &skills_dir,
                              const std::string &knowledge_root) { /*{{{*/
  base::Code ret = config_.Load(agent_conf);
  if (ret != base::kOk) {
    LOG_ERR("agent load conf failed:%s, ret:%d\n", agent_conf.c_str(), ret);
    return ret;
  }
  ret = skills_.Load(skills_dir);
  if (ret != base::kOk) {
    LOG_ERR("agent load skills failed:%s, ret:%d\n", skills_dir.c_str(), ret);
    return ret;
  }
  retriever_.Init(knowledge_root);  // 空则自动禁用源码检索
  tools_.Init(knowledge_root, &skills_);  // 工具文件访问同样限定在知识库根（csutil 根）
  ready_ = true;
  return base::kOk;
} /*}}}*/

bool AgentService::IsReady() const { return ready_; }

const AgentConfig &AgentService::Config() const { return config_; }

const SkillRegistry &AgentService::Skills() const { return skills_; }

void AgentService::SetBookToolFn(const ToolExecutor::BookToolFn &fn) { tools_.SetBookToolFn(fn); }

bool AgentService::ToolsEnabled() const { return tools_.Enabled(); }

std::string AgentService::Route(const std::string &domain, const std::string &question) const { /*{{{*/
  if (!domain.empty() && domain != "auto" && skills_.Get(domain) != NULL) return domain;

  std::string lowered = ToLowerCopy(question);
  std::string best = kGeneralDomain;
  uint32_t best_hits = 0;
  uint32_t table_num = static_cast<uint32_t>(sizeof(kRouteTable) / sizeof(kRouteTable[0]));
  for (uint32_t i = 0; i < table_num; ++i) {
    uint32_t hits = 0;
    for (uint32_t j = 0; kRouteTable[i].keywords[j] != NULL; ++j) {
      if (lowered.find(kRouteTable[i].keywords[j]) != std::string::npos) ++hits;
    }
    if (hits > best_hits && skills_.Get(kRouteTable[i].domain) != NULL) {
      best_hits = hits;
      best = kRouteTable[i].domain;
    }
  }
  if (skills_.Get(best) == NULL) best = kGeneralDomain;
  return best;
} /*}}}*/

const ModelProvider *AgentService::ResolveProvider(const std::string &req_model, const Skill *skill) const { /*{{{*/
  if (!req_model.empty()) {
    const ModelProvider *p = config_.Get(req_model);
    if (p != NULL) return p;
  }
  if (skill != NULL && !skill->preferred_model.empty()) {
    const ModelProvider *p = config_.Get(skill->preferred_model);
    if (p != NULL) return p;
  }
  return config_.Get(config_.DefaultModel());
} /*}}}*/

base::Code AgentService::AssembleMessages(const std::string &req_model, const std::string &domain,
                                          const std::vector<ChatMessage> &user_msgs,
                                          const std::vector<std::string> &extra_systems,
                                          std::vector<ChatMessage> *out_messages, const ModelProvider **out_provider,
                                          std::string *out_domain) { /*{{{*/
  // 取最新一条 user 消息作为路由依据
  std::string question;
  for (uint32_t i = static_cast<uint32_t>(user_msgs.size()); i > 0; --i) {
    if (user_msgs[i - 1].role == "user") {
      question = user_msgs[i - 1].content;
      break;
    }
  }

  std::string domain_final = Route(domain, question);
  const Skill *skill = skills_.Get(domain_final);
  if (skill == NULL) {
    skill = skills_.Get(kGeneralDomain);
    domain_final = kGeneralDomain;
  }

  const ModelProvider *provider = ResolveProvider(req_model, skill);
  if (provider == NULL) return base::kNotFound;
  *out_provider = provider;
  *out_domain = domain_final;

  if (skill != NULL && !skill->body.empty()) {
    ChatMessage sys;
    sys.role = "system";
    sys.content = skill->body;
    out_messages->push_back(sys);
  }
  // 轻量检索：把与问题相关的真实源码片段作为附加 system 注入，供模型据实回答
  if (skill != NULL) {
    std::string kctx = retriever_.Retrieve(*skill, question);
    if (!kctx.empty()) {
      ChatMessage ctx;
      ctx.role = "system";
      ctx.content = kctx;
      out_messages->push_back(ctx);
    }
  }
  // 额外 system 块：长期记忆（L3）+ 滚动摘要（L2），在 SKILL/检索之后、history 之前注入
  for (uint32_t i = 0; i < extra_systems.size(); ++i) {
    if (extra_systems[i].empty()) continue;
    ChatMessage sys;
    sys.role = "system";
    sys.content = extra_systems[i];
    out_messages->push_back(sys);
  }
  for (uint32_t i = 0; i < user_msgs.size(); ++i) out_messages->push_back(user_msgs[i]);
  return base::kOk;
} /*}}}*/

base::Code AgentService::ChatStream(const std::string &req_model, const std::string &domain,
                                    const std::vector<ChatMessage> &user_msgs,
                                    const std::vector<std::string> &extra_systems, const MetaCallback &on_meta,
                                    const LlmClient::DeltaCallback &on_delta) { /*{{{*/
  if (!ready_) return base::kNotFound;

  std::vector<ChatMessage> messages;
  const ModelProvider *provider = NULL;
  std::string domain_final;
  base::Code ret = AssembleMessages(req_model, domain, user_msgs, extra_systems, &messages, &provider, &domain_final);
  if (ret != base::kOk) return ret;

  if (on_meta) on_meta(provider->name, domain_final);

  std::string api_key = AgentConfig::GetApiKey(*provider);
  return llm_.ChatStream(*provider, api_key, messages, on_delta);
} /*}}}*/

base::Code AgentService::ChatStreamAgentic(const std::string &req_model, const std::string &domain,
                                           const std::vector<ChatMessage> &user_msgs,
                                           const std::vector<std::string> &extra_systems, uint32_t max_iters,
                                           const MetaCallback &on_meta, const ToolEventCallback &on_tool_event,
                                           const LlmClient::DeltaCallback &on_delta) { /*{{{*/
  if (!ready_) return base::kNotFound;
  // 工具不可用时降级为普通流式
  if (!tools_.Enabled()) return ChatStream(req_model, domain, user_msgs, extra_systems, on_meta, on_delta);

  std::vector<ChatMessage> messages;
  const ModelProvider *provider = NULL;
  std::string domain_final;
  base::Code ret = AssembleMessages(req_model, domain, user_msgs, extra_systems, &messages, &provider, &domain_final);
  if (ret != base::kOk) return ret;

  // 在最前面注入工具使用引导，提升模型主动调用工具的比例
  {
    ChatMessage guide;
    guide.role = "system";
    guide.content = kToolGuidance;
    messages.insert(messages.begin(), guide);
  }

  if (on_meta) on_meta(provider->name, domain_final);
  std::string api_key = AgentConfig::GetApiKey(*provider);
  const std::string &tools_schema = tools_.ToolsSchemaJson();

  for (uint32_t iter = 0; iter < max_iters; ++iter) {
    std::string content;
    std::vector<ToolCall> calls;
    base::Code cret = llm_.ChatWithTools(*provider, api_key, messages, tools_schema, &content, &calls);
    if (cret != base::kOk) return cret;

    if (calls.empty()) {
      // 无工具调用：把已拿到的完整答案分块下发（保持前端流式观感）
      StreamOut(content, on_delta);
      return base::kOk;
    }

    // 回灌 assistant 的 tool_calls 轮，再逐个执行工具并追加 tool 结果
    ChatMessage assistant;
    assistant.role = "assistant";
    assistant.content = content;
    assistant.tool_calls_json = BuildToolCallsJson(calls);
    messages.push_back(assistant);

    for (uint32_t i = 0; i < calls.size(); ++i) {
      if (on_tool_event) on_tool_event("tool_call", calls[i].name, calls[i].arguments);
      std::string result = tools_.Run(calls[i].name, calls[i].arguments);
      if (on_tool_event) on_tool_event("tool_result", calls[i].name, result);
      ChatMessage tool_msg;
      tool_msg.role = "tool";
      tool_msg.name = calls[i].name;
      tool_msg.tool_call_id = calls[i].id;
      tool_msg.content = result;
      messages.push_back(tool_msg);
    }
  }

  // 迭代用尽仍未收敛：去掉 tools 强制模型据已有工具结果作答（普通流式）
  return llm_.ChatStream(*provider, api_key, messages, on_delta);
} /*}}}*/

base::Code AgentService::Summarize(const std::string &req_model, const std::string &prior_summary,
                                   const std::vector<ChatMessage> &to_cover, std::string *out_summary) { /*{{{*/
  if (!ready_ || out_summary == NULL) return base::kNotFound;
  const ModelProvider *provider = ResolveProvider(req_model, NULL);
  if (provider == NULL) return base::kNotFound;

  std::string payload = "【已有摘要】\n";
  payload += prior_summary.empty() ? "（无）" : prior_summary;
  payload += "\n\n【新增对话】\n";
  for (uint32_t i = 0; i < to_cover.size(); ++i) {
    payload += (to_cover[i].role == "assistant") ? "助手: " : "用户: ";
    payload += to_cover[i].content;
    payload += "\n";
  }

  std::vector<ChatMessage> messages;
  ChatMessage sys;
  sys.role = "system";
  sys.content = kSummarySystem;
  messages.push_back(sys);
  ChatMessage user;
  user.role = "user";
  user.content = payload;
  messages.push_back(user);

  std::string api_key = AgentConfig::GetApiKey(*provider);
  return llm_.Chat(*provider, api_key, messages, out_summary);
} /*}}}*/

base::Code AgentService::ExtractMemories(const std::string &req_model, const std::string &user_text,
                                         const std::string &assistant_text,
                                         std::vector<MemoryCandidate> *out) { /*{{{*/
  if (!ready_ || out == NULL) return base::kNotFound;
  const ModelProvider *provider = ResolveProvider(req_model, NULL);
  if (provider == NULL) return base::kNotFound;

  std::string payload = "用户: " + user_text + "\n助手: " + assistant_text;
  std::vector<ChatMessage> messages;
  ChatMessage sys;
  sys.role = "system";
  sys.content = kExtractSystem;
  messages.push_back(sys);
  ChatMessage user;
  user.role = "user";
  user.content = payload;
  messages.push_back(user);

  std::string api_key = AgentConfig::GetApiKey(*provider);
  std::string content;
  base::Code ret = llm_.Chat(*provider, api_key, messages, &content);
  if (ret != base::kOk) return ret;

  ParseMemoryJson(content, out);
  return base::kOk;
} /*}}}*/

}  // namespace book_agent
