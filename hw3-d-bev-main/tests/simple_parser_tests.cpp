//----------------------------------------------------------------------
// FILE: simple_parser_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic parser tests
//----------------------------------------------------------------------

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "../src/mypl_exception.h"
#include "../src/simple_parser.h"


using namespace std;


// NOTE: To disable a test, you can add DISABLED_ to the front of the test name

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

TEST(BasicSimpleParserTests, EmptyInput) {
  stringstream in("");
  SimpleParser(Lexer(in)).parse();
}

TEST(BasicSimpleParserTests, EmptyStruct) {
  stringstream in("struct s {}");
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, EmptyFunction) {
  stringstream in("void f() {}");
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, StructWithBaseTypeVars) {
  stringstream in(build_string({
        "struct my_struct {",
        "  int x1, ",
        "  double x2, ",
        "  bool x3, ",
        "  char x4, ",
        "  string x5",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, StructWithNonBaseTypeVars) {
  stringstream in(build_string({
        "struct my_struct {",
        "  array int x1, ",
        "  my_struct x2, ",
        "  array my_struct x3",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, BaseValueReturn) {
  stringstream in(build_string({
        "int my_fun() {",
        "  return 0",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, NonBaseValueReturn) {
  stringstream in(build_string({
        "int my_fun() {",
        "  return a",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, OneParam) {
  stringstream in(build_string({
        "int my_fun(int x) {",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, BaseTypeParams) {
  stringstream in(build_string({
        "int my_fun(int x1, double x2, bool x3, char x4, string x5) {",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, NonBaseTypeParams) {
  stringstream in(build_string({
        "int my_fun(my_struct x1, array int x2, array my_struct x3) {",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, VarDecls) {
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
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, SimpleIf) {
  stringstream in(build_string({
        "void my_fun() {",
        "  if (true) {}",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, FullIf) {
  stringstream in(build_string({
        "void my_fun() {",
        "  if (true) {x = 1}",
        "  elseif (false) {x = 2}",
        "  elseif (true) {x = 3}",
        "  else {x = 4}",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, SimpleFor) {
  stringstream in(build_string({
        "void my_fun() {",
        "  for (int i = 0; i < 10; i = i + 1) {", 
        "    x = x + 1", 
        "  }", 
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, SimpleWhile) {
  stringstream in(build_string({
        "void my_fun() {",
        "  while (i < 10) {", 
        "    x = x + 1", 
        "  }", 
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, PathExpressions) {
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
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, FunctionCalls) {
  stringstream in(build_string({
        "void my_fun(int x, int y) {",
        "  z = f()",
        "  z = f(x)", 
        "  z = f(x, y)",
        "  z = f(x, y, z)",
        "  f() f(x) f(x,y) f(x,y,z)", 
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, LogicalExpressions) {
  stringstream in(build_string({
        "bool my_fun(bool x, bool y) {",
        "  bool z = x and y or true and not false",
        "  z = not (x and y) and not ((x and z) or y)", 
        "  return (x or not y) and (not x or y) and not not (true or true or false)",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, RelationalExpressions) {
  stringstream in(build_string({
        "bool my_fun(int x, int y) {",
        "  bool z = x == y or (x < y) or (x !=y) or (x > y)",
        "  return not (z or x < y or x > y) and ((x == y) or (x != y))",
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, ArithmeticExpressions) {
  stringstream in(build_string({
        "int my_fun(int x, int y) {",
        "  z = x + y - z * u / v",
        "  return ((x + y) / (x - y) + z) / (x * (x - y))", 
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, NewExpressions) {
  stringstream in(build_string({
        "void my_fun(int z) {",
        "  my_struct x = new my_struct", 
        "  array int y = new int[10]",
        "  y = new int[z * (z-1)]",
        "  array my_struct z = new my_struct[z + 1]"
        "}"
      }));
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, MixOfStatements) {
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
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, NestedStatements) {
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
  SimpleParser(Lexer(in)).parse();  
}

TEST(BasicSimpleParserTests, MixOfStructAndFun) {
  stringstream in(build_string({
        "struct S1 {int x}",
        "void f1() {}",
        "struct S2 {int y, double z}",
        "int f2(S1 s1, S2 s2) {return s1.x + s2.y}",
        "char f3() {}",
        "struct S3 {}",
        "void main() {}",
      }));
  SimpleParser(Lexer(in)).parse();  
}


//------------------------------------------------------------
// Negative Test Cases
//------------------------------------------------------------

TEST(BasicSimpleParserTests, StatementOutsideFunction) {
  stringstream in("int x1 = 0");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NotStructOrFunction) {
  stringstream in("{}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// structs

TEST(BasicSimpleParserTests, MissingStructId) {
  stringstream in("struct {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingStructOpen) {
  stringstream in("struct s int x}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingStructClose) {
  stringstream in("struct { int x");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingDataTypeInField) {
  stringstream in("struct my_struct {x, int y}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingIdInField) {
  stringstream in("struct my_struct {int}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingCommaInFields) {
  stringstream in("struct my_struct {int x int y}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, TooManyCommasInFields) {
  stringstream in("struct my_struct {int x, int y,}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// functions

TEST(BasicSimpleParserTests, MissingFunctionOpenParen) {
  stringstream in("int f) {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingFunctionCloseParen) {
  stringstream in("int f( {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingFunctionOpenBrace) {
  stringstream in("int f() }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingFunctionCloseBrace) {
  stringstream in("int f() {");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingFunctionReturnType) {
  stringstream in("f() {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingTypeInParam) {
  stringstream in("int f(x, int y) {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, MissingIdInParams) {
  stringstream in("int f(int x, int) {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, TooManyCommasInParams) {
  stringstream in("int f(int x, int y,) {}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ReturnWithoutValue) {
  stringstream in("int f(int x) {return}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// declaration

TEST(BasicSimpleParserTests, MutipleVarsInVarDecl) {
  stringstream in("void f() { int x x = 0 }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NoVarsInVarDecl) {
  stringstream in("void f() { int = 0 }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NoAssignInVarDecl) {
  stringstream in("void f() { int x }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NoInitInVarDecl) {
  stringstream in("void f() { int x = }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// if

TEST(BasicSimpleParserTests, NoParensInIf) {
  stringstream in("void f() { if true {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NoExprInIf) {
  stringstream in("void f() { if () {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NoIfBody) {
  stringstream in("void f() { if (true) }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, NonTerminatingIfBody) {
  stringstream in("void f() { if (true) { }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseIfWithNoIf) {
  stringstream in("void f() { elseif (true) {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseIfWithNoCondition) {
  stringstream in("void f() { if (true) {} elseif {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseIfWithEmptyParens) {
  stringstream in("void f() { if (true) {} elseif () {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseIfWithNoBody) {
  stringstream in("void f() { if (true) {} elseif (false) }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseIfWithNonTerminatingBody) {
  stringstream in("void f() { if (true) {} elseif (false) { }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseWithNoIf) {
  stringstream in("void f() { else {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseWithNoBody) {
  stringstream in("void f() { if (true) {} else }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ElseWithNonTerminatingBody) {
  stringstream in("void f() { if (true) {} else { }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// while

TEST(BasicSimpleParserTests, WhileWithNoCondition) {
  stringstream in("void f() { while {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, WhileWithEmptyCondition) {
  stringstream in("void f() { while () {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, WhileWithNonTerminatingBody) {
  stringstream in("void f() { while (true) { }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

// for

TEST(BasicSimpleParserTests, ForWithNoLoopControl) {
  stringstream in("void f() { for {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithEmptyLoopControl) {
  stringstream in("void f() { for () {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithMissingVarDecl) {
  stringstream in("void f() { for (i < 10; i = i + 1) {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithMissingCondition) {
  stringstream in("void f() { for (int i = 0; i = i + 1) {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithMissingAssign) {
  stringstream in("void f() { for (int i = 0; i < 10) {} }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithMissingBody) {
  stringstream in("void f() { for (int i = 0; i < 10; i = i + 1) }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, ForWithNonTerminatedBody) {
  stringstream in("void f() { for (int i = 0; i < 10; i = i + 1) { }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}


// assign

TEST(BasicSimpleParserTests, AssignWithMissingExpression) {
  stringstream in("void f() { i = }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(BasicSimpleParserTests, AssignWithAssignOp) {
  stringstream in("void f() { i 0 }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}


//----------------------------------------------------------------------
// TODO: Add at least 5 of your own "negative" expression tests below: 
//----------------------------------------------------------------------

TEST(CustomParserTests, BadForLoop){
  stringstream in("for(int i = 0; ; i = 1){ int x = 0 }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(CustomParserTests, BadIf){
  stringstream in("if( x = 2 = false){}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(CustomParserTests, BadIfElse){
  stringstream in("if(x!=3) else {x=4}");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(CustomParserTests, NoReturnFunction){
  stringstream in("void f() { i=0 return }");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

TEST(CustomParserTests, AssignmentBadness){
  stringstream in("void e = 4 = type = banana.bread");
  try {
    SimpleParser(Lexer(in)).parse();
    FAIL();
  }
  catch(MyPLException& e) {
    string msg = e.what();
    ASSERT_EQ("Parser Error: ", msg.substr(0, 14));
  }
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

