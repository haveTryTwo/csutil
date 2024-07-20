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
#include "base/mutex.h"
#include "base/statistic.h"
#include "base/status.h"
#include "sock/base_server.h"
#include "sock/tcp_proto.h"

namespace base {

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

struct TcpConn { /*{{{*/
  std::string content;
  int left_count;

  int fd;
}; /*}}}*/

class TcpServer;

class ConnWorker { /*{{{*/
 public:
  ConnWorker(TcpServer *server);
  ~ConnWorker();

 public:
  Code Init();
  Code Run();

  Code AddClientFdAndNotify(int fd);
  Code NotifyEventInternalAction(int fd);
  Code ClientEventInternalAction(int fd, int evt);

 private:
  Code CloseConn(TcpConn *conn);

 private:
  std::deque<int> cli_fds_;
  std::map<int, TcpConn *> conns_;
  pthread_t worker_id_;

  TcpServer *server_;

  int notify_fds_[2];
  EventType event_type_;
  EventLoop *worker_loop_;

  Mutex mu_;

  LoadCtrl flow_ctrl_;

 private:
  ConnWorker(const ConnWorker &w);
  ConnWorker &operator=(const ConnWorker &w);
}; /*}}}*/

class TcpServer : public BaseServer { /*{{{*/
 public:
  TcpServer(const Config &conf, DataProtoFunc data_proto_func, RpcAction action);
  ~TcpServer();

 public:
  Code Init();
  Code Run();

  Code DumpStatAction();
  Code AcceptEventInternalAction(int fd, int evt);

 private:
  TcpServer(const TcpServer &);
  TcpServer &operator=(const TcpServer &);

 private:
  Config conf_;
  uint16_t port_;
  DataProtoFunc data_proto_func_;
  RpcAction action_;
  int serv_fd_;

  bool is_running_;
  int workers_num_;
  std::deque<ConnWorker *> workers_;

  EventType event_type_;
  EventLoop *main_loop_;

  int max_flow_;

  Statistic *stat_;
  int stat_dump_circle_;
  pthread_t stat_id_;

 private:
  friend class ConnWorker;
}; /*}}}*/

};  // namespace base

#endif
