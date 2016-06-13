// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mutex.h"

#ifdef _MUTEX_MAIN_TEST_
#include <stdio.h>
#include <pthread.h>

int value = 0;
int step = 1;

void* PthreadFunc(void *param)
{/*{{{*/
    pthread_t self = pthread_self();

    for (int i = 0; i < 10000; ++i)
    {
        // 1. Lock test
        // Note: The last result must be 20000
        base::FetchAndLock(&value, step);

        // 2. No Lock test
        // Note: The last result may not be 20000
        // value += step;
    }
#if defined(__linux__) || defined(__unix__)
    fprintf(stderr, "pthread_id:%#llx, value:%d, step:%d\n", (uint64_t)self, value, step);
#elif defined(__APPLE__)
    fprintf(stderr, "pthread_id:%p, value:%d, step:%d\n", self, value, step);
#endif

    pthread_exit(NULL);
}/*}}}*/

int main()
{/*{{{*/
    using namespace base;

    pthread_t pth[2];

    pthread_create(pth, NULL, PthreadFunc, NULL);
    pthread_create(pth+1, NULL, PthreadFunc, NULL);

    pthread_join(pth[0], NULL);
    pthread_join(pth[1], NULL);

    return 0;
}/*}}}*/
#endif
