// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_AGENT_KNOWLEDGE_RETRIEVER_H_
#define SOCK_DEMO_BOOK_AGENT_KNOWLEDGE_RETRIEVER_H_

#include <string>

#include "sock/demo_book/agent/skill_registry.h"

namespace book_agent {

/**
 * @brief 轻量知识检索：按 SKILL.knowledge_globs 与问题里的符号，
 *        把命中的真实源码片段拼成一段上下文注入给模型（无 embedding，零额外依赖）。
 *
 * 目的：解决「模型只拿到 SKILL 摘要、看不到真实实现」导致的臆测
 * （如把 my_map 当哈希表）。检索规则：从问题中抽取 `[A-Za-z0-9_]` 词元，
 * 对每个词元在 glob 目录下尝试 `词元.h` 与 `my_词元.h`，命中即读取片段。
 */
class KnowledgeRetriever { /*{{{*/
 public:
  KnowledgeRetriever();

  /**
   * @brief 设置知识库根目录（knowledge_globs 相对该目录解析）
   * @param root 仓库根目录（相对网关运行目录或绝对路径）；空则禁用检索
   */
  void Init(const std::string &root);

  /**
   * @brief 是否启用（root 非空）
   */
  bool IsReady() const;

  /**
   * @brief 依据 skill 的 knowledge_globs 与问题检索相关源码片段
   * @param skill 命中领域的技能（提供 knowledge_globs）
   * @param question 用户问题
   * @return 拼装好的上下文（含使用说明与文件标记）；未命中返回空串
   */
  std::string Retrieve(const Skill &skill, const std::string &question) const;

 private:
  std::string root_;
  bool ready_;
}; /*}}}*/

}  // namespace book_agent

#endif
