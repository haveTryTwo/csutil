// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_DEMO_BOOK_LEVELDB_WRAPPER_H_
#define SOCK_DEMO_BOOK_LEVELDB_WRAPPER_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/status.h"

namespace leveldb {
class DB;
}  // namespace leveldb

namespace book_mgr {

/**
 * @brief LevelDB 静态库的轻量封装
 *
 * 将 leveldb::DB 的 Open/Put/Get/Delete 接口封装为返回 base::Code 的形式，
 * 屏蔽 leveldb::Status，统一项目的错误码风格。DB 实例进程内单例，
 * leveldb 本身支持多线程并发访问，无需额外加锁。
 */
class LevelDbWrapper { /*{{{*/
 public:
  LevelDbWrapper();
  ~LevelDbWrapper();

  /**
   * @brief 打开（不存在则创建）指定路径的 LevelDB 数据库
   * @param db_path 数据库目录路径
   * @return kOk 成功；kInvalidParam 路径为空；kOpenError 打开失败
   */
  base::Code Init(const std::string &db_path);

  /**
   * @brief 写入一条 key-value，已存在则覆盖
   * @param key 键
   * @param value 值（可为序列化后的字节串）
   * @return kOk 成功；kNotInit 未初始化；kWriteError 写入失败
   */
  base::Code Put(const std::string &key, const std::string &value);

  /**
   * @brief 读取指定 key 的值
   * @param key 键
   * @param value 输出参数，读取到的值
   * @return kOk 成功；kNotInit 未初始化；kNotFound 键不存在；kReadError 读取失败
   */
  base::Code Get(const std::string &key, std::string *value);

  /**
   * @brief 删除指定 key（key 不存在也返回成功）
   * @param key 键
   * @return kOk 成功；kNotInit 未初始化；kWriteError 删除失败
   */
  base::Code Delete(const std::string &key);

  /**
   * @brief 按 key 字典序范围扫描（游标分页 + 前缀过滤），单次最多返回 limit 条
   * @param prefix key 前缀过滤，空表示不过滤
   * @param start_after 分页游标，从该 key 之后开始（不含），空表示从头
   * @param limit 单页最大条数，须 > 0
   * @param keys 输出：本页命中的 key 列表
   * @param values 输出：与 keys 一一对应的 value 列表
   * @param next_cursor 输出：满页时为本页最后一个 key（下一页游标），否则为空
   * @return kOk 成功；kNotInit 未初始化；kInvalidParam 参数非法
   */
  base::Code Scan(const std::string &prefix, const std::string &start_after, uint32_t limit,
                  std::vector<std::string> *keys, std::vector<std::string> *values, std::string *next_cursor);

 private:
  LevelDbWrapper(const LevelDbWrapper &w);
  LevelDbWrapper &operator=(const LevelDbWrapper &w);

 private:
  leveldb::DB *db_;
}; /*}}}*/

}  // namespace book_mgr

#endif
