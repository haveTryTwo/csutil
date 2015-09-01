#include <iostream>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

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

    return base::kOk;
}

void* ThreadFunc(void *param)
{
    base::Client client;
    base::Code ret = client.Init(base::kPoll);
    assert(ret == base::kOk);

    std::string ip("127.0.0.1");
    uint16_t port = 9090;
    ret = client.Connect(ip, port);
    assert(ret == base::kOk);

    pthread_t self = pthread_self();
    for (int i = 0; i < 10000; ++i)
    {
        std::string buf_out("hello world");
        ret = client.Write(buf_out);
#if defined(__linux__) || defined(__unix__)
        fprintf(stderr, "pthread_id:%#llx, buf_out:%s\n", (uint64_t)self, buf_out.c_str());
#elif defined(__APPLE__)
        fprintf(stderr, "pthread_id:%p, buf_out:%s\n", self, buf_out.c_str());
#endif
        assert(ret == base::kOk);

        std::string buf_in;
        ret = client.Read(&buf_in);
        assert(ret == base::kOk);
#if defined(__linux__) || defined(__unix__)
        fprintf(stderr, "pthread_id:%#llx, buf_in:%s\n", (uint64_t)self, buf_in.c_str());
#elif defined(__APPLE__)
        fprintf(stderr, "pthread_id:%p, buf_out:%s\n", self, buf_out.c_str());
#endif

        usleep(1000);
    }

    pthread_exit(NULL);
}

int MultiThreadTestClient()
{
    pthread_t pth[100];
    int ret = 0;
    for (int i = 0; i < (int)(sizeof(pth)/sizeof(pth[0])); ++i)
    {
        ret = pthread_create(pth+i, NULL, ThreadFunc, NULL);
        if (ret != 0)
        {
            fprintf(stderr, "[%s:%s:%d] Failed to pthread_create, errno:%d\n",
                    __FILE__, __func__, __LINE__, ret);
            return base::kPthreadCreateFailed;
        }
    }

    for (int i = 0; i < (int)(sizeof(pth)/sizeof(pth[0])); ++i)
    {
        pthread_join(pth[i], NULL);
    }

    return base::kOk;
}

}

int main(int argc, char *argv[])
{
    test::TestClient();

    test::MultiThreadTestClient();

    return 0;
}
