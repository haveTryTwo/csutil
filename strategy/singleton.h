#ifndef STRATEGY_SINGLETON_H_
#define STRATEGY_SINGLETON_H_

#include <stdio.h>

#include "base/mutex.h"

namespace strategy
{

template <typename T>
class Singleton
{
    public:
        static T* Instance();

    public:
        // NOTE: This function may be no used!
        // It's used for memory recyled!
        static void DestroyInstance();

    protected:
        Singleton();

    public:
        virtual ~Singleton();

    private:
        static Singleton *singleton_;
        static base::Mutex mu_;

        T object_;
};

template <typename T>
Singleton<T>* Singleton<T>::singleton_ = NULL;

template <typename T>
base::Mutex Singleton<T>::mu_;

template <typename T>
T* Singleton<T>::Instance()
{/*{{{*/
    if (singleton_ == NULL)
    {
        base::MutexLock mlock(&mu_);

        if (singleton_ == NULL)
            singleton_ = new Singleton();
    }

    return &(singleton_->object_);
}/*}}}*/

template <typename T>
void Singleton<T>::DestroyInstance()
{/*{{{*/
    if (singleton_ != NULL)
    {
        base::MutexLock mlock(&mu_);

        if (singleton_ != NULL)
        {
            delete singleton_;
            singleton_ = NULL;
        }
    }
}/*}}}*/

template <typename T>
Singleton<T>::Singleton()
{
}

template <typename T>
Singleton<T>::~Singleton()
{
}

}

#endif
