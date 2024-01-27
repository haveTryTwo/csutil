// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/daemon.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace base {
static void SignalForward(int sig);

Code Daemon() { /*{{{*/
  pid_t pid = fork();
  if (pid == -1) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  pid = setsid();
  if (pid == -1) exit(EXIT_FAILURE);

  signal(SIGHUP, SIG_IGN);

  pid = fork();
  if (pid == -1) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  int fd = open("/dev/null", O_RDWR);
  if (fd == -1) return kOpenError;
  dup2(fd, STDIN_FILENO);
  dup2(fd, STDOUT_FILENO);
  dup2(fd, STDERR_FILENO);
  for (fd = 3; fd < sysconf(_SC_OPEN_MAX); ++fd) close(fd);

  return kOk;
} /*}}}*/

static void SignalForward(int sig) {
  signal(sig, SIG_IGN);
  kill(0, sig);
}

Code DaemonAndKeepAlive() { /*{{{*/
  Code ret = Daemon();
  if (ret != kOk) return ret;

  int proc_num = 0;

  while (true) {
    pid_t pid = 0;

    if (proc_num < 1) {
      pid = fork();
      if (pid == 0) return kOk;
      if (pid < 0) return kForkFailed;

      signal(SIGINT, SignalForward);
      signal(SIGPIPE, SignalForward);
      signal(SIGTERM, SignalForward);
      signal(SIGHUP, SignalForward);
      signal(SIGUSR1, SignalForward);
      signal(SIGUSR2, SignalForward);

      ++proc_num;
    }

    int exit_status = 0;
    pid_t ret = waitpid(pid, &exit_status, 0);
    if (ret == -1) continue;

    if (WIFEXITED(exit_status)) {
      if (WEXITSTATUS(exit_status) == 0) return kExitOk;

      signal(SIGINT, SIG_DFL);
      signal(SIGHUP, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      sleep(1);
      --proc_num;
    } else if (WIFSIGNALED(exit_status)) {
      signal(SIGINT, SIG_DFL);
      signal(SIGHUP, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
      sleep(1);
      --proc_num;
    } else if (WIFSTOPPED(exit_status)) {
    }
  }

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _DAEMON_MAIN_TEST_
int main(int argc, char *argv[]) {
  using namespace base;
  fprintf(stderr, "Before daemon, pid:%d\n", getpid());
  Code ret = DaemonAndKeepAlive();
  if (ret != kOk) return EXIT_FAILURE;
  fprintf(stderr, "After daemon, pid:%d\n", getpid());

  sleep(5);
  exit(-1);

  return 0;
}
#endif
