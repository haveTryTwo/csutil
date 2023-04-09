// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SIMPLE_REG_H_
#define BASE_SIMPLE_REG_H_

#include <string>

#include <stdint.h>
#include <unistd.h>

#include "base/status.h"

namespace base {

class RegExp {
 public:
  RegExp(const std::string &reg_str);
  ~RegExp();

 public:
  Code Init();
  Code Check(const std::string &str);

 private:
  Code Compile(bool paren, uint32_t *node_start_pos);
  Code ProcessBaranch(uint32_t *node_start_pos);
  Code ProcessBlock(uint32_t *node_start_pos);
  Code ProcessChar(uint32_t *node_start_pos);

  Code InsertNode(uint32_t insert_pos, char opcode);
  Code AppendNode(char opcode, uint32_t *node_start_pos);
  Code AppendChar(char ch);

  Code LinkNode(uint32_t first_node_pos, uint32_t second_node_pos);
  Code LinkOperandNode(uint32_t first_node_pos, uint32_t second_node_pos);
  Code GetFirstEmptyNextNodePos(uint32_t node_pos, uint32_t *first_empty_next_node_pos);
  bool IsRepetitionOp(char reg_ch);
  Code GetOperandPos(uint32_t node_pos, uint32_t *operand_pos);
  Code GetNextNodePos(uint32_t node_pos, uint32_t *next_node_pos);
  Code CheckIfBOL();

  Code Match(uint32_t check_str_index, uint32_t cur_node_pos);

 public:
  Code PrintNfa();
  Code PrintNfa(const std::string &nfa);
  Code GetRegNfa(std::string *reg_nfa);
  Code GetCheckIfBOL(bool *check_if_bol);

 private:
  void PrintRegNodeInfo(const std::string &opcode_info, uint16_t next_pos);
  void PrintString(const char *str, uint32_t *next_pos);

 private:
  std::string reg_str_;
  std::string reg_nfa_;
  uint32_t reg_parse_index_;
  uint32_t paren_num_;
  bool just_check_bol_;

  std::string check_str_;
  uint32_t check_str_index_;
};

#pragma pack(push)
#pragma pack(1)
struct RegNode {
  char opcode;
  uint16_t next_pos;
  char operand[0];
};
#pragma pack(pop)

}  // namespace base

#endif
