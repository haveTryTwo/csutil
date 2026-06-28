// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/file_service_provider.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "base/log.h"

namespace base {

static const char kEndpointsKeySuffix[] = ".endpoints";

/**
 * @brief 去除字符串首尾空白
 */
static std::string Trim(const std::string &s) { /*{{{*/
  uint32_t begin = 0;
  uint32_t end = (uint32_t)s.size();
  while (begin < end && (s[begin] == ' ' || s[begin] == '\t' || s[begin] == '\r' || s[begin] == '\n')) ++begin;
  while (end > begin && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) --end;
  return s.substr(begin, end - begin);
} /*}}}*/

/**
 * @brief 按分隔符切分字符串
 */
static void Split(const std::string &s, char sep, std::vector<std::string> *out) { /*{{{*/
  out->clear();
  std::string::size_type start = 0;
  while (true) {
    std::string::size_type pos = s.find(sep, start);
    if (pos == std::string::npos) {
      out->push_back(s.substr(start));
      break;
    }
    out->push_back(s.substr(start, pos - start));
    start = pos + 1;
  }
} /*}}}*/

Code ParseEndpoints(const std::string &spec, std::vector<Endpoint> *out) { /*{{{*/
  if (out == NULL) return kInvalidParam;
  out->clear();

  std::vector<std::string> items;
  Split(spec, ',', &items);

  for (uint32_t i = 0; i < (uint32_t)items.size(); ++i) {
    std::string item = Trim(items[i]);
    if (item.empty()) continue;

    std::vector<std::string> parts;
    Split(item, ':', &parts);
    if (parts.size() < 2) continue;  // 至少 ip:port

    Endpoint ep;
    ep.ip = Trim(parts[0]);
    ep.port = (uint16_t)atoi(Trim(parts[1]).c_str());
    ep.weight = (parts.size() >= 3) ? (uint32_t)atoi(Trim(parts[2]).c_str()) : 1;
    if (ep.weight == 0) ep.weight = 1;
    if (ep.ip.empty() || ep.port == 0) continue;

    out->push_back(ep);
  }

  if (out->empty()) return kNotFound;
  return kOk;
} /*}}}*/

FileServiceProvider::FileServiceProvider(const std::string &conf_path)
    : conf_path_(conf_path),
      mu_(),
      last_mtime_(0),
      watchers_(),
      thread_(0),
      running_(false),
      poll_interval_ms_(0) { /*{{{*/
} /*}}}*/

FileServiceProvider::~FileServiceProvider() { /*{{{*/
  Stop();
} /*}}}*/

Code FileServiceProvider::Resolve(const std::string &service_name, std::vector<Endpoint> *out) { /*{{{*/
  if (out == NULL) return kInvalidParam;

  Config conf;
  Code ret = conf.LoadFile(conf_path_);
  if (ret != kOk) return ret;

  MutexLock ml(&mu_);
  return ResolveFromConfig(&conf, service_name, out);
} /*}}}*/

Code FileServiceProvider::ResolveFromConfig(Config *conf, const std::string &service_name,
                                            std::vector<Endpoint> *out) { /*{{{*/
  std::string key = service_name + kEndpointsKeySuffix;
  std::string spec;
  Code ret = conf->GetValue(key, &spec);
  if (ret != kOk) return kNotFound;

  return ParseEndpoints(spec, out);
} /*}}}*/

Code FileServiceProvider::Watch(const std::string &service_name, OnEndpointsChanged cb, void *ctx) { /*{{{*/
  if (cb == NULL) return kInvalidParam;

  MutexLock ml(&mu_);
  Watcher w;
  w.service_name = service_name;
  w.cb = cb;
  w.ctx = ctx;
  watchers_.push_back(w);
  return kOk;
} /*}}}*/

Code FileServiceProvider::GetFileMtime(uint64_t *mtime) { /*{{{*/
  struct stat st;
  if (stat(conf_path_.c_str(), &st) != 0) return kStatFailed;

  *mtime = (uint64_t)st.st_mtime;
  return kOk;
} /*}}}*/

Code FileServiceProvider::CheckAndReload(bool force) { /*{{{*/
  uint64_t mtime = 0;
  Code ret = GetFileMtime(&mtime);
  if (ret != kOk) return ret;

  // 收集需要回调的 watcher 快照与最新实例，回调放到锁外，避免回调内再进 ServiceManager 形成锁序风险
  std::vector<Watcher> snapshot;
  {
    MutexLock ml(&mu_);
    if (!force && mtime == last_mtime_) return kOk;
    last_mtime_ = mtime;
    snapshot = watchers_;
  }

  Config conf;
  ret = conf.LoadFile(conf_path_);
  if (ret != kOk) {
    LOG_ERR("reload conf failed, path:%s, ret:%d\n", conf_path_.c_str(), ret);
    return ret;
  }

  for (uint32_t i = 0; i < (uint32_t)snapshot.size(); ++i) {
    std::vector<Endpoint> eps;
    Code r = kOk;
    {
      MutexLock ml(&mu_);
      r = ResolveFromConfig(&conf, snapshot[i].service_name, &eps);
    }
    if (r != kOk) {
      LOG_ERR("resolve service:%s failed on reload, ret:%d\n", snapshot[i].service_name.c_str(), r);
      continue;  // 解析失败保留上次列表，不回灌空列表
    }
    snapshot[i].cb(snapshot[i].service_name, eps, snapshot[i].ctx);
  }

  return kOk;
} /*}}}*/

Code FileServiceProvider::Start(uint32_t poll_interval_ms) { /*{{{*/
  if (poll_interval_ms == 0) return kInvalidParam;

  {
    MutexLock ml(&mu_);
    if (running_) return kOk;
    running_ = true;
    poll_interval_ms_ = poll_interval_ms;
  }

  int ret = pthread_create(&thread_, NULL, &FileServiceProvider::ThreadMain, this);
  if (ret != 0) {
    MutexLock ml(&mu_);
    running_ = false;
    return kPthreadCreateFailed;
  }
  return kOk;
} /*}}}*/

Code FileServiceProvider::Stop() { /*{{{*/
  bool need_join = false;
  {
    MutexLock ml(&mu_);
    if (running_) {
      running_ = false;
      need_join = true;
    }
  }
  if (need_join) pthread_join(thread_, NULL);
  return kOk;
} /*}}}*/

void *FileServiceProvider::ThreadMain(void *arg) { /*{{{*/
  FileServiceProvider *self = (FileServiceProvider *)arg;

  while (true) {
    {
      MutexLock ml(&self->mu_);
      if (!self->running_) break;
    }

    // 分小步睡眠，便于 Stop 快速生效
    uint32_t slept = 0;
    const uint32_t step_ms = 100;
    while (slept < self->poll_interval_ms_) {
      {
        MutexLock ml(&self->mu_);
        if (!self->running_) break;
      }
      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = step_ms * 1000000;  // 100ms，赋给系统结构体字段 tv_nsec
      nanosleep(&ts, NULL);
      slept += step_ms;
    }

    {
      MutexLock ml(&self->mu_);
      if (!self->running_) break;
    }
    self->CheckAndReload(false);
  }

  return NULL;
} /*}}}*/

}  // namespace base
