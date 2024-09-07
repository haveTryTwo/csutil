// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include <assert.h>
#include <stdio.h>

#include "base/common.h"
#include "base/file_util.h"
#include "base/util.h"

#include "create_cc_file.h"

namespace tools {
base::Code CreateCCFile(const std::string &file_name, const std::string &func_name) {
  if (file_name.empty() || func_name.empty()) return base::kInvalidParam;

  // Note: create file of cplusplus
  char buf[base::kBufLen] = {0};
  int ret = snprintf(buf, sizeof(buf) / sizeof(buf[0]),
                     "#include <stdio.h>\n"
                     "\n"
                     "int %s() {\n"
                     "  return 0;\n"
                     "}\n"
                     "\n"
                     "int main(int argc, char *agrv[]) {\n"
                     "  return %s();\n"
                     "}\n",
                     func_name.c_str(), func_name.c_str());

  std::string src_file_name = file_name + ".cc";
  FILE *src_fp = fopen(src_file_name.c_str(), "w+");
  size_t w_ret = fwrite(buf, sizeof(buf[0]), ret, src_fp);
  if (w_ret != (size_t)ret) {
    fprintf(stderr, "Failed to create file:%s\n", file_name.c_str());
    return base::kWriteError;
  }
  fclose(src_fp);
  src_fp = NULL;

  // Note: create makefile
  ret = snprintf(buf, sizeof(buf) / sizeof(buf[0]),
                 "#File:%s\n"
                 "\n"
                 "#PARAM\n"
                 "CC        = g++\n"
                 "CFLAGS    = -g -c -Wall -std=c++11 -fPIC -D__STDC_FORMAT_MACROS\n"
                 "OBJS      = %s.o\n"
                 "\n"
                 "#.PHONY\n"
                 ".PHONY : all build clean\n"
                 "\n"
                 "all : build clean\n"
                 "\n"
                 "build : $(OBJS)\n"
                 "\t$(CC) -o $@ $^ -lpthread\n"
                 "\n"
                 "clean:\n"
                 "\trm -fr $(OBJS)\n"
                 "\n"
                 "#OBJS\n"
                 "$(OBJS) : %s.o : %s.cc\n"
                 "\t$(CC) $(CFLAGS) -o $@ $<",
                 file_name.c_str(), file_name.c_str(), "%", "%");
  std::string makefile_name = "makefile";
  FILE *makefile_fp = fopen(makefile_name.c_str(), "w+");
  w_ret = fwrite(buf, sizeof(buf[0]), ret, makefile_fp);
  if (w_ret != (size_t)ret) {
    fprintf(stderr, "Failed to create file:%s\n", makefile_name.c_str());
    return base::kWriteError;
  }
  fclose(makefile_fp);
  makefile_fp = NULL;

  return base::kOk;
}

}  // namespace tools
