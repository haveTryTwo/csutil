// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TIME_H_
#define BASE_TIME_H_

#include <string>

#include <sys/time.h>

#include "base/status.h"

namespace base
{

class Time
{/*{{{*/
    public:
        Time();
        ~Time();
        Time(const Time &t);
        Time& operator= (const Time &t);

    public:
        
        /**
         * date format need to be "YYYY-mm-dd HH:MM:SS"
         */
        static Code GetSecond(const std::string &date, time_t *time);

        static Code GetDate(time_t second, std::string *date);

    public:
        void Begin();
        void End();
        void Print() const;

    private:
        struct timeval begin_;
        struct timeval end_;
};/*}}}*/

}

#endif
