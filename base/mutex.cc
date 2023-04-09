// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mutex.h"

#ifdef _MUTEX_MAIN_TEST_
#include <pthread.h>
#include <stdio.h>

// int value = 0;
// int step = 1;

uint64_t value = 0xff44ffffffff;
uint64_t step = 1;

void* PthreadFunc(void* param) { /*{{{*/
  pthread_t self = pthread_self();

  for (int i = 0; i < 0x1000; ++i) {
    // 1. Lock test
    // Note: The last result must be 20000
    // base::FetchAndAdd((uint32_t*)&value, (uint32_t)step);
    base::FetchAndAdd(&value, step);

    // 2. No Lock test
    // Note: The last result may not be 20000
    // value += step;
  }
#if defined(__linux__) || defined(__unix__)
  fprintf(stderr, "pthread_id:%##llx, value:%#llx, step:%#llx\n", (uint64_t)self, value, step);
#elif defined(__APPLE__)
  fprintf(stderr, "pthread_id:%p, value:%#llx, step:%#llx\n", self, value, step);
#endif

  pthread_exit(NULL);
} /*}}}*/

int main() { /*{{{*/
  using namespace base;

  pthread_t pth[2];

  pthread_create(pth, NULL, PthreadFunc, NULL);
  pthread_create(pth + 1, NULL, PthreadFunc, NULL);

  pthread_join(pth[0], NULL);
  pthread_join(pth[1], NULL);

  uint64_t version = 0xff22ffffffffffff;
  uint64_t step = 1;
  fprintf(stderr, "[BEGIN] version:%#llx, step:%#llx\n", version, step);
  uint64_t tmp_ver = FetchAndAdd(&version, step);
  fprintf(stderr, "[END] version:%#llx, step:%#llx, tmp_ver:%#llx\n\n", version, step, tmp_ver);

  int mem = 200;
  int old_value = 200;
  int new_value = 300;
  // 1.1 CompareAndSwap succ
  fprintf(stderr, "[BEGIN] mem:%d, old_value:%d, new_value:%d\n", mem, old_value, new_value);
  int tmp_value = CompareAndSwap((uint32_t*)&mem, (uint32_t)old_value, (uint32_t)new_value);
  fprintf(stderr, "[END] mem:%d, old_value:%d, new_value:%d, tmp_value:%d\n\n", mem, old_value,
          new_value, tmp_value);

  // 1.2 CompareAndSwap failed
  mem = 400;
  fprintf(stderr, "[BEGIN] mem:%d, old_value:%d, new_value:%d\n", mem, old_value, new_value);
  tmp_value = CompareAndSwap((uint32_t*)&mem, (uint32_t)old_value, (uint32_t)new_value);
  fprintf(stderr, "[END] mem:%d, old_value:%d, new_value:%d, tmp_value:%d\n\n", mem, old_value,
          new_value, tmp_value);

  mem = 200;
  old_value = 200;
  new_value = 300;
  // 2.1 CAS succ
  fprintf(stderr, "[BEGIN] mem:%d, old_value:%d, new_value:%d\n", mem, old_value, new_value);
  Code ret = CAS((uint32_t*)&mem, (uint32_t)old_value, (uint32_t)new_value);
  if (ret == kOk) {
    fprintf(stderr, "[END] SUCC to CAS! mem:%d, old_value:%d, new_value:%d\n\n", mem, old_value,
            new_value);
  } else {
    fprintf(stderr, "[END] FAILED to CAS! mem:%d, old_value:%d, new_value:%d\n\n", mem, old_value,
            new_value);
  }

  mem = 400;
  old_value = 200;
  new_value = 300;
  // 2.2 CAS failed
  fprintf(stderr, "[BEGIN] mem:%d, old_value:%d, new_value:%d\n", mem, old_value, new_value);
  ret = CAS((uint32_t*)&mem, (uint32_t)old_value, (uint32_t)new_value);
  if (ret == kOk) {
    fprintf(stderr, "[END] SUCC to CAS! mem:%d, old_value:%d, new_value:%d\n\n", mem, old_value,
            new_value);
  } else {
    fprintf(stderr, "[END] FAILED to CAS! mem:%d, old_value:%d, new_value:%d\n\n", mem, old_value,
            new_value);
  }

  uint64_t mem64 = 0xff1122334455;
  uint64_t old_value64 = 0xff1122334455;
  uint64_t new_value64 = 0xff1122334477;
  // 3.1 CAS64 succ
  fprintf(stderr, "[BEGIN] mem:%#llx, old_value:%#llx, new_value:%#llx\n", mem64, old_value64,
          new_value64);
  ret = CAS(&mem64, old_value64, new_value64);
  if (ret == kOk) {
    fprintf(stderr, "[END] SUCC to CAS! mem:%#llx, old_value:%#llx, new_value:%#llx\n\n", mem64,
            old_value64, new_value64);
  } else {
    fprintf(stderr, "[END] FAILED to CAS! mem:%#llx, old_value:%#llx, new_value:%#llx\n\n", mem64,
            old_value64, new_value64);
  }

  mem64 = 0xff1122334488;
  old_value64 = 0xff1122334455;
  new_value64 = 0xff1122334477;
  // 3.2 CAS64 failed
  fprintf(stderr, "[BEGIN] mem:%#llx, old_value:%#llx, new_value:%#llx\n", mem64, old_value64,
          new_value64);
  ret = CAS(&mem64, old_value64, new_value64);
  if (ret == kOk) {
    fprintf(stderr, "[END] SUCC to CAS! mem:%#llx, old_value:%#llx, new_value:%#llx\n\n", mem64,
            old_value64, new_value64);
  } else {
    fprintf(stderr, "[END] FAILED to CAS! mem:%#llx, old_value:%#llx, new_value:%#llx\n\n", mem64,
            old_value64, new_value64);
  }

  return 0;
} /*}}}*/
#endif
