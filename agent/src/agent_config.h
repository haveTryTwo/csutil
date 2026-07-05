// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AGENT_SRC_AGENT_CONFIG_H_
#define AGENT_SRC_AGENT_CONFIG_H_

#include <map>
#include <string>
#include <vector>

#include "base/status.h"

namespace book_agent {

/**
 * @brief 一个可调用的大模型 Provider（OpenAI 兼容 Chat Completions）
 *
 * 密钥不落库/落码：仅保存环境变量名 api_key_env，运行时经 getenv 读取实际密钥。
 */
struct ModelProvider { /*{{{*/
  std::string name;         // 前端下拉/路由展示用的唯一 key
  std::string base_url;     // OpenAI 兼容根路径，如 http://127.0.0.1:11434/v1
  std::string model;        // 请求体里的 model 字段
  std::string api_key_env;  // 环境变量名；为空表示本地模型无需密钥
}; /*}}}*/

/**
 * @brief 模型 Provider 注册表：从 agent.conf 加载，屏蔽密钥明文
 *
 * 加载后只读，可被多个 worker 线程并发访问。
 */
class AgentConfig { /*{{{*/
 public:
  AgentConfig();
  ~AgentConfig();

  /**
   * @brief 加载 agent.conf（扁平 key=value；provider 用 <name>.<field> 命名）
   * @param conf_path 配置文件路径
   * @return kOk 成功；kOpenError 打不开；kNotFound 无任何有效 provider
   */
  base::Code Load(const std::string &conf_path);

  /**
   * @brief 按名字取 provider
   * @param name provider 名
   * @return 命中返回指针；否则返回 NULL
   */
  const ModelProvider *Get(const std::string &name) const;

  /**
   * @brief 按注册顺序列出全部 provider
   * @param out 输出 provider 指针列表
   */
  void List(std::vector<const ModelProvider *> *out) const;

  /**
   * @brief 默认模型名（缺省取首个注册项）
   * @return 默认 provider 名
   */
  const std::string &DefaultModel() const;

  /**
   * @brief 从环境变量读取 provider 的密钥
   * @param provider 目标 provider
   * @return 密钥字符串；api_key_env 为空或 env 缺失时返回空串
   */
  static std::string GetApiKey(const ModelProvider &provider);

  /**
   * @brief provider 是否可用：本地模型（无 api_key_env）恒可用；云模型需 env 存在
   * @param provider 目标 provider
   * @return 可用返回 true
   */
  static bool IsAvailable(const ModelProvider &provider);

 private:
  AgentConfig(const AgentConfig &c);
  AgentConfig &operator=(const AgentConfig &c);

 private:
  std::map<std::string, ModelProvider> providers_;
  std::vector<std::string> order_;
  std::string default_model_;
}; /*}}}*/

}  // namespace book_agent

#endif
