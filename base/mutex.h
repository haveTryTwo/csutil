// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MUTEX_H_
#define BASE_MUTEX_H_

#include <pthread.h>

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

inline int FetchAndLock(int *value, int step)
{/*{{{*/
    asm volatile ("lock; xaddl %%eax, %2;"
            : "=a"(step)
            : "a"(step), "m"(*value)
            : "memory");

    return step;
}/*}}}*/

}

#endif
