#include <iostream>
#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>

#include "base/log.h"
#include "base/status.h"
#include "base/daemon.h"
#include "base/config.h"
#include "base/common.h"

#include "cs/server.h"

namespace demo
{

static void SignalHandle(int sig);

int TestServer(const std::string &conf_path)
{/*{{{*/
    // read conf
    base::Config conf;
    base::Code ret = conf.LoadFile(conf_path);
    if (ret != base::kOk)
    {
        fprintf(stderr, "Load conf failed! ret:%d\n", ret);
        return ret;
    }

    // set log info
    std::string log_path;
    ret = conf.GetValue(base::kLogPathKey, &log_path);
    if (ret != base::kOk) return ret;
    int log_level = 0;
    ret = conf.GetInt32Value(base::kLogLevelKey, &log_level);
    if (ret != base::kOk) return ret;
    base::InitLog(log_path.c_str(), log_level);

    // set siganl
    signal(SIGTERM, SignalHandle);

    // set daemon
    int daemon_flags = 0;
    ret = conf.GetInt32Value(base::kDaemonKey, &daemon_flags);
    if (ret != base::kOk) return ret;
    if (daemon_flags)
    {
        ret = base::DaemonAndKeepAlive();
        if (ret != base::kOk) return ret;
    }

    // get port
    int port = 0; 
    ret = conf.GetInt32Value(base::kPortKey, &port);
    if (ret != base::kOk) return ret;

    int threads_num = 0;
    ret = conf.GetInt32Value(base::kThreadsNumKey, &threads_num);
    if (ret != base::kOk) threads_num = base::kDefaultWorkersNum;

    int flow_restrict = 0;
    ret = conf.GetInt32Value(base::kFlowRestrictKey, &flow_restrict);
    if (ret != base::kOk) flow_restrict = base::kMaxFlowRestrict;

    base::Server server((uint16_t)port, base::DefaultAction, threads_num, flow_restrict);
    ret = server.Init();
    if (ret != base::kOk)
    {
        base::LOG_ERR("Failed to init server! ret:%d", ret);
        return ret;
    }

    server.Run();

    return 0;
}/*}}}*/

static void SignalHandle(int sig)
{/*{{{*/
    switch (sig)
    {
        case SIGTERM:
            exit(0);
        default:
            exit(-1);
    }
}/*}}}*/

}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Note: please input config path\n");
        return -1;
    }

    demo::TestServer(argv[1]);

    return 0;
}
