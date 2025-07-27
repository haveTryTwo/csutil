// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_server.h"

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

static void *RealWorkerThreadAction(void *param);
static Code RealWorkerNotifyEventAction(int fd, int evt, void *param);

static void *ConnWorkerThreadAction(void *param);
static Code ConnWorkerNotifyEventAction(int fd, int evt, void *param);
static Code ConnWorkerRespDataNotifyEventAction(int fd, int evt, void *param);
static Code ClientEventAction(int fd, int evt, void *param);
static Code AcceptEventAction(int fd, int evt, void *param);

Code DefaultRpcAction(const std::string &in, std::string *out) { /*{{{*/
  if (out == NULL) return kInvalidParam;
  out->assign("default resp:");
  out->append(in);
  return kOk;
} /*}}}*/

static void *RealWorkerThreadAction(void *param) {
  RealWorker *worker = reinterpret_cast<RealWorker *>(param);
  worker->Run();

  pthread_exit(NULL);
}

static Code RealWorkerNotifyEventAction(int fd, int evt, void *param) {
  RealWorker *worker = reinterpret_cast<RealWorker *>(param);
  Code ret = worker->NotifyEventInternalAction(fd);
  return ret;
}

static void *ConnWorkerThreadAction(void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  worker->Run();

  pthread_exit(NULL);
}

static void *StatThreadAction(void *param) {
  RpcServer *serv = reinterpret_cast<RpcServer *>(param);
  serv->DumpStatAction();

  pthread_exit(NULL);
}

static Code ConnWorkerNotifyEventAction(int fd, int evt, void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  Code ret = worker->NotifyEventInternalAction(fd);
  return ret;
}

static Code ConnWorkerRespDataNotifyEventAction(int fd, int evt, void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  Code ret = worker->RespDataNotifyEventInternalAction(fd);
  return ret;
}

static Code ClientEventAction(int fd, int evt, void *param) {
  ConnWorker *worker = reinterpret_cast<ConnWorker *>(param);
  Code ret = worker->ClientEventInternalAction(fd, evt);
  return ret;
}

static Code AcceptEventAction(int fd, int evt, void *param) {
  RpcServer *server = reinterpret_cast<RpcServer *>(param);
  Code ret = server->AcceptEventInternalAction(fd, evt);
  return ret;
}

/****************************************
 * RealWorker: deal with request
 */
RealWorker::RealWorker(RpcServer *server)
    : server_(server),
      event_type_(server_->event_type_),
      worker_loop_(NULL),
      mu_(),
      flow_ctrl_(kDefaultFlowGridNum, kDefaultFlowUnitNum, server_->max_flow_),
      data_proto_func_(server->GetDataProtoFunc()) { /*{{{*/ } /*}}}*/

RealWorker::~RealWorker() { /*{{{*/
  if (worker_loop_ != NULL) {
    delete worker_loop_;
    worker_loop_ = NULL;
  }
} /*}}}*/

Code RealWorker::Init() { /*{{{*/
  int ret = pipe(notify_fds_);
  if (ret != 0) return kPipeFailed;

  Code r = SetFdNonblock(notify_fds_[0]);
  if (r != kOk) return r;
  r = SetFdNonblock(notify_fds_[1]);
  if (r != kOk) return r;

  r = flow_ctrl_.Init();
  if (r != kOk) return r;

  if (data_proto_func_ == NULL) return kInvalidParam;

  worker_loop_ = new EventLoop();
  worker_loop_->Init(event_type_);
  worker_loop_->Add(notify_fds_[0], EV_IN, RealWorkerNotifyEventAction, this);

  pthread_create(&worker_id_, NULL, RealWorkerThreadAction, this);
  return kOk;
} /*}}}*/

Code RealWorker::Run() { /*{{{*/
  Code ret = worker_loop_->Run();
  return ret;
} /*}}}*/

Code RealWorker::AddOneDataBlockAndNotify(const OneDataBlock &one_data_block) { /*{{{*/
  MutexLock ml(&mu_);
  request_data_blocks_.push_back(one_data_block);
  LOG_ERR("try to add request id:%lu fd:%d to real worker!", (unsigned long)one_data_block.id, one_data_block.fd);

  char buf[1] = {'1'};
  int ret = write(notify_fds_[1], buf, sizeof(buf));
  if (ret == -1) return kWriteError;

  return kOk;
} /*}}}*/

Code RealWorker::NotifyEventInternalAction(int fd) { /*{{{*/
  char buf[1];
  assert(fd == notify_fds_[0]);
  int r = read(notify_fds_[0], buf, sizeof(buf));
  if (r == -1 && errno == EAGAIN) return kOk;
  if (r == -1 && errno != EAGAIN) return kReadError;

  std::deque<OneDataBlock> tmp_data_blocks;
  {
    MutexLock ml(&mu_);
    tmp_data_blocks = std::move(request_data_blocks_);
  }

  std::deque<OneDataBlock>::iterator it = tmp_data_blocks.begin();
  while (it != tmp_data_blocks.end()) {
    Code ret = DealWithRequestOneDataBlock(*it);
    if (ret != kOk) {
      LOG_ERR("Failed to deal One Data Block! ret:%d real data:%s, id:%lu, fd:%d\n", ret, it->real_data.size(),
              (unsigned long)it->id, it->fd);
    }

    tmp_data_blocks.pop_front();
    it = tmp_data_blocks.begin();
  }

  return kOk;
} /*}}}*/

Code RealWorker::DealWithRequestOneDataBlock(const OneDataBlock &one_data_block) { /*{{{*/
  Code ret = kOk;
  std::string user_data;
  ret = server_->get_user_data_func_(one_data_block.real_data.data(), one_data_block.real_data.size(), &user_data);
  if (ret != kOk) return ret;

  std::string out_data;
  ret = server_->action_(user_data, &out_data);
  if (ret != kOk) return ret;

  std::string resp_data;
  ret = server_->format_user_data_func_(out_data, &resp_data);
  if (ret != kOk) return ret;

  OneDataBlock resp_data_block;
  resp_data_block.real_data = std::move(resp_data);
  resp_data_block.id = one_data_block.id;
  resp_data_block.fd = one_data_block.fd;
  resp_data_block.conn_worker = one_data_block.conn_worker;

  ret = one_data_block.conn_worker->AddResponseAndNotify(resp_data_block);
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

/****************************************
 * ConnWorker: recv and send request
 */
ConnWorker::ConnWorker(RpcServer *server)
    : server_(server),
      event_type_(server_->event_type_),
      worker_loop_(NULL),
      mu_(),
      data_mu_(),
      flow_ctrl_(kDefaultFlowGridNum, kDefaultFlowUnitNum, server_->max_flow_),
      data_proto_func_(server->GetDataProtoFunc()),
      unique_id_(0) { /*{{{*/ } /*}}}*/

ConnWorker::~ConnWorker() { /*{{{*/
  std::map<int, TcpConn>::iterator it = conns_.begin();
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

  ret = pipe(data_notify_fds_);
  if (ret != 0) return kPipeFailed;
  r = SetFdNonblock(data_notify_fds_[0]);
  if (r != kOk) return r;
  r = SetFdNonblock(data_notify_fds_[1]);
  if (r != kOk) return r;

  r = flow_ctrl_.Init();
  if (r != kOk) return r;

  if (data_proto_func_ == NULL) return kInvalidParam;

  worker_loop_ = new EventLoop();
  worker_loop_->Init(event_type_);
  worker_loop_->Add(notify_fds_[0], EV_IN, ConnWorkerNotifyEventAction, this);
  worker_loop_->Add(data_notify_fds_[0], EV_IN, ConnWorkerRespDataNotifyEventAction, this);

  pthread_create(&worker_id_, NULL, ConnWorkerThreadAction, this);
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
  int r = read(notify_fds_[0], buf, sizeof(buf));
  if (r == -1 && errno == EAGAIN) return kOk;
  if (r == -1 && errno != EAGAIN) return kReadError;

  MutexLock ml(&mu_);

  std::deque<int>::iterator it = cli_fds_.begin();
  while (it != cli_fds_.end()) {
    int client_fd = *it;
    TcpConn conn;
    conn.fd = client_fd;
    conn.id = GeneraterId();
    conns_.insert(std::pair<int, TcpConn>(client_fd, conn));

    worker_loop_->Add(client_fd, EV_IN, ClientEventAction, this);

    cli_fds_.pop_front();
    it = cli_fds_.begin();
  }

  return kOk;
} /*}}}*/

Code ConnWorker::ClientEventInternalAction(int fd, int evt) { /*{{{*/
  LOG_ERR("evt:%d in, fd:%d", evt, fd);
  // TODO:htt, support EV_OUT

  if ((evt & EV_ERR) || (evt & EV_HUP)) {
    LOG_ERR("invalid evt:%#x, then close fd:%d", evt, fd);
    TcpConn conn;
    conn.fd = fd;
    CloseConn(conn);
    return kSocketError;
  }

  std::map<int, TcpConn>::iterator it;
  {
    MutexLock ml(&mu_);
    it = conns_.find(fd);
    if (it == conns_.end()) return kNotFound;
  }

  TcpConn &conn = it->second;
  assert(fd == conn.fd);

  Code ret = kOk;
  bool have_read = false;
  while (true) {
    char read_buf[kBufLen] = {0};
    int real_len = 0;
    ret = data_proto_func_(conn.content.data(), conn.content.size(), &real_len);
    if (ret != kOk && ret != kDataNotEnough) {
      CloseConn(conn);
      return ret;
    }

    if (ret == kDataNotEnough) { /*{{{*/
      if (have_read) {
        LOG_ERR("fd:%d has been read, no need read, ret:%d", fd);
        return kOk;  // NOTE;htt, no need read again
      }

      int r = read(fd, read_buf, kBufLen);
      if (r == 0 || (r == -1 && errno != EAGAIN)) {
        LOG_ERR("Failed to read fd:%d, ret:%d", fd, r);
        CloseConn(conn);
        return kSocketError;
      }

      if (r == -1 && errno == EAGAIN) {  // NOTE:htt, no data, then wait again
        return kOk;
      }

      conn.content.append(read_buf, r);
      have_read = true;
      continue;
    } /*}}}*/

    // NOTE:htt, execute
    if (real_len <= 0 || real_len > conn.content.size()) {
      LOG_ERR("Invalid real_len%d, content size:%zu", real_len, conn.content.size());
      CloseConn(conn);
      return kDataDealFailed;
    }

    ret = SendRequestToRealWorker(conn.content, real_len, conn.fd, conn.id);
    if (ret != kOk) {
      LOG_ERR("Failed to send request to real work! ret:%d, fd:%d, id:%lu", ret, conn.fd, conn.id);
      CloseConn(conn);
      return kDataDealFailed;
    }

    if (real_len == conn.content.size()) {
      conn.content.clear();
    } else {
      conn.content = conn.content.substr(real_len);
    }
  }

  return kOk;
} /*}}}*/

Code ConnWorker::SendRequestToRealWorker(const std::string &content, int real_len, int fd, uint64_t id) { /*{{{*/
  if (content.empty() || real_len <= 0 || real_len > content.size()) return kInvalidParam;
  OneDataBlock request_data_block;
  request_data_block.real_data.assign(content.data(), real_len);
  request_data_block.id = id;
  request_data_block.fd = fd;
  request_data_block.conn_worker = this;

  int n = rand() % (server_->real_workers_.size());
  RealWorker *worker = server_->real_workers_[n];
  Code ret = worker->AddOneDataBlockAndNotify(request_data_block);
  if (ret != kOk) {
    LOG_ERR("Failed to Add one data block to real worker! ret:%d, fd:%d, id:%lu\n", ret, fd, (unsigned long)id);
  }
  return kOk;
} /*}}}*/

Code ConnWorker::AddResponseAndNotify(const OneDataBlock &one_data_block) { /*{{{*/
  MutexLock ml(&data_mu_);
  resp_data_blocks_.push_back(one_data_block);
  LOG_ERR("try to add resp id:%lu fd:%d to conn worker!", (unsigned long)one_data_block.id, one_data_block.fd);

  char buf[1] = {'1'};
  int ret = write(data_notify_fds_[1], buf, sizeof(buf));
  if (ret == -1) return kWriteError;

  return kOk;
} /*}}}*/

Code ConnWorker::RespDataNotifyEventInternalAction(int fd) { /*{{{*/
  char buf[1];
  assert(fd == data_notify_fds_[0]);
  int ret = read(data_notify_fds_[0], buf, sizeof(buf));
  if (ret == -1) return kReadError;

  std::deque<OneDataBlock> tmp_data_blocks;
  {
    MutexLock ml(&data_mu_);
    tmp_data_blocks = std::move(resp_data_blocks_);
  }

  std::deque<OneDataBlock>::iterator it = tmp_data_blocks.begin();
  while (it != tmp_data_blocks.end()) {
    Code r = DealWithRespOneDataBlock(*it);
    if (r != kOk) {
      LOG_ERR("Failed to deal with resp data block! ret:%d, id:%lu, fd:%d\n", ret, (unsigned long)it->id, it->fd);
    }

    tmp_data_blocks.pop_front();
    it = tmp_data_blocks.begin();
  }

  return kOk;
} /*}}}*/

Code ConnWorker::DealWithRespOneDataBlock(const OneDataBlock &one_data_block) { /*{{{*/
  TcpConn conn;
  LOG_ERR("try to find id:%lu fd:%d!", (unsigned long)one_data_block.id, one_data_block.fd);
  {
    MutexLock ml(&mu_);
    std::map<int, TcpConn>::iterator it = conns_.begin();
    bool fd_found = false;
    while (it != conns_.end()) {
      if (it->second.id == one_data_block.id && it->second.fd == one_data_block.fd) {
        conn.fd = it->second.fd;
        fd_found = true;
        break;
      }
    }

    if (!fd_found) {
      LOG_ERR("id:%lu fd:%d is not found, which may be closed!", (unsigned long)one_data_block.id, one_data_block.fd);
      return kOk;
    }
  }
  LOG_ERR("id:%lu fd:%d is found!", (unsigned long)one_data_block.id, one_data_block.fd);

  // TODO:htt, use EV_OUT to reponse data
  int left_len = one_data_block.real_data.size();
  while (left_len > 0) {
    int r = write(one_data_block.fd, one_data_block.real_data.data() + one_data_block.real_data.size() - left_len,
                  left_len);
    if (r == 0 || (r == -1 && errno != EAGAIN)) {
      LOG_ERR("Failed to write fd:%d, ret:%d", one_data_block.fd, r);
      CloseConn(conn);
      return kSocketError;
    }
    if (r == -1 && errno == EAGAIN) continue;
    left_len -= r;
  }
  return kOk;
} /*}}}*/

uint64_t ConnWorker::GeneraterId() { /*{{{*/
  unique_id_++;
  return unique_id_;
} /*}}}*/

Code ConnWorker::CloseConn(const TcpConn &conn) { /*{{{*/
  MutexLock ml(&mu_);
  close(conn.fd);
  conns_.erase(conn.fd);
  Code ret = worker_loop_->Del(conn.fd);

  return ret;
} /*}}}*/

/****************************************
 * RpcServer: listen tcp
 */
RpcServer::RpcServer(const Config &conf, DataProtoFunc data_proto_func, GetUserDataFunc get_user_data_func,
                     FormatUserDataFunc format_user_data_func, RpcAction action)
    : conf_(conf),
      data_proto_func_(data_proto_func),
      get_user_data_func_(get_user_data_func),
      format_user_data_func_(format_user_data_func),
      action_(action) { /*{{{*/
  int port = 0;
  int ret = conf_.GetInt32Value(kPortKey, &port);
  if (ret != kOk) port = kDefaultPort;
  port_ = port;

  serv_fd_ = -1;
  is_running_ = false;

  ret = conf_.GetInt32Value(kRealWorkerThreadsNumKey, &real_workers_num_);
  if (ret != kOk) real_workers_num_ = kDefaultRealWorkersNum;
  if (real_workers_num_ <= 0) real_workers_num_ = kDefaultRealWorkersNum;

  ret = conf_.GetInt32Value(kConnWorkerThreadsNumKey, &conn_workers_num_);
  if (ret != kOk) conn_workers_num_ = kDefaultConnWorkersNum;
  if (conn_workers_num_ <= 0) conn_workers_num_ = kDefaultConnWorkersNum;

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

RpcServer::~RpcServer() { /*{{{*/
  is_running_ = false;
  std::deque<RealWorker *>::iterator real_it = real_workers_.begin();
  while (real_it != real_workers_.end()) {
    delete (*real_it);
    real_it = real_workers_.begin();
  }

  std::deque<ConnWorker *>::iterator conn_it = conn_workers_.begin();
  while (conn_it != conn_workers_.end()) {
    delete (*conn_it);
    conn_it = conn_workers_.begin();
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

Code RpcServer::Init() { /*{{{*/
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

  for (int i = 0; i < real_workers_num_; ++i) {
    RealWorker *worker = new RealWorker(this);
    Code r = worker->Init();
    assert(r == kOk);
    real_workers_.push_back(worker);
  }
  if (real_workers_.size() == 0) return kInvalidParam;

  for (int i = 0; i < conn_workers_num_; ++i) {
    ConnWorker *worker = new ConnWorker(this);
    Code r = worker->Init();
    assert(r == kOk);
    conn_workers_.push_back(worker);
  }
  if (conn_workers_.size() == 0) return kInvalidParam;

  is_running_ = true;
  pthread_create(&stat_id_, NULL, StatThreadAction, this);

  srand(time(NULL));
  return kOk;
} /*}}}*/

Code RpcServer::Run() { /*{{{*/
  Code ret = main_loop_->Run();
  return ret;
} /*}}}*/

Code RpcServer::AcceptEventInternalAction(int fd, int evt) { /*{{{*/
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(fd, (struct sockaddr *)(&client_addr), &client_addr_len);
  if (client_fd == -1) return kAcceptError;

  Code r = SetFdReused(client_fd);
  if (r != kOk) return r;
  r = SetFdNonblock(client_fd);
  if (r != kOk) return r;

  assert(conn_workers_.size() > 0);
  int n = rand() % (conn_workers_.size());
  ConnWorker *worker = conn_workers_[n];
  r = worker->AddClientFdAndNotify(client_fd);

  return r;
} /*}}}*/

Code RpcServer::DumpStatAction() { /*{{{*/
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

  RpcServer server(config, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc, DefaultRpcAction);
  ret = server.Init();
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
