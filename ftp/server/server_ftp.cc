#include <iostream>
#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>

#include <google/protobuf/message.h>

#include "base/log.h"
#include "base/status.h"
#include "base/daemon.h"
#include "base/config.h"
#include "base/common.h"
#include "base/file_util.h"

#include "cs/server.h"

#include "ftp/server/action.h"

namespace ftp
{

static void SignalHandle(int sig);

int FtpServer(const std::string &conf_path)
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
    std::string log_dir;
    ret = conf.GetValue(base::kLogDirKey, &log_dir);
    if (ret != base::kOk) log_dir = "../log";
    ret = base::CreateDir(log_dir);
    if (ret != base::kOk)
    {
        fprintf(stderr, "Failed to create log dir:%s\n", log_dir.c_str());
        return ret;
    }
    std::string log_name;
    ret = conf.GetValue(base::kLogFileNameKey, &log_name);
    if (ret != base::kOk) return ret;
    std::string log_path = log_dir + "/" + log_name;
    fprintf(stderr, "log path:%s\n", log_path.c_str());
    int log_level = 0;
    ret = conf.GetInt32Value(base::kLogLevelKey, &log_level);
    if (ret != base::kOk) return ret;
    base::InitLog(log_path.c_str(), log_level);

    // store directory
    std::string store_dir;
    ret = conf.GetValue(base::kFtpStoreDirKey, &store_dir);
    if (ret != base::kOk) store_dir = base::kDefaultFtpStoreDir;
    ret = base::CreateDir(store_dir);
    if (ret != base::kOk)
    {
        fprintf(stderr, "Failed to create store dir:%s\n", store_dir.c_str());
        return ret;
    }
   

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

    base::Server server(conf, ftp::FtpAction);
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

    ftp::FtpServer(argv[1]);

    return 0;
}
