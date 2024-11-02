// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_H_
#define BASE_EVENT_H_

#include "base/status.h"

namespace base {

template <typename T>
inline void Swap(T *first, T *second) {
  T tmp = *first;
  *first = *second;
  *second = tmp;
}

enum EventType { /*{{{*/
                 kSelect,
                 kPoll,
                 kEPoll,
}; /*}}}*/

enum EV {                  /*{{{*/
          EV_IN = 0x001,   // There is data to read
          EV_PRI = 0x002,  // There is urgent data to read
          EV_OUT = 0x004,  // Writing now will not block

          EV_ERR = 0x008,  // Error on fd, this is always used, and it's not necessery to set
          EV_HUP = 0x010,  // Hup on fd, this is alway used, and it's not necessary to set
          EV_NVAL = 0x020,

          EV_RDNORM = 0x040,  // Normal data may be read
          EV_RDBAND = 0x080,  // Priority data may be read
          EV_WRNORM = 0x100,  // Writing now will not block
          EV_WRBAND = 0x200,  // Priority data may be write

          EV_ET = (1 << 31),  // Set the Edge Triggered behavior for the fd
}; /*}}}*/

class Event { /*{{{*/
 public:
  Event() {}
  virtual ~Event() {}

 public:
  virtual Code Create(int nfds) = 0;
  virtual Code Wait(int time_out_ms) = 0;
  virtual Code GetEvents(int *fd, int *evt) = 0;

  virtual Code Add(int fd, int evt) = 0;
  virtual Code Mod(int fd, int evt) = 0;
  virtual Code Del(int fd) = 0;

  virtual void Print() = 0;

 private:
  Event(const Event &ev);
  Event &operator=(const Event &ev);
}; /*}}}*/

}  // namespace base

#include "base/event_poll.h"

#if defined(__linux__)
#  include "base/event_epoll.h"
#endif

#endif
