// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/tcp_server.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "base/coding.h"
#include "base/daemon.h"
#include "base/ip.h"
#include "base/log.h"
#include "base/msg.h"
#include "base/util.h"

#include "cs/server.h"
#include "cs/version.h"

namespace base {

static void *WorkerThreadAction(void *param);
static Code NotifyEventAction(int fd, int evt, void *param);
static Code ClientEventAction(int fd, int evt, void *param);
static Code AcceptEventAction(int fd, int evt, void *param);

Code DefaultRpcAction(const std::string &in, std::string *out) { /*{{{*/
  if (out == NULL) return kInvalidParam;
  out->assign(in);
  return kOk;
} /*}}}*/

static void *WorkerThreadAction(void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  worker->Run();

  pthread_exit(NULL);
}

static void *StatThreadAction(void *param) {
  TcpServer *serv = reinterpret_cast<TcpServer *>(param);
  serv->DumpStatAction();

  pthread_exit(NULL);
}

static Code NotifyEventAction(int fd, int evt, void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  Code ret = worker->NotifyEventInternalAction(fd);
  return ret;
}

static Code ClientEventAction(int fd, int evt, void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  Code ret = worker->ClientEventInternalAction(fd, evt);
  return ret;
}

static Code AcceptEventAction(int fd, int evt, void *param) {
  TcpServer *server = reinterpret_cast<TcpServer *>(param);
  Code ret = server->AcceptEventInternalAction(fd, evt);
  return ret;
}

ConnWorker::ConnWorker(TcpServer *server)
    : server_(server),
      event_type_(server_->event_type_),
      worker_loop_(NULL),
      mu_(),
      flow_ctrl_(kDefaultFlowGridNum, kDefaultFlowUnitNum, server_->max_flow_) { /*{{{*/
} /*}}}*/

ConnWorker::~ConnWorker() { /*{{{*/
  std::map<int, TcpConn *>::iterator it = conns_.begin();
  while (it != conns_.end()) {
    CloseConn(it->second);
    it = conns_.begin();
  }

  if (worker_loop_ != NULL) {
    delete worker_loop_;
    worker_loop_ = NULL;
  }
} /*}}}*/

Code ConnWorker::Init() { /*{{{*/
  int ret = pipe(notify_fds_);
  if (ret != 0) return kPipeFailed;

  Code r = SetFdNonblock(notify_fds_[0]);
  if (r != kOk) return r;
  r = SetFdNonblock(notify_fds_[1]);
  if (r != kOk) return r;

  r = flow_ctrl_.Init();
  if (r != kOk) return r;

  worker_loop_ = new EventLoop();
  worker_loop_->Init(event_type_);
  worker_loop_->Add(notify_fds_[0], EV_IN, NotifyEventAction, this);

  pthread_create(&worker_id_, NULL, WorkerThreadAction, this);
  return kOk;
} /*}}}*/

Code ConnWorker::Run() { /*{{{*/
  Code ret = worker_loop_->Run();
  return ret;
} /*}}}*/

Code ConnWorker::AddClientFdAndNotify(int fd) { /*{{{*/
  MutexLock ml(&mu_);
  cli_fds_.push_back(fd);

  char buf[1] = {'1'};
  int ret = write(notify_fds_[1], buf, sizeof(buf));
  if (ret == -1) return kWriteError;

  return kOk;
} /*}}}*/

Code ConnWorker::NotifyEventInternalAction(int fd) { /*{{{*/
  char buf[1];
  assert(fd == notify_fds_[0]);
  int ret = read(notify_fds_[0], buf, sizeof(buf));
  if (ret == -1) return kReadError;

  MutexLock ml(&mu_);

  std::deque<int>::iterator it = cli_fds_.begin();
  while (it != cli_fds_.end()) {
    int client_fd = *it;
    TcpConn *conn = new TcpConn;
    conn->left_count = 0;
    conn->fd = client_fd;
    conns_.insert(std::pair<int, TcpConn *>(client_fd, conn));

    worker_loop_->Add(client_fd, EV_IN, ClientEventAction, this);

    cli_fds_.pop_front();
    it = cli_fds_.begin();
  }

  return kOk;
} /*}}}*/

Code ConnWorker::ClientEventInternalAction(int fd, int evt) { /*{{{*/
  std::map<int, TcpConn *>::iterator it = conns_.find(fd);
  if (it == conns_.end()) return kNotFound;

  TcpConn *conn = it->second;
  assert(fd == conn->fd);

  char cmd_buf[kHeadLen];
  char read_buf[kBufLen];

  // TODO:htt, read data and execute

  return kOk;
} /*}}}*/

Code ConnWorker::CloseConn(TcpConn *conn) { /*{{{*/
  close(conn->fd);
  conns_.erase(conn->fd);
  Code ret = worker_loop_->Del(conn->fd);
  delete conn;

  return ret;
} /*}}}*/

TcpServer::TcpServer(const Config &conf, DataProtoFunc data_proto_func, RpcAction action)
    : conf_(conf), data_proto_func_(data_proto_func), action_(action) { /*{{{*/
  int port = 0;
  int ret = conf_.GetInt32Value(kPortKey, &port);
  if (ret != kOk) port = kDefaultPort;
  port_ = port;

  serv_fd_ = -1;
  is_running_ = false;

  ret = conf_.GetInt32Value(kThreadsNumKey, &workers_num_);
  if (ret != kOk) workers_num_ = kDefaultWorkersNum;

  event_type_ = kPoll;
#if defined(__linux__)
  event_type_ = kEPoll;
#endif
  main_loop_ = NULL;

  ret = conf_.GetInt32Value(kFlowRestrictKey, &max_flow_);
  if (ret != kOk) max_flow_ = kMaxFlowRestrict;

  std::string stat_path;
  ret = conf_.GetValue(kStatPathKey, &stat_path);
  if (ret != kOk) stat_path = kDefaultStatPath;

  int stat_file_size;
  ret = conf_.GetInt32Value(kStatFileSizeKey, &stat_file_size);
  if (ret != kOk) stat_file_size = kDefaultStatFileSize;

  stat_ = new Statistic(stat_path, stat_file_size);

  ret = conf_.GetInt32Value(kStatDumpCirclekey, &stat_dump_circle_);
  if (ret != kOk) stat_dump_circle_ = kDefaultStatDumpCircle;
} /*}}}*/

TcpServer::~TcpServer() { /*{{{*/
  is_running_ = false;
  std::deque<ConnWorker *>::iterator it = workers_.begin();
  while (it != workers_.end()) {
    delete (*it);
    it = workers_.begin();
  }

  if (main_loop_ != NULL) {
    delete main_loop_;
    main_loop_ = NULL;
  }

  if (serv_fd_ != -1) {
    close(serv_fd_);
    serv_fd_ = -1;
  }
} /*}}}*/

Code TcpServer::Init() { /*{{{*/
  if (action_ == NULL) action_ = DefaultRpcAction;

  serv_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_fd_ == -1) return kSocketError;

  Code r = SetFdReused(serv_fd_);
  if (r != kOk) return r;
  r = SetFdNonblock(serv_fd_);
  if (r != kOk) return r;

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int ret = bind(serv_fd_, (struct sockaddr *)(&serv_addr), sizeof(serv_addr));
  if (ret == -1) return kBindError;

  ret = listen(serv_fd_, kDefaultBacklog);

  main_loop_ = new EventLoop();
  main_loop_->Init(event_type_);
  main_loop_->Add(serv_fd_, EV_IN, AcceptEventAction, this);

  is_running_ = true;
  for (int i = 0; i < workers_num_; ++i) {
    ConnWorker *worker = new ConnWorker(this);
    Code r = worker->Init();
    assert(r == kOk);
    workers_.push_back(worker);
  }

  pthread_create(&stat_id_, NULL, StatThreadAction, this);

  srand(time(NULL));
  return kOk;
} /*}}}*/

Code TcpServer::Run() { /*{{{*/
  Code ret = main_loop_->Run();
  return ret;
} /*}}}*/

Code TcpServer::AcceptEventInternalAction(int fd, int evt) { /*{{{*/
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(fd, (struct sockaddr *)(&client_addr), &client_addr_len);
  if (client_fd == -1) return kAcceptError;

  Code r = SetFdReused(client_fd);
  if (r != kOk) return r;
  r = SetFdNonblock(client_fd);
  if (r != kOk) return r;

  assert(workers_.size() > 0);
  int n = rand() % (workers_.size());
  ConnWorker *worker = workers_[n];
  r = worker->AddClientFdAndNotify(client_fd);

  return r;
} /*}}}*/

Code TcpServer::DumpStatAction() { /*{{{*/
  while (true) {
    sleep(stat_dump_circle_);
    stat_->DumpStat();
  }

  return kOk;
} /*}}}*/

}  // namespace base

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string conf_path = "./conf/server.conf";
  if (argc == 1) {
    fprintf(stderr, "config path using default: %s\n\n", conf_path.c_str());
  } else {
    int32_t opt = 0;
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
      switch (opt) {
        case 's':
          conf_path = optarg;
          break;
        case 'h':
          Help(argv[0]);
          return 0;
        default:
          fprintf(stderr, "Not right options\n");
          Help(argv[0]);
          return -1;
      }
    }
  }

  Config config;
  Code ret = config.LoadFile(conf_path);
  if (ret != kOk) {
    fprintf(stderr, "Failed to load conf:%d, then use no config\n", ret);
  }

  int is_daemon = 0;
  ret = config.GetInt32Value(kDaemonKey, &is_daemon);
  if (ret != kOk) {
    fprintf(stderr, "Failed to get daemon conf:%d, then use no daemon\n", ret);
  }

  if (is_daemon == 1) {
    fprintf(stderr, "Keep daemon\n");
    ret = DaemonAndKeepAlive();
    if (ret != kOk) return EXIT_FAILURE;
  } else {
    fprintf(stderr, "Keep not daemon\n");
  }

  TcpServer server(config, DefaultProtoFunc, DefaultRpcAction);
  ret = server.Init();
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
