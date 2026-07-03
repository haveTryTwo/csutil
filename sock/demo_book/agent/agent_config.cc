// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/agent_config.h"

#include <stdlib.h>

#include "base/config.h"

namespace book_agent {

namespace {

/**
 * @brief 去除字符串两端空白
 * @param in 输入
 * @return 去空白后的字符串
 */
std::string TrimSpace(const std::string &in) { /*{{{*/
  uint32_t begin = 0;
  uint32_t end = static_cast<uint32_t>(in.size());
  while (begin < end && (in[begin] == ' ' || in[begin] == '\t')) ++begin;
  while (end > begin && (in[end - 1] == ' ' || in[end - 1] == '\t')) --end;
  return in.substr(begin, end - begin);
} /*}}}*/

/**
 * @brief 按分隔符切分并对每段去空白，跳过空段
 * @param in 输入串
 * @param sep 分隔符
 * @param out 输出段列表
 */
void SplitAndTrim(const std::string &in, char sep, std::vector<std::string> *out) { /*{{{*/
  std::string::size_type pos = 0;
  while (pos <= in.size()) {
    std::string::size_type next = in.find(sep, pos);
    std::string token = (next == std::string::npos) ? in.substr(pos) : in.substr(pos, next - pos);
    std::string trimmed = TrimSpace(token);
    if (!trimmed.empty()) out->push_back(trimmed);
    if (next == std::string::npos) break;
    pos = next + 1;
  }
} /*}}}*/

}  // namespace

AgentConfig::AgentConfig() {}

AgentConfig::~AgentConfig() {}

base::Code AgentConfig::Load(const std::string &conf_path) { /*{{{*/
  base::Config conf;
  base::Code ret = conf.LoadFile(conf_path);
  if (ret != base::kOk) return ret;

  conf.GetValue("default_model", "", &default_model_);

  std::string providers_line;
  conf.GetValue("providers", "", &providers_line);
  std::vector<std::string> names;
  SplitAndTrim(providers_line, ',', &names);

  for (uint32_t i = 0; i < names.size(); ++i) {
    const std::string &name = names[i];
    ModelProvider provider;
    provider.name = name;
    conf.GetValue(name + ".base_url", "", &provider.base_url);
    conf.GetValue(name + ".model", "", &provider.model);
    conf.GetValue(name + ".api_key_env", "", &provider.api_key_env);
    if (provider.base_url.empty() || provider.model.empty()) continue;  // 跳过无效项

    providers_[name] = provider;
    order_.push_back(name);
  }

  if (default_model_.empty() && !order_.empty()) default_model_ = order_[0];
  if (providers_.empty()) return base::kNotFound;
  return base::kOk;
} /*}}}*/

const ModelProvider *AgentConfig::Get(const std::string &name) const { /*{{{*/
  std::map<std::string, ModelProvider>::const_iterator it = providers_.find(name);
  if (it == providers_.end()) return NULL;
  return &(it->second);
} /*}}}*/

void AgentConfig::List(std::vector<const ModelProvider *> *out) const { /*{{{*/
  if (out == NULL) return;
  for (uint32_t i = 0; i < order_.size(); ++i) {
    std::map<std::string, ModelProvider>::const_iterator it = providers_.find(order_[i]);
    if (it != providers_.end()) out->push_back(&(it->second));
  }
} /*}}}*/

const std::string &AgentConfig::DefaultModel() const { return default_model_; }

std::string AgentConfig::GetApiKey(const ModelProvider &provider) { /*{{{*/
  if (provider.api_key_env.empty()) return "";
  const char *value = getenv(provider.api_key_env.c_str());
  return (value != NULL) ? std::string(value) : std::string();
} /*}}}*/

bool AgentConfig::IsAvailable(const ModelProvider &provider) { /*{{{*/
  if (provider.api_key_env.empty()) return true;  // 本地模型无需密钥
  return getenv(provider.api_key_env.c_str()) != NULL;
} /*}}}*/

}  // namespace book_agent
