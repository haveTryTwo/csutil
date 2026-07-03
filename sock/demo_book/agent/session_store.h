// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_AGENT_SESSION_STORE_H_
#define SOCK_DEMO_BOOK_AGENT_SESSION_STORE_H_

#include <pthread.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"
#include "sock/demo_book/leveldb_wrapper.h"

namespace book_agent {

/**
 * @brief 一个会话的元信息（持久化于 sess:meta:{id}）
 */
struct SessionMeta { /*{{{*/
  std::string session_id;
  std::string title;
  int64_t created_ms;
  int64_t updated_ms;
  std::string model;   // 该会话默认模型（可空）
  std::string domain;  // 该会话默认领域（可空/auto）
  uint32_t msg_count;  // 已存消息条数，同时用作下一条 seq
  int64_t know_ver_ms;  // 最近一条 assistant 回复所用的知识版本（进程启动时刻，见网关 g_knowledge_version_ms）

  SessionMeta() : created_ms(0), updated_ms(0), msg_count(0), know_ver_ms(0) {}
}; /*}}}*/

/**
 * @brief 会话滚动摘要（L2，持久化于 summary:{id}）
 *
 * covered_seq：已被摘要覆盖到的消息 seq 上界（不含）；即 [0, covered_seq) 的历史已浓缩进 text，
 * 请求时只需带「text + [covered_seq, msg_count) 的原文窗口」。
 */
struct SummaryData { /*{{{*/
  uint32_t covered_seq;
  std::string text;
  int64_t updated_ms;

  SummaryData() : covered_seq(0), updated_ms(0) {}
}; /*}}}*/

/**
 * @brief 一条对话消息（持久化于 msg:{id}:{seq}）
 */
struct StoredMessage { /*{{{*/
  uint32_t seq;
  std::string role;  // user | assistant
  std::string content;
  std::string model;
  std::string domain;
  int64_t ts_ms;
  bool partial;  // 生成被中止（停止）时为 true

  StoredMessage() : seq(0), ts_ms(0), partial(false) {}
}; /*}}}*/

/**
 * @brief Agent 会话/消息的持久化存储（复用 LevelDbWrapper，网关进程内单实例）
 *
 * key 设计（有序，便于 Scan）：
 *   sess:meta:{id}            -> SessionMeta JSON
 *   sess:idx:{inv_updated}:{id} -> ""（按最近更新倒序的列表索引）
 *   msg:{id}:{seq:010d}       -> StoredMessage JSON（前缀扫描即时间序）
 * 写操作用内部互斥锁串行化（seq 分配、meta 读改写、idx 维护需原子）。
 */
class SessionStore { /*{{{*/
 public:
  SessionStore();
  ~SessionStore();

  /**
   * @brief 打开底层 LevelDB
   * @param db_path 数据库目录
   * @return kOk 成功；否则透传 LevelDbWrapper::Init 的错误码
   */
  base::Code Init(const std::string &db_path);

  /**
   * @brief 是否已就绪（Init 成功）
   */
  bool IsReady() const;

  /**
   * @brief 新建会话
   * @param title 标题（空则用「新对话」）
   * @param model 默认模型（可空）
   * @param domain 默认领域（可空）
   * @param know_ver_ms 创建时的知识版本（0 表示未知/不记录）
   * @param out 输出新建的会话元信息
   * @return kOk 成功
   */
  base::Code CreateSession(const std::string &title, const std::string &model, const std::string &domain,
                           int64_t know_ver_ms, SessionMeta *out);

  /**
   * @brief 列出会话（按最近更新倒序）
   * @param limit 最大条数
   * @param out 输出会话列表
   * @return kOk 成功
   */
  base::Code ListSessions(uint32_t limit, std::vector<SessionMeta> *out);

  /**
   * @brief 取单个会话元信息
   * @param session_id 会话 id
   * @param out 输出元信息
   * @return kOk 成功；kNotFound 不存在
   */
  base::Code GetSession(const std::string &session_id, SessionMeta *out);

  /**
   * @brief 局部更新会话元信息（title/model/domain），并维护倒序索引
   * @param session_id 会话 id
   * @param title 新标题（NULL 表示不改）
   * @param model 新模型（NULL 表示不改）
   * @param domain 新领域（NULL 表示不改）
   * @return kOk 成功；kNotFound 不存在
   */
  base::Code PatchSession(const std::string &session_id, const std::string *title, const std::string *model,
                          const std::string *domain);

  /**
   * @brief 删除会话及其全部消息、索引
   * @param session_id 会话 id
   * @return kOk 成功（不存在也返回成功）
   */
  base::Code DeleteSession(const std::string &session_id);

  /**
   * @brief 取会话的全部消息（按 seq 升序）
   * @param session_id 会话 id
   * @param out 输出消息列表
   * @return kOk 成功
   */
  base::Code GetMessages(const std::string &session_id, std::vector<StoredMessage> *out);

  /**
   * @brief 追加一条消息，并更新会话 msg_count/updated_ms 与倒序索引
   * @param session_id 会话 id
   * @param role user | assistant
   * @param content 内容
   * @param model 生成该消息的模型（assistant 用）
   * @param domain 领域（assistant 用）
   * @param partial 是否被中止
   * @param know_ver_ms 生成该消息时的知识版本（role=assistant 且 >0 时写入会话 meta，供陈旧提示使用；否则忽略）
   * @param seq_out 输出分配到的 seq（可 NULL）
   * @return kOk 成功；kNotFound 会话不存在
   */
  base::Code AppendMessage(const std::string &session_id, const std::string &role, const std::string &content,
                           const std::string &model, const std::string &domain, bool partial, int64_t know_ver_ms,
                           uint32_t *seq_out);

  /**
   * @brief 删除会话最后一条 assistant 消息（用于「重新生成」）
   * @param session_id 会话 id
   * @return kOk 删除成功；kNotFound 无 assistant 可删
   */
  base::Code DeleteLastAssistant(const std::string &session_id);

  /**
   * @brief 取会话滚动摘要
   * @param session_id 会话 id
   * @param out 输出摘要（不存在则 covered_seq=0/text 空）
   * @return kOk 成功（含不存在，视为空摘要）
   */
  base::Code GetSummary(const std::string &session_id, SummaryData *out);

  /**
   * @brief 写入/更新会话滚动摘要
   * @param session_id 会话 id
   * @param covered_seq 已覆盖到的 seq 上界（不含）
   * @param text 摘要正文
   * @return kOk 成功
   */
  base::Code PutSummary(const std::string &session_id, uint32_t covered_seq, const std::string &text);

 private:
  SessionStore(const SessionStore &s);
  SessionStore &operator=(const SessionStore &s);

  // 无锁内部版本（调用方已持锁）
  base::Code GetSessionLocked(const std::string &session_id, SessionMeta *out);
  base::Code PutSessionLocked(const SessionMeta &meta, int64_t old_updated_ms, bool old_exists);

 private:
  book_mgr::LevelDbWrapper db_;
  bool ready_;
  uint32_t id_counter_;
  pthread_mutex_t mu_;
}; /*}}}*/

}  // namespace book_agent

#endif
