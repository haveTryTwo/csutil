// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AGENT_SRC_SKILL_REGISTRY_H_
#define AGENT_SRC_SKILL_REGISTRY_H_

#include <map>
#include <string>
#include <vector>

#include "base/status.h"

namespace book_agent {

/**
 * @brief 一个领域 Agent 的知识/规范载体（来自一个 SKILL.md）
 *
 * 渐进式披露：name+description 用于路由(L1)，body 用作 system prompt(L2)，
 * knowledge_globs 供 P2 RAG 限定检索范围(L3)。
 */
struct Skill { /*{{{*/
  std::string name;                          // frontmatter.name（= domain）
  std::string description;                   // frontmatter.description（路由依据）
  std::string body;                          // 正文（作为 system prompt）
  std::string preferred_model;               // frontmatter.model（可选）
  std::vector<std::string> knowledge_globs;  // frontmatter.knowledge_globs（P2 RAG 范围）
}; /*}}}*/

/**
 * @brief 扫描并解析 skills/<domain>/SKILL.md，进程内单份持有
 *
 * 加载后只读，可被多个 worker 线程并发访问。frontmatter 用极简
 * `key: value`（支持缩进续行），`---` 之后为 body，不引入 YAML 依赖。
 */
class SkillRegistry { /*{{{*/
 public:
  SkillRegistry();
  ~SkillRegistry();

  /**
   * @brief 扫描目录下每个子目录中的 SKILL.md 并加载
   * @param skills_dir 形如 ./agent/skills 的目录
   * @return kOk 成功（至少加载 0 个不报错）；kOpenError 目录打不开
   */
  base::Code Load(const std::string &skills_dir);

  /**
   * @brief 按领域名取 skill
   * @param name 领域名（= SKILL.md 的 name）
   * @return 命中返回指针；否则 NULL
   */
  const Skill *Get(const std::string &name) const;

  /**
   * @brief 按加载顺序列出全部 skill
   * @param out 输出 skill 指针列表
   */
  void List(std::vector<const Skill *> *out) const;

 private:
  SkillRegistry(const SkillRegistry &r);
  SkillRegistry &operator=(const SkillRegistry &r);

  /**
   * @brief 读取并解析单个 SKILL.md
   * @param path SKILL.md 文件路径
   * @param fallback_name 无 frontmatter.name 时用目录名兜底
   * @return kOk 成功；kOpenError/kReadError 失败
   */
  base::Code LoadOne(const std::string &path, const std::string &fallback_name);

 private:
  std::map<std::string, Skill> skills_;
  std::vector<std::string> order_;
}; /*}}}*/

}  // namespace book_agent

#endif
