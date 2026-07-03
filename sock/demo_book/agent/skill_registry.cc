// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/skill_registry.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

namespace book_agent {

namespace {

/**
 * @brief 去除字符串两端空白（空格/制表符/回车）
 * @param in 输入
 * @return 去空白后的字符串
 */
std::string TrimSpace(const std::string &in) { /*{{{*/
  uint32_t begin = 0;
  uint32_t end = static_cast<uint32_t>(in.size());
  while (begin < end && (in[begin] == ' ' || in[begin] == '\t' || in[begin] == '\r')) ++begin;
  while (end > begin && (in[end - 1] == ' ' || in[end - 1] == '\t' || in[end - 1] == '\r')) --end;
  return in.substr(begin, end - begin);
} /*}}}*/

/**
 * @brief 按分隔符切分并对每段去空白，跳过空段
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

/**
 * @brief 读取整个文件到字符串
 * @param path 文件路径
 * @param content 输出内容
 * @return kOk 成功；kOpenError 打不开
 */
base::Code ReadWholeFile(const std::string &path, std::string *content) { /*{{{*/
  FILE *fp = fopen(path.c_str(), "rb");
  if (fp == NULL) return base::kOpenError;
  char buf[4096];
  size_t n = 0;
  while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) content->append(buf, n);
  fclose(fp);
  return base::kOk;
} /*}}}*/

/**
 * @brief 按 '\n' 拆行（保留原始内容，去掉行尾 '\r'）
 */
void SplitLines(const std::string &text, std::vector<std::string> *lines) { /*{{{*/
  std::string::size_type pos = 0;
  while (pos <= text.size()) {
    std::string::size_type nl = text.find('\n', pos);
    std::string line = (nl == std::string::npos) ? text.substr(pos) : text.substr(pos, nl - pos);
    if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
    lines->push_back(line);
    if (nl == std::string::npos) break;
    pos = nl + 1;
  }
} /*}}}*/

}  // namespace

SkillRegistry::SkillRegistry() {}

SkillRegistry::~SkillRegistry() {}

base::Code SkillRegistry::LoadOne(const std::string &path, const std::string &fallback_name) { /*{{{*/
  std::string content;
  base::Code ret = ReadWholeFile(path, &content);
  if (ret != base::kOk) return ret;

  std::vector<std::string> lines;
  SplitLines(content, &lines);

  Skill skill;
  std::string globs_raw;

  uint32_t idx = 0;
  while (idx < lines.size() && TrimSpace(lines[idx]).empty()) ++idx;  // 跳过前导空行

  bool has_frontmatter = (idx < lines.size() && TrimSpace(lines[idx]) == "---");
  if (has_frontmatter) {
    ++idx;  // 跳过起始 ---
    std::string *cur = NULL;
    for (; idx < lines.size(); ++idx) {
      const std::string &line = lines[idx];
      if (TrimSpace(line) == "---") {
        ++idx;  // 跳过结束 ---
        break;
      }
      if (line.empty()) continue;

      if (line[0] == ' ' || line[0] == '\t') {  // 缩进续行：接到当前字段
        if (cur != NULL) {
          if (!cur->empty()) cur->append(" ");
          cur->append(TrimSpace(line));
        }
        continue;
      }

      std::string::size_type colon = line.find(':');
      if (colon == std::string::npos) continue;
      std::string key = TrimSpace(line.substr(0, colon));
      std::string val = TrimSpace(line.substr(colon + 1));
      if (key == "name") {
        skill.name = val;
        cur = &skill.name;
      } else if (key == "description") {
        skill.description = val;
        cur = &skill.description;
      } else if (key == "model") {
        skill.preferred_model = val;
        cur = &skill.preferred_model;
      } else if (key == "knowledge_globs") {
        globs_raw = val;
        cur = &globs_raw;
      } else {
        cur = NULL;
      }
    }
  }

  // body = frontmatter 之后的剩余行
  std::string body;
  for (uint32_t i = idx; i < lines.size(); ++i) {
    body.append(lines[i]);
    if (i + 1 < lines.size()) body.append("\n");
  }
  skill.body = TrimSpace(body);
  if (skill.name.empty()) skill.name = fallback_name;
  SplitAndTrim(globs_raw, ',', &skill.knowledge_globs);

  if (skills_.find(skill.name) == skills_.end()) order_.push_back(skill.name);
  skills_[skill.name] = skill;
  return base::kOk;
} /*}}}*/

base::Code SkillRegistry::Load(const std::string &skills_dir) { /*{{{*/
  DIR *dir = opendir(skills_dir.c_str());
  if (dir == NULL) return base::kOpenError;

  struct dirent *entry = NULL;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

    std::string sub_path = skills_dir + "/" + entry->d_name;
    struct stat st;
    if (stat(sub_path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) continue;

    std::string skill_file = sub_path + "/SKILL.md";
    LoadOne(skill_file, entry->d_name);  // 无 SKILL.md 则忽略
  }
  closedir(dir);
  return base::kOk;
} /*}}}*/

const Skill *SkillRegistry::Get(const std::string &name) const { /*{{{*/
  std::map<std::string, Skill>::const_iterator it = skills_.find(name);
  if (it == skills_.end()) return NULL;
  return &(it->second);
} /*}}}*/

void SkillRegistry::List(std::vector<const Skill *> *out) const { /*{{{*/
  if (out == NULL) return;
  for (uint32_t i = 0; i < order_.size(); ++i) {
    std::map<std::string, Skill>::const_iterator it = skills_.find(order_[i]);
    if (it != skills_.end()) out->push_back(&(it->second));
  }
} /*}}}*/

}  // namespace book_agent
