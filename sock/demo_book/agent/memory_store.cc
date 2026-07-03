// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/agent/memory_store.h"

#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace book_agent {

namespace {

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;

const uint32_t kMaxScanBatch = 512;
const char kDefaultScope[] = "global";

/**
 * @brief 当前时间（毫秒）
 */
int64_t NowMs() { /*{{{*/
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<int64_t>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
} /*}}}*/

std::string ItemKey(const std::string &mem_id) { return "mem:item:" + mem_id; }

/**
 * @brief 列表索引 key：mem:idx:{scope}:{inv_created}:{mem_id}，inv=UINT64_MAX-created（最近在前）
 */
std::string IdxKey(const std::string &scope, int64_t created_ms, const std::string &mem_id) { /*{{{*/
  uint64_t inv = static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL) - static_cast<uint64_t>(created_ms);
  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "%020llu", static_cast<unsigned long long>(inv));
  return "mem:idx:" + scope + ":" + std::string(buf) + ":" + mem_id;
} /*}}}*/

std::string IdxPrefix(const std::string &scope) { return "mem:idx:" + scope + ":"; }

/**
 * @brief 归一化文本用于去重：去首尾空白、内部连续空白折叠为单空格、转小写（仅 ASCII）
 */
std::string Normalize(const std::string &in) { /*{{{*/
  std::string out;
  bool in_space = true;  // 起始视为空白，天然去掉前导空白
  for (uint32_t i = 0; i < in.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(in[i]);
    bool is_space = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    if (is_space) {
      if (!in_space) out.push_back(' ');
      in_space = true;
    } else {
      out.push_back(static_cast<char>(tolower(c)));
      in_space = false;
    }
  }
  while (!out.empty() && out[out.size() - 1] == ' ') out.resize(out.size() - 1);  // 去尾部空白
  return out;
} /*}}}*/

/**
 * @brief 校验 kind，非法归一为 fact
 */
std::string NormalizeKind(const std::string &kind) { /*{{{*/
  if (kind == "fact" || kind == "preference" || kind == "summary") return kind;
  return "fact";
} /*}}}*/

/**
 * @brief 序列化 MemoryItem 为 JSON
 */
std::string SerializeItem(const MemoryItem &m) { /*{{{*/
  rapidjson::StringBuffer sb;
  JsonWriter w(sb);
  w.StartObject();
  w.Key("mem_id");
  w.String(m.mem_id.c_str());
  w.Key("scope");
  w.String(m.scope.c_str());
  w.Key("kind");
  w.String(m.kind.c_str());
  w.Key("text");
  w.String(m.text.c_str(), static_cast<rapidjson::SizeType>(m.text.size()));
  w.Key("source_session");
  w.String(m.source_session.c_str());
  w.Key("created_ms");
  w.Int64(m.created_ms);
  w.Key("updated_ms");
  w.Int64(m.updated_ms);
  w.EndObject();
  return sb.GetString();
} /*}}}*/

/**
 * @brief 解析 MemoryItem JSON
 */
base::Code ParseItem(const std::string &json, MemoryItem *m) { /*{{{*/
  rapidjson::Document doc;
  doc.Parse(json.c_str(), json.size());
  if (doc.HasParseError() || !doc.IsObject()) return base::kInvalidParam;
  if (doc.HasMember("mem_id") && doc["mem_id"].IsString()) m->mem_id = doc["mem_id"].GetString();
  if (doc.HasMember("scope") && doc["scope"].IsString()) m->scope = doc["scope"].GetString();
  if (doc.HasMember("kind") && doc["kind"].IsString()) m->kind = doc["kind"].GetString();
  if (doc.HasMember("text") && doc["text"].IsString()) {
    m->text.assign(doc["text"].GetString(), doc["text"].GetStringLength());
  }
  if (doc.HasMember("source_session") && doc["source_session"].IsString()) {
    m->source_session = doc["source_session"].GetString();
  }
  if (doc.HasMember("created_ms") && doc["created_ms"].IsInt64()) m->created_ms = doc["created_ms"].GetInt64();
  if (doc.HasMember("updated_ms") && doc["updated_ms"].IsInt64()) m->updated_ms = doc["updated_ms"].GetInt64();
  return base::kOk;
} /*}}}*/

}  // namespace

MemoryStore::MemoryStore() : ready_(false), id_counter_(0) { pthread_mutex_init(&mu_, NULL); }

MemoryStore::~MemoryStore() { pthread_mutex_destroy(&mu_); }

base::Code MemoryStore::Init(const std::string &db_path) { /*{{{*/
  base::Code ret = db_.Init(db_path);
  if (ret != base::kOk) return ret;
  ready_ = true;
  return base::kOk;
} /*}}}*/

bool MemoryStore::IsReady() const { return ready_; }

base::Code MemoryStore::GetLocked(const std::string &mem_id, MemoryItem *out) { /*{{{*/
  std::string json;
  base::Code ret = db_.Get(ItemKey(mem_id), &json);
  if (ret != base::kOk) return ret;  // kNotFound 透传
  return ParseItem(json, out);
} /*}}}*/

base::Code MemoryStore::Create(const std::string &kind, const std::string &text, const std::string &source_session,
                               MemoryItem *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  std::string norm = Normalize(text);
  if (norm.empty()) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);

  // 去重：扫描 global 索引，命中归一相等则返回既有项
  std::string prefix = IdxPrefix(kDefaultScope);
  std::string start_after;
  while (true) {
    std::vector<std::string> keys;
    std::vector<std::string> values;
    std::string next_cursor;
    base::Code sret = db_.Scan(prefix, start_after, kMaxScanBatch, &keys, &values, &next_cursor);
    if (sret != base::kOk) break;
    for (uint32_t i = 0; i < keys.size(); ++i) {
      std::string::size_type pos = keys[i].rfind(':');
      if (pos == std::string::npos) continue;
      std::string id = keys[i].substr(pos + 1);
      MemoryItem exist;
      if (GetLocked(id, &exist) == base::kOk && Normalize(exist.text) == norm) {
        if (out != NULL) *out = exist;
        pthread_mutex_unlock(&mu_);
        return base::kExist;
      }
    }
    if (next_cursor.empty()) break;
    start_after = next_cursor;
  }

  int64_t now = NowMs();
  char id_buf[32] = {0};
  snprintf(id_buf, sizeof(id_buf), "%lld_%04u", static_cast<long long>(now), (++id_counter_) % 10000);

  MemoryItem item;
  item.mem_id = id_buf;
  item.scope = kDefaultScope;
  item.kind = NormalizeKind(kind);
  item.text = text;
  item.source_session = source_session;
  item.created_ms = now;
  item.updated_ms = now;

  base::Code ret = db_.Put(ItemKey(item.mem_id), SerializeItem(item));
  if (ret == base::kOk) ret = db_.Put(IdxKey(item.scope, item.created_ms, item.mem_id), "");
  if (ret == base::kOk && out != NULL) *out = item;

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code MemoryStore::List(const std::string &scope, uint32_t limit, std::vector<MemoryItem> *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  if (out == NULL || limit == 0) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);

  std::string real_scope = scope.empty() ? kDefaultScope : scope;
  std::vector<std::string> keys;
  std::vector<std::string> values;
  std::string next_cursor;
  base::Code ret = db_.Scan(IdxPrefix(real_scope), "", limit, &keys, &values, &next_cursor);
  if (ret == base::kOk) {
    for (uint32_t i = 0; i < keys.size(); ++i) {
      std::string::size_type pos = keys[i].rfind(':');
      if (pos == std::string::npos) continue;
      std::string id = keys[i].substr(pos + 1);
      MemoryItem item;
      if (GetLocked(id, &item) == base::kOk) out->push_back(item);
    }
  }

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code MemoryStore::Get(const std::string &mem_id, MemoryItem *out) { /*{{{*/
  if (!ready_) return base::kNotInit;
  if (out == NULL) return base::kInvalidParam;
  pthread_mutex_lock(&mu_);
  base::Code ret = GetLocked(mem_id, out);
  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code MemoryStore::Patch(const std::string &mem_id, const std::string *kind, const std::string *text) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  MemoryItem item;
  base::Code ret = GetLocked(mem_id, &item);
  if (ret != base::kOk) {
    pthread_mutex_unlock(&mu_);
    return ret;
  }
  if (kind != NULL) item.kind = NormalizeKind(*kind);
  if (text != NULL && !Normalize(*text).empty()) item.text = *text;
  item.updated_ms = NowMs();
  ret = db_.Put(ItemKey(item.mem_id), SerializeItem(item));  // 索引 key 含 created_ms 不变，无需重建

  pthread_mutex_unlock(&mu_);
  return ret;
} /*}}}*/

base::Code MemoryStore::Delete(const std::string &mem_id) { /*{{{*/
  if (!ready_) return base::kNotInit;
  pthread_mutex_lock(&mu_);

  MemoryItem item;
  base::Code got = GetLocked(mem_id, &item);
  if (got == base::kOk) {
    db_.Delete(IdxKey(item.scope, item.created_ms, item.mem_id));
    db_.Delete(ItemKey(item.mem_id));
  }

  pthread_mutex_unlock(&mu_);
  return base::kOk;
} /*}}}*/

}  // namespace book_agent
