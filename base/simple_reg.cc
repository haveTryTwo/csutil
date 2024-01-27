// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/simple_reg.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "base/log.h"

namespace base {

enum OpCode { /*{{{*/
              kInvalid = 0,
              kEnd = 1,
              kBOL = 2,
              kEOL = 3,
              kAny = 4,
              kAnyOf = 5,
              kAnyExcept = 6,
              kBranch = 7,
              kBack = 8,
              kNothing = 9,
              kPrecise = 10,
              kParenStart = 11,
              kParenEnd = 12,
}; /*}}}*/

const uint32_t kStartValidPos = sizeof(RegNode);
const uint32_t kMaxRegNfaLen = 0xffff;
const uint32_t kMaxParenNum = 20;
const std::string kMetaOperators = "^$()[*+?|.\\";

RegExp::RegExp(const std::string &reg_str) : reg_str_(reg_str) { /*{{{*/
  reg_parse_index_ = 0;
  paren_num_ = 0;
  just_check_bol_ = false;

} /*}}}*/

RegExp::~RegExp() { /*{{{*/
} /*}}}*/

Code RegExp::Init() { /*{{{*/
  uint32_t invalid_node_start_pos = 0;
  Code ret = AppendNode(kInvalid, &invalid_node_start_pos);
  if (ret != base::kOk) return ret;

  uint32_t cur_node_start_pos = 0;
  ret = Compile(false, &cur_node_start_pos);
  if (ret != kOk) return ret;

  ret = CheckIfBOL();
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

Code RegExp::Check(const std::string &str) { /*{{{*/
  check_str_ = str;

  Code ret = kOk;
  uint32_t cur_node_pos = kStartValidPos;
  if (just_check_bol_) {
    ret = Match(0, cur_node_pos);
    return ret;
  }

  for (uint32_t check_index = 0; check_index < check_str_.size(); ++check_index) {
    ret = Match(check_index, cur_node_pos);
    if (ret == kOk) return ret;
    if (ret != kRegNotMatch) return ret;
  }

  return ret;
} /*}}}*/

Code RegExp::Match(uint32_t check_str_index, uint32_t cur_node_pos) { /*{{{*/
  if (cur_node_pos < kStartValidPos) return kInvalidParam;

  Code ret = kOk;
  uint32_t next_node_start_pos = 0;
  bool no_next_node = false;
  while (true) { /*{{{*/
    ret = GetNextNodePos(cur_node_pos, &next_node_start_pos);
    if (ret == kNoNextNode) {
      no_next_node = true;
    } else if (ret != kOk) {
      return ret;
    }

    if (cur_node_pos > reg_nfa_.size()) {
      LOG_ERR("cur_node_pos:%u, reg_nfa_.size:%zu\n", cur_node_pos, reg_nfa_.size());
      return kInvalidRange;
    }
    const RegNode *cur_node = (const RegNode *)(reg_nfa_.data() + cur_node_pos);
    switch (cur_node->opcode) { /*{{{*/
      case kBOL:
        if (check_str_index != 0) return kRegNotMatch;
        break;
      case kEOL:
        if (check_str_index != check_str_.size()) return kRegNotMatch;
        break;
      case kAny:
        if (check_str_index >= check_str_.size()) return kRegNotMatch;
        check_str_index++;
        break;
      case kAnyOf: { /*{{{*/
        uint32_t cur_node_operand_pos = 0;
        ret = GetOperandPos(cur_node_pos, &cur_node_operand_pos);
        if (ret != kOk) return ret;

        if (cur_node_operand_pos > reg_nfa_.size()) {
          LOG_ERR("cur_node_operand_pos:%u, reg_nfa_.size:%zu\n", cur_node_operand_pos,
                  reg_nfa_.size());
          return kInvalidRange;
        }
        if (check_str_index > check_str_.size()) {
          LOG_ERR("check_str_index:%u, check_str_.size:%zu\n", check_str_index, check_str_.size());
          return kInvalidRange;
        }

        const char *locate_pointer =
            strchr(reg_nfa_.data() + cur_node_operand_pos, *(check_str_.data() + check_str_index));
        if (locate_pointer == NULL) return kRegNotMatch;

        check_str_index++;
      } /*}}}*/
      break;
      case kAnyExcept: { /*{{{*/
        uint32_t cur_node_operand_pos = 0;
        ret = GetOperandPos(cur_node_pos, &cur_node_operand_pos);
        if (ret != kOk) return ret;

        if (cur_node_operand_pos > reg_nfa_.size()) {
          LOG_ERR("cur_node_operand_pos:%u, reg_nfa_.size:%zu\n", cur_node_operand_pos,
                  reg_nfa_.size());
          return kInvalidRange;
        }
        if (check_str_index > check_str_.size()) {
          LOG_ERR("check_str_index:%u, check_str_.size:%zu\n", check_str_index, check_str_.size());
          return kInvalidRange;
        }
        const char *locate_pointer =
            strchr(reg_nfa_.data() + cur_node_operand_pos, *(check_str_.data() + check_str_index));
        if (locate_pointer != NULL) return kRegNotMatch;

        check_str_index++;
      } /*}}}*/
      break;
      case kPrecise: { /*{{{*/
        uint32_t cur_node_operand_pos = 0;
        ret = GetOperandPos(cur_node_pos, &cur_node_operand_pos);
        if (ret != kOk) return ret;

        if (cur_node_operand_pos > reg_nfa_.size()) {
          LOG_ERR("cur_node_operand_pos:%u, reg_nfa_.size:%zu\n", cur_node_operand_pos,
                  reg_nfa_.size());
          return kInvalidRange;
        }
        size_t cur_precise_size = strlen(reg_nfa_.data() + cur_node_operand_pos);

        if (check_str_index > check_str_.size()) {
          LOG_ERR("check_str_index:%u, check_str_.size:%zu\n", check_str_index, check_str_.size());
          return kInvalidRange;
        }
        size_t cur_check_str_left_size = check_str_.size() - check_str_index;
        if (cur_check_str_left_size < cur_precise_size) return kRegNotMatch;

        int equal_flag = memcmp(reg_nfa_.data() + cur_node_operand_pos,
                                check_str_.data() + check_str_index, cur_precise_size);
        if (equal_flag != 0) return kRegNotMatch;

        check_str_index += cur_precise_size;
      } /*}}}*/
      break;
      case kNothing:
        break;
      case kBack:
        break;
      case kParenStart:
      case kParenEnd:
        if (no_next_node) return kInvalidRegNfa;
        ret = Match(check_str_index, next_node_start_pos);
        return ret;
        break;
      case kBranch: { /*{{{*/
        if (no_next_node) return kInvalidRegNfa;

        if (next_node_start_pos > reg_nfa_.size()) {
          LOG_ERR("cur_node_start_pos:%u, reg_nfa_.size:%zu\n", next_node_start_pos,
                  reg_nfa_.size());
          return kInvalidRange;
        }
        const RegNode *next_node = (const RegNode *)(reg_nfa_.data() + next_node_start_pos);
        if (next_node->opcode != kBranch) {
          ret = GetOperandPos(cur_node_pos, &next_node_start_pos);
          if (ret != kOk) return ret;
          break;
        } else { /*{{{*/
          while (true) {
            uint32_t tmp_operand_pos = 0;
            ret = GetOperandPos(cur_node_pos, &tmp_operand_pos);
            if (ret != kOk) return ret;

            ret = Match(check_str_index, tmp_operand_pos);
            if (ret == kOk) return ret;
            if (ret != kRegNotMatch) return ret;

            cur_node_pos = next_node_start_pos;
            ret = GetNextNodePos(cur_node_pos, &next_node_start_pos);
            if (ret != kOk) return ret;  // TODO: kNoNextNode

            if (next_node_start_pos > reg_nfa_.size()) {
              LOG_ERR("cur_node_start_pos:%u, reg_nfa_.size:%zu\n", next_node_start_pos,
                      reg_nfa_.size());
              return kInvalidRange;
            }
            next_node = (const RegNode *)(reg_nfa_.data() + next_node_start_pos);
            if (next_node->opcode != kBranch) {
              ret = GetOperandPos(cur_node_pos, &next_node_start_pos);
              if (ret != kOk) return ret;
              break;
            }
          }
          break;
        } /*}}}*/
      }   /*}}}*/
      break;
      case kEnd:
        return kOk;
        break;
      default:
        return kRegNotMatch;
        break;
    } /*}}}*/

    if (no_next_node) break;
    cur_node_pos = next_node_start_pos;
  } /*}}}*/

  return kRegNotMatch;
} /*}}}*/

Code RegExp::Compile(bool paren, uint32_t *node_start_pos) { /*{{{*/
  if (node_start_pos == NULL) return kInvalidParam;

  Code ret = kOk;
  uint32_t paren_node_start_pos = 0;
  if (paren) {
    paren_num_++;
    if (paren_num_ >= kMaxParenNum) return kParenNumExceed;

    ret = AppendNode(kParenStart, &paren_node_start_pos);
    if (ret != kOk) return ret;
  }

  uint32_t branch_node_start_pos = 0;
  ret = ProcessBaranch(&branch_node_start_pos);
  if (ret != base::kOk) return ret;

  uint32_t cur_node_start_pos = branch_node_start_pos;
  if (paren) {
    ret = LinkNode(paren_node_start_pos, branch_node_start_pos);
    if (ret != kOk) return ret;

    cur_node_start_pos = paren_node_start_pos;
  }

  char ch = *(reg_str_.data() + reg_parse_index_);
  while (ch == '|' && reg_parse_index_ < reg_str_.size()) {
    reg_parse_index_++;
    uint32_t new_branch_node_start_pos = 0;
    ret = ProcessBaranch(&new_branch_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, new_branch_node_start_pos);
    if (ret != kOk) return ret;

    ch = *(reg_str_.data() + reg_parse_index_);
  }

  uint32_t end_node_start_pos = 0;
  if (paren) {
    ret = AppendNode(kParenEnd, &end_node_start_pos);
    if (ret != kOk) return ret;
    ret = LinkNode(cur_node_start_pos, end_node_start_pos);
  } else {
    ret = AppendNode(kEnd, &end_node_start_pos);
    if (ret != kOk) return ret;
    ret = LinkNode(cur_node_start_pos, end_node_start_pos);
  }

  uint32_t tmp_cur_node_start_pos = cur_node_start_pos;
  uint32_t next_node_start_pos = 0;
  while (true) {
    ret = GetNextNodePos(tmp_cur_node_start_pos, &next_node_start_pos);
    if (ret == kNoNextNode) {
      ret = kOk;
      break;
    }
    if (ret != kOk) return ret;

    LinkOperandNode(tmp_cur_node_start_pos, end_node_start_pos);

    tmp_cur_node_start_pos = next_node_start_pos;
  }

  if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
  ch = *(reg_str_.data() + reg_parse_index_);
  if (paren) {
    if (ch != ')') return kInvalidMetaOperator;
    reg_parse_index_++;
  } else {
    if (ch != '\0') return kInvalidRegEnd;
  }

  *node_start_pos = cur_node_start_pos;

  return ret;
} /*}}}*/

Code RegExp::ProcessBaranch(uint32_t *node_start_pos) { /*{{{*/
  if (node_start_pos == NULL) return kInvalidParam;

  uint32_t branch_node_start_pos = 0;
  Code ret = AppendNode(kBranch, &branch_node_start_pos);
  if (ret != base::kOk) return ret;

  uint32_t pre_node_start_pos = 0;
  uint32_t cur_node_start_pos = 0;
  char ch = *(reg_str_.data() + reg_parse_index_);
  while (ch != '\0' && ch != '|' && ch != ')' && (reg_parse_index_ < reg_str_.size())) {
    ret = ProcessBlock(&cur_node_start_pos);
    if (ret != base::kOk) return ret;

    if (pre_node_start_pos != 0) {
      ret = LinkNode(pre_node_start_pos, cur_node_start_pos);
      if (ret != base::kOk) return ret;
    }

    pre_node_start_pos = cur_node_start_pos;

    ch = *(reg_str_.data() + reg_parse_index_);
  }

  if (pre_node_start_pos == 0) {
    uint32_t nothing_node_start_pos = 0;
    ret = AppendNode(kNothing, &nothing_node_start_pos);
    if (ret != kOk) return ret;
  }

  *node_start_pos = branch_node_start_pos;
  return ret;
} /*}}}*/

Code RegExp::ProcessBlock(uint32_t *node_start_pos) { /*{{{*/
  if (node_start_pos == NULL) return kInvalidParam;

  uint32_t cur_node_start_pos = 0;
  Code ret = ProcessChar(&cur_node_start_pos);
  if (ret != kOk) return ret;

  if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
  char ch = *(reg_str_.data() + reg_parse_index_);
  if (!IsRepetitionOp(ch)) {
    *node_start_pos = cur_node_start_pos;
    return ret;
  }

  // repetition operator
  if (ch == '*') { /*{{{*/
    ret = InsertNode(cur_node_start_pos, kBranch);
    if (ret != kOk) return ret;

    uint32_t back_node_start_pos = 0;
    ret = AppendNode(kBack, &back_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkOperandNode(cur_node_start_pos, back_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkOperandNode(cur_node_start_pos, cur_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t new_branch_node_start_pos = 0;
    ret = AppendNode(kBranch, &new_branch_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, new_branch_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t nothing_node_start_pos = 0;
    ret = AppendNode(kNothing, &nothing_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, nothing_node_start_pos);
    if (ret != kOk) return ret;
  }                     /*}}}*/
  else if (ch == '+') { /*{{{*/
    uint32_t cur_branch_node_start_pos = 0;
    ret = AppendNode(kBranch, &cur_branch_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, cur_branch_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t back_node_start_pos = 0;
    ret = AppendNode(kBack, &back_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(back_node_start_pos, cur_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t new_branch_node_start_pos = 0;
    ret = AppendNode(kBranch, &new_branch_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_branch_node_start_pos, new_branch_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t nothing_node_start_pos = 0;
    ret = AppendNode(kNothing, &nothing_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(new_branch_node_start_pos, nothing_node_start_pos);
    if (ret != kOk) return ret;
  }                     /*}}}*/
  else if (ch == '?') { /*{{{*/
    ret = InsertNode(cur_node_start_pos, kBranch);
    if (ret != kOk) return ret;

    uint32_t new_branch_node_start_pos = 0;
    ret = AppendNode(kBranch, &new_branch_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, new_branch_node_start_pos);
    if (ret != kOk) return ret;

    uint32_t nothing_node_start_pos = 0;
    ret = AppendNode(kNothing, &nothing_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkNode(cur_node_start_pos, nothing_node_start_pos);
    if (ret != kOk) return ret;

    ret = LinkOperandNode(cur_node_start_pos, nothing_node_start_pos);
    if (ret != kOk) return ret;
  } /*}}}*/

  reg_parse_index_++;
  if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
  ch = *(reg_str_.data() + reg_parse_index_);
  if (IsRepetitionOp(ch)) return kInvalidRegExp;

  *node_start_pos = cur_node_start_pos;

  return ret;
} /*}}}*/

Code RegExp::ProcessChar(uint32_t *node_start_pos) { /*{{{*/
  if (node_start_pos == NULL) return kInvalidParam;

  if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
  char ch = *(reg_str_.data() + reg_parse_index_);
  reg_parse_index_++;

  uint32_t cur_node_start_pos = 0;
  Code ret = kOk;
  switch (ch) { /*{{{*/
    case '^':
      ret = AppendNode(kBOL, &cur_node_start_pos);
      if (ret != kOk) return ret;
      break;
    case '$':
      ret = AppendNode(kEOL, &cur_node_start_pos);
      if (ret != kOk) return ret;
      break;
    case '.':
      ret = AppendNode(kAny, &cur_node_start_pos);
      if (ret != kOk) return ret;
      break;
    case '[': { /*{{{*/
      if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
      ch = *(reg_str_.data() + reg_parse_index_);
      if (ch == '^') {
        ret = AppendNode(kAnyExcept, &cur_node_start_pos);
        if (ret != kOk) return ret;
        reg_parse_index_++;
      } else {
        ret = AppendNode(kAnyOf, &cur_node_start_pos);
        if (ret != kOk) return ret;
      }

      if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
      ch = *(reg_str_.data() + reg_parse_index_);
      if (ch == ']' || ch == '-') {
        ret = AppendChar(ch);
        if (ret != kOk) return ret;
        reg_parse_index_++;
      }

      if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
      ch = *(reg_str_.data() + reg_parse_index_);
      reg_parse_index_++;
      while (ch != '\0' && ch != ']' && (reg_parse_index_ < reg_str_.size())) {
        if (ch != '-') {
          ret = AppendChar(ch);
          if (ret != kOk) return ret;
        } else if ((ch = *(reg_str_.data() + reg_parse_index_)) == ']' || ch == '\0') {
          ret = AppendChar('-');
          if (ret != kOk) return ret;
        } else {
          char start_ch = *(reg_str_.data() + reg_parse_index_ - 2);
          char end_ch = ch;
          if (start_ch > end_ch) return kInvalidRange;
          for (char in_ch = start_ch + 1; in_ch <= end_ch; ++in_ch) {
            ret = AppendChar(in_ch);
            if (ret != kOk) return ret;
          }
          reg_parse_index_++;
        }

        if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
        ch = *(reg_str_.data() + reg_parse_index_);
        reg_parse_index_++;
      }
      if (ch != ']') return kInvalidInBracket;

      ret = AppendChar('\0');
      if (ret != kOk) return ret;

      break;
    } /*}}}*/
    case '(':
      ret = Compile(true, &cur_node_start_pos);
      if (ret != kOk) return ret;
      break;
    case ')':
    case '|':
    case '*':
    case '+':
    case '?':
      return kInvalidMetaOperator;
      break;
    case '\0':
      return kInvalidRegEnd;
      break;
    case '\\':
      if (reg_parse_index_ > reg_str_.size()) return kInvalidRange;
      ch = *(reg_str_.data() + reg_parse_index_);
      if (ch == '\0') return kInvalidRegEnd;
      ret = AppendNode(kPrecise, &cur_node_start_pos);
      if (ret != kOk) return ret;

      ret = AppendChar(ch);
      if (ret != kOk) return ret;
      ret = AppendChar('\0');
      if (ret != kOk) return ret;

      reg_parse_index_++;
      break;
    default: {
      if (reg_parse_index_ == 0) return kInvalidRange;
      reg_parse_index_--;
      size_t precise_str_len = strcspn(reg_str_.data() + reg_parse_index_, kMetaOperators.c_str());
      if (precise_str_len == 0) return kInvalidRegExp;

      uint32_t precise_index_end = reg_parse_index_ + precise_str_len;
      if (precise_str_len > 1 && IsRepetitionOp(*(reg_str_.data() + precise_index_end)))
        precise_str_len--;

      ret = AppendNode(kPrecise, &cur_node_start_pos);
      if (ret != kOk) return ret;
      while (precise_str_len > 0) {
        ret = AppendChar(*(reg_str_.data() + reg_parse_index_));
        if (ret != kOk) return ret;

        reg_parse_index_++;
        precise_str_len--;
      }
      ret = AppendChar('\0');
      if (ret != kOk) return ret;
      break;
    }
  } /*}}}*/

  *node_start_pos = cur_node_start_pos;
  return ret;
} /*}}}*/

Code RegExp::InsertNode(uint32_t insert_pos, char opcode) { /*{{{*/
  if (insert_pos < kStartValidPos) return kInvalidParam;

  RegNode insert_node = {opcode, 0};
  reg_nfa_.insert(insert_pos, (const char *)(&insert_node), sizeof(insert_node));

  return kOk;
} /*}}}*/

Code RegExp::AppendNode(char opcode, uint32_t *node_start_pos) { /*{{{*/
  if (node_start_pos == NULL) return kInvalidParam;

  RegNode reg_node;
  reg_node.opcode = opcode;
  reg_node.next_pos = 0;

  *node_start_pos = reg_nfa_.size();
  reg_nfa_.append((const char *)(void *)&reg_node, sizeof(reg_node));

  if (reg_nfa_.size() >= kMaxRegNfaLen) return kInvalidLength;

  return base::kOk;
} /*}}}*/

Code RegExp::AppendChar(char ch) { /*{{{*/
  reg_nfa_.append(1, ch);
  return kOk;
} /*}}}*/

Code RegExp::LinkNode(uint32_t first_node_pos, uint32_t second_node_pos) { /*{{{*/
  uint32_t first_empty_next_node_pos = 0;
  Code ret = GetFirstEmptyNextNodePos(first_node_pos, &first_empty_next_node_pos);
  if (ret != kOk) return ret;
  const RegNode *first_empty_next_node =
      (const RegNode *)(reg_nfa_.data() + first_empty_next_node_pos);

  uint32_t distance = 0;
  const RegNode *second_node = (const RegNode *)(reg_nfa_.data() + second_node_pos);
  if (first_empty_next_node->opcode == kBack) {
    if ((const char *)first_empty_next_node <= (const char *)second_node) return kInvalidRange;
    distance = (const char *)first_empty_next_node - (const char *)second_node;
  } else {
    if ((const char *)second_node <= (const char *)first_empty_next_node) return kInvalidRange;
    distance = (const char *)second_node - (const char *)first_empty_next_node;
  }

  (const_cast<RegNode *>(first_empty_next_node))->next_pos = distance;

  return base::kOk;
} /*}}}*/

Code RegExp::LinkOperandNode(uint32_t first_node_pos, uint32_t second_node_pos) { /*{{{*/
  const RegNode *first_node = (const RegNode *)(reg_nfa_.data() + first_node_pos);
  if (first_node->opcode != kBranch) return kOk;

  uint32_t operand_pos = 0;
  Code ret = GetOperandPos(first_node_pos, &operand_pos);
  if (ret != kOk) return ret;

  return LinkNode(operand_pos, second_node_pos);
} /*}}}*/

Code RegExp::GetFirstEmptyNextNodePos(uint32_t node_pos,
                                      uint32_t *first_empty_next_node_pos) { /*{{{*/
  if (first_empty_next_node_pos == NULL) return kInvalidParam;

  const RegNode *first_empty_next_node = (const RegNode *)(reg_nfa_.data() + node_pos);
  while (first_empty_next_node->next_pos != 0) {
    if (first_empty_next_node->opcode == kBack) {
      first_empty_next_node =
          (const RegNode *)((const char *)first_empty_next_node - first_empty_next_node->next_pos);
    } else {
      first_empty_next_node =
          (const RegNode *)((const char *)first_empty_next_node + first_empty_next_node->next_pos);
    }
  }
  *first_empty_next_node_pos = (const char *)first_empty_next_node - reg_nfa_.data();

  return kOk;
} /*}}}*/

bool RegExp::IsRepetitionOp(char reg_ch) { /*{{{*/
  return (reg_ch == '*' || reg_ch == '+' || reg_ch == '?');
} /*}}}*/

Code RegExp::GetOperandPos(uint32_t node_pos, uint32_t *operand_pos) { /*{{{*/
  if (node_pos < kStartValidPos || operand_pos == NULL) return kInvalidParam;

  *operand_pos = node_pos + sizeof(RegNode);

  return kOk;
} /*}}}*/

Code RegExp::GetNextNodePos(uint32_t node_pos, uint32_t *next_node_pos) { /*{{{*/
  if (next_node_pos == NULL) return kInvalidParam;
  if (node_pos > reg_nfa_.size()) return kInvalidRange;

  uint32_t tmp_next_node_pos = 0;
  const RegNode *cur_node = (const RegNode *)(reg_nfa_.data() + node_pos);
  if (cur_node->next_pos == 0) return kNoNextNode;

  if (cur_node->opcode == kBack) {
    if (node_pos <= cur_node->next_pos) return kInvalidRange;
    tmp_next_node_pos = node_pos - cur_node->next_pos;
  } else {
    tmp_next_node_pos = node_pos + cur_node->next_pos;
  }
  *next_node_pos = tmp_next_node_pos;

  return kOk;
} /*}}}*/

Code RegExp::CheckIfBOL() { /*{{{*/
  if (reg_nfa_.size() < kStartValidPos) return kNotInit;

  Code ret = kOk;
  const RegNode *reg_node = (const RegNode *)(reg_nfa_.data() + kStartValidPos);
  if (reg_node->opcode == kBranch && reg_node->next_pos != 0) {
    uint32_t next_node_pos = kStartValidPos + reg_node->next_pos;
    if (next_node_pos > reg_nfa_.size()) return kInvalidRange;
    const RegNode *next_reg_node = (const RegNode *)(reg_nfa_.data() + next_node_pos);
    if (next_reg_node->opcode == kEnd) {
      uint32_t next_opcode_node_pos = 0;
      ret = GetOperandPos(kStartValidPos, &next_opcode_node_pos);
      if (ret != kOk) return ret;

      if (next_opcode_node_pos > reg_nfa_.size()) return kInvalidRange;
      const RegNode *next_opcode_node = (const RegNode *)(reg_nfa_.data() + next_opcode_node_pos);
      if (next_opcode_node->opcode == kBOL) {
        just_check_bol_ = true;
      }
    }
  }

  return ret;
} /*}}}*/

Code RegExp::PrintNfa() { /*{{{*/
  if (reg_nfa_.size() < kStartValidPos) return kNotInit;

  return PrintNfa(reg_nfa_);
} /*}}}*/

Code RegExp::PrintNfa(const std::string &nfa) { /*{{{*/
  if (nfa.size() < kStartValidPos) return kNotInit;

  Code ret = kOk;
  uint32_t start_pos = 0;
  while (start_pos < nfa.size()) { /*{{{*/
    const RegNode *reg_node = (const RegNode *)(nfa.data() + start_pos);
    start_pos += sizeof(RegNode);

    switch (reg_node->opcode) {
      case kInvalid:
        PrintRegNodeInfo("kInvalid", reg_node->next_pos);
        break;
      case kBOL:
        PrintRegNodeInfo("kBOL", reg_node->next_pos);
        break;
      case kEOL:
        PrintRegNodeInfo("kEOL", reg_node->next_pos);
        break;
      case kAny:
        PrintRegNodeInfo("kAny", reg_node->next_pos);
        break;
      case kAnyOf:
        PrintRegNodeInfo("kAnyOf", reg_node->next_pos);
        PrintString(reg_nfa_.data() + start_pos, &start_pos);
        break;
      case kAnyExcept:
        PrintRegNodeInfo("kAnyExcept", reg_node->next_pos);
        PrintString(reg_nfa_.data() + start_pos, &start_pos);
        break;
      case kParenStart:
        PrintRegNodeInfo("kParenStart", reg_node->next_pos);
        break;
      case kParenEnd:
        PrintRegNodeInfo("kParenEnd", reg_node->next_pos);
        break;
      case kPrecise:
        PrintRegNodeInfo("kPrecise", reg_node->next_pos);
        PrintString(reg_nfa_.data() + start_pos, &start_pos);
        break;
      case kBranch:
        PrintRegNodeInfo("kBranch", reg_node->next_pos);
        break;
      case kBack:
        PrintRegNodeInfo("kBack", reg_node->next_pos);
        break;
      case kEnd:
        PrintRegNodeInfo("kEnd", reg_node->next_pos);
        break;
      case kNothing:
        PrintRegNodeInfo("kNothing", reg_node->next_pos);
        break;
      default:
        ret = kInvalidRegEnd;
        break;
    }
  } /*}}}*/

  return ret;
} /*}}}*/

Code RegExp::GetRegNfa(std::string *reg_nfa) { /*{{{*/
  if (reg_nfa == NULL) return kInvalidParam;

  *reg_nfa = reg_nfa_;

  return kOk;
} /*}}}*/

Code RegExp::GetCheckIfBOL(bool *check_if_bol) { /*{{{*/
  if (check_if_bol == NULL) return kInvalidParam;

  *check_if_bol = just_check_bol_;
  return kOk;
} /*}}}*/

void RegExp::PrintRegNodeInfo(const std::string &opcode_info, uint16_t next_pos) { /*{{{*/
  LOG_ERR("%s\n", opcode_info.c_str());
  LOG_ERR("0x%02x\n", next_pos & 0x00ff);
  LOG_ERR("0x%02x\n", (next_pos >> 8) & 0x00ff);
} /*}}}*/

void RegExp::PrintString(const char *str, uint32_t *next_pos) { /*{{{*/
  assert(str != NULL && next_pos != NULL);

  const char *tmp_str = str;
  while (*tmp_str != '\0') {
    LOG_ERR("%c\n", *tmp_str);
    tmp_str++;
  }
  LOG_ERR("\\0\n");
  tmp_str++;

  *next_pos += tmp_str - str;
} /*}}}*/

}  // namespace base
