// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_FILE_SERVICE_PROVIDER_H_
#define SOCK_FILE_SERVICE_PROVIDER_H_

#include <pthread.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/config.h"
#include "base/mutex.h"
#include "base/status.h"
#include "sock/service_provider.h"

namespace base {

/**
 * @brief 基于本地配置文件的服务发现数据源（P0）
 *
 * 配置约定：每个服务一行实例列表键 "<service>.endpoints = ip:port[:weight], ..."。
 * 通过后台线程周期性比对文件 mtime，变化即热加载并回灌全量实例列表，支持"新增 IP 无需重启"。
 * 线程安全；后台线程可选（不 Start 时退化为仅启动期 Resolve）。
 */
class FileServiceProvider : public ServiceProvider { /*{{{*/
 public:
  /**
   * @brief 构造
   * @param conf_path 实例列表配置文件路径
   */
  explicit FileServiceProvider(const std::string &conf_path);
  virtual ~FileServiceProvider();

  virtual Code Resolve(const std::string &service_name, std::vector<Endpoint> *out);
  virtual Code Watch(const std::string &service_name, OnEndpointsChanged cb, void *ctx);

  /**
   * @brief 启动后台热加载线程
   * @param poll_interval_ms 文件 mtime 轮询间隔（毫秒），须 > 0
   * @return kOk 成功；kInvalidParam 间隔为 0；kPthreadCreateFailed 线程创建失败
   */
  Code Start(uint32_t poll_interval_ms);

  /**
   * @brief 停止后台热加载线程（阻塞至线程退出）
   * @return kOk 成功
   */
  Code Stop();

  /**
   * @brief 检查文件是否变化并按需重载、回灌（公开以便测试驱动）
   * @param force true 时无视 mtime 强制重载并回调
   * @return kOk 成功（含"无变化"）；kStatFailed 文件状态读取失败
   */
  Code CheckAndReload(bool force);

 private:
  struct Watcher { /*{{{*/
    std::string service_name;
    OnEndpointsChanged cb;
    void *ctx;
    Watcher() : cb(NULL), ctx(NULL) {}
  }; /*}}}*/

  FileServiceProvider(const FileServiceProvider &);
  FileServiceProvider &operator=(const FileServiceProvider &);

  /**
   * @brief 从已加载的 Config 中解析某服务实例列表（调用方持有 mu_）
   */
  Code ResolveFromConfig(Config *conf, const std::string &service_name, std::vector<Endpoint> *out);

  /**
   * @brief 读取文件 mtime（秒）
   * @return kOk 成功并写入 mtime；kStatFailed 失败
   */
  Code GetFileMtime(uint64_t *mtime);

  static void *ThreadMain(void *arg);

 private:
  std::string conf_path_;
  Mutex mu_;
  uint64_t last_mtime_;
  std::vector<Watcher> watchers_;

  pthread_t thread_;
  bool running_;
  uint32_t poll_interval_ms_;
}; /*}}}*/

}  // namespace base
#endif
