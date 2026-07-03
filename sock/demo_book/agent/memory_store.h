// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_AGENT_MEMORY_STORE_H_
#define SOCK_DEMO_BOOK_AGENT_MEMORY_STORE_H_

#include <pthread.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"
#include "sock/demo_book/leveldb_wrapper.h"

namespace book_agent {

/**
 * @brief 一条长期记忆（跨会话，对标 Cursor Memories，持久化于 mem:{scope}:{id}）
 */
struct MemoryItem { /*{{{*/
  std::string mem_id;
  std::string scope;           // global（当前仅用全局）；预留 session_id 粒度
  std::string kind;            // fact | preference | summary
  std::string text;            // 记忆正文（用户可见、可删）
  std::string source_session;  // 抽取来源会话（可空）
  int64_t created_ms;
  int64_t updated_ms;

  MemoryItem() : created_ms(0), updated_ms(0) {}
}; /*}}}*/

/**
 * @brief 长期记忆的持久化存储（独立 LevelDB 实例，进程内单实例，内部加锁）
 *
 * 与会话库分库存放（不同目录），规避 LevelDB「同目录进程内仅能 Open 一次」限制，
 * 且记忆是跨会话的全局资产，物理隔离便于导出/清理。key 设计（有序，便于 Scan）：
 *   mem:item:{mem_id}                    -> MemoryItem JSON（主键，按 id 直取）
 *   mem:idx:{scope}:{inv_created}:{mem_id} -> ""（按创建倒序的列表索引，最近在前）
 * 写操作用内部互斥锁串行化（id 分配、去重查重需原子）。
 */
class MemoryStore { /*{{{*/
 public:
  MemoryStore();
  ~MemoryStore();

  /**
   * @brief 打开底层 LevelDB
   * @param db_path 数据库目录（须与会话库不同目录）
   * @return kOk 成功；否则透传 LevelDbWrapper::Init 的错误码
   */
  base::Code Init(const std::string &db_path);

  /**
   * @brief 是否已就绪（Init 成功）
   */
  bool IsReady() const;

  /**
   * @brief 新增一条记忆（文本归一后与既有记忆重复则跳过，返回既有项）
   * @param kind fact|preference|summary（非法值归一为 fact）
   * @param text 记忆正文（空则拒绝）
   * @param source_session 来源会话（可空）
   * @param out 输出落库/命中的记忆项（可 NULL）
   * @return kOk 新增成功；kExist 命中去重（out 为既有项）；kInvalidParam 文本为空
   */
  base::Code Create(const std::string &kind, const std::string &text, const std::string &source_session,
                    MemoryItem *out);

  /**
   * @brief 列出记忆（按创建倒序）
   * @param scope 作用域（空则默认 global）
   * @param limit 最大条数
   * @param out 输出记忆列表
   * @return kOk 成功
   */
  base::Code List(const std::string &scope, uint32_t limit, std::vector<MemoryItem> *out);

  /**
   * @brief 取单条记忆
   * @param mem_id 记忆 id
   * @param out 输出记忆项
   * @return kOk 成功；kNotFound 不存在
   */
  base::Code Get(const std::string &mem_id, MemoryItem *out);

  /**
   * @brief 修改记忆正文/类型
   * @param mem_id 记忆 id
   * @param kind 新类型（NULL 不改）
   * @param text 新正文（NULL 不改）
   * @return kOk 成功；kNotFound 不存在
   */
  base::Code Patch(const std::string &mem_id, const std::string *kind, const std::string *text);

  /**
   * @brief 删除记忆（不存在也返回成功）
   * @param mem_id 记忆 id
   * @return kOk 成功
   */
  base::Code Delete(const std::string &mem_id);

 private:
  MemoryStore(const MemoryStore &s);
  MemoryStore &operator=(const MemoryStore &s);

  // 无锁内部版本（调用方已持锁）：按 id 读主键 item
  base::Code GetLocked(const std::string &mem_id, MemoryItem *out);

 private:
  book_mgr::LevelDbWrapper db_;
  bool ready_;
  uint32_t id_counter_;
  pthread_mutex_t mu_;
}; /*}}}*/

}  // namespace book_agent

#endif
