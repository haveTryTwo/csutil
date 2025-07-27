// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_TCP_SERVER_H_
#define SOCK_TCP_SERVER_H_

#include <stdint.h>

#include <deque>
#include <map>
#include <string>

#include "base/config.h"
#include "base/event_loop.h"
#include "base/load_ctrl.h"
#include "base/log.h"
#include "base/mutex.h"
#include "base/statistic.h"
#include "base/status.h"
#include "sock/base_server.h"
#include "sock/tcp_proto.h"

namespace base {

const char kRealWorkerThreadsNumKey[] = "real_worker_thread_num";
const char kConnWorkerThreadsNumKey[] = "conn_worker_thread_num";

const int kDefaultRealWorkersNum = 50;
const int kDefaultConnWorkersNum = 10;

/**
 * This function is for being used when reading from client socket.
 * Users need to define the real action.
 *
 * @param in: binary string that getting from client socket
 * @param out: binary string that would return to client socket
 */
typedef Code (*RpcAction)(const std::string &in, std::string *out);

/**
 * Just return input data
 *
 * @param in: binary string that getting from client socket
 * @param out: binary string that would return to client socket
 */
Code DefaultRpcAction(const std::string &in, std::string *out);

class RpcServer;
class ConnWorker;
class RealWorker;

struct TcpConn { /*{{{*/
  std::string content;
  uint64_t id;
  int fd;

  TcpConn() : id(0), fd(0) {}
}; /*}}}*/

struct OneDataBlock { /*{{{*/
  std::string real_data;
  uint64_t id;
  int fd;
  ConnWorker *conn_worker;

  OneDataBlock() : id(0), fd(0), conn_worker(NULL) {}
}; /*}}}*/

class RealWorker { /*{{{*/
 public:
  RealWorker(RpcServer *server);
  ~RealWorker();

 public:
  Code Init();
  Code Run();

  Code AddOneDataBlockAndNotify(const OneDataBlock &one_data_block);
  Code NotifyEventInternalAction(int fd);

  Code DealWithRequestOneDataBlock(const OneDataBlock &one_data_block);

 private:
  std::deque<OneDataBlock> request_data_blocks_;
  pthread_t worker_id_;

  RpcServer *server_;

  int notify_fds_[2];
  EventType event_type_;
  EventLoop *worker_loop_;

  Mutex mu_;

  LoadCtrl flow_ctrl_;

  DataProtoFunc data_proto_func_;  // NOTE:htt, get data from tcp

 private:
  RealWorker(const RealWorker &w);
  RealWorker &operator=(const RealWorker &w);
}; /*}}}*/

class ConnWorker { /*{{{*/
 public:
  ConnWorker(RpcServer *server);
  ~ConnWorker();

 public:
  Code Init();
  Code Run();

  Code AddClientFdAndNotify(int fd);
  Code NotifyEventInternalAction(int fd);
  Code RespDataNotifyEventInternalAction(int fd);
  Code ClientEventInternalAction(int fd, int evt);

  Code SendRequestToRealWorker(const std::string &content, int real_len, int fd, uint64_t id);
  Code AddResponseAndNotify(const OneDataBlock &one_data_block);

  Code DealWithRespOneDataBlock(const OneDataBlock &one_data_block);

 public:
  uint64_t GeneraterId();

 private:
  Code CloseConn(const TcpConn &conn);

 private:
  std::deque<int> cli_fds_;
  std::map<int, TcpConn> conns_;
  std::deque<OneDataBlock> resp_data_blocks_;
  pthread_t worker_id_;

  RpcServer *server_;

  int notify_fds_[2];
  int data_notify_fds_[2];
  EventType event_type_;
  EventLoop *worker_loop_;

  Mutex mu_;
  Mutex data_mu_;

  LoadCtrl flow_ctrl_;

  DataProtoFunc data_proto_func_;  // NOTE:htt, get data from tcp
  uint64_t unique_id_ = 0;

 private:
  ConnWorker(const ConnWorker &w);
  ConnWorker &operator=(const ConnWorker &w);
}; /*}}}*/

class RpcServer : public BaseServer { /*{{{*/
 public:
  RpcServer(const Config &conf, DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
            FormatUserDataFunc format_user_data_func, RpcAction action);
  ~RpcServer();

 public:
  Code Init();
  Code Run();

  Code DumpStatAction();
  Code AcceptEventInternalAction(int fd, int evt);

  DataProtoFunc GetDataProtoFunc() { return data_proto_func_; }

 private:
  RpcServer(const RpcServer &);
  RpcServer &operator=(const RpcServer &);

 private:
  Config conf_;
  uint16_t port_;
  DataProtoFunc data_proto_func_;
  GetUserDataFunc get_user_data_func_;
  FormatUserDataFunc format_user_data_func_;
  RpcAction action_;
  int serv_fd_;

  bool is_running_;
  int real_workers_num_;
  std::deque<RealWorker *> real_workers_;

  int conn_workers_num_;
  std::deque<ConnWorker *> conn_workers_;

  EventType event_type_;
  EventLoop *main_loop_;

  int max_flow_;

  Statistic *stat_;
  int stat_dump_circle_;
  pthread_t stat_id_;

 private:
  friend class RealWorker;
  friend class ConnWorker;
}; /*}}}*/

};  // namespace base

#endif
