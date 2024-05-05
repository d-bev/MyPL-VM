//----------------------------------------------------------------------
// FILE: code_generate_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic code generator tests
//----------------------------------------------------------------------



#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "mypl_exception.h"
#include "lexer.h"
#include "ast_parser.h"
#include "vm.h"
#include "code_generator.h"

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

string build_string(initializer_list<string> strs)
{
  string result = "";
  for (string s : strs)
    result += s + "\n";
  return result;
}


//----------------------------------------------------------------------
// Simple getting started tests
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, EmptyProram) {
  stringstream in("void main() {}");
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  vm.run();
}

TEST(BasicCodeGenTest, SimplePrint) {
  stringstream in("void main() {print(42)}");
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42", out.str());
  restore_cout();
}

//------------------------------------------------------------
// Basic Variables and Assignement
//------------------------------------------------------------

TEST(BasicCodeGenTest, SimpleVariableDecls) {
  stringstream in(build_string({
        "void main() {",
        "  int x1 = 3",
        "  bool x2 = true",
        "  char x3 = 'a'",
        "  double x4 = 2.7",
        "  string x5 = \"abc\"", 
        "  print(x1)",
        "  print(x2)",
        "  print(x3)",
        "  print(x4)",
        "  print(x5)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3truea2.700000abc", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleVariableAssigns) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 3",
        "  print(x)",
        "  x = 4", 
        "  print(x)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("34", out.str());
  restore_cout();
}        

//----------------------------------------------------------------------
// Arithmetic expressions
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, SimpleAdd) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 3 + 4",
        "  double y = 3.25 + 4.5", 
        "  print(x)",
        "  print(' ')",
        "  print(y)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("7 7.750000", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleSub) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 4 - 3",
        "  double y = 4.5 - 3.25", 
        "  print(x)",
        "  print(' ')",
        "  print(y)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("1 1.250000", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleMult) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 4 * 3",
        "  double y = 4.5 * 3.25", 
        "  print(x)",
        "  print(' ')",
        "  print(y)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("12 14.625000", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleDiv) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 9 / 2",
        "  double y = 4.5 / 1.25", 
        "  print(x)",
        "  print(' ')",
        "  print(y)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("4 3.600000", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, LongerArithExpr) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 3 + (6 - 5) + (5 * 2) + (2 / 2)",
        "  print(x)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("15", out.str());
  restore_cout();
}        

//----------------------------------------------------------------------
// Boolean expressions
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, SimpleAnd) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = true and true",
        "  bool x2 = true and false", 
        "  bool x3 = false and true",         
        "  bool x4 = false and false", 
        "  print(x1)",
        "  print(' ')",
        "  print(x2)",
        "  print(' ')",
        "  print(x3)",
        "  print(' ')",
        "  print(x4)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("true false false false", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleOr) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = true or true",
        "  bool x2 = true or false", 
        "  bool x3 = false or true",         
        "  bool x4 = false or false", 
        "  print(x1)",
        "  print(' ')",
        "  print(x2)",
        "  print(' ')",
        "  print(x3)",
        "  print(' ')",
        "  print(x4)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("true true true false", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, SimpleNot) {   // FIXME
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = not true", 
        "  bool x2 = not false", 
        "  print(x1)",
        "  print(' ')",
        "  print(x2)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("false true", out.str());
  restore_cout();
}        

TEST(BasicCodeGenTest, LongerBoolExpr) {  // FIXME
  stringstream in(build_string({
        "void main() {",
        "  int x = true or (true and false) or (false or (true and true))", 
        "  int y = not (true and (false or (true or false)) and true)",
        "  print(x)",
        "  print(' ')",
        "  print(y)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("true false", out.str());
  restore_cout();
}        

//----------------------------------------------------------------------
// Comparison operators
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, TrueNumberComparisons) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = 3 < 4",
        "  bool x2 = 3 <= 4",
        "  bool x3 = 3 <= 3",
        "  bool x4 = 4 > 3",
        "  bool x5 = 4 >= 3",
        "  bool x6 = 3 >= 3",
        "  bool x7 = 3 == 3",
        "  bool x8 = 3 != 4",
        "  print(x1 and x2 and x3 and x4 and x5 and x6 and x7 and x8)",
        "  bool y1 = 3.25 < 4.5",
        "  bool y2 = 3.25 <= 4.5",
        "  bool y3 = 3.25 <= 3.25",
        "  bool y4 = 4.5 > 3.25",
        "  bool y5 = 4.5 >= 3.25",
        "  bool y6 = 3.25 >= 3.25",
        "  bool y7 = 3.25 == 3.25",
        "  bool y8 = 3.25 != 4.5",
        "  print(y1 and y2 and y3 and y4 and y5 and y6 and y7 and y8)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetrue", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, FalseNumberComparisons) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = 4 < 3",
        "  bool x2 = 4 <= 3",
        "  bool x3 = 3 > 4",
        "  bool x4 = 3 >= 4",
        "  bool x5 = 3 == 4",
        "  bool x6 = 3 != 3",
        "  print(x1 or x2 or x3 or x4 or x5 or x6)",
        "  bool y1 = 4.5 < 3.25",
        "  bool y2 = 4.5 <= 3.25",
        "  bool y3 = 3.25 > 4.5",
        "  bool y4 = 3.25 >= 4.5",
        "  bool y5 = 3.25 == 4.5",
        "  bool y6 = 3.25 != 3.25",
        "  print(y1 or y2 or y3 or y4 or y5 or y6)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalse", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, TrueAlphaComparisons) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = 'a' < 'b'",
        "  bool x2 = 'a' <= 'b'",
        "  bool x3 = 'a' <= 'a'",
        "  bool x4 = 'b' > 'a'",
        "  bool x5 = 'b' >= 'a'",
        "  bool x6 = 'a' >= 'a'",
        "  bool x7 = 'a' == 'a'",
        "  bool x8 = 'a' != 'b'",
        "  print(x1 and x2 and x3 and x4 and x5 and x6 and x7 and x8)",
        "  bool y1 = \"aa\" < \"ab\"",
        "  bool y2 = \"aa\" <= \"ab\"",
        "  bool y3 = \"aa\" <= \"aa\"",
        "  bool y4 = \"ab\" > \"aa\"",
        "  bool y5 = \"ab\" >= \"aa\"",
        "  bool y6 = \"aa\" >= \"aa\"",
        "  bool y7 = \"aa\" == \"aa\"",
        "  bool y8 = \"aa\" != \"ab\"",
        "  print(y1 and y2 and y3 and y4 and y5 and y6 and y7 and y8)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("truetrue", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, FalseAlphaComparisons) {
  stringstream in(build_string({
        "void main() {",
        "  bool x1 = 'b' < 'a'",
        "  bool x2 = 'b' <= 'a'",
        "  bool x3 = 'a' > 'b'",
        "  bool x4 = 'a' >= 'b'",
        "  bool x5 = 'a' == 'b'",
        "  bool x6 = 'a' != 'a'",
        "  print(x1 or x2 or x3 or x4 or x5 or x6)",
        "  bool y1 = \"ab\" < \"aa\"",
        "  bool y2 = \"ab\" <= \"aa\"",
        "  bool y3 = \"aa\" > \"ab\"",
        "  bool y4 = \"aa\" >= \"ab\"",
        "  bool y5 = \"aa\" == \"ab\"",
        "  bool y6 = \"aa\" != \"aa\"",
        "  print(y1 or y2 or y3 or y4 or y5 or y6)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsefalse", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, NullComparisons) {
  stringstream in(build_string({
        "void main() {",
        "  int i = 3",
        "  double d = 2.75",
        "  char c = 'a'", 
        "  string s = \"abc\"",
        "  bool b = false", 
        "  bool x1 = null != null", 
        "  bool x2 = (i == null) or (d == null) or (c == null) or (s == null) or ",
        "            (b == null)",
        "  bool x3 = (null == i) or (null == d) or (null == c) or (null == s) or ",
        "            (null == b)",
        "  print(x1 or x2 or x3)", 
        "  bool x4 = null == null",
        "  bool x5 = (i != null) or (d != null) or (c != null) or (s != null) or ",
        "            (b != null)",
        "  bool x6 = (null != i) or (null != d) or (null != c) or (null != s) or ",
        "            (null != b)",
        "  print(x4 and x5 and x6)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetrue", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// While loops
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, BasicWhile) {
  stringstream in(build_string({
        "void main() {",
        "  int i = 0",
        "  while (i < 5) {",
        "    i = i + 1",
        "  }",
        "  print(i)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("5", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, MoreInvolvedWhile) {
  stringstream in(build_string({
        "void main() {",
        "  int i = 0",
        "  while (i < 7) {",
        "    int j = i * 2",
        "    print(j)",
        "    print(' ')", 
        "    i = i + 1",
        "  }",
        "  print(i)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 2 4 6 8 10 12 7", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, NestedWhile) {
  stringstream in(build_string({
        "void main() {",
        "  int i = 0",
        "  while (i < 5) {",
        "    print(i)",
        "    int j = 0", 
        "    while (j < i) {", 
        "      print(j)",
        "      j = j + 1",
        "    }",
        "    print(' ')", 
        "    i = i + 1",
        "  }",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 10 201 3012 40123 ", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// For loops
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, BasicFor) {
  stringstream in(build_string({
        "void main() {",
        "  for (int i = 0; i < 5; i = i + 1) {", 
        "    print(i)",
        "    print(' ')",
        "  }",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 2 3 4 ", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, NestedFor) {
  stringstream in(build_string({
        "void main() {",
        "  int x = 0", 
        "  for (int i = 1; i <= 5; i = i + 1) {", 
        "    for (int j = 1; j <= 4; j = j + 1) {",
        "      x = x + (i * j)",
        "    }", 
        "  }",
        "  print(x)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("150", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ForOuterShadowing) {
  stringstream in(build_string({
        "void main() {",
        "  int i = 42", 
        "  for (int i = 0; i < 5; i = i + 1) {", 
        "    print(i)",
        "    print(' ')",
        "  }",
        "  print(i)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 2 3 4 42", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ForInnerShadowing) {
  stringstream in(build_string({
        "void main() {",
        "  for (int i = 0; i < 3; i = i + 1) {", 
        "    int i = 42",
        "    print(i)",
        "    print(' ')",
        "  }",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42 42 42 ", out.str());
  restore_cout();
}


//----------------------------------------------------------------------
// If statements
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, BasicIf) {
  stringstream in(build_string({
        "void main() {",
        "  print(0)",
        "  if (3 < 4) {",
        "    print(' ')",
        "    print(1)",
        "  }",
        "  print(' ')", 
        "  print(0)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 0", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ConsecutiveIfs) {
  stringstream in(build_string({
        "void main() {",
        "  print(0)",
        "  if (3 < 4) {",
        "    print(' ')",
        "    print(1)",
        "  }",
        "  if (true) {",
        "    print(' ')",
        "    print(2)", 
        "  }",
        "  if (4 < 3) {",
        "    print(' ')",
        "    print(3)", 
        "  }",
        "  print(' ')", 
        "  print(0)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 2 0", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, BasicElseIf) {
  stringstream in(build_string({
        "void main() {",
        "  print(0)",
        "  if (3 < 4) {",
        "    print(' ')",
        "    print(1)",
        "  }",
        "  elseif (4 > 3) {",
        "    print(' ')",
        "    print(2)", 
        "  }",
        "  else {",
        "    print(' ')",
        "    print(3)", 
        "  }",
        "  if (4 < 3) {",
        "    print(' ')",
        "    print(1)", 
        "  }",
        "  elseif (3 < 4) {",
        "    print(' ')",
        "    print(2)", 
        "  }",
        "  else {",
        "    print(' ')",
        "    print(3)",
        "  }",
        "  if (4 < 3) {",
        "    print(' ')",
        "    print(1)",
        "  }",
        "  elseif (3 != 3) {",
        "    print(' ')",
        "    print(2)",
        "  }",
        "  else {",
        "    print(' ')",
        "    print(3)",
        "  }", 
        "  print(' ')", 
        "  print(0)", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 2 3 0", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Function calls
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, NoArgCall) {
  stringstream in(build_string({
        "void f() {}", 
        "void main() {",
        "  print(f())", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("null", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, OneArgCall) {
  stringstream in(build_string({
        "int f(int x) {return x}", 
        "void main() {",
        "  print(f(3))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, TwoArgCall) {
  stringstream in(build_string({
        "int f(int x, int y) {return x * y}", 
        "void main() {",
        "  print(f(3, 2))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("6", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ThreeArgCall) {
  stringstream in(build_string({
        "int f(int x, int y, int z) {return (x * y) - z}", 
        "void main() {",
        "  print(f(3, 2, 4))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, MultilevelCall) {
  stringstream in(build_string({
        "string f(string s) {",
        "  return s",
        "}", 
        "string g(string s1, string s2) {",
        "  return concat(f(s1), s2)",
        "}", 
        "string h(string s1, string s2, string s3) {",
        "  return concat(g(s1,s2), s3)",
        "}",
        "void main() {",
        "  print(h(\"go\", \"za\", \"gs\"))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("gozags", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Recursive functions
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, BasicRecursion) {
  stringstream in(build_string({
        "int non_negative_sum(int x) {", 
        "  if (x <= 0) {",
        "    return 0", 
        "  }",
        "  return x + non_negative_sum(x - 1)", 
        "}", 
        "void main() {",
        "  print(non_negative_sum(0))",
        "  print(' ')",
        "  print(non_negative_sum(1))",
        "  print(' ')",
        "  print(non_negative_sum(10))",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("0 1 55", out.str());
  restore_cout();
}

//----------------------------------------------------------------------
// Structs
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, EmptyStruct) {
  stringstream in(build_string({
        "struct T {}",
        "void main() {",
        "  T t = new T",
        "  print(t)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, SimpleFieldStruct) {
  stringstream in(build_string({
        "struct T {int x}",
        "void main() {",
        "  T t = new T",
        "  print(t.x)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("null", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, SimpleTwoFieldStruct) {
  stringstream in(build_string({
        "struct T {int x, bool y}",
        "void main() {",
        "  T t = new T",
        "  print(t.x)",
        "  print(t.y)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("nullnull", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, FieldInitialization) {
  stringstream in(build_string({
        "struct T {int x, bool y}",
        "void main() {",
        "  T t = new T",
        "  t.x = 42",
        "  t.y = true",
        "  print(t.x)",
        "  print(t.y)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42true", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, StructAssign) {
  stringstream in(build_string({
        "struct T {int x, bool y}",
        "void main() {",
        "  T t1 = new T",
        "  t1.x = 42",
        "  t1.y = true",
        "  T t2 = t1", 
        "  print(t2.x)",
        "  print(t2.y)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42true", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, TwoStructs) {
  stringstream in(build_string({
        "struct T1 {int val, T2 t2}",
        "struct T2 {int val, T1 t1}",
        "void main() {",
        "  T1 x = new T1",
        "  T2 y = new T2",
        "  x.val = 42",
        "  y.val = 27",
        "  x.t2 = y",
        "  y.t1 = x",
        "  print(x.val)",
        "  print(x.t2.val)",
        "  print(y.val)",
        "  print(y.t1.val)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42272742", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, RecursiveStruct) {
  stringstream in(build_string({
        "struct Node {int val, Node next}",
        "void main() {",
        "  Node r = new Node",
        "  print(r)",
        "  print(' ')", 
        "  r.val = 10",
        "  print(r.val)",
        "  print(' ')", 
        "  r.next = new Node",
        "  print(r.next)",
        "  print(' ')", 
        "  r.next.val = 20",
        "  print(r.next.val)",
        "  print(' ')", 
        "  r.next.next = new Node",
        "  print(r.next.next)",
        "  print(' ')", 
        "  r.next.next.val = 30",
        "  print(r.next.next.val)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023 10 2024 20 2025 30", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, StructsAsParams) {
  stringstream in(build_string({
        "struct Node {int val, Node next}",
        "int val(Node n) {print(n) return n.val}",
        "void main() {",
        "  Node r = new Node",
        "  r.val = 42",
        "  print(val(r))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("202342", out.str());
  restore_cout();
}


//----------------------------------------------------------------------
// Arrays
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, ArrayCreation) {
  stringstream in(build_string({
        "void main() {",
        "  array int xs = new int[5]", 
        "  print(xs)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ArrayInit) {
  stringstream in(build_string({
        "void main() {",
        "  array bool xs = new bool[3]", 
        "  xs[0] = false",
        "  xs[1] = true",
        "  print(xs[0])",
        "  print(xs[1])",
        "  print(xs[2])",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("falsetruenull", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ArrayOfStruct) {
  stringstream in(build_string({
        "struct T {bool x, int y}",
        "void main() {",
        "  array T xs = new T[3]", 
        "  xs[0] = new T",
        "  xs[0].x = true",
        "  xs[0].y = 42",
        "  print(xs)",
        "  print(' ')", 
        "  print(xs[0])",
        "  print(' ')",
        "  print(xs[0].x)",
        "  print(' ')", 
        "  print(xs[0].y)",
        "  print(' ')",
        "  print(xs[1])",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023 2024 true 42 null", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, StructOfArray) {
  stringstream in(build_string({
        "struct Node {int val, array Node children}", 
        "void main() {",
        "  Node r = new Node",
        "  r.children = new Node[3]",
        "  print(r)",
        "  print(' ')",
        "  print(r.children)",
        "  print(' ')",
        "  r.children[0] = new Node"
        "  r.children[0].val = 42"
        "  print(r.children[0])",
        "  print(' ')",
        "  print(r.children[0].val)",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023 2024 2025 42", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, ArrayAsParam) {
  stringstream in(build_string({
        "bool val(array bool xs, int index) {print(xs) return xs[index]}",
        "void main() {",
        "  array bool xs = new bool[5]",
        "  xs[0] = true"
        "  print(val(xs, 0))",
        "  print(' ')",
        "  xs[1] = false",
        "  print(val(xs, 1))",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("2023true 2023false", out.str());
  restore_cout();
}


//----------------------------------------------------------------------
// Built-in functions
//----------------------------------------------------------------------

TEST(BasicCodeGenTest, SimpleToStr) {
  stringstream in(build_string({
        "void main() {",
        "  print(to_string(42))",
        "  print(' ')",
        "  print(to_string(3.14))",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42 3.140000", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, SimpleToInt) {
  stringstream in(build_string({
        "void main() {",
        "  print(to_int(\"42\"))",
        "  print(' ')",
        "  print(to_int(3.14))",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("42 3", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, SimpleToDouble) {
  stringstream in(build_string({
        "void main() {",
        "  print(to_double(\"3.14\"))",
        "  print(' ')",
        "  print(to_double(3))",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("3.140000 3.000000", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, StringLength) {
  stringstream in(build_string({
        "void main() {",
        "  print(length(\"abcdefg\"))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("7", out.str());
  restore_cout();
}

// skipping array length since relies on semantic checker

TEST(BasicCodeGenTest, StringGet) {
  stringstream in(build_string({
        "void main() {",
        "  string s = \"bluegreen\""
        "  for(int i = 0; i < length(s); i = i + 1) {",
        "    print(get(i, s))"
        "  }",
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen", out.str());
  restore_cout();
}

TEST(BasicCodeGenTest, StringConcat) {
  stringstream in(build_string({
        "void main() {",
        "  string s1 = \"blue\"", 
        "  string s2 = \"green\"",
        "  print(concat(concat(s1, s2), \" \"))", 
        "  print(concat(concat(s1, s1), \" \"))", 
        "  print(concat(concat(s2, s1), \" \"))", 
        "  print(concat(s2, s2))", 
        "}"
      }));
  VM vm;
  CodeGenerator generator(vm);
  ASTParser(Lexer(in)).parse().accept(generator);
  stringstream out;
  change_cout(out);
  vm.run();
  EXPECT_EQ("bluegreen blueblue greenblue greengreen", out.str());
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

