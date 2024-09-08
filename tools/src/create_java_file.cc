// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "base/common.h"
#include "base/file_util.h"
#include "base/util.h"

#include "create_java_file.h"

namespace tools {

const std::string kGradle("gradle");
const std::string kBuildGradle("build.gradle");
const std::string kSettingsGradle("settings.gradle");
const std::string kDistribution("distribution");
const std::string kJacocoGradle("jacoco.gradle");
const std::string kJavaSrcFile("App.java");
const std::string kJavaTestFile("AppTest.java");

base::Code CreateJavaFile(const std::string &project_name, const std::string &package,
                          const std::string &modules) { /*{{{*/
  if (project_name.empty()) return base::kInvalidParam;
  std::string pkg;
  if (package.empty()) {
    pkg = "org.havetrytwo.demo";
  } else {
    pkg = package;
  }

  base::Code ret = base::kOk;
  std::deque<std::string> pkg_2_dirs;
  ret = base::Strtok(pkg, '.', &pkg_2_dirs);
  if (ret != base::kOk) return ret;

  std::deque<std::string> exclude_names;
  exclude_names.push_back(kGradle);
  exclude_names.push_back(kDistribution);

  std::deque<std::string> modules_name;
  if (modules.size() == 0) {
    modules_name.push_back("client");
    modules_name.push_back("server");
  } else {
    ret = base::Strtok(modules, ',', &modules_name);
    if (ret != base::kOk) return ret;
  }

  std::deque<std::string>::iterator it = modules_name.begin();
  for (; it != modules_name.end(); ++it) {
    std::deque<std::string>::iterator exclude_it = exclude_names.begin();
    for (; exclude_it != exclude_names.end(); ++exclude_it) {
      if (*exclude_it == *it) {
        fprintf(stderr, "Invalid module name:%s\n", it->c_str());
        return base::kInvalidParam;
      }
    }
  }

  // 创建根项目
  ret = base::CreateDir(project_name);
  if (ret != base::kOk) return ret;

  // 创建根项目的 build.gradle
  char buf[base::kBufLen] = {0};
  int r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), kBuildGradleStr.c_str(), pkg.c_str());
  if (r < 0) return base::kInvalidParam;

  ret = base::CompareAndWriteWholeFile(project_name + "/" + kBuildGradle, buf);
  if (ret != base::kOk) return ret;

  // 创建根项目的 settings.gradle
  memset(buf, 0, sizeof(buf) / sizeof(buf[0]));
  int sum = 0;
  r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), "include ");
  if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;
  sum += r;
  for (it = modules_name.begin(); it != modules_name.end(); ++it) {
    r = snprintf(buf + sum, sizeof(buf) / sizeof(buf[0]) - sum, "'%s', ", it->c_str());
    if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;
    sum += r;
  }
  r = snprintf(buf + sum, sizeof(buf) / sizeof(buf[0]) - sum, "'%s'\n", kDistribution.c_str());
  if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;

  ret = base::CompareAndWriteWholeFile(project_name + "/" + kSettingsGradle, buf);
  if (ret != base::kOk) return ret;

  // 创建jacoco.gradle，用于获取测试覆盖率
  ret = base::CreateDir(project_name + "/" + kGradle);
  if (ret != base::kOk) return ret;
  ret = base::CompareAndWriteWholeFile(project_name + "/" + kGradle + "/" + kJacocoGradle, kJacocoStr);
  if (ret != base::kOk) return ret;

  // 生成 gradlew，并调整依赖gradle版本为4.5-all
  r = system("which gradle");
  if (r != 0) {
    fprintf(stderr, "No gradle, please check where it install, ret code:%d\n", r);
    return base::kInvalidParam;
  }

  r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), kGradleWrapper.c_str(), project_name.c_str());
  if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;
  r = system(buf);
  if (r != 0) {
    fprintf(stderr, "gradle wrapper failed, ret code:%d\n", r);
    return base::kInvalidParam;
  }

  // 创建distribution, 设置发布
  char buf1[base::kBufLen] = {0};
  sum = 0;
  for (it = modules_name.begin(); it != modules_name.end(); ++it) {
    r = snprintf(buf1 + sum, sizeof(buf1) / sizeof(buf1[0]) - sum, kDependsOnStr.c_str(), it->c_str());
    if (r < 0 || r >= sizeof(buf1) / sizeof(buf1[0])) return base::kInvalidParam;
    sum += r;
  }
  char buf2[base::kBufLen] = {0};
  sum = 0;
  for (it = modules_name.begin(); it != modules_name.end(); ++it) {
    r = snprintf(buf2 + sum, sizeof(buf2) / sizeof(buf2[0]) - sum, kFromStr.c_str(), it->c_str());
    if (r < 0 || r >= sizeof(buf2) / sizeof(buf2[0])) return base::kInvalidParam;
    sum += r;
  }
  r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), kDistributionGradleStr.c_str(), buf1, buf2);
  if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;

  ret = base::CreateDir(project_name + "/" + kDistribution);
  if (ret != base::kOk) return ret;
  ret = base::CompareAndWriteWholeFile(project_name + "/" + kDistribution + "/" + kBuildGradle, buf);
  if (ret != base::kOk) return ret;

  // 循环创建项目，包括目录，build.gradle, 源文件（App.java)，测试文件(AppTest.java)
  for (it = modules_name.begin(); it != modules_name.end(); ++it) { /*{{{*/
    std::deque<std::string> src_dir;
    std::deque<std::string> test_dir;

    src_dir.push_back(*it);
    src_dir.push_back("src");
    src_dir.push_back("main");
    src_dir.push_back("java");
    test_dir.push_back(*it);
    test_dir.push_back("src");
    test_dir.push_back("test");
    test_dir.push_back("java");

    std::deque<std::string>::iterator tmp_it;
    for (tmp_it = pkg_2_dirs.begin(); tmp_it != pkg_2_dirs.end(); ++tmp_it) {
      src_dir.push_back(*tmp_it);
      test_dir.push_back(*tmp_it);
    }

    std::string tmp_src_dir(project_name);
    for (tmp_it = src_dir.begin(); tmp_it != src_dir.end(); ++tmp_it) {
      tmp_src_dir.append("/").append(*tmp_it);
      ret = base::CreateDir(tmp_src_dir);
      if (ret != base::kOk) return ret;
    }

    std::string tmp_test_dir(project_name);
    for (tmp_it = test_dir.begin(); tmp_it != test_dir.end(); ++tmp_it) {
      tmp_test_dir.append("/").append(*tmp_it);
      ret = base::CreateDir(tmp_test_dir);
      if (ret != base::kOk) return ret;
    }

    ret = base::CompareAndWriteWholeFile(project_name + "/" + (*it) + "/" + kBuildGradle, kSubProjectBuildGradleStr);
    if (ret != base::kOk) return ret;

    r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), kJavaSrcStr.c_str(), pkg.c_str());
    if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;
    ret = base::CompareAndWriteWholeFile(tmp_src_dir + "/" + kJavaSrcFile, buf);
    if (ret != base::kOk) return ret;

    r = snprintf(buf, sizeof(buf) / sizeof(buf[0]), kJavaTestStr.c_str(), pkg.c_str());
    if (r < 0 || r >= sizeof(buf) / sizeof(buf[0])) return base::kInvalidParam;
    ret = base::CompareAndWriteWholeFile(tmp_test_dir + "/" + kJavaTestFile, buf);
    if (ret != base::kOk) return ret;
  } /*}}}*/

  return ret;
} /*}}}*/

}  // namespace tools
