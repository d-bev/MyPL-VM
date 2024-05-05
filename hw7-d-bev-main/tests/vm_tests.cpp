//----------------------------------------------------------------------
// FILE: vm_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic vm tests
//----------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "mypl_exception.h"
#include "vm_frame.h"
#include "vm.h"

using namespace std;


streambuf* stream_buffer;


void change_cout(stringstream& out)
{
  stream_buffer = cout.rdbuf();
  cout.rdbuf(out.rdbuf());
}

void restore_cout()
{
  cout.rdbuf(stream_buffer);
}


//----------------------------------------------------------------------
// Simple getting started tests
//----------------------------------------------------------------------

TEST(BasicVMTest, SingleNOP) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::NOP());
  VM vm;
  vm.add(main);
  vm.run();
}

TEST(BasicVMTest, SingleWrite) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Consts/Vars
//----------------------------------------------------------------------

TEST(BasicVMTest, SinglePop) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::PUSH("green"));  
  main.instructions.push_back(VMInstr::POP());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

TEST(BasicVMTest, StoreAndLoad) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::STORE(0));
  cout << "" << endl;
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;

  //

  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Special instructions
//----------------------------------------------------------------------

TEST(BasicVMTest, Dup) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::DUP());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blueblue", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Ops
//----------------------------------------------------------------------

TEST(BasicVMTest, IntAdd) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(42));
  main.instructions.push_back(VMInstr::PUSH(43));
  main.instructions.push_back(VMInstr::ADD());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("85", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleAdd) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3.50));
  main.instructions.push_back(VMInstr::PUSH(2.25));
  main.instructions.push_back(VMInstr::ADD());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("5.750000", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullAddFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::ADD());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: ADD())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullAddSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::ADD());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: ADD())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntSub) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(15));
  main.instructions.push_back(VMInstr::PUSH(9));
  main.instructions.push_back(VMInstr::SUB());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("6", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleSub) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3.75));
  main.instructions.push_back(VMInstr::PUSH(2.50));
  main.instructions.push_back(VMInstr::SUB());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1.250000", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullSubFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::SUB());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: SUB())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullSubSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::SUB());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: SUB())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntMul) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(15));
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::MUL());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("45", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleMul) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::PUSH(3.0));
  main.instructions.push_back(VMInstr::MUL());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3.750000", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullMulFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::MUL());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: MUL())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullMulSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::MUL());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: MUL())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntDiv) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(16));
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::DIV());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("5", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleDiv) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3.75));
  main.instructions.push_back(VMInstr::PUSH(3.0));
  main.instructions.push_back(VMInstr::DIV());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1.250000", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullDivFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::DIV());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: DIV())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullDivSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(10));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::DIV());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: DIV())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, And) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::AND());
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::AND());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::AND());
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::AND());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());  
  main.instructions.push_back(VMInstr::WRITE());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalsefalsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullAndFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::AND());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: AND())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullAndSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::AND());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: AND())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, Or) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::OR());
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::OR());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::OR());
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::OR());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetruetruetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullOrFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::OR());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: OR())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullOrSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::OR());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: OR())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, Not) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::NOT());
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::NOT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullNot) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::NOT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: NOT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntLessThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleLessThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringLessThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPLT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullLessThanFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPLT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPLT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullLessThanSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPLT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPLT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntLessEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetruetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleLessEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetruetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringLessEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPLE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetruetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullLessEqualFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPLE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPLE())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullLessEqualSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPLE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPLE())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntGreaterThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleGreaterThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringGreaterThan) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPGT());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullGreaterThanFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPGT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPGT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullGreatThanSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPGT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPGT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, IntGreaterEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleGreaterEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH(1.50));
  main.instructions.push_back(VMInstr::PUSH(1.25));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringGreaterEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPGE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullGreaterEqualFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPGE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPGE())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullGreatEqualSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPGE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CMPGE())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(nullptr));  
  main.instructions.push_back(VMInstr::CMPEQ());  
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("true", out.str());
  restore_cout();
}

TEST(BasicVMTest, IntEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.250));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(1.125));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, BoolEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPEQ());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalsetrue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullNotEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::PUSH(nullptr));  
  main.instructions.push_back(VMInstr::CMPNE());  
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("false", out.str());
  restore_cout();
}

TEST(BasicVMTest, IntNotEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, DoubleNotEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(2.125));
  main.instructions.push_back(VMInstr::PUSH(2.250));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(1.125));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, StringNotEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH("abd"));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH("abc"));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

TEST(BasicVMTest, BoolNotEqual) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::PUSH(nullptr));
  main.instructions.push_back(VMInstr::CMPNE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetruefalse", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Jumps
//----------------------------------------------------------------------

TEST(BasicVMTest, JumpForward) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::JMP(3));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("green", out.str());
  restore_cout();
}

TEST(BasicVMTest, JumpFalseForward) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(false));
  main.instructions.push_back(VMInstr::JMPF(4));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("green", out.str());
  restore_cout();
}

TEST(BasicVMTest, JumpFalseNoJump) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::JMPF(4));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicVMTest, JumpBackwards) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(0));       // 0
  main.instructions.push_back(VMInstr::STORE(0));      // 1
  main.instructions.push_back(VMInstr::LOAD(0));       // 2
  main.instructions.push_back(VMInstr::PUSH(2));       // 3
  main.instructions.push_back(VMInstr::CMPLT());       // 4
  main.instructions.push_back(VMInstr::JMPF(15));      // 5
  main.instructions.push_back(VMInstr::PUSH("blue"));  // 6
  main.instructions.push_back(VMInstr::WRITE());       // 7
  main.instructions.push_back(VMInstr::PUSH("green")); // 8
  main.instructions.push_back(VMInstr::WRITE());       // 9
  main.instructions.push_back(VMInstr::LOAD(0));       // 10
  main.instructions.push_back(VMInstr::PUSH(1));       // 11
  main.instructions.push_back(VMInstr::ADD());         // 12
  main.instructions.push_back(VMInstr::STORE(0));      // 13
  main.instructions.push_back(VMInstr::JMP(2));        // 14
  main.instructions.push_back(VMInstr::PUSH("red"));   // 15
  main.instructions.push_back(VMInstr::WRITE());       // 16
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreenbluegreenred", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------

TEST(BasicVMTest, FunctionReturnsConst) {
  VMFrameInfo f {"f", 0};
  f.instructions.push_back(VMInstr::PUSH("blue"));
  f.instructions.push_back(VMInstr::RET());
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::CALL("f"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(f);
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

TEST(BasicVMTest, FunctionReturnsModifiedParam) {
  // int f(int x) {return 4 + x}
  VMFrameInfo f {"f", 1};                      
  f.instructions.push_back(VMInstr::STORE(0)); // x -> var[0]
  f.instructions.push_back(VMInstr::PUSH(4));
  f.instructions.push_back(VMInstr::LOAD(0));  // push x
  f.instructions.push_back(VMInstr::ADD());
  f.instructions.push_back(VMInstr::RET());
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::CALL("f"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(f);
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("7", out.str());
  restore_cout();
}

TEST(BasicVMTest, FunctionWithTwoParamatersSubtracted) {
  // int f(int x, int y) {return x - y}
  VMFrameInfo f {"f", 2};                      
  f.instructions.push_back(VMInstr::STORE(0)); // x -> var[0]
  f.instructions.push_back(VMInstr::STORE(1)); // y -> var[1]
  f.instructions.push_back(VMInstr::LOAD(0));  // push x
  f.instructions.push_back(VMInstr::LOAD(1));  // push y
  f.instructions.push_back(VMInstr::SUB()); // y - x
  f.instructions.push_back(VMInstr::RET()); // return (y-x)
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(4));
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::CALL("f"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(f);
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1", out.str());
  restore_cout();
}

TEST(BasicVMTest, FunctionWithParamatersPrinted) {
  // void f(int x, int y) {print(x) print(y)}
  VMFrameInfo f {"f", 2};                      
  f.instructions.push_back(VMInstr::STORE(0));      // x -> var[0]
  f.instructions.push_back(VMInstr::STORE(1));      // y -> var[1]
  f.instructions.push_back(VMInstr::LOAD(0));       // push x
  f.instructions.push_back(VMInstr::WRITE());       // print(x)
  f.instructions.push_back(VMInstr::LOAD(1));       // push y
  f.instructions.push_back(VMInstr::WRITE());       // print(y)  
  f.instructions.push_back(VMInstr::PUSH(nullptr)); // push null
  f.instructions.push_back(VMInstr::RET());
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::CALL("f"));
  main.instructions.push_back(VMInstr::POP());      // remove nullptr
  VM vm;
  vm.add(f);
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicVMTest, RecursiveSumFunction) {
  VMFrameInfo f {"sum", 1};                      
  f.instructions.push_back(VMInstr::STORE(0));    // x -> var[0]
  f.instructions.push_back(VMInstr::LOAD(0));     // push x
  f.instructions.push_back(VMInstr::PUSH(0));  
  f.instructions.push_back(VMInstr::CMPLE());     // 0 <= x
  f.instructions.push_back(VMInstr::JMPF(7));     // x < 0
  f.instructions.push_back(VMInstr::PUSH(0));
  f.instructions.push_back(VMInstr::RET());       // return 0
  f.instructions.push_back(VMInstr::LOAD(0));     // push x
  f.instructions.push_back(VMInstr::PUSH(1)); 
  f.instructions.push_back(VMInstr::SUB());       // x - 1
  f.instructions.push_back(VMInstr::CALL("sum")); // sum(x-1) 
  f.instructions.push_back(VMInstr::LOAD(0));     // push x
  f.instructions.push_back(VMInstr::ADD());       // sum(x-1) + x
  f.instructions.push_back(VMInstr::RET());       // return sum(x-1) + x  
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::CALL("sum"));
  main.instructions.push_back(VMInstr::WRITE());   
  VM vm;
  vm.add(f);
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("6", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Heap-Related
//----------------------------------------------------------------------

TEST(BasicVMTest, BasicNoFieldStructAlloc) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::ALLOCS());
  main.instructions.push_back(VMInstr::ALLOCS());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::WRITE());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("20242023", out.str());
  restore_cout();
}

TEST(BasicVMTest, OneFieldStructAlloc) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::ALLOCS()); // pushes oid
  main.instructions.push_back(VMInstr::DUP());    // now 2 oid on stack
  main.instructions.push_back(VMInstr::ADDF("field_1"));  // add field 1 to oid
  main.instructions.push_back(VMInstr::DUP());  // again, 2 oid on stack
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::SETF("field_1"));
  main.instructions.push_back(VMInstr::GETF("field_1"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

TEST(BasicVMTest, OneFieldTwoStructAlloc) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::ALLOCS());
  main.instructions.push_back(VMInstr::STORE(0));     // x = oid_1
  main.instructions.push_back(VMInstr::ALLOCS());
  main.instructions.push_back(VMInstr::STORE(1));     // y = oid_1
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::ADDF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(1));
  main.instructions.push_back(VMInstr::ADDF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::SETF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(1));
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::SETF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(0));  
  main.instructions.push_back(VMInstr::GETF("field_1"));
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::LOAD(1));  
  main.instructions.push_back(VMInstr::GETF("field_1"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicVMTest, TwoFieldsOneStructAlloc) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::ALLOCS());
  main.instructions.push_back(VMInstr::STORE(0));     // x = oid_1
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::ADDF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::ADDF("field_2"));
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::SETF("field_1"));
  main.instructions.push_back(VMInstr::LOAD(0));
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::SETF("field_2"));
  main.instructions.push_back(VMInstr::LOAD(0));  
  main.instructions.push_back(VMInstr::GETF("field_1"));
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::LOAD(0));  
  main.instructions.push_back(VMInstr::GETF("field_2"));
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullObjectFieldAccess) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr)); // oid is null
  main.instructions.push_back(VMInstr::GETF("a"));
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: GETF(a))";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullObjectAddField) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr)); // oid is null
  main.instructions.push_back(VMInstr::ADDF("a"));     // add field "a"
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: ADDF(a))";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullObjectFieldSet) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr)); // oid is null
  main.instructions.push_back(VMInstr::PUSH(0));       // value is 0
  main.instructions.push_back(VMInstr::SETF("a"));
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: SETF(a))";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, BasicArrayAlloc) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(10));    // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 10 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::WRITE());    
  main.instructions.push_back(VMInstr::PUSH(" "));  
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(""));    // fill with 5 ""'s
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::WRITE());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023 2024", out.str());
  restore_cout();
}

TEST(BasicVMTest, BasicArrayIndexAccess) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::PUSH(0));     // index
  main.instructions.push_back(VMInstr::GETI());
  main.instructions.push_back(VMInstr::WRITE());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0", out.str());
  restore_cout();
}

TEST(BasicVMTest, BasicArrayIndexUpdate) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::STORE(0));    // x = oid
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(0));     // push index 0
  main.instructions.push_back(VMInstr::PUSH(10));    // push value 10
  main.instructions.push_back(VMInstr::SETI());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(0));     // push index 0
  main.instructions.push_back(VMInstr::GETI());
  main.instructions.push_back(VMInstr::WRITE());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("10", out.str());
  restore_cout();
}

TEST(BasicVMTest, LoopWithDifferentIndexUpdates) {
  VMFrameInfo main {"main", 0};
  main.instructions.push_back(VMInstr::PUSH(3));     // push 3
  main.instructions.push_back(VMInstr::PUSH(0));     // push 0
  main.instructions.push_back(VMInstr::ALLOCA());    // fill with 3 0's
  main.instructions.push_back(VMInstr::STORE(0));    // x = oid
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(0));     // push index 0
  main.instructions.push_back(VMInstr::PUSH(10));    // push value 10
  main.instructions.push_back(VMInstr::SETI());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(1));     // push index 1
  main.instructions.push_back(VMInstr::PUSH(20));    // push value 20
  main.instructions.push_back(VMInstr::SETI());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(2));     // push index 2
  main.instructions.push_back(VMInstr::PUSH(30));    // push value 30
  main.instructions.push_back(VMInstr::SETI());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(0));     // push index 0
  main.instructions.push_back(VMInstr::GETI());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(1));     // push index 0
  main.instructions.push_back(VMInstr::GETI());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::LOAD(0));     // push oid
  main.instructions.push_back(VMInstr::PUSH(2));     // push index 0
  main.instructions.push_back(VMInstr::GETI());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("102030", out.str());
  restore_cout();
}

TEST(BasicVMTest, TooLargeArraySetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::PUSH(5));     // push index 5
  main.instructions.push_back(VMInstr::PUSH(10));    // push value 10
  main.instructions.push_back(VMInstr::SETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds array index ";
    msg += "(in main at 5: SETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, TooSmallArraySetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::PUSH(-1));    // push index -1
  main.instructions.push_back(VMInstr::PUSH(10));    // push value 10
  main.instructions.push_back(VMInstr::SETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds array index ";
    msg += "(in main at 5: SETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullArraySetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr)); // oid is null
  main.instructions.push_back(VMInstr::PUSH(0));       // push index 0
  main.instructions.push_back(VMInstr::PUSH(10));      // push value 10
  main.instructions.push_back(VMInstr::SETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 3: SETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, TooLargeArrayGetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::PUSH(5));     // push index 5
  main.instructions.push_back(VMInstr::GETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds array index ";
    msg += "(in main at 4: GETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, TooSmallArrayGetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(5));     // length
  main.instructions.push_back(VMInstr::PUSH(0));     // fill with 5 0's
  main.instructions.push_back(VMInstr::ALLOCA());
  main.instructions.push_back(VMInstr::PUSH(-1));    // push index -1
  main.instructions.push_back(VMInstr::GETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds array index ";
    msg += "(in main at 4: GETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullArrayGetIndex) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // oid is null
  main.instructions.push_back(VMInstr::PUSH(0));        // push index 0
  main.instructions.push_back(VMInstr::GETI());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: GETI())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

//----------------------------------------------------------------------
// Built-Ins
//----------------------------------------------------------------------

TEST(BasicVMTest, StringLength) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::SLEN());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH("green"));  
  main.instructions.push_back(VMInstr::SLEN());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(""));  
  main.instructions.push_back(VMInstr::SLEN());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("450", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullStringLength) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // string is null
  main.instructions.push_back(VMInstr::SLEN());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: SLEN())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, ArrayLength) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(0));
  main.instructions.push_back(VMInstr::PUSH(""));
  main.instructions.push_back(VMInstr::ALLOCA());  
  main.instructions.push_back(VMInstr::ALEN());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::PUSH(true));
  main.instructions.push_back(VMInstr::ALLOCA());  
  main.instructions.push_back(VMInstr::ALEN());
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(1000));
  main.instructions.push_back(VMInstr::PUSH('a'));
  main.instructions.push_back(VMInstr::ALLOCA());  
  main.instructions.push_back(VMInstr::ALEN());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("031000", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullArrayLength) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // string is null
  main.instructions.push_back(VMInstr::ALEN());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: ALEN())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, GetCharacterFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(0));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(1));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(2));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  main.instructions.push_back(VMInstr::WRITE());
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("blue", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullIndexInGetCharacterFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // index is null
  main.instructions.push_back(VMInstr::PUSH("blue"));  
  main.instructions.push_back(VMInstr::GETC());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: GETC())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullStringInGetCharacterFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(0));  
  main.instructions.push_back(VMInstr::PUSH(nullptr)); // string is null
  main.instructions.push_back(VMInstr::GETC());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: GETC())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, TooLargeGetCharacterFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(4));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds string index ";
    msg += "(in main at 2: GETC())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, TooSmallGetCharacterFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(-1));
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::GETC());  
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: out-of-bounds string index ";
    msg += "(in main at 2: GETC())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}


TEST(BasicVMTest, ToIntFromDouble) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(3.14159));
  main.instructions.push_back(VMInstr::TOINT());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3", out.str());
  restore_cout();
}

TEST(BasicVMTest, ToIntFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("42"));
  main.instructions.push_back(VMInstr::TOINT());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42", out.str());
  restore_cout();
}

TEST(BasicVMTest, BadToIntFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("bad int"));
  main.instructions.push_back(VMInstr::TOINT());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string msg = string(ex.what());
    string err = "VM Error: cannot convert string to int ";
    err += "(in main at 1: TOINT())";
    EXPECT_EQ(err, msg);
  }
  restore_cout();
}

TEST(BasicVMTest, ToDoubleFromInt) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(3));
  main.instructions.push_back(VMInstr::TODBL());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3.000000", out.str());
  restore_cout();
}

TEST(BasicVMTest, ToDoubleFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("3.14159"));
  main.instructions.push_back(VMInstr::TODBL());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3.141590", out.str());
  restore_cout();
}

TEST(BasicVMTest, BadToDoubleFromString) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("bad double"));
  main.instructions.push_back(VMInstr::TODBL());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string msg = string(ex.what());
    string err = "VM Error: cannot convert string to double ";
    err += "(in main at 1: TODBL())";
    EXPECT_EQ(err, msg);
  }
  restore_cout();
}

TEST(BasicVMTest, NullToInt) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // oid is null
  main.instructions.push_back(VMInstr::TOINT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: TOINT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullToDbl) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // oid is null
  main.instructions.push_back(VMInstr::TODBL());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: TODBL())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullToStr) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  // oid is null
  main.instructions.push_back(VMInstr::TOSTR());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 1: TOSTR())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, ConcatStrings) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("blue"));
  main.instructions.push_back(VMInstr::PUSH("green"));
  main.instructions.push_back(VMInstr::CONCAT());
  main.instructions.push_back(VMInstr::WRITE());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicVMTest, NullConcatFirstOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH(nullptr));  
  main.instructions.push_back(VMInstr::PUSH("green"));  
  main.instructions.push_back(VMInstr::CONCAT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CONCAT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

TEST(BasicVMTest, NullConcatSecondOperand) {
  VMFrameInfo main {"main", 0};                      
  main.instructions.push_back(VMInstr::PUSH("blue"));  
  main.instructions.push_back(VMInstr::PUSH(nullptr));  
  main.instructions.push_back(VMInstr::CONCAT());
  VM vm;
  vm.add(main);
  stringstream out;
  change_cout(out);
  try {
    vm.run();
    FAIL();
  } catch(MyPLException& ex) {
    string err = ex.what();
    string msg = "VM Error: null reference ";
    msg += "(in main at 2: CONCAT())";
    EXPECT_EQ(msg, err);
  }
  restore_cout();
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

