// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AGENT_SRC_TOOL_EXECUTOR_H_
#define AGENT_SRC_TOOL_EXECUTOR_H_

#include <functional>
#include <string>
#include <vector>

#include "base/status.h"

namespace book_agent {

class SkillRegistry;

/**
 * @brief 只读工具执行器（agentic loop / ReAct 的“动手”能力，见 §16.6）
 *
 * 提供 4 个只读工具，供 LLM 通过 function calling 调用：
 *   - search_code(query, glob?)：在 csutil 根目录下用 grep 检索关键字/正则（fork+exec，无 shell，防注入）
 *   - read_file(path, start?, end?)：读取根目录下的源码/文档片段（路径白名单，禁 `..`/软链越权）
 *   - list_books(prefix?, limit?, start_after?) / get_book(book_id)：查 demo 业务库（经网关注入的回调调 Control RPC）
 *   - list_skills()：列出已加载领域与描述（读 SkillRegistry）
 *
 * 安全约束：所有文件系统访问严格限制在 root_dir（csutil 根）之内；结果有字节/行数上限；子进程有超时。
 * 初始化后只读，可被多个 worker 线程并发调用（book_fn_ 由网关在启动阶段一次性注入）。
 */
class ToolExecutor { /*{{{*/
 public:
  // 业务库工具回调：由网关注入（内部走 Control RPC）；入参为工具名与 JSON 参数串，返回结果文本
  typedef std::function<std::string(const std::string &name, const std::string &args_json)> BookToolFn;

  ToolExecutor();
  ~ToolExecutor();

  /**
   * @brief 初始化：解析并记录根目录 realpath，绑定技能表
   * @param root_dir 工具可访问的根目录（csutil 根）；空则禁用文件类工具
   * @param skills 技能表指针（用于 list_skills；可为 NULL）
   */
  void Init(const std::string &root_dir, const SkillRegistry *skills);

  /**
   * @brief 注入业务库工具回调（list_books/get_book）
   * @param fn 回调
   */
  void SetBookToolFn(const BookToolFn &fn);

  /**
   * @brief 工具是否可用（root 解析成功）
   */
  bool Enabled() const;

  /**
   * @brief OpenAI 兼容的 tools schema（JSON 数组字符串），随请求下发给模型
   * @return schema JSON 串
   */
  const std::string &ToolsSchemaJson() const;

  /**
   * @brief 执行一个工具，返回结果文本（始终返回非空文本，出错也返回可读说明）
   * @param name 工具名
   * @param args_json 工具参数 JSON 串（模型给出的 arguments）
   * @return 结果文本（已做长度上限截断）
   */
  std::string Run(const std::string &name, const std::string &args_json);

 private:
  ToolExecutor(const ToolExecutor &e);
  ToolExecutor &operator=(const ToolExecutor &e);

  std::string RunSearchCode(const std::string &args_json);
  std::string RunReadFile(const std::string &args_json);
  std::string RunListSkills();

  // 把用户给定的相对/绝对路径解析为根目录内的真实路径；越权/不存在返回 false
  bool ResolveInRoot(const std::string &path, std::string *real_out);

 private:
  std::string root_real_;         // 根目录的 realpath（末尾不含 '/'）
  const SkillRegistry *skills_;   // 技能表（list_skills 用）
  BookToolFn book_fn_;            // 业务库工具回调
  std::string schema_;            // tools schema JSON
}; /*}}}*/

}  // namespace book_agent

#endif
