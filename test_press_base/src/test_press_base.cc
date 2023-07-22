// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <deque>
#include <string>

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "base/common.h"
#include "base/log.h"
#include "base/mutex.h"
#include "base/util.h"

#include "test_press_base/include/test_press_base.h"
#include "test_press_base/include/test_press_controller.h"

namespace test {
const int kMaxLine = 30;

std::string g_choose_press_name;
int g_thread_num = 10;
int g_log_interval_ms = 1000;
std::string g_dst_ip_port_protos;

// If it is an integration test, then exit when there is an exception, so that the exception can be quickly located and processed;
// If it is a pressure test, it does not exit when there is an exception, but only counts the abnormal information, so that the success rate and failure rate during the pressure test can be obtained, which is used to evaluate the usability of the module under different pressures
int g_exit_if_failed = 1; // NOTE:htt, default is 1 so that exit when failing

base::Mutex g_mutex;
ResultInfo *g_thread_result_infos = NULL;
ResultInfo g_all_result_info;

PressObject::PressObject(const std::string &test_name)
    : test_name_(test_name), thread_index_(0) { /*{{{*/
  struct timeval tv;
  gettimeofday(&tv, NULL);
  r_seed_ = tv.tv_sec / 100 + tv.tv_usec;
} /*}}}*/

PressObject::~PressObject() { /*{{{*/
  std::map<std::string, std::vector<BusiClient *> >::iterator it = busi_clients_.begin();
  for (; it != busi_clients_.end(); ++it) {
    std::vector<BusiClient *>::iterator v_it = it->second.begin();
    for (; v_it != it->second.end(); ++v_it) {
      delete *v_it;
    }
  }
} /*}}}*/

base::Code PressObject::Init(const std::string &dst_ip_port_protos) { /*{{{*/
  dst_ip_port_protos_ = dst_ip_port_protos;

  std::deque<std::string> ip_port_protos;
  base::Code ret = base::Strtok(dst_ip_port_protos, base::kCommaChar, &ip_port_protos);
  if (ret != base::kOk) return ret;

  std::deque<std::string>::iterator it = ip_port_protos.begin();
  for (; it != ip_port_protos.end(); ++it) {
    if (it->empty()) continue;

    std::deque<std::string> ip_port_proto;
    ret = base::Strtok(*it, base::kColon, &ip_port_proto);
    if (ret != base::kOk) return ret;
    if (ip_port_proto.size() < 3 || ip_port_proto.size() > 4) return base::kInvalidParam;

    BusiClient *busi_client = NULL;
    ret = strategy::Singleton<test::TestPressController>::Instance()->GetNewBusiClient(
        ip_port_proto[2], &busi_client);
    if (ret != base::kOk) return ret;

    ret = busi_client->Init(*it);
    if (ret != base::kOk) return ret;

    busi_clients_[busi_client->GetName()].push_back(busi_client);
  }

  return base::kOk;
} /*}}}*/

base::Code PressObject::ExecBody() { /*{{{*/
  return base::kOk;
} /*}}}*/

base::Code PressObject::Exec(ResultInfo *res_info) { /*{{{*/
  if (res_info == NULL) return base::kInvalidParam;
  memset(res_info, 0, sizeof(ResultInfo));

  struct timeval cases_begin;
  struct timeval cases_end;
  gettimeofday(&cases_begin, NULL);

  base::Code ret = ExecBody();

  gettimeofday(&cases_end, NULL);
  int64_t diff_all_time = (cases_end.tv_sec - cases_begin.tv_sec) * base::kUnitConvOfMicrosconds +
                          (cases_end.tv_usec - cases_begin.tv_usec);

  res_info->total_num_ = 1;
  if (ret == base::kOk || ret == base::kExitOk) {  // NOTE:htt, The success of a single step or the
                                                   // final success is considered a success
    res_info->succ_num_ = 1;
  } else {
    res_info->fail_num_ = 1;
  }
  res_info->max_req_ms_ = diff_all_time / base::kThousand;

  res_info->total_time_us_ += diff_all_time;

  return ret;
} /*}}}*/

std::string PressObject::GetTestName() { /*{{{*/
  return test_name_;
} /*}}}*/

BusiClient *PressObject::GetBusiClient(const std::string &proto_name) { /*{{{*/
  std::map<std::string, std::vector<BusiClient *> >::iterator it = busi_clients_.find(proto_name);
  if (it == busi_clients_.end()) return NULL;

  if (it->second.size() == 0) return NULL;
  rand_r(&r_seed_);
  return it->second[r_seed_ % it->second.size()];
} /*}}}*/

base::Code PressObject::SetThreadIndex(int thread_index) { /*{{{*/
  thread_index_ = thread_index;
  return base::kOk;
} /*}}}*/

int PressObject::GetThreadIndex() { /*{{{*/
  return thread_index_;
} /*}}}*/

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "[-p dst_ip_port_protos] [-n thread_num] [-m log_interval_ms] [-c choose press name] [-x exit_if_failed]\n"
          "dst_ip_port_protos format: ip:port:proto,ip:port:proto...\n"
          "ex: ./press_test -p 127.0.0.1:80:http,127.0.0.1:9090:rpc -n 10 -m 1000 -x 0 -c PressHttp\n",
          program.c_str());
} /*}}}*/

void *PressFunc(void *param) { /*{{{*/
  int thread_index = static_cast<int>(reinterpret_cast<long>(param));
  PressObject *press_obj;
  base::Code ret = strategy::Singleton<test::TestPressController>::Instance()->GetNewPressObject(
      g_choose_press_name, &press_obj);
  if (ret != base::kOk) {
    LOG_ERR("Thread[%d] Failed to find press object:%s", thread_index, g_choose_press_name.c_str());
    pthread_exit(NULL);
  }

  ret = press_obj->Init(g_dst_ip_port_protos);
  if (ret != base::kOk) {
    LOG_ERR("Thread[%d] Failed to init:%s, ret:%d", thread_index, g_dst_ip_port_protos.c_str(),
            ret);
    pthread_exit(NULL);
  }
  ret = press_obj->SetThreadIndex(thread_index);
  if (ret != base::kOk) {
    LOG_ERR("Thread[%d] Failed to set thead_index, ret:%d", thread_index, ret);
    pthread_exit(NULL);
  }

  ResultInfo res_info;
  while (true) {
    memset(&res_info, 0, sizeof(ResultInfo));
    ret = press_obj->Exec(&res_info);
    if (ret != base::kOk) {
      if (ret == base::kExitOk) {
        LOG_INFO("Thread[%d] Successful to Exec, ret:%d", thread_index, ret);
        delete press_obj;
        press_obj = NULL;
        pthread_exit(NULL);
      }

      LOG_ERR("Thread[%d] Failed to Exec, ret:%d", thread_index, ret);

      if (g_exit_if_failed == 1) {
        delete press_obj;
        press_obj = NULL;
        pthread_exit(NULL);
      }
    }

    base::MutexLock mlock(&g_mutex);
    g_thread_result_infos[thread_index].total_num_ += res_info.total_num_;
    g_thread_result_infos[thread_index].succ_num_ += res_info.succ_num_;
    g_thread_result_infos[thread_index].fail_num_ += res_info.fail_num_;
    if (res_info.max_req_ms_ > g_thread_result_infos[thread_index].max_req_ms_) {
      g_thread_result_infos[thread_index].max_req_ms_ = res_info.max_req_ms_;
    }
    g_thread_result_infos[thread_index].total_time_us_ += res_info.total_time_us_;
  }

  delete press_obj;
  press_obj = NULL;
  pthread_exit(NULL);
} /*}}}*/

}  // namespace test

test::PressObject *MakeRegister(const std::string &test_name, test::PressObject *test_obj) { /*{{{*/
  base::Code ret =
      strategy::Singleton<test::TestPressController>::Instance()->RegisterObject(test_obj);
  assert(base::kOk == ret);
  return test_obj;
} /*}}}*/

test::BusiClient *MakeBusiClientRegister(const std::string &client_name,
                                         test::BusiClient *busi_client) { /*{{{*/
  base::Code ret =
      strategy::Singleton<test::TestPressController>::Instance()->RegisterClient(busi_client);
  assert(base::kOk == ret);
  return busi_client;
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace test;
  if (argc < 2) {
    Help(argv[0]);
    return -1;
  }

  int32_t opt = 0;
  while ((opt = getopt(argc, argv, "p:n:m:c:")) != -1) { /*{{{*/
    switch (opt) {
      case 'p':
        test::g_dst_ip_port_protos = optarg;
        break;
      case 'n':
        test::g_thread_num = atoi(optarg);
        break;
      case 'm':
        test::g_log_interval_ms = atoi(optarg);
        break;
      case 'c':
        test::g_choose_press_name = optarg;
        break;
      case 'x':
        test::g_exit_if_failed = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Not right options\n");
        Help(argv[0]);
        return -1;
    }
  } /*}}}*/

  fprintf(stderr, "dst_ip_port_protos:%s, thread_num:%d log_interval_ms:%d, choose press name:%s\n",
          test::g_dst_ip_port_protos.c_str(), test::g_thread_num, test::g_log_interval_ms,
          test::g_choose_press_name.c_str());

  SetLogLevel(base::kInfoLevel);

  test::g_thread_result_infos = new ResultInfo[test::g_thread_num];
  memset(test::g_thread_result_infos, 0, sizeof(ResultInfo) * test::g_thread_num);

  pthread_t *press_ths = new pthread_t[test::g_thread_num];
  for (int i = 0; i < test::g_thread_num; ++i) {
    int ret = pthread_create(press_ths + i, NULL, test::PressFunc, reinterpret_cast<void *>(i));
    assert(ret == 0);
  }

  int line_num = 0;
  while (true) { /*{{{*/
    struct timeval all_cases_begin;
    struct timeval all_cases_end;
    gettimeofday(&all_cases_begin, NULL);

    usleep(g_log_interval_ms * 1000);

    memset(&g_all_result_info, 0, sizeof(ResultInfo));
    for (int i = 0; i < test::g_thread_num; ++i) {
      base::MutexLock mlock(&g_mutex);
      g_all_result_info.total_num_ += g_thread_result_infos[i].total_num_;
      g_all_result_info.succ_num_ += g_thread_result_infos[i].succ_num_;
      g_all_result_info.fail_num_ += g_thread_result_infos[i].fail_num_;
      if (g_thread_result_infos[i].max_req_ms_ > g_all_result_info.max_req_ms_)
        g_all_result_info.max_req_ms_ += g_thread_result_infos[i].max_req_ms_;
      g_all_result_info.total_time_us_ += g_thread_result_infos[i].total_time_us_;

      memset(g_thread_result_infos + i, 0, sizeof(ResultInfo));
    }

    if (line_num % kMaxLine == 0) {
      fprintf(stderr,
              "SpendTime[ms]  TotalRequset    SuccessRequest  FailedRequest   MaxSpendTime[ms]  "
              "AvgSpendTime[ms]\n");
    }
    line_num++;

    gettimeofday(&all_cases_end, NULL);
    int64_t diff_all_time =
        (all_cases_end.tv_sec - all_cases_begin.tv_sec) * base::kUnitConvOfMicrosconds +
        (all_cases_end.tv_usec - all_cases_begin.tv_usec);

    double avg_time_ms = g_all_result_info.total_num_ == 0
                             ? 0
                             : (double)g_all_result_info.total_time_us_ /
                                   g_all_result_info.total_num_ / base::kThousand;
    fprintf(stderr, "%10lld  %10u  %15u  %15u  %15u %17f\n", diff_all_time / base::kThousand,
            g_all_result_info.total_num_, g_all_result_info.succ_num_, g_all_result_info.fail_num_,
            g_all_result_info.max_req_ms_, avg_time_ms);

    bool is_threads_alive = false;
    for (int i = 0; i < test::g_thread_num; ++i) {
      int ret = pthread_kill(press_ths[i], 0);
      if (ret == 0) {
        is_threads_alive = true;
        break;
      }
    }
    if (!is_threads_alive) break;  // NOTE: all threads are not alive
  }                                /*}}}*/

  delete[] test::g_thread_result_infos;
  delete[] press_ths;

  return 0;
} /*}}}*/
