// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "agent/src/session_store.h"

#include <stdio.h>
#include <sys/time.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace book_agent {

namespace {

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;

const uint32_t kMaxScanBatch = 512;

/**
 * @brief 当前时间（毫秒）
 */
int64_t NowMs() { /*{{{*/
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<int64_t>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
} /*}}}*/

std::string MetaKey(const std::string &id) { return "sess:meta:" + id; }

std::string SummaryKey(const std::string &id) { return "summary:" + id; }

std::string MsgPrefix(const std::string &id) { return "msg:" + id + ":"; }

/**
 * @brief 消息 key：msg:{id}:{seq:010d}（零填充保证字典序=时间序）
 */
std::string MsgKey(const std::string &id, uint32_t seq) { /*{{{*/
  char buf[16] = {0};
  snprintf(buf, sizeof(buf), "%010u", seq);
  return "msg:" + id + ":" + buf;
} /*}}}*/

/**
 * @brief 倒序索引 key：sess:idx:{inv_updated}:{id}，inv=UINT64_MAX-updated（最近在前）
 */
std::string IdxKey(int64_t updated_ms, const std::string &id) { /*{{{*/
  uint64_t inv = static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL) - static_cast<uint64_t>(updated_ms);
  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "%020llu", static_cast<unsigned long long>(inv));
  return "sess:idx:" + std::string(buf) + ":" + id;
} /*}}}*/

/**
 * @brief 序列化 SessionMeta 为 JSON
 */
std::string SerializeMeta(const SessionMeta &m) { /*{{{*/
  rapidjson::StringBuffer sb;
  JsonWriter w(sb);
  w.StartObject();
  w.Key("session_id");
  w.String(m.session_id.c_str());
  w.Key("title");
  w.String(m.title.c_str(), static_cast<rapidjson::SizeType>(m.title.size()));
  w.Key("created_ms");
  w.Int64(m.created_ms);
  w.Key("updated_ms");
  w.Int64(m.updated_ms);
  w.Key("model");
  w.String(m.model.c_str());
  w.Key("domain");
  w.String(m.domain.c_str());
  w.Key("msg_count");
  w.Uint(m.msg_count);
  w.Key("know_ver_ms");
  w.Int64(m.know_ver_ms);
  w.EndObject();
  return sb.GetString();
} /*}}}*/

/**
 * @brief 解析 SessionMeta JSON
 * @return kOk 成功；kInvalidParam 非法
 */
base::Code ParseMeta(const std::string &json, SessionMeta *m) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json.c_str(), json.size());
  if (doc.HasParseError() || !doc.IsObject()) return base::kInvalidParam;
  if (doc.HasMember("session_id") && doc["session_id"].IsString()) m->session_id = doc["session_id"].GetString();
  if (doc.HasMember("title") && doc["title"].IsString()) m->title = doc["title"].GetString();
  if (doc.HasMember("created_ms") && doc["created_ms"].IsInt64()) m->created_ms = doc["created_ms"].GetInt64();
  if (doc.HasMember("updated_ms") && doc["updated_ms"].IsInt64()) m->updated_ms = doc["updated_ms"].GetInt64();
  if (doc.HasMember("model") && doc["model"].IsString()) m->model = doc["model"].GetString();
  if (doc.HasMember("domain") && doc["domain"].IsString()) m->domain = doc["domain"].GetString();
  if (doc.HasMember("msg_count") && doc["msg_count"].IsUint()) m->msg_count = doc["msg_count"].GetUint();
  if (doc.HasMember("know_ver_ms") && doc["know_ver_ms"].IsInt64()) m->know_ver_ms = doc["know_ver_ms"].GetInt64();
  return base::kOk;
} /*}}}*/

/**
 * @brief 序列化 StoredMessage 为 JSON
 */
std::string SerializeMsg(const StoredMessage &msg) { /*{{{*/
  rapidjson::StringBuffer sb;
  JsonWriter w(sb);
  w.StartObject();
  w.Key("seq");
  w.Uint(msg.seq);
  w.Key("role");
  w.String(msg.role.c_str());
  w.Key("content");
  w.String(msg.content.c_str(), static_cast<rapidjson::SizeType>(msg.content.size()));
  w.Key("model");
  w.String(msg.model.c_str());
  w.Key("domain");
  w.String(msg.domain.c_str());
  w.Key("ts_ms");
  w.Int64(msg.ts_ms);
  w.Key("partial");
  w.Bool(msg.partial);
  w.EndObject();
  return sb.GetString();
} /*}}}*/

/**
 * @brief 解析 StoredMessage JSON
 */
base::Code ParseMsg(const std::string &json, StoredMessage *msg) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json.c_str(), json.size());
  if (doc.HasParseError() || !doc.IsObject()) return base::kInvalidParam;
  if (doc.HasMember("seq") && doc["seq"].IsUint()) msg->seq = doc["seq"].GetUint();
  if (doc.HasMember("role") && doc["role"].IsString()) msg->role = doc["role"].GetString();
  if (doc.HasMember("content") && doc["content"].IsString()) {
    msg->content.assign(doc["content"].GetString(), doc["content"].GetStringLength());
  }
  if (doc.HasMember("model") && doc["model"].IsString()) msg->model = doc["model"].GetString();
  if (doc.HasMember("domain") && doc["domain"].IsString()) msg->domain = doc["domain"].GetString();
  if (doc.HasMember("ts_ms") && doc["ts_ms"].IsInt64()) msg->ts_ms = doc["ts_ms"].GetInt64();
  if (doc.HasMember("partial") && doc["partial"].IsBool()) msg->partial = doc["partial"].GetBool();
  return base::kOk;
} /*}}}*/

}  // namespace

SessionStore::SessionStore() : ready_(false), id_counter_(0) { pthread_mutex_init(&mu_, NULL); }

SessionStore::~SessionStore() { pthread_mutex_destroy(&mu_); }

base::Code SessionStore::Init(const std::string &db_path) { /*{{{*/
  base::Code ret = db_.Init(db_path);
  if (ret != base::kOk) return ret;
  ready_ = true;
  return base::kOk;
} /*}}}*/

bool SessionStore::IsReady() const { return ready_; }

base::Code SessionStore::GetSessionLocked(const std::string &session_id, SessionMeta *out) { /*{{{*/
  std::string json;
  base::Code ret = db_.Get(MetaKey(session_id), &json);
  if (ret != base::kOk) return ret;  // kNotFound 透传
  return ParseMeta(json, out);
} /*}}}*/

base::Code SessionStore::PutSessionLocked(const SessionMeta &meta, int64_t old_updated_ms, bool old_exists) { /*{{{*/
  if (old_exists) db_.Delete(IdxKey(old_updated_ms, meta.session_id));  // 移除旧索引
  base::Code ret = db_.Put(MetaKey(meta.session_id), SerializeMeta(meta));
  if (ret != base::kOk) return ret;
  return db_.Put(IdxKey(meta.updated_ms, meta.session_id), "");
} /*}}}*/

base::Code SessionStore::CreateSession(const std::string &title, const std::string &model, const std::string &domain,
                                       int64_t know_ver_ms, SessionMeta *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  int64_t now = NowMs();
  char suffix[8] = {0};
  snprintf(suffix, sizeof(suffix), "%04u", (++id_counter_) % 10000);

  SessionMeta meta;
  char id_buf[32] = {0};
  snprintf(id_buf, sizeof(id_buf), "%lld_%s", static_cast<long long>(now), suffix);
  meta.session_id = id_buf;
  meta.title = title.empty() ? "新对话" : title;
  meta.created_ms = now;
  meta.updated_ms = now;
  meta.model = model;
  meta.domain = domain;
  meta.msg_count = 0;
  meta.know_ver_ms = know_ver_ms;

  base::Code ret = PutSessionLocked(meta, 0, false);
  if (ret == base::kOk && out != NULL) *out = meta;

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code SessionStore::ListSessions(uint32_t limit, std::vector<SessionMeta> *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  if (out == NULL || limit == 0) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);

  std::vector<std::string> keys;
  std::vector<std::string> values;
  std::string next_cursor;
  base::Code ret = db_.Scan("sess:idx:", "", limit, &keys, &values, &next_cursor);
  if (ret == base::kOk) {
    for (uint32_t i = 0; i < keys.size(); ++i) {
      // key = sess:idx:{inv}:{id}，取最后一个 ':' 之后为 id
      std::string::size_type pos = keys[i].rfind(':');
      if (pos == std::string::npos) continue;
      std::string id = keys[i].substr(pos + 1);
      SessionMeta meta;
      if (GetSessionLocked(id, &meta) == base::kOk) out->push_back(meta);
    }
  }

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code SessionStore::GetSession(const std::string &session_id, SessionMeta *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);
  base::Code ret = GetSessionLocked(session_id, out);
  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code SessionStore::PatchSession(const std::string &session_id, const std::string *title,
                                      const std::string *model, const std::string *domain) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  SessionMeta meta;
  base::Code ret = GetSessionLocked(session_id, &meta);
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }
  int64_t old_updated = meta.updated_ms;
  if (title != NULL) meta.title = *title;
  if (model != NULL) meta.model = *model;
  if (domain != NULL) meta.domain = *domain;
  meta.updated_ms = NowMs();
  ret = PutSessionLocked(meta, old_updated, true);

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code SessionStore::DeleteSession(const std::string &session_id) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  SessionMeta meta;
  base::Code got = GetSessionLocked(session_id, &meta);
  if (got == base::kOk) {
    db_.Delete(IdxKey(meta.updated_ms, session_id));
    db_.Delete(MetaKey(session_id));
    db_.Delete("summary:" + session_id);
  }

  // 批量删除该会话所有消息
  std::string prefix = MsgPrefix(session_id);
  while (true) {
    std::vector<std::string> keys;
    std::vector<std::string> values;
    std::string next_cursor;
    base::Code ret = db_.Scan(prefix, "", kMaxScanBatch, &keys, &values, &next_cursor);
    if (ret != base::kOk || keys.empty()) break;
    for (uint32_t i = 0; i < keys.size(); ++i) db_.Delete(keys[i]);
    if (next_cursor.empty()) break;  // 未满页，已删完
  }

  pthread_mutex_unlock(&mu_);
  return base::kOk;
} /*}}}*/

base::Code SessionStore::GetMessages(const std::string &session_id, std::vector<StoredMessage> *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  if (out == NULL) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);

  std::string prefix = MsgPrefix(session_id);
  std::string start_after;
  while (true) {
    std::vector<std::string> keys;
    std::vector<std::string> values;
    std::string next_cursor;
    base::Code ret = db_.Scan(prefix, start_after, kMaxScanBatch, &keys, &values, &next_cursor);
    if (ret != base::kOk) {
      pthread_mutex_unlock(&mu_);
      return ret;
    }
    for (uint32_t i = 0; i < values.size(); ++i) {
      StoredMessage msg;
      if (ParseMsg(values[i], &msg) == base::kOk) out->push_back(msg);
    }
    if (next_cursor.empty()) break;
    start_after = next_cursor;
  }

  pthread_mutex_unlock(&mu_);
  return base::kOk;
} /*}}}*/

base::Code SessionStore::AppendMessage(const std::string &session_id, const std::string &role,
                                       const std::string &content, const std::string &model,
                                       const std::string &domain, bool partial, int64_t know_ver_ms,
                                       uint32_t *seq_out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  SessionMeta meta;
  base::Code ret = GetSessionLocked(session_id, &meta);
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }

  StoredMessage msg;
  msg.seq = meta.msg_count;
  msg.role = role;
  msg.content = content;
  msg.model = model;
  msg.domain = domain;
  msg.ts_ms = NowMs();
  msg.partial = partial;

  ret = db_.Put(MsgKey(session_id, msg.seq), SerializeMsg(msg));
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }
  if (seq_out != NULL) *seq_out = msg.seq;

  int64_t old_updated = meta.updated_ms;
  meta.msg_count += 1;
  meta.updated_ms = msg.ts_ms;
  if (role == "assistant" && know_ver_ms > 0) meta.know_ver_ms = know_ver_ms;
  ret = PutSessionLocked(meta, old_updated, true);

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code SessionStore::DeleteLastAssistant(const std::string &session_id) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  SessionMeta meta;
  base::Code ret = GetSessionLocked(session_id, &meta);
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }
  if (meta.msg_count == 0) {
    pthread_mutex_unlock(&mu_);
    return base::kNotFound;
  }

  uint32_t last_seq = meta.msg_count - 1;
  std::string json;
  ret = db_.Get(MsgKey(session_id, last_seq), &json);
  StoredMessage last;
  if (ret != base::kOk || ParseMsg(json, &last) != base::kOk || last.role != "assistant") {
    pthread_mutex_unlock(&mu_);
    return base::kNotFound;
  }

  db_.Delete(MsgKey(session_id, last_seq));
  int64_t old_updated = meta.updated_ms;
  meta.msg_count -= 1;
  meta.updated_ms = NowMs();
  PutSessionLocked(meta, old_updated, true);

  pthread_mutex_unlock(&mu_);
  return base::kOk;
} /*}}}*/

base::Code SessionStore::GetSummary(const std::string &session_id, SummaryData *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  if (out == NULL) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);

  std::string json;
  base::Code ret = db_.Get(SummaryKey(session_id), &json);
  if (ret == base::kNotFound) {
    pthread_mutex_unlock(&mu_);
    return base::kOk;  // 无摘要视为空摘要
  }
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }

  rapidjson::Document doc;
  doc.Parse(json.c_str(), json.size());
  if (!doc.HasParseError() && doc.IsObject()) {
    if (doc.HasMember("covered_seq") && doc["covered_seq"].IsUint()) out->covered_seq = doc["covered_seq"].GetUint();
    if (doc.HasMember("text") && doc["text"].IsString()) {
      out->text.assign(doc["text"].GetString(), doc["text"].GetStringLength());
    }
    if (doc.HasMember("updated_ms") && doc["updated_ms"].IsInt64()) out->updated_ms = doc["updated_ms"].GetInt64();
  }

  pthread_mutex_unlock(&mu_);
  return base::kOk;
} /*}}}*/

base::Code SessionStore::PutSummary(const std::string &session_id, uint32_t covered_seq,
                                    const std::string &text) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  rapidjson::StringBuffer sb;
  JsonWriter w(sb);
  w.StartObject();
  w.Key("covered_seq");
  w.Uint(covered_seq);
  w.Key("text");
  w.String(text.c_str(), static_cast<rapidjson::SizeType>(text.size()));
  w.Key("updated_ms");
  w.Int64(NowMs());
  w.EndObject();
  base::Code ret = db_.Put(SummaryKey(session_id), sb.GetString());

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

}  // namespace book_agent
