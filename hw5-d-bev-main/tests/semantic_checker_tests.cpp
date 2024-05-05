//----------------------------------------------------------------------
// FILE: semantic_checker_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Non-comprehensive set of basics tests for the semantic checker
//----------------------------------------------------------------------

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "mypl_exception.h"
#include "lexer.h"
#include "ast_parser.h"
#include "semantic_checker.h"

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
// BASIC FUNCTION DEFINITION CASES
//------------------------------------------------------------

TEST(BasicSemanticCheckerTests, SmallestProgram)
{
  stringstream in("void main() {}");
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ValidFunctionDefs)
{
  stringstream in(build_string({
      "void f1(int x) {}",
      "int f2(double x) {}",
      "char f3(bool x) {}",
      "bool f4(string x) {}",
      "string f5(int p1, bool p2) {}",
      "void f6(double p1, int p2, char p3) {}",
      "int f7(int p1, int p2, string p3) {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, MissingMain)
{
  stringstream in("");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, MainWithBadArgs)
{
  stringstream in("void main(int x) {}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, MainWithBadReturnType)
{
  stringstream in("int main() {}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, BuiltInDefinedWithSameNameAsBuiltIn)
{
  stringstream in(build_string({"void input(string msg) {}",
                                "void main() {}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, TwoFunctionsWithSameName)
{
  stringstream in(build_string({
      "int f() {}",
      "void f(int x) {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, FunctionWithTwoIdenticallyNamedParams)
{
  stringstream in(build_string({
      "int f(int x, double y, char x) {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//------------------------------------------------------------
// BASIC STRUCT DEFINITION CASES
//------------------------------------------------------------

TEST(BasicSemanticCheckerTests, BasicStruct)
{
  stringstream in(build_string({
      "struct S1 {int x, int y}",
      "struct S2 {bool x, char y, double z}",
      "struct S3 {string x}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, StructWithSelfRef)
{
  stringstream in(build_string({
      "struct Node {int val, Node next, Node prev}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, MutualStructRefs)
{
  stringstream in(build_string({
      "struct S1 {int x, S2 y}",
      "struct S2 {int u, S1 v}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, StructAndFunSameNames)
{
  stringstream in(build_string({
      "struct s {}",
      "void s() {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, FunctionStructParams)
{
  stringstream in(build_string({
      "void f(int x, S y, array S z) {}",
      "struct S {int u, string v}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, TwoStructsWithSameName)
{
  stringstream in(build_string({
      "struct S {}",
      "struct S {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, StructReferencesUndefinedStruct)
{
  stringstream in(build_string({
      "struct S1 {int x, S2 y}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, FunctionParamReferencesUndefinedStruct)
{
  stringstream in(build_string({
      "void f(int x, Node y) {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, FunctionReturnTypeIsUndefinedStruct)
{
  stringstream in(build_string({
      "Node f(int x) {}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, StructWithTwoIdenticallyNamedFields)
{
  stringstream in(build_string({
      "struct S {int x, double y, char x}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// VARIABLE DECLARATIONS
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, BasicVarDeclarations)
{
  stringstream in(build_string({
      "void main() {",
      "  int x1 = 0",
      "  double x2 = 0.0",
      "  bool x3 = false",
      "  char x4 = 'a'",
      "  string x5 = \"foo\"",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, BasicVarDeclarationsWithNull)
{
  stringstream in(build_string({
      "struct S {}",
      "void main() {",
      "  int x1 = null",
      "  double x2 = null",
      "  bool x3 = null",
      "  char x4 = null",
      "  string x5 = null",
      "  S s = null",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, TwoVarsWithSameName)
{
  stringstream in(build_string({
      "void main() {",
      "  int x = 0",
      "  double y = 3.14",
      "  char x = 'a'",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, MismatchedVariableDeclarationTypes)
{
  stringstream in(build_string({
      "void main() {",
      "  int x = 3.14",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, ExpressionExamplesWithoutParensAndVars)
{
  stringstream in(build_string({
      "void main() {",
      "  int x1 = 1 + 2 + 3 * 4 / 5 - 6 - 7",
      "  double x2 = 1.0 + 2.1 + 3.3 * 4.4 / 5.5 - 6.6 - 7.7",
      "  bool x3 = not true or false and true and not false",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ExpressionExamplesWithParensAndWithoutVars)
{
  stringstream in(build_string({
      "void main() {",
      "  int x1 = ((1 + 2) + (3 * 4) / (5 - 6)) - 7",
      "  double x2 = ((1.0 + 2.1) + (3.3 * 4.4) / (5.5 - 6.6)) - 7.7",
      "  bool x3 = not (true or false) and (true and not false)",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ExpressionExamplesWithParensAndVars)
{
  stringstream in(build_string({
      "void main() {",
      "  int x1 = (1 + 2) + (3 * 4)",
      "  int x2 = (5 - 6) - 7",
      "  int x3 = ((x1 / x2) + x1 - x2) / (x1 + x2)",
      "  double x4 = (1.0 + 2.1) + (3.3 * 4.4)",
      "  double x5 = (5.5 - 6.6) - 7.7",
      "  double x6 = ((x4 / x5) + x5 - x4) / (x4 + x5)",
      "  bool x7 = not (true or false)",
      "  bool x8 = true and not false",
      "  bool x9 = (x7 and x8) or (not x7 and x8) or (x7 and not x8)",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, BasicRelationalOperators)
{
  stringstream in(build_string({
      "void main() {",
      "  bool x1 = 0 < 1",
      "  bool x2 = 0 <= 1",
      "  bool x3 = 0 > 1",
      "  bool x4 = 0 >= 1",
      "  bool x5 = 0 != 1",
      "  bool x6 = 0 == 1",
      "  bool x7 = 0 != null",
      "  bool x8 = 0 == null",
      "  bool x9 = null != null",
      "  bool x10 = null == null",
      "  bool x11 = not 0 < 1",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, CombinedRelationalOperators)
{
  stringstream in(build_string({
      "void main() {",
      "  bool x1 = (0 < 1) and ('a' < 'b') and (3.1 < 3.2)",
      "  bool x2 = (not ('a' == null)) or (not (3.1 != null))",
      "  bool x4 = (\"abc\" <= \"abde\") or (x1 == false)",
      "  bool x5 = (not x2 == null) and 3.1 >= 4.1",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ArrayComparisons)
{
  stringstream in(build_string({
      "void main() {",
      "  array int x1 = new int[10]",
      "  array int x2 = x1",
      "  bool x3 = (x2 != null) and ((x1 != x2) or (x1 == x2))",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, BadBoolRelationalComparison)
{
  stringstream in(build_string({
      "void main() {",
      "  bool x1 = (true < false)",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, BadArrayRelationalOp)
{
  stringstream in(build_string({
      "void main() {",
      "  array int x1 = new int[10]",
      "  array int x2 = x1",
      "  bool x1 = x1 <= x2",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, BadLogicalNegation)
{
  stringstream in(build_string({
      "void main() {",
      "  bool x = not (1 + 2)",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, FunctionReturnTypeMatch)
{
  stringstream in(build_string({
      "int f() {return 42}",
      "int g() {return null}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, MismatchFunctionReturnTypes)
{
  stringstream in(build_string({
      "int f() {return true}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, NonNullReturnInFunction)
{
  stringstream in("void main() {return 0}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, OneReturnInFunctionWithBadType)
{
  stringstream in(build_string({
      "int f(int x) {",
      "  if (x < 0) {return 0}",
      "  else {return false}",
      "}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, NonBoolIfCondition)
{
  stringstream in("void main() {if (1) {}}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, NonBoolElseIfCondition)
{
  stringstream in("void main() {if (false) {} elseif ('a') {}}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, BoolArrayIfCondition)
{
  stringstream in(build_string({"void main() {",
                                "  array bool flags = new bool[2]",
                                "  if (flags) {}",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, NonBoolWhileCondition)
{
  stringstream in("void main() {while (1) {}}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, NonBoolConditionInForLoop)
{
  stringstream in("void main() {for(int i = 0; i + 1; i = i + 1) {}}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, CallToUndefinedFunction)
{
  stringstream in("void main() {f()}");
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, TooFewArgsInFunctionCall)
{
  stringstream in(build_string({
      "void f(int x) {}",
      "void main() {f()}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, TooManyArgsInFunctionCall)
{
  stringstream in(build_string({
      "void f(int x) {}",
      "void main() {f(0, 1)}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Shadowing
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, AllowedShadowingExample)
{
  stringstream in(build_string({"void main() {",
                                "  int x = 0",
                                "  if (true) {",
                                "    double x = 1.0",
                                "    double y = x * 0.01",
                                "  }",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, IllegalShadowingExample)
{
  stringstream in(build_string({"void main() {",
                                "  int x = 0",
                                "  if (true) {",
                                "    int y = x + 1",
                                "  }",
                                "  double x = 1.0",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Built-In Functions
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, PrintExamples)
{
  stringstream in(build_string({"void main() {",
                                "  print(0)",
                                "  print(1.0)",
                                "  print(true)",
                                "  print('a')",
                                "  print(\"abc\")",
                                "  int x = print(0)", // print() returns void
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, PrintStructObject)
{
  stringstream in(build_string({"struct S {int x}",
                                "void main() {",
                                "  S s = new S",
                                "  print(s)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, PrintArrayObject)
{
  stringstream in(build_string({"void main() {",
                                "  array int xs = new int[10]",
                                "  print(xs)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, PrintArgsMismatch)
{
  stringstream in(build_string({"void main() {",
                                "  print(0, 1)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, InputExample)
{
  stringstream in(build_string({"void main() {",
                                "  input()",
                                "  string s = input()",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, InputReturnTypeMismatch)
{
  stringstream in(build_string({"void main() {",
                                "  int s = input()",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, InputTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  int s = input(\"Name:\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToStringExamples)
{
  stringstream in(build_string({"void main() {",
                                "  int x = 0",
                                "  double y = 1.0",
                                "  char z = 'a'",
                                "  string s1 = to_string(3)",
                                "  string s2 = to_string(x)",
                                "  string s3 = to_string(1.0)",
                                "  string s4 = to_string(y)",
                                "  string s5 = to_string('a')",
                                "  string s6 = to_string(z)",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ToStringTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  to_string(0, 1)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToStringBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  to_string(true)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToIntExamples)
{
  stringstream in(build_string({"void main() {",
                                "  double x = 1.0",
                                "  string y = \"1\"",
                                "  int i1 = to_int(1.0)",
                                "  int i2 = to_int(x)",
                                "  int s3 = to_int(\"1\")",
                                "  int s4 = to_int(y)",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ToIntTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  to_int(1.0, 2.0)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToIntBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  to_int(10)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToDoubleExamples)
{
  stringstream in(build_string({"void main() {",
                                "  int x = 1",
                                "  string y = \"1.0\"",
                                "  double d1 = to_double(1)",
                                "  double d2 = to_double(x)",
                                "  double d3 = to_double(\"1.0\")",
                                "  double d4 = to_double(y)",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ToDoubleTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  to_double(1, 2)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ToDoubleBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  to_double(1.0)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, LengthExamples)
{
  stringstream in(build_string({"void main() {",
                                "  string s = \"abc\"",
                                "  array bool xs = new bool[10]",
                                "  int x1 = length(\"abc\")",
                                "  int x2 = length(s)",
                                "  int x3 = length(new bool[10])",
                                "  int x4 = length(xs)",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, LengthTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  length(\"abc\", \"def\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, LengthBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  length(1.0)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, GetExamples)
{
  stringstream in(build_string({"void main() {",
                                "  string s = \"abc\"",
                                "  char c1 = get(0, \"abc\")",
                                "  char c2 = get(2, s)",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, GetTooFewArgs)
{
  stringstream in(build_string({"void main() {",
                                "  get(\"abc\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, GetBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  get(1.0, \"abc\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ConcatExamples)
{
  stringstream in(build_string({"void main() {",
                                "  string s1 = \"abc\"",
                                "  string s2 = concat(s1, \"def\")",
                                "  string s3 = concat(s2, concat(s2, s2))",
                                "}"}));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ConcatTooFewArgs)
{
  stringstream in(build_string({"void main() {",
                                "  string s = concat(\"abc\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ConcatTooManyArgs)
{
  stringstream in(build_string({"void main() {",
                                "  string s = concat(\"a\", \"b\", \"c\")",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ConcatBadArg)
{
  stringstream in(build_string({"void main() {",
                                "  concat(\"abc\", 0)",
                                "}"}));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//------------------------------------------------------------
// Basic parameter passing
//------------------------------------------------------------

TEST(BasicSemanticCheckerTests, SingleParameterAccess)
{
  stringstream in(build_string({
      "int f(int x) {return x}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, TwoParameterAccesses)
{
  stringstream in(build_string({
      "double f(double x, double y) {return x + y}",
      "void main() {}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, AccessInvalidParameter)
{
  stringstream in(build_string({
      "int f(int x) {return x + y}",
      "void main() {}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Additional Array Tests
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, ArrayCreation)
{
  stringstream in(build_string({
      "struct S {}",
      "void main() {",
      "  int n = 10",
      "  array int a1 = new int[n]",
      "  array int a2 = null",
      "  a2 = a1",
      "  array double a3 = new double[10]",
      "  array char a4 = new char[n+1]",
      "  array string a5 = null",
      "  array bool a6 = new bool[n]",
      "  array S a7 = new S[n]",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ArrayAccess)
{
  stringstream in(build_string({
      "struct S {char val}",
      "void main() {",
      "  int n = 10",
      "  array bool a1 = new bool[n]",
      "  array S a2 = new S[n]",
      "  bool x = a1[n-5]",
      "  a1[0] = x or true",
      "  a2[0] = new S",
      "  S s = a2[1]",
      "  char y = a2[0].val",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, ArrayAssignedInvalidType)
{
  stringstream in(build_string({
      "void main() {",
      "  array bool a1 = new bool[n]",
      "  a1[0] = 1",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, ArrayValueInvalidType)
{
  stringstream in(build_string({
      "void main() {",
      "  array bool a1 = new bool[n]",
      "  int x = a1[0]",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Struct Path Tests
//----------------------------------------------------------------------

TEST(BasicSemanticCheckerTests, StructPathExamples)
{
  stringstream in(build_string({
      "struct S {double val, T t}",
      "struct T {bool val, S s}",
      "void main() {",
      "  S s = new S",
      "  T t = new T",
      "  s.val = 1.0",
      "  t.val = true",
      "  s.t.val = false",
      "  t.s.val = 2.0",
      "  s.t.s.val = 3.0",
      "  t.s.t.val = true",
      "  double x = s.val",
      "  bool y = t.val",
      "  y = s.t.val",
      "  x = t.s.val",
      "  x = s.t.s.val",
      "  y = t.s.t.val",
      "}",
  }));
  SemanticChecker checker;
  ASTParser(Lexer(in)).parse().accept(checker);
}

TEST(BasicSemanticCheckerTests, StructPathAssignedInvalidType)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  s.s.val = 0",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(BasicSemanticCheckerTests, StructPathInvalidType)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  int x = s.s.val",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch (MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

//----------------------------------------------------------------------
// Custom Tests
//----------------------------------------------------------------------

TEST(CustomSemanticCheckerTests, CircularStruct)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  s.s = S",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch(MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(CustomSemanticCheckerTests, CircularStruct)
{
  stringstream in(build_string({
      "",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch(MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(CustomSemanticCheckerTests, CircularStruct)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  s.s = S",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch(MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(CustomSemanticCheckerTests, CircularStruct)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  s.s = S",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch(MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}

TEST(CustomSemanticCheckerTests, CircularStruct)
{
  stringstream in(build_string({
      "struct S {double val, S s}",
      "void main() {",
      "  S s = new S",
      "  s.s = S",
      "}",
  }));
  SemanticChecker checker;
  try
  {
    ASTParser(Lexer(in)).parse().accept(checker);
    FAIL();
  }
  catch(MyPLException &ex)
  {
    string msg = ex.what();
    ASSERT_TRUE(msg.starts_with("Static Error:"));
  }
}



//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
