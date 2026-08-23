// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AGENT_SRC_SKILL_REGISTRY_H_
#define AGENT_SRC_SKILL_REGISTRY_H_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "base/mutex.h"
#include "base/status.h"

namespace book_agent {

/**
 * @brief 一个领域 Agent 的知识/规范载体（来自一个 SKILL.md）
 *
 * 渐进式披露：name+description+keywords 用于路由(L1)，body 用作 system prompt(L2)，
 * knowledge_globs 供 P2 RAG 限定检索范围(L3)。
 */
struct Skill { /*{{{*/
  std::string name;                          // frontmatter.name（= domain）
  std::string description;                   // frontmatter.description（展示与路由兜底）
  std::string body;                          // 正文（作为 system prompt）
  std::string preferred_model;               // frontmatter.model（可选）
  std::vector<std::string> knowledge_globs;  // frontmatter.knowledge_globs（P2 RAG 范围）
  std::vector<std::string> keywords;         // frontmatter.keywords（路由主依据）
}; /*}}}*/

/**
 * @brief 扫描并解析 skills/<domain>/SKILL.md，支持热加载
 *
 * frontmatter 用极简 `key: value`（支持缩进续行），`---` 之后为 body，不引入 YAML 依赖。
 */
class SkillRegistry { /*{{{*/
 public:
  SkillRegistry();
  ~SkillRegistry();

  /**
   * @brief 记录 skills 目录并全量加载（等价于设置目录后 Reload）
   * @param skills_dir 形如 ./skills 的目录
   * @return kOk 成功；kOpenError 目录打不开
   */
  base::Code Load(const std::string &skills_dir);

  /**
   * @brief 重新扫描 skills_dir_，原子替换内部表；线程安全
   * @return kOk 成功；kOpenError 目录未设置或打不开
   */
  base::Code Reload();

  /**
   * @brief 最近一次成功 Load/Reload 的版本时间戳（毫秒）
   */
  int64_t Version() const;

  /**
   * @brief 当前 skills 根目录（Load 时设置）
   */
  std::string SkillsDir() const;

  /**
   * @brief 探测 skills 目录下各 SKILL.md 的最大 mtime（用于自动热加载）
   */
  int64_t ProbeSkillsMtime() const;

  /**
   * @brief 上次 Reload 后缓存的 mtime 快照
   */
  int64_t CachedMtime() const;

  /**
   * @brief 按领域名取 skill（指针仅在无并发 Reload 的短生命周期内有效）
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
   * @brief 扫描目录填充新表（不加锁，供 Reload 使用）
   */
  base::Code ScanInto(std::map<std::string, Skill> *skills, std::vector<std::string> *order) const;

  /**
   * @brief 读取并解析单个 SKILL.md
   */
  base::Code LoadOne(const std::string &path, const std::string &fallback_name, Skill *out) const;

 private:
  mutable base::Mutex mutex_;
  std::string skills_dir_;
  std::map<std::string, Skill> skills_;
  std::vector<std::string> order_;
  int64_t version_ms_;
  int64_t cached_mtime_;
}; /*}}}*/

}  // namespace book_agent

#endif
