// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pid_controller.h"

namespace base {

PidController::PidController(double dt, double Kp, double kd, double ki)
    : dt_(dt), kp_(Kp), kd_(kd), ki_(ki), pre_error_(0), integral_(0) {
  if (dt == 0.0) {
    dt_ = 1.0;  // NOTE:htt, 如果dt_为0，则将调整为默认值1.0
  }
}

PidController::~PidController() {}

/**
 * NOTE:htt, 获取本次新增值
 */
double PidController::calculate(double setpoint, double pre_value) {
  // 计算误差
  double error = setpoint - pre_value;

  // 比例项
  double pout = kp_ * error;

  // 积分项
  integral_ += error * dt_;
  double iout = ki_ * integral_;

  // 导数项
  double derivative = (error - pre_error_) / dt_;
  double dout = kd_ * derivative;

  double output = pout + iout + dout;

  pre_error_ = error;

  return output;
}

}  // namespace base
