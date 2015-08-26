#include <iostream>

#include <assert.h>

#include "base/status.h"
#include "base/daemon.h"

#include "cs/server.h"

namespace demo
{

int TestServer()
{
    base::Code ret = base::DaemonAndKeepAlive();
    if (ret != base::kOk) return ret;

    uint16_t port = 9090;
    base::Server server(port, base::DefaultAction);
    ret = server.Init();
    assert(ret == base::kOk);

    server.Run();

    return 0;
}

}

int main(int argc, char *argv[])
{
    demo::TestServer();

    return 0;
}
