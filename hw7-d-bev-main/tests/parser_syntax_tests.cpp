//----------------------------------------------------------------------
// FILE: parser_syntax_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: AST parser tests for syntax (same as HW3's simple_parser_tests)
//----------------------------------------------------------------------

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "mypl_exception.h"
#include "ast.h"
#include "ast_parser.h"

// for debugging as needed
#include <iostream>

using namespace std;


//------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------

string build_string(initializer_list<string> strs)
{
  string result = "";
  for (string s : strs)
    result += s + "\n";
  return result;
}


//------------------------------------------------------------
// Positive Test Cases
//------------------------------------------------------------

TEST(BasicASTParserTests, EmptyInput) {
  stringstream in("");
  ASTParser(Lexer(in)).parse();
}

TEST(BasicASTParserTests, EmptyStruct) {
  stringstream in("struct s {}");
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, EmptyFunction) {
  stringstream in("void f() {}");
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, StructWithBaseTypeVars) {
  stringstream in(build_string({
        "struct my_struct {",
        "  int x1, ",
        "  double x2, ",
        "  bool x3, ",
        "  char x4, ",
        "  string x5",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, StructWithNonBaseTypeVars) {
  stringstream in(build_string({
        "struct my_struct {",
        "  array int x1, ",
        "  my_struct x2, ",
        "  array my_struct x3",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, BaseValueReturn) {
  stringstream in(build_string({
        "int my_fun() {",
        "  return 0",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, NonBaseValueReturn) {
  stringstream in(build_string({
        "int my_fun() {",
        "  return a",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, OneParam) {
  stringstream in(build_string({
        "int my_fun(int x) {",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, BaseTypeParams) {
  stringstream in(build_string({
        "int my_fun(int x1, double x2, bool x3, char x4, string x5) {",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, NonBaseTypeParams) {
  stringstream in(build_string({
        "int my_fun(my_struct x1, array int x2, array my_struct x3) {",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, VarDecls) {
  stringstream in(build_string({
        "void my_fun() {",
        "  int x1 = 0",
        "  double x2 = 0.0",
        "  bool x3 = true",
        "  char x4 = 'a'",
        "  string x5 = \"\"",
        "  array int x1 = new int[10]",
        "  my_struct x2 = null",
        "  array my_struct x3 = new my_struct[5]",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, SimpleIf) {
  stringstream in(build_string({
        "void my_fun() {",
        "  if (true) {}",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, FullIf) {
  stringstream in(build_string({
        "void my_fun() {",
        "  if (true) {x = 1}",
        "  elseif (false) {x = 2}",
        "  elseif (true) {x = 3}",
        "  else {x = 4}",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, SimpleFor) {
  stringstream in(build_string({
        "void my_fun() {",
        "  for (int i = 0; i < 10; i = i + 1) {", 
        "    x = x + 1", 
        "  }", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, SimpleWhile) {
  stringstream in(build_string({
        "void my_fun() {",
        "  while (i < 10) {", 
        "    x = x + 1", 
        "  }", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, PathExpressions) {
  stringstream in(build_string({
        "void my_fun() {",
        "  a.b.c = 0", 
        "  x = a.b.c", 
        "  xs[0] = 5",
        "  x = xs[1]",
        "  y = xs[0].att1.att2[y].att3", 
        "  xs[1].att1.att2[0].att3 = 3", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, FunctionCalls) {
  stringstream in(build_string({
        "void my_fun(int x, int y) {",
        "  z = f()",
        "  z = f(x)", 
        "  z = f(x, y)",
        "  z = f(x, y, z)",
        "  f() f(x) f(x,y) f(x,y,z)", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, LogicalExpressions) {
  stringstream in(build_string({
        "bool my_fun(bool x, bool y) {",
        "  bool z = x and y or true and not false",
        "  z = not (x and y) and not ((x and z) or y)", 
        "  return (x or not y) and (not x or y) and not not (true or true or false)",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, RelationalExpressions) {
  stringstream in(build_string({
        "bool my_fun(int x, int y) {",
        "  bool z = x == y or (x < y) or (x !=y) or (x > y)",
        "  return not (z or x < y or x > y) and ((x == y) or (x != y))",
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, ArithmeticExpressions) {
  stringstream in(build_string({
        "int my_fun(int x, int y) {",
        "  z = x + y - z * u / v",
        "  return ((x + y) / (x - y) + z) / (x * (x - y))", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, NewExpressions) {
  stringstream in(build_string({
        "void my_fun(int z) {",
        "  my_struct x = new my_struct", 
        "  array int y = new int[10]",
        "  y = new int[z * (z-1)]",
        "  array my_struct z = new my_struct[z + 1]"
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, MixOfStatements) {
  stringstream in(build_string({
        "bool my_fun(int z) {",
        "  int x = 1",
        "  x = 2", 
        "  if (odd(x)) {return true}",
        "  while (true) {x = x + 1}", 
        "  for (int i = 0; i < x; i = i + 1) {",
        "    x = x + 2",
        "  }",
        "  f(x, z)", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, NestedStatements) {
  stringstream in(build_string({
        "bool my_fun(int z) {",
        "  if (odd(x)) {", 
        "    while (true) {", 
        "      for (int i = 0; i < x; i = i + 1) {",
        "        x = x + 2 + f(x + z)",
        "      }",
        "    }", 
        "  }", 
        "}"
      }));
  ASTParser(Lexer(in)).parse();  
}

TEST(BasicASTParserTests, MixOfStructAndFun) {
  stringstream in(build_string({
        "struct S1 {int x}",
        "void f1() {}",
        "struct S2 {int y, double z}",
        "int f2(S1 s1, S2 s2) {return s1.x + s2.y}",
        "char f3() {}",
        "struct S3 {}",
        "void main() {}",
      }));
  ASTParser(Lexer(in)).parse();  
}


//------------------------------------------------------------
// Negative Test Cases
//------------------------------------------------------------

TEST(BasicASTParserTests, StatementOutsideFunction) {
  stringstream in("int x1 = 0");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NotStructOrFunction) {
  stringstream in("{}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// structs

TEST(BasicASTParserTests, MissingStructId) {
  stringstream in("struct {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingStructOpen) {
  stringstream in("struct s int x}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingStructClose) {
  stringstream in("struct { int x");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingDataTypeInField) {
  stringstream in("struct my_struct {x, int y}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingIdInField) {
  stringstream in("struct my_struct {int}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingCommaInFields) {
  stringstream in("struct my_struct {int x int y}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, TooManyCommasInFields) {
  stringstream in("struct my_struct {int x, int y,}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// functions

TEST(BasicASTParserTests, MissingFunctionOpenParen) {
  stringstream in("int f) {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingFunctionCloseParen) {
  stringstream in("int f( {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingFunctionOpenBrace) {
  stringstream in("int f() }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingFunctionCloseBrace) {
  stringstream in("int f() {");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingFunctionReturnType) {
  stringstream in("f() {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingTypeInParam) {
  stringstream in("int f(x, int y) {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, MissingIdInParams) {
  stringstream in("int f(int x, int) {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, TooManyCommasInParams) {
  stringstream in("int f(int x, int y,) {}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ReturnWithoutValue) {
  stringstream in("int f(int x) {return}");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// declaration

TEST(BasicASTParserTests, MutipleVarsInVarDecl) {
  stringstream in("void f() { int x x = 0 }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NoVarsInVarDecl) {
  stringstream in("void f() { int = 0 }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NoAssignInVarDecl) {
  stringstream in("void f() { int x }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NoInitInVarDecl) {
  stringstream in("void f() { int x = }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// if

TEST(BasicASTParserTests, NoParensInIf) {
  stringstream in("void f() { if true {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NoExprInIf) {
  stringstream in("void f() { if () {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NoIfBody) {
  stringstream in("void f() { if (true) }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, NonTerminatingIfBody) {
  stringstream in("void f() { if (true) { }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseIfWithNoIf) {
  stringstream in("void f() { elseif (true) {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseIfWithNoCondition) {
  stringstream in("void f() { if (true) {} elseif {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseIfWithEmptyParens) {
  stringstream in("void f() { if (true) {} elseif () {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseIfWithNoBody) {
  stringstream in("void f() { if (true) {} elseif (false) }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseIfWithNonTerminatingBody) {
  stringstream in("void f() { if (true) {} elseif (false) { }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseWithNoIf) {
  stringstream in("void f() { else {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseWithNoBody) {
  stringstream in("void f() { if (true) {} else }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ElseWithNonTerminatingBody) {
  stringstream in("void f() { if (true) {} else { }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// while

TEST(BasicASTParserTests, WhileWithNoCondition) {
  stringstream in("void f() { while {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, WhileWithEmptyCondition) {
  stringstream in("void f() { while () {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, WhileWithNonTerminatingBody) {
  stringstream in("void f() { while (true) { }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// for

TEST(BasicASTParserTests, ForWithNoLoopControl) {
  stringstream in("void f() { for {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithEmptyLoopControl) {
  stringstream in("void f() { for () {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithMissingVarDecl) {
  stringstream in("void f() { for (i < 10; i = i + 1) {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithMissingCondition) {
  stringstream in("void f() { for (int i = 0; i = i + 1) {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithMissingAssign) {
  stringstream in("void f() { for (int i = 0; i < 10) {} }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithMissingBody) {
  stringstream in("void f() { for (int i = 0; i < 10; i = i + 1) }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, ForWithNonTerminatedBody) {
  stringstream in("void f() { for (int i = 0; i < 10; i = i + 1) { }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}


// assign

TEST(BasicASTParserTests, AssignWithMissingExpression) {
  stringstream in("void f() { i = }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicASTParserTests, AssignWithAssignOp) {
  stringstream in("void f() { i 0 }");
  try {
    ASTParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}


//----------------------------------------------------------------------
// TODO: Add in your 5 tests from HW-3
//----------------------------------------------------------------------

// expressions


//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

