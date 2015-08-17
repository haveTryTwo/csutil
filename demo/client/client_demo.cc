#include <iostream>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "base/status.h"

#include "cs/client.h"

namespace test
{

int TestClient()
{
    base::Client client;
    base::Code ret = client.Init(base::kPoll);
    assert(ret == base::kOk);

    std::string ip("127.0.0.1");
    uint16_t port = 9090;
    ret = client.Connect(ip, port);
    assert(ret == base::kOk);

    std::string buf_out("hello world");
    ret = client.Write(buf_out);
    fprintf(stderr, "buf_out:%s\n", buf_out.c_str());
    assert(ret == base::kOk);

    std::string buf_in;
    ret = client.Read(&buf_in);
    assert(ret == base::kOk);
    fprintf(stderr, "buf_in:%s\n", buf_in.c_str());

    return 0;
}

}

int main(int argc, char *argv[])
{
    test::TestClient();

    return 0;
}
