// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PID_CONTROLLER_H_
#define BASE_PID_CONTROLLER_H_

#include <stdint.h>
#include <sys/time.h>

#include <map>
#include <string>

#include "base/common.h"
#include "base/mutex.h"
#include "base/status.h"

namespace base {

// NOTE:htt, see: https://gist.github.com/bradley219/5373998
class PidController {
 public:
  PidController(double dt, double kp, double kd, double ki);
  ~PidController();

  // NOTE:htt, 获取新增值
  double calculate(double setpoint, double pre_value);

 private:
  double dt_;         // loop interval time
  double kp_;         // 比例增益
  double kd_;         // 微分增益
  double ki_;         // 积分增益
  double pre_error_;  // 上一次误差值
  double integral_;   // 积分项
};

}  // namespace base
#endif
