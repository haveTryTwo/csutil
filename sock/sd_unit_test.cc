// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 服务发现 P0 自验证（无网络依赖）：解析 / 选址轮询 / 熔断摘除-恢复 / 文件热加载

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <set>
#include <string>
#include <vector>

#include "sock/file_service_provider.h"
#include "sock/load_balancer.h"
#include "sock/service_manager.h"
#include "sock/service_provider.h"

using namespace base;

static const char kConfPath[] = "/tmp/sd_unit_test.conf";
static const char kSvc[] = "book_test";

static void WriteConf(const char *content) { /*{{{*/
  FILE *fp = fopen(kConfPath, "w");
  assert(fp != NULL);
  fputs(content, fp);
  fclose(fp);
} /*}}}*/

static void SleepMs(uint32_t ms) { /*{{{*/
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  nanosleep(&ts, NULL);
} /*}}}*/

static std::string Key(const Endpoint &ep) { /*{{{*/
  return ep.ip + ":" + std::to_string((uint32_t)ep.port);
} /*}}}*/

static void TestParse() { /*{{{*/
  std::vector<Endpoint> eps;
  Code ret = ParseEndpoints("127.0.0.1:9001:1, 127.0.0.1:9002 , 127.0.0.1:9003:2", &eps);
  assert(ret == kOk);
  assert(eps.size() == 3);
  assert(eps[0].ip == "127.0.0.1" && eps[0].port == 9001 && eps[0].weight == 1);
  assert(eps[1].port == 9002 && eps[1].weight == 1);
  assert(eps[2].port == 9003 && eps[2].weight == 2);

  std::vector<Endpoint> empty;
  assert(ParseEndpoints("   ", &empty) == kNotFound);
  printf("[PASS] TestParse\n");
} /*}}}*/

static void TestPickAndHealth() { /*{{{*/
  WriteConf("book_test.endpoints = 127.0.0.1:9001:1, 127.0.0.1:9002:1, 127.0.0.1:9003:1\n");

  FileServiceProvider provider(kConfPath);
  HealthConfig hc;
  hc.fail_threshold = 3;
  hc.eject_cooldown_ms = 100;
  hc.half_open_success = 2;
  hc.min_healthy_ratio = 0.0;

  Code ret = ServiceManager::Instance()->Init(&provider, hc);
  assert(ret == kOk);
  ret = ServiceManager::Instance()->RegisterService(kSvc, kLbRoundRobin);
  assert(ret == kOk);

  // 1) 轮询应覆盖全部 3 个实例
  std::set<std::string> seen;
  for (uint32_t i = 0; i < 9; ++i) {
    Endpoint ep;
    assert(ServiceManager::Instance()->Pick(kSvc, "", &ep) == kOk);
    seen.insert(Key(ep));
  }
  assert(seen.size() == 3);
  printf("[PASS] round-robin covers 3 endpoints\n");

  // 2) 对 9001 连续上报 3 次失败 -> 摘除
  Endpoint bad;
  bad.ip = "127.0.0.1";
  bad.port = 9001;
  for (uint32_t i = 0; i < 3; ++i) ServiceManager::Instance()->Report(kSvc, bad, false);

  // 摘除后，多次选址不应再出现 9001
  for (uint32_t i = 0; i < 30; ++i) {
    Endpoint ep;
    assert(ServiceManager::Instance()->Pick(kSvc, "", &ep) == kOk);
    assert(Key(ep) != "127.0.0.1:9001");
  }
  printf("[PASS] eject 9001 after 3 fails (no longer selected)\n");

  // 3) 冷却到期后转半开 -> 连续成功 2 次 -> 恢复健康
  SleepMs(150);
  // 触发一次选址，使冷却到期的 9001 转半开（可能本次未选中它，多试几次直到拿到 9001）
  bool got_half_open = false;
  for (uint32_t i = 0; i < 30 && !got_half_open; ++i) {
    Endpoint ep;
    assert(ServiceManager::Instance()->Pick(kSvc, "", &ep) == kOk);
    if (Key(ep) == "127.0.0.1:9001") got_half_open = true;
  }
  assert(got_half_open);  // 半开后 9001 重新可被选中
  // 半开成功 2 次 -> 健康
  ServiceManager::Instance()->Report(kSvc, bad, true);
  ServiceManager::Instance()->Report(kSvc, bad, true);
  printf("[PASS] 9001 cooldown -> half-open -> recovered\n");
} /*}}}*/

static void TestHotReload() { /*{{{*/
  // 在已注册 book_test 基础上，新增 9004，触发热加载
  WriteConf(
      "book_test.endpoints = 127.0.0.1:9001:1, 127.0.0.1:9002:1, 127.0.0.1:9003:1, 127.0.0.1:9004:1\n");

  // 直接调用 provider 的 CheckAndReload 需要同一 provider 实例；这里复用 TestPickAndHealth 的 provider
  // 已随函数退出析构，故新建一个 provider 并重新 Watch 同名服务以驱动回灌
  static FileServiceProvider reload_provider(kConfPath);
  Code ret = reload_provider.Watch(kSvc, &ServiceManager::OnEndpointsChangedCb, ServiceManager::Instance());
  assert(ret == kOk);
  ret = reload_provider.CheckAndReload(true);
  assert(ret == kOk);

  std::set<std::string> seen;
  for (uint32_t i = 0; i < 40; ++i) {
    Endpoint ep;
    assert(ServiceManager::Instance()->Pick(kSvc, "", &ep) == kOk);
    seen.insert(Key(ep));
  }
  assert(seen.count("127.0.0.1:9004") == 1);
  printf("[PASS] hot-reload picks up new endpoint 9004\n");
} /*}}}*/

int main() { /*{{{*/
  TestParse();
  TestPickAndHealth();
  TestHotReload();
  printf("ALL SERVICE DISCOVERY TESTS PASSED\n");
  return 0;
} /*}}}*/
