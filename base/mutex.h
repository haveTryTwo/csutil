// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MUTEX_H_
#define BASE_MUTEX_H_

#include <stdint.h>
#include <pthread.h>

#include <stdio.h>

#include "status.h"

namespace base
{

class Mutex
{/*{{{*/
    public:
        Mutex() 
        {
            pthread_mutex_init(&mutex_, NULL);
        }

        ~Mutex()
        {
            pthread_mutex_destroy(&mutex_);
        }

    public:
        void Lock()
        {
            pthread_mutex_lock(&mutex_);
        }

        void UnLock()
        {
            pthread_mutex_unlock(&mutex_);
        }

    private:
        Mutex(const Mutex &);
        Mutex& operator= (const Mutex &);

    private:
        pthread_mutex_t mutex_;
};/*}}}*/

class MutexLock
{/*{{{*/
    public:
        explicit MutexLock(Mutex *mu) : mu_(mu)
        {
            mu_->Lock();
        }

        ~MutexLock()
        {
            mu_->UnLock();
        }

    private:
        MutexLock(const MutexLock &);
        MutexLock& operator= (const MutexLock &);

    private:
        Mutex *mu_;
};/*}}}*/

inline uint32_t FetchAndAdd(uint32_t *value, uint32_t step)
{/*{{{*/
    asm volatile ("lock; xaddl %%eax, %2;"
            : "=a"(step)
            : "a"(step), "m"(*value)
            : "memory");

    return step;
}/*}}}*/

inline uint64_t FetchAndAdd(uint64_t *value, uint64_t step)
{/*{{{*/
    asm volatile ("lock; xaddq %0, %1;"
            : "+r"(step), "+m"(*value)
            :
            : "memory");

    return step;
}/*}}}*/

inline uint32_t CompareAndSwap(uint32_t *mem, uint32_t old_value, uint32_t new_value)
{/*{{{*/
    uint32_t tmp_value = 0;
    asm volatile ("lock; cmpxchgl %1, %2;"
            : "=a"(tmp_value)
            : "r"(new_value), "m"(*mem), "0"(old_value)
            : "memory");

    return tmp_value;
}/*}}}*/

inline Code CAS(uint32_t *mem, uint32_t old_value, uint32_t new_value)
{/*{{{*/
    uint32_t tmp_value = CompareAndSwap(mem, old_value, new_value);
    if (tmp_value == old_value)
        return kOk;         // CAS is successful
    else
        return kCASFailed;  // CAS is failed
}/*}}}*/

inline uint64_t CompareAndSwap(uint64_t *mem, uint64_t old_value, uint64_t new_value)
{/*{{{*/
    uint32_t low_tmp_value = 0;
    uint32_t high_tmp_value = 0;
    asm volatile ("lock; cmpxchg8b %2"
            : "=a"(low_tmp_value), "=d"(high_tmp_value), "+m"(*mem)
            : "b"((unsigned int)new_value),
              "c"((unsigned int)(new_value >> 32)),
              "a"((unsigned int)old_value),
              "d"((unsigned int)(old_value >> 32))
            : "memory"
            );
    uint64_t tmp_value = (((uint64_t)high_tmp_value) << 32) + low_tmp_value;

    return tmp_value;
}/*}}}*/

inline Code CAS(uint64_t *mem, uint64_t old_value, uint64_t new_value)
{/*{{{*/
    uint64_t tmp_value = CompareAndSwap(mem, old_value, new_value);
    if (tmp_value == old_value)
        return kOk;         // CAS is successful
    else
        return kCASFailed;  // CAS is failed
}/*}}}*/

}

#endif
