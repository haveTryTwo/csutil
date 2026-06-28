// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/tcp_client.h"

#include <assert.h>
#include <unistd.h>

#include "base/msg.h"

#ifdef _TCP_CLIENT_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  TcpClient tcp_client;
  base::EventType event_type = base::kEPoll;
  base::Code ret = tcp_client.Init(event_type, DefaultProtoFunc);
  assert(ret == base::kOk);

  std::string ip("127.0.0.1");
  uint16_t port = 9090;
  fprintf(stderr, "Start to connect ip:port <%s, %d>\n", ip.c_str(), port);
  ret = tcp_client.Connect(ip, port);
  assert(ret == base::kOk);

  for (int j = 0; j < 2; ++j) {
    // Encode the data and send
    std::string buf_out("hello world");

    for (int i = 0; i < 3; ++i) {
      std::string tmp_str = buf_out + std::to_string(j * 3 + i);
      std::string encode_buf_out;
      ret = EncodeToMsg(tmp_str, &encode_buf_out);
      assert(ret == kOk);

      fprintf(stderr, "buf out:%s\n", tmp_str.c_str());
      ret = tcp_client.SendNative(encode_buf_out);
      if (ret != kOk) {
        fprintf(stderr, "Failed to send native! ret:%d\n", (int)ret);
        return ret;
      }
    }

    // Receive the data and decode
    std::string buf_in;
    std::string decode_buf_in;

    for (int i = 0; i < 3; ++i) {
      ret = tcp_client.Recv(&buf_in);
      assert(ret == base::kOk);
      ret = DecodeFromMsg(buf_in, &decode_buf_in);
      assert(ret == kOk);
      fprintf(stderr, "buf in:%s\n", decode_buf_in.c_str());
    }

    fprintf(stderr, "sleep 10s, then again\n");
    sleep(10);
  }

  return 0;
} /*}}}*/
#endif
