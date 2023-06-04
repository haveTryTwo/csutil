// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/time.h>

#include "base/common.h"

#include "test_press_base/include/test_press_controller.h"

namespace test {

TestPressController::TestPressController() { /*{{{*/
} /*}}}*/

TestPressController::~TestPressController() { /*{{{*/
  Clear();
} /*}}}*/

base::Code TestPressController::RegisterObject(PressObject* press_object) { /*{{{*/
  if (press_object == NULL) return base::kInvalidParam;

  base::MutexLock mlock(&mu_);

  std::map<std::string, PressObject*>::iterator it =
      press_factory_.find(press_object->GetTestName());
  if (it != press_factory_.end()) return base::kExist;

  press_factory_.insert(
      std::pair<std::string, PressObject*>(press_object->GetTestName(), press_object));

  return base::kOk;
} /*}}}*/

base::Code TestPressController::GetNewPressObject(const std::string& press_name,
                                                  PressObject** press_object) { /*{{{*/
  base::MutexLock mlock(&mu_);

  std::map<std::string, PressObject*>::iterator it = press_factory_.find(press_name);
  if (it == press_factory_.end()) return base::kNotFound;

  *press_object = it->second->Create();

  return base::kOk;
} /*}}}*/

base::Code TestPressController::RegisterClient(BusiClient* busi_client) { /*{{{*/
  if (busi_client == NULL) return base::kInvalidParam;

  base::MutexLock mlock(&mu_);

  std::map<std::string, BusiClient*>::iterator it = client_factory_.find(busi_client->GetName());
  if (it != client_factory_.end()) return base::kExist;

  client_factory_.insert(std::pair<std::string, BusiClient*>(busi_client->GetName(), busi_client));

  return base::kOk;
} /*}}}*/

base::Code TestPressController::GetNewBusiClient(const std::string& client_name,
                                                 BusiClient** busi_client) { /*{{{*/
  base::MutexLock mlock(&mu_);

  std::map<std::string, BusiClient*>::iterator it = client_factory_.find(client_name);
  if (it == client_factory_.end()) return base::kNotFound;

  *busi_client = it->second->Create();

  return base::kOk;
} /*}}}*/

void TestPressController::Clear() { /*{{{*/
  base::MutexLock mlock(&mu_);

  std::map<std::string, PressObject*>::iterator it = press_factory_.begin();
  for (; it != press_factory_.end(); ++it) {
    delete it->second;
  }
  std::map<std::string, BusiClient*>::iterator client_it = client_factory_.begin();
  for (; client_it != client_factory_.end(); ++client_it) {
    delete client_it->second;
  }
} /*}}}*/

}  // namespace test
