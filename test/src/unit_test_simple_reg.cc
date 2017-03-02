// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <string.h>

#include "base/util.h"
#include "base/status.h"
#include "base/common.h"
#include "base/simple_reg.h"

#include "test_base/include/test_base.h"

enum OpCode
{/*{{{*/
    kInvalid        = 0,
    kEnd            = 1,
    kBOL            = 2,
    kEOL            = 3,
    kAny            = 4,
    kAnyOf          = 5,
    kAnyExcept      = 6,
    kBranch         = 7,
    kBack           = 8,
    kNothing        = 9,
    kPrecise        = 10,
    kParenStart     = 11,
    kParenEnd       = 12,
};/*}}}*/


TEST(RegExp, NodeSize)
{/*{{{*/
    using namespace base;

    fprintf(stderr, "Size of RegNode: %zu\n", sizeof(RegNode));

    RegNode tmp_reg_node;
    fprintf(stderr, "Size of RegNode: %zu\n", sizeof(tmp_reg_node));
}/*}}}*/

TEST(RegExp, NodeData)
{/*{{{*/
    using namespace base;

    RegNode reg_node;
    reg_node.opcode = 1;
    reg_node.next_pos = 0x1122;
    fprintf(stderr, "RegNode opcode:%d, next_pos:0x%x\n", reg_node.opcode, reg_node.next_pos);
    fprintf(stderr, "reg_node pos:%p, opcode pos:%p, next_pos:%p\n", 
            &reg_node, &(reg_node.opcode), &(reg_node.next_pos));
    fprintf(stderr, "reg_node[0]:%02x, [1]:%02x, [2]:%02x\n", 
            *(char*)(&reg_node), *((char*)(&reg_node)+1), *((char*)(&reg_node)+2));

    std::string dump_str;
    dump_str.append((const char*)(void*)&reg_node, sizeof(reg_node));
    fprintf(stderr, "dump_str[0]:0x%02x, [1]:0x%02x, [2]:0x%02x\n", 
            *(dump_str.data()), *(dump_str.data()+1), *(dump_str.data()+2));

    const RegNode* pump_node = (const RegNode*)(dump_str.data());
    fprintf(stderr, "PumpRegNode opcode:%d, next_pos:0x%x\n", pump_node->opcode, pump_node->next_pos);
    (const_cast<RegNode*>(pump_node))->next_pos = 0x3344;
    fprintf(stderr, "PumpRegNode opcode:%d, next_pos:0x%x\n", pump_node->opcode, pump_node->next_pos);
}/*}}}*/

TEST(RegExp, InsertNodeData)
{/*{{{*/
    using namespace base;

    RegNode reg_node;
    reg_node.opcode = 1;
    reg_node.next_pos = 0x1122;

    std::string dump_str;
    dump_str.append((const char*)(void*)&reg_node, sizeof(reg_node));
    dump_str.append((const char*)(void*)&reg_node, sizeof(reg_node));

    for (int i = 0; i < 2; ++i)
    {
        const RegNode* pump_node = (const RegNode*)(dump_str.data() + i*sizeof(RegNode));
        fprintf(stderr, "PumpRegNode opcode:%d, next_pos:0x%x\n", pump_node->opcode, pump_node->next_pos);
    }

    RegNode insert_reg_node = {2, 0x3344};
    dump_str.insert(sizeof(RegNode), (const char*)(&insert_reg_node), sizeof(insert_reg_node));
    for (int i = 0; i < 3; ++i)
    {
        const RegNode* pump_node = (const RegNode*)(dump_str.data() + i*sizeof(RegNode));
        fprintf(stderr, "PumpRegNode opcode:%d, next_pos:0x%x\n", pump_node->opcode, pump_node->next_pos);
    }
}/*}}}*/

TEST(RegExp, Normal_NfaPrecise)
{/*{{{*/
    using namespace base;

    std::string reg_str = "(ab)c";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

//    ret = reg_exp.PrintNfa();
//    EXPECT_EQ(kOk, ret);

    std::string manual_reg_nfa;
    RegNode reg_node;

    // kInvalid Node
    reg_node.opcode = kInvalid;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    // kBranch Node
    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x17;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kParenStart;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x09;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'a');
    manual_reg_nfa.append(1, 'b');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kParenEnd;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x05;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'c');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kEnd;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    std::string test_reg_nfa;
    ret = reg_exp.GetRegNfa(&test_reg_nfa);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(manual_reg_nfa, test_reg_nfa);
}/*}}}*/

TEST(RegExp, Normal_NfaStar)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)*|cd$";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

//    ret = reg_exp.PrintNfa();
//    EXPECT_EQ(kOk, ret);

    std::string manual_reg_nfa;
    RegNode reg_node;

    reg_node.opcode = kInvalid;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x21;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBOL;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x15;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kParenStart;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x09;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'a');
    manual_reg_nfa.append(1, 'b');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kParenEnd;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBack;
    reg_node.next_pos = 0x12;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kNothing;
    reg_node.next_pos = 0x0f;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x0c;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'c');
    manual_reg_nfa.append(1, 'd');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kEOL;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kEnd;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    std::string test_reg_nfa;
    ret = reg_exp.GetRegNfa(&test_reg_nfa);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(manual_reg_nfa, test_reg_nfa);
}/*}}}*/

TEST(RegExp, Normal_NfaPlus)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)+|cd$";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

//    ret = reg_exp.PrintNfa();
//    EXPECT_EQ(kOk, ret);

    std::string manual_reg_nfa;
    RegNode reg_node;

    reg_node.opcode = kInvalid;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x21;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBOL;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kParenStart;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x09;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'a');
    manual_reg_nfa.append(1, 'b');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kParenEnd;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBack;
    reg_node.next_pos = 0x12;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kNothing;
    reg_node.next_pos = 0x0f;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x0c;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'c');
    manual_reg_nfa.append(1, 'd');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kEOL;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kEnd;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    std::string test_reg_nfa;
    ret = reg_exp.GetRegNfa(&test_reg_nfa);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(manual_reg_nfa, test_reg_nfa);
}/*}}}*/

TEST(RegExp, Normal_NfaQuestion)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)?|cd$";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

//    ret = reg_exp.PrintNfa();
//    EXPECT_EQ(kOk, ret);

    std::string manual_reg_nfa;
    RegNode reg_node;

    reg_node.opcode = kInvalid;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x1e;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBOL;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x12;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kParenStart;
    reg_node.next_pos = 0x3;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x09;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'a');
    manual_reg_nfa.append(1, 'b');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kParenEnd;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kNothing;
    reg_node.next_pos = 0x0f;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x0c;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kPrecise;
    reg_node.next_pos = 0x06;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));
    manual_reg_nfa.append(1, 'c');
    manual_reg_nfa.append(1, 'd');
    manual_reg_nfa.append(1, '\0');

    reg_node.opcode = kEOL;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kEnd;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    std::string test_reg_nfa;
    ret = reg_exp.GetRegNfa(&test_reg_nfa);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(manual_reg_nfa, test_reg_nfa);
}/*}}}*/

TEST(RegExp, Normal_EmptyRegStr)
{/*{{{*/
    using namespace base;

    std::string reg_str = "";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

//    ret = reg_exp.PrintNfa();
//    EXPECT_EQ(kOk, ret);

    std::string manual_reg_nfa;
    RegNode reg_node;

    // kInvalid Node
    reg_node.opcode = kInvalid;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    // kBranch Node
    reg_node.opcode = kBranch;
    reg_node.next_pos = 0x6;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kNothing;
    reg_node.next_pos = 0x03;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    reg_node.opcode = kEnd;
    reg_node.next_pos = 0x00;
    manual_reg_nfa.append((const char*)(void*)&reg_node, sizeof(reg_node));

    std::string test_reg_nfa;
    ret = reg_exp.GetRegNfa(&test_reg_nfa);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(manual_reg_nfa, test_reg_nfa);
}/*}}}*/

TEST(RegExp, Normal_CheckIsBOL)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^abc";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

    bool check_if_bol = false;
    ret = reg_exp.GetCheckIfBOL(&check_if_bol);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, check_if_bol);
}/*}}}*/

TEST(RegExp, Normal_CheckIsNotBOL)
{/*{{{*/
    using namespace base;

    std::string reg_str = "abc";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

    bool check_if_bol = false;
    ret = reg_exp.GetCheckIfBOL(&check_if_bol);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, check_if_bol);
}/*}}}*/

TEST(RegExp, Normal_CheckIsNotBOLForBranch)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^abc|cd$";
    RegExp reg_exp(reg_str);
    Code ret = reg_exp.Init();
    EXPECT_EQ(kOk, ret);

    bool check_if_bol = false;
    ret = reg_exp.GetCheckIfBOL(&check_if_bol);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, check_if_bol);
}/*}}}*/

TEST(RegExp, Normal_HorizontalLine)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[a-]$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "-";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(RegExp, Normal_JoinerLine)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[a-h1-9A-E]$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "-";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    for (char ch = 'a'; ch <= 'h'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'i'; ch <= 'z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = '1'; ch <= '9'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'A'; ch <= 'E'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'F'; ch <= 'Z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }
}/*}}}*/

TEST(RegExp, Normal_NonBrackets)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[^a-h1-9A-E]$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (char ch = 'a'; ch <= 'h'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'i'; ch <= 'z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = '1'; ch <= '9'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'A'; ch <= 'E'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'F'; ch <= 'Z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/

TEST(RegExp, Normal_PartReg)
{/*{{{*/
    using namespace base;

    std::string reg_str = "test";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "justinesto";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "justintesto";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(RegExp, Normal_PartBranch)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^ab|cd$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "bg";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ffcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "cdcdcdcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(RegExp, Normal_PartBranchRepeated)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)+|cd$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "abababab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ababababc";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "bg";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ffcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "cdcdcdcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(RegExp, Normal_WholePlusRepeated)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)+$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "abababab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ababababc";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "bgcd";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);
}/*}}}*/

TEST(RegExp, Normal_HttpCheck)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^http://([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)+(:[0-9]+)?(/[a-zA-Z0-9]*)*(\\?[a-zA-Z0-9=]*)?$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "htt://zhangd";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.havetrytwo.:8090/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.havetrytwo:aa90/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.sz.gov.cn/cn/hdjl/zxts/dfyj/";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "http://www.havetrytwo:8090/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(RegExp, Normal_Press_OneHoudredThousandTimes)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^http://([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)+(:[0-9]+)?(/[a-zA-Z0-9]*)*(\\?[a-zA-Z0-9=]*)?$";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (uint32_t i = 0; i < kMillion/10; ++i)
    {
        str = "http://www.havetrytwo:8090/index?name=good";
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/

TEST(RegExp, Normal_Press_OneMillionTimes)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[hH][tT][tT][pP]/[0-9]\\.[0-9] [0-9][0-9][0-9] .*";
    RegExp reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (uint32_t i = 0; i < kMillion; ++i)
    {
        str = "http/1.1 200 OK";
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/
