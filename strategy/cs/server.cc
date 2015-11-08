// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "base/ip.h"
#include "base/msg.h"
#include "base/log.h"
#include "base/util.h"
#include "base/coding.h"

#include "cs/server.h"
#include "cs/version.h"

#include "ftp/server/action.h"

namespace base
{

static void* WorkerThreadAction(void *param);
static Code NotifyEventAction(int fd, int evt, void *param);
static Code ClientEventAction(int fd, int evt, void *param);
static Code AcceptEventAction(int fd, int evt, void *param);

Code DefaultAction(const Config &conf, const std::string &in, std::string *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;
    out->assign(in);
    return kOk;
}/*}}}*/

static void* WorkerThreadAction(void *param)
{
    Worker *worker = reinterpret_cast<Worker*>(param);
    worker->Run();

    pthread_exit(NULL);
}

static void* StatThreadAction(void *param)
{
    Server *serv = reinterpret_cast<Server*>(param);
    serv->DumpStatAction();

    pthread_exit(NULL);
}

static Code NotifyEventAction(int fd, int evt, void *param)
{
    Worker *worker = reinterpret_cast<Worker*>(param);
    Code ret = worker->NotifyEventInternalAction(fd);
    return ret;
}

static Code ClientEventAction(int fd, int evt, void *param)
{
    Worker *worker = reinterpret_cast<Worker*>(param);
    Code ret = worker->ClientEventInternalAction(fd, evt);
    return ret;
}

static Code AcceptEventAction(int fd, int evt, void *param)
{
    Server *server = reinterpret_cast<Server*>(param);
    Code ret = server->AcceptEventInternalAction(fd, evt);
    return ret;
}


Worker::Worker(Server *server) : server_(server),
    event_type_(server_->event_type_), worker_loop_(NULL), mu_(),
    flow_ctrl_(kDefaultFlowGridNum, kDefaultFlowUnitNum, server_->max_flow_)
{/*{{{*/
}/*}}}*/

Worker::~Worker()
{/*{{{*/
    std::map<int, Conn*>::iterator it = conns_.begin();
    while (it != conns_.end())
    {
        CloseConn(it->second);
        it = conns_.begin();
    }

    if (worker_loop_ != NULL)
    {
        delete worker_loop_;
        worker_loop_ = NULL;
    }
}/*}}}*/

Code Worker::Init() {/*{{{*/ int ret = pipe(notify_fds_); if (ret != 0) return kPipeFailed;

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
}/*}}}*/

Code Worker::Run()
{/*{{{*/
    Code ret = worker_loop_->Run();
    return ret;
}/*}}}*/

Code Worker::AddClientFdAndNotify(int fd)
{/*{{{*/
    MutexLock ml(&mu_);
    cli_fds_.push_back(fd);

    char buf[1] = {'1'};
    int ret = write(notify_fds_[1], buf, sizeof(buf));
    if (ret == -1) return kWriteError;

    return kOk;
}/*}}}*/

Code Worker::NotifyEventInternalAction(int fd)
{/*{{{*/
    char buf[1];
    assert(fd == notify_fds_[0]);
    int ret = read(notify_fds_[0], buf, sizeof(buf));
    if (ret == -1) return kReadError;

    MutexLock ml(&mu_);

    std::deque<int>::iterator it = cli_fds_.begin();
    while (it != cli_fds_.end())
    {
        int client_fd = *it;
        Conn *conn = new Conn;
        conn->left_count = 0;
        conn->fd = client_fd;
        conn->conn_status = kConnCmd;
        conns_.insert(std::make_pair<int, Conn*>(client_fd, conn));

        worker_loop_->Add(client_fd, EV_IN, ClientEventAction, this);
        
        cli_fds_.pop_front();
        it = cli_fds_.begin();
    }

    return kOk;
}/*}}}*/

Code Worker::ClientEventInternalAction(int fd, int evt)
{/*{{{*/
    std::map<int, Conn*>::iterator it = conns_.find(fd);
    if (it == conns_.end()) return kNotFound;

    Conn *conn = it->second;
    assert(fd == conn->fd);

    char cmd_buf[kHeadLen];
    char read_buf[kBufLen];

    switch (conn->conn_status)
    {
        case kConnCmd:
            /*{{{*/
            conn->content.clear();
            conn->left_count = kHeadLen;

            while (conn->left_count > 0)
            {
                int ret = read(conn->fd, cmd_buf, conn->left_count);
                if (ret == 0 || (ret == -1 && errno != EAGAIN))
                {
                    CloseConn(conn);
                    break;
                }

                if (ret == -1 && errno == EAGAIN)
                {
                    conn->conn_status = kConnWait;
                    break;
                }

                conn->content.append(cmd_buf, ret);
                conn->left_count -= ret;

                if (conn->left_count == 0)
                {
                    conn->conn_status = kConnRead;
                    Code r = DecodeFixed32(conn->content, reinterpret_cast<uint32_t*>(&(conn->left_count)));
                    assert(r == kOk);
                    conn->content.clear();
                    break;
                }
            }
            break;
            /*}}}*/
        case kConnWait:
            /*{{{*/
            while (conn->left_count > 0)
            {
                int ret = read(conn->fd, cmd_buf, conn->left_count);
                if (ret == 0 || (ret == -1 && errno != EAGAIN))
                {
                    CloseConn(conn);
                    break;
                }

                if (ret == -1 && errno == EAGAIN)
                    break;

                conn->content.append(cmd_buf, ret);
                conn->left_count -= ret;

                if (conn->left_count == 0)
                {
                    conn->conn_status = kConnRead;
                    Code r = DecodeFixed32(conn->content, reinterpret_cast<uint32_t*>(&(conn->left_count)));
                    assert(r == kOk);
                    conn->content.clear();
                    break;
                }
            }
            break;
            /*}}}*/
        case kConnRead:
            /*{{{*/
            if (conn->left_count == 0)
            {
                // check flow
                struct timeval now;
                gettimeofday(&now, NULL);
                bool is_restrict = false;
                Code r = flow_ctrl_.CheckFlow(now, &is_restrict);
                assert(r == kOk);

                // statistic
                struct timeval end;
                int recv_size = conn->content.size();
                std::string ret_value;
                if (is_restrict)
                {
                    std::string peer_ip;
                    GetPeerIp(fd, &peer_ip);
                    LOG_INFO("fd:%d, ip:%s exceeds max flow, Now flow restrict!", fd, peer_ip.c_str());
                    // TODO: may just close the connection
                    ret_value.assign(FlowInfo);
                    r = kFlowRestrict;
                }
                else
                {
                    r = server_->action_(server_->conf_, conn->content, &ret_value);
                    // TODO: may just check the return value, but not reset ret_value
                    if (r != kOk || ret_value.empty())
                        ret_value.assign(ActionFailedInfo);
                }
                gettimeofday(&end, NULL);
                server_->stat_->AddStat(kModel, r, now, end, recv_size, ret_value.size(), 1);

                r = EncodeToMsg(ret_value, &(conn->content));
                assert(r == kOk);
                conn->left_count = conn->content.size();

                conn->conn_status = kConnWrite;
                worker_loop_->Mod(conn->fd, EV_OUT, ClientEventAction, this);
                break;
            }

            while (conn->left_count > 0)
            {
                int would_read_len = (int)sizeof(read_buf) < conn->left_count ?
                    sizeof(read_buf) : conn->left_count;

                int ret = read(conn->fd, read_buf, would_read_len);
                if (ret == 0 || (ret == -1 && errno != EAGAIN))
                {
                    CloseConn(conn);
                    break;
                }

                if (ret == -1 && errno == EAGAIN)
                    break;

                conn->content.append(read_buf, ret);
                conn->left_count -= ret;

                if (conn->left_count == 0)
                {
                    // flow check
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    bool is_restrict = false;
                    Code r = flow_ctrl_.CheckFlow(now, &is_restrict);
                    assert(r == kOk);

                    // statistic
                    struct timeval end;
                    int recv_size = conn->content.size();
                    std::string ret_value;
                    if (is_restrict)
                    {
                        std::string peer_ip;
                        GetPeerIp(fd, &peer_ip);
                        LOG_INFO("fd:%d, ip:%s exceeds max flow, Now flow restrict!", fd, peer_ip.c_str());
                        // TODO: may just close the connection
                        ret_value.assign("Now flow restrict!");
                        r = kFlowRestrict;
                    }
                    else
                    {
                        r = server_->action_(server_->conf_, conn->content, &ret_value);
                        // TODO: may just check the return value, but not reset ret_value
                        if (r != kOk || ret_value.empty())
                            ret_value.assign("Failed to do user action");
                    }
                    gettimeofday(&end, NULL);
                    server_->stat_->AddStat(kModel, r, now, end, recv_size, ret_value.size(), 1);

                    r = EncodeToMsg(ret_value, &(conn->content));
                    assert(r == kOk);
                    conn->left_count = conn->content.size();

                    conn->conn_status = kConnWrite;
                    worker_loop_->Mod(conn->fd, EV_OUT, ClientEventAction, this);
                    break;
                }
            }
            break;
            /*}}}*/
        case kConnWrite:
            /*{{{*/
            if (conn->left_count == 0)
            {
                conn->content.clear();
                conn->conn_status = kConnCmd;
                worker_loop_->Mod(conn->fd, EV_IN, ClientEventAction, this);
                break;
            }

            while (conn->left_count > 0)
            {
                int ret = write(conn->fd, conn->content.data()+conn->content.size()-conn->left_count,
                        conn->left_count);

                if (ret == 0 || (ret == -1 && errno != EAGAIN))
                {
                    CloseConn(conn);
                    break;
                }

                if (ret == -1 && errno == EAGAIN)
                    break;

                conn->left_count -= ret;

                if (conn->left_count == 0)
                {
                    conn->content.clear();
                    conn->conn_status = kConnCmd;
                    worker_loop_->Mod(conn->fd, EV_IN, ClientEventAction, this);
                    break;
                }
            }
            break;
            /*}}}*/
        case kConnClose:
            CloseConn(conn);
            break;
        default:
            break;
    }

    return kOk;
}/*}}}*/

Code Worker::CloseConn(Conn *conn)
{/*{{{*/
    close(conn->fd);
    conns_.erase(conn->fd);
    Code ret = worker_loop_->Del(conn->fd);
    delete conn;

    return ret;
}/*}}}*/


Server::Server(const Config &conf, Action action) : conf_(conf), action_(action)
{/*{{{*/
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
}/*}}}*/

Server::~Server()
{/*{{{*/
    is_running_ = false;
    std::deque<Worker*>::iterator it = workers_.begin();
    while (it != workers_.end())
    {
        delete (*it);
        it = workers_.begin();
    }

    if (main_loop_ != NULL)
    {
        delete main_loop_;
        main_loop_ = NULL;
    }

    if (serv_fd_ != -1)
    {
        close(serv_fd_);
        serv_fd_ = -1;
    }
}/*}}}*/

Code Server::Init()
{/*{{{*/
    if (action_ == NULL) action_ = DefaultAction;

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

    int ret = bind(serv_fd_, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
    if (ret == -1) return kBindError;

    ret = listen(serv_fd_, kDefaultBacklog);

    main_loop_ = new EventLoop();
    main_loop_->Init(event_type_);
    main_loop_->Add(serv_fd_, EV_IN, AcceptEventAction, this);

    is_running_ = true;
    for (int i = 0; i < workers_num_; ++i)
    {
        Worker *worker = new Worker(this);
        Code r = worker->Init();
        assert(r == kOk);
        workers_.push_back(worker);
    }

    pthread_create(&stat_id_, NULL, StatThreadAction, this);

    srand(time(NULL));
    return kOk; 
}/*}}}*/

Code Server::Run()
{/*{{{*/
    Code ret = main_loop_->Run();
    return ret;
}/*}}}*/

Code Server::AcceptEventInternalAction(int fd, int evt)
{/*{{{*/
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr*)(&client_addr), &client_addr_len);
    if (client_fd == -1) return kAcceptError;
    
    Code r = SetFdReused(client_fd);
    if (r != kOk) return r;
    r = SetFdNonblock(client_fd);
    if (r != kOk) return r;

    assert(workers_.size() > 0);
    int n = rand()%(workers_.size());
    Worker *worker = workers_[n];
    r = worker->AddClientFdAndNotify(client_fd);

    return r;
}/*}}}*/

Code Server::DumpStatAction()
{/*{{{*/
    while (true)
    {
        sleep(stat_dump_circle_);
        stat_->DumpStat();
    }

    return kOk;
}/*}}}*/

}

#ifdef _SERVER_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;

    uint16_t port = 9090;

    Server server(port, DefaultAction);
    Code ret = server.Init();
    assert(ret == kOk);

    ret = server.Run();

    return 0;
}
#endif
