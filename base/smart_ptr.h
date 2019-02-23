// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SMART_PTR_H_
#define SMART_PTR_H_

#include <stdlib.h>
#include <assert.h>

#include "base/status.h"

namespace base
{

template <typename T>
class SmartPtr
{
    private:
        struct InternalData
        {
            T *ptr;
            int count;
        };

    public:
        SmartPtr() : _internal_data(NULL) {}
        SmartPtr(T *data);

        ~SmartPtr();

        SmartPtr(const SmartPtr &smart_ptr);
        SmartPtr& operator = (const SmartPtr &smart_ptr);

        T operator * ()
        {/*{{{*/
            assert(_internal_data != NULL);
            return *(_internal_data->ptr);
        }/*}}}*/

        T* operator -> ()
        {/*{{{*/
            assert(_internal_data != NULL);
            return _internal_data->ptr;
        }/*}}}*/

    private:
        InternalData *_internal_data;
};

template <typename T>
SmartPtr<T>::SmartPtr(T *data)
{/*{{{*/
    _internal_data = new InternalData();
    _internal_data->ptr = data;
    _internal_data->count = 1;
}/*}}}*/

template <typename T>
SmartPtr<T>::~SmartPtr()
{/*{{{*/
    if (_internal_data != NULL)
    {
        --(_internal_data->count);
        if (_internal_data->count == 0)
        {
            delete _internal_data->ptr;
            _internal_data->ptr = NULL;
            delete _internal_data;
            _internal_data = NULL;
        }
    }
}/*}}}*/

template <typename T>
SmartPtr<T>::SmartPtr(const SmartPtr &smart_ptr)
{/*{{{*/
    _internal_data = smart_ptr._internal_data;
    if (_internal_data != NULL)
    {
        _internal_data->count++;
    }
}/*}}}*/

template <typename T>
SmartPtr<T>& SmartPtr<T>::operator = (const SmartPtr &smart_ptr)
{/*{{{*/
    if (&smart_ptr == this)
        return *this;

    if (_internal_data != NULL)
    {
        --_internal_data->count;
        if (_internal_data->count == 0)
        {
            delete _internal_data->ptr;
            delete _internal_data;
        }
    }

    _internal_data = smart_ptr._internal_data;
    if (_internal_data != NULL)
    {
        _internal_data->count++;
    }

    return *this;
}/*}}}*/


}

#endif
