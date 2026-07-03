// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/knowledge_retriever.h"

#include <ctype.h>
#include <stdio.h>

#include <set>
#include <vector>

namespace book_agent {

namespace {

const uint32_t kMinTokenLen = 3;      // 词元最小长度，过滤 of/in 等噪声
const uint32_t kMaxFiles = 3;         // 单次最多注入的文件数
const uint32_t kMaxBytesPerFile = 4000;  // 单文件片段字节上限
const uint32_t kMaxTotalBytes = 8000;    // 全部片段字节上限

/**
 * @brief 从问题中抽取 `[A-Za-z0-9_]` 词元并转小写（长度 >= kMinTokenLen）
 */
void ExtractTokens(const std::string &q, std::vector<std::string> *tokens) { /*{{{*/
  std::string cur;
  for (uint32_t i = 0; i <= q.size(); ++i) {
    char c = (i < q.size()) ? q[i] : '\0';
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
      cur.push_back(static_cast<char>(tolower(static_cast<unsigned char>(c))));
    } else {
      if (cur.size() >= kMinTokenLen) tokens->push_back(cur);
      cur.clear();
    }
  }
} /*}}}*/

/**
 * @brief 拆分 glob（如 "mystl/*.h"）为目录与后缀
 * @param glob 形如 "dir/sub/*.h" 或 "dir/name.h"
 * @param dir 输出目录部分（相对根）
 * @param suffix 输出文件后缀（如 ".h"）
 */
void SplitGlob(const std::string &glob, std::string *dir, std::string *suffix) { /*{{{*/
  std::string::size_type slash = glob.rfind('/');
  std::string filepat = (slash == std::string::npos) ? glob : glob.substr(slash + 1);
  *dir = (slash == std::string::npos) ? "" : glob.substr(0, slash);
  std::string::size_type star = filepat.find('*');
  *suffix = (star == std::string::npos) ? filepat : filepat.substr(star + 1);
} /*}}}*/

/**
 * @brief 读取文件前若干字节（文件不存在返回 false）
 */
bool ReadHead(const std::string &path, uint32_t max_bytes, std::string *out) { /*{{{*/
  FILE *fp = fopen(path.c_str(), "rb");
  if (fp == NULL) return false;
  char buf[4096];
  uint32_t total = 0;
  while (total < max_bytes) {
    size_t want = sizeof(buf);
    if (want > max_bytes - total) want = max_bytes - total;
    size_t n = fread(buf, 1, want, fp);
    if (n == 0) break;
    out->append(buf, n);
    total += static_cast<uint32_t>(n);
  }
  fclose(fp);
  return true;
} /*}}}*/

}  // namespace

KnowledgeRetriever::KnowledgeRetriever() : ready_(false) {}

void KnowledgeRetriever::Init(const std::string &root) { /*{{{*/
  root_ = root;
  ready_ = !root.empty();
} /*}}}*/

bool KnowledgeRetriever::IsReady() const { return ready_; }

std::string KnowledgeRetriever::Retrieve(const Skill &skill, const std::string &question) const { /*{{{*/
  if (!ready_ || skill.knowledge_globs.empty()) return "";

  std::vector<std::string> tokens;
  ExtractTokens(question, &tokens);
  if (tokens.empty()) return "";

  std::set<std::string> seen_paths;  // 去重
  std::string blocks;
  uint32_t file_num = 0;
  uint32_t total_bytes = 0;

  for (uint32_t g = 0; g < skill.knowledge_globs.size() && file_num < kMaxFiles; ++g) {
    std::string dir;
    std::string suffix;
    SplitGlob(skill.knowledge_globs[g], &dir, &suffix);

    for (uint32_t t = 0; t < tokens.size() && file_num < kMaxFiles; ++t) {
      // 对每个词元尝试 <token><suffix> 与 my_<token><suffix>
      std::string names[2];
      names[0] = tokens[t] + suffix;
      names[1] = (tokens[t].compare(0, 3, "my_") == 0) ? "" : ("my_" + tokens[t] + suffix);

      for (uint32_t k = 0; k < 2; ++k) {
        if (names[k].empty()) continue;
        std::string rel = dir.empty() ? names[k] : (dir + "/" + names[k]);
        std::string path = root_.empty() ? rel : (root_ + "/" + rel);
        if (seen_paths.count(path) > 0) continue;

        std::string content;
        if (!ReadHead(path, kMaxBytesPerFile, &content)) continue;  // 不存在则跳过
        seen_paths.insert(path);

        if (total_bytes + content.size() > kMaxTotalBytes) content.resize(kMaxTotalBytes - total_bytes);
        blocks += "// 文件: " + rel + "\n";
        blocks += content;
        blocks += "\n\n";
        total_bytes += static_cast<uint32_t>(content.size());
        ++file_num;
        if (file_num >= kMaxFiles || total_bytes >= kMaxTotalBytes) break;
      }
    }
  }

  if (blocks.empty()) return "";
  return "以下是 csutil 仓库中的真实源码片段，回答必须严格依据这些代码"
         "（如与你的先验冲突，一律以代码为准；例如据 typedef/成员函数判断容器底层实现）：\n\n" +
         blocks;
} /*}}}*/

}  // namespace book_agent
