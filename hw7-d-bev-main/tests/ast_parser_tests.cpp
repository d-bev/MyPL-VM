//----------------------------------------------------------------------
// FILE: ast_parser_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic ast parser tests
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
// Basic function signatures
//------------------------------------------------------------

TEST(BasicASTParserTests, EmptyInput) {
  stringstream in("");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(0, p.fun_defs.size());
  ASSERT_EQ(0, p.struct_defs.size());
}

TEST(BasicASTParserTests, EmptyFunction) {
  stringstream in("int f() {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs.size());
  ASSERT_EQ(0, p.struct_defs.size());
  ASSERT_EQ("int", p.fun_defs[0].return_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].return_type.is_array);  
  ASSERT_EQ("f", p.fun_defs[0].fun_name.lexeme());
  ASSERT_EQ(0, p.fun_defs[0].params.size());
  ASSERT_EQ(0, p.fun_defs[0].stmts.size());
}

TEST(BasicASTParserTests, EmptyFunctionArrayReturn) {
  stringstream in("array bool f() {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ("bool", p.fun_defs[0].return_type.type_name);
  ASSERT_TRUE(p.fun_defs[0].return_type.is_array);
}

TEST(BasicASTParserTests, EmptyFunctionOneParam) {
  stringstream in("int f(string x) {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs[0].params.size());
  ASSERT_EQ("string", p.fun_defs[0].params[0].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[0].data_type.is_array);
  ASSERT_EQ("x", p.fun_defs[0].params[0].var_name.lexeme());
}

TEST(BasicASTParserTests, EmptyFunctionOneIdParam) {
  stringstream in("int f(S s) {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs[0].params.size());
  ASSERT_EQ("S", p.fun_defs[0].params[0].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[0].data_type.is_array);
  ASSERT_EQ("s", p.fun_defs[0].params[0].var_name.lexeme());
}

TEST(BasicASTParserTests, EmptyFunctionOneArrayParam) {
  stringstream in("int f(array int xs) {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs[0].params.size());
  ASSERT_EQ("int", p.fun_defs[0].params[0].data_type.type_name);
  ASSERT_TRUE(p.fun_defs[0].params[0].data_type.is_array);
  ASSERT_EQ("xs", p.fun_defs[0].params[0].var_name.lexeme());
}

TEST(BasicASTParserTests, EmptyFunctionTwoParams) {
  stringstream in("int f(bool x, int y) {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(2, p.fun_defs[0].params.size());
  ASSERT_EQ("bool", p.fun_defs[0].params[0].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[0].data_type.is_array);
  ASSERT_EQ("x", p.fun_defs[0].params[0].var_name.lexeme());
  ASSERT_EQ("int", p.fun_defs[0].params[1].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[1].data_type.is_array);
  ASSERT_EQ("y", p.fun_defs[0].params[1].var_name.lexeme());
}

TEST(BasicASTParserTests, EmptyFunctionThreeParams) {
  stringstream in("int f(bool x, int y, array string z) {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(3, p.fun_defs[0].params.size());
  ASSERT_EQ("bool", p.fun_defs[0].params[0].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[0].data_type.is_array);
  ASSERT_EQ("x", p.fun_defs[0].params[0].var_name.lexeme());
  ASSERT_EQ("int", p.fun_defs[0].params[1].data_type.type_name);
  ASSERT_FALSE(p.fun_defs[0].params[1].data_type.is_array);
  ASSERT_EQ("y", p.fun_defs[0].params[1].var_name.lexeme());
  ASSERT_EQ("string", p.fun_defs[0].params[2].data_type.type_name);
  ASSERT_TRUE(p.fun_defs[0].params[2].data_type.is_array);
  ASSERT_EQ("z", p.fun_defs[0].params[2].var_name.lexeme());
}

TEST(BasicASTParserTests, MultipleEmptyFunctions) {
  stringstream in(build_string({
        "void f() {}",
        "int g() {}",
        "void main() {}",
      }));
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(3, p.fun_defs.size());
  ASSERT_EQ(0, p.struct_defs.size());
}

//----------------------------------------------------------------------
// Basic Struct Definitions
//----------------------------------------------------------------------

TEST(BasicASTParserTests, EmptyStruct) {
  stringstream in("struct S {}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(0, p.fun_defs.size());
  ASSERT_EQ(1, p.struct_defs.size());
  ASSERT_EQ("S", p.struct_defs[0].struct_name.lexeme());
  ASSERT_EQ(0, p.struct_defs[0].fields.size());
}

TEST(BasicASTParserTests, OneFieldStruct) {
  stringstream in("struct S {int x}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.struct_defs[0].fields.size());
  ASSERT_EQ("int", p.struct_defs[0].fields[0].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[0].data_type.is_array);
  ASSERT_EQ("x", p.struct_defs[0].fields[0].var_name.lexeme());
}

TEST(BasicASTParserTests, OneIdFieldStruct) {
  stringstream in("struct S {S s}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.struct_defs[0].fields.size());
  ASSERT_EQ("S", p.struct_defs[0].fields[0].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[0].data_type.is_array);
  ASSERT_EQ("s", p.struct_defs[0].fields[0].var_name.lexeme());
}

TEST(BasicASTParserTests, OneArrayFieldStruct) {
  stringstream in("struct S {array int x}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.struct_defs[0].fields.size());
  ASSERT_EQ("int", p.struct_defs[0].fields[0].data_type.type_name);
  ASSERT_TRUE(p.struct_defs[0].fields[0].data_type.is_array);
  ASSERT_EQ("x", p.struct_defs[0].fields[0].var_name.lexeme());
}

TEST(BasicASTParserTests, TwoArrayFields) {
  stringstream in("struct S {int x, bool y}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(2, p.struct_defs[0].fields.size());
  ASSERT_EQ("int", p.struct_defs[0].fields[0].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[0].data_type.is_array);
  ASSERT_EQ("x", p.struct_defs[0].fields[0].var_name.lexeme());
  ASSERT_EQ("bool", p.struct_defs[0].fields[1].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[1].data_type.is_array);
  ASSERT_EQ("y", p.struct_defs[0].fields[1].var_name.lexeme());
}

TEST(BasicASTParserTests, ThreeArrayFields) {
  stringstream in("struct S {int x, bool y, array string z}");
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(3, p.struct_defs[0].fields.size());
  ASSERT_EQ("int", p.struct_defs[0].fields[0].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[0].data_type.is_array);
  ASSERT_EQ("x", p.struct_defs[0].fields[0].var_name.lexeme());
  ASSERT_EQ("bool", p.struct_defs[0].fields[1].data_type.type_name);
  ASSERT_FALSE(p.struct_defs[0].fields[1].data_type.is_array);
  ASSERT_EQ("y", p.struct_defs[0].fields[1].var_name.lexeme());
  ASSERT_EQ("string", p.struct_defs[0].fields[2].data_type.type_name);
  ASSERT_TRUE(p.struct_defs[0].fields[2].data_type.is_array);
  ASSERT_EQ("z", p.struct_defs[0].fields[2].var_name.lexeme());
}

TEST(BasicASTParserTests, MultipleEmptyStructs) {
  stringstream in(build_string({
        "struct S1 {}", 
        "int f() {}", 
        "struct S2 {}",
        "int g() {}", 
        "struct S3 {}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(2, p.fun_defs.size());
  ASSERT_EQ(3, p.struct_defs.size());
}

//----------------------------------------------------------------------
// Variable Declaration Statements
//----------------------------------------------------------------------

TEST(BasicASTParserTests, VarDecls) {
  stringstream in(build_string({
        "void main() {", 
        "  int x1 = 0", 
        "  double x2 = 0.0",
        "  bool x3 = true",
        "  string x4 = \"\"",
        "  char x5 = 'a'",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(5, p.fun_defs[0].stmts.size());
  VarDeclStmt& s = (VarDeclStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ("int", s.var_def.data_type.type_name);
  ASSERT_EQ("x1", s.var_def.var_name.lexeme());
  s = (VarDeclStmt&)*p.fun_defs[0].stmts[1];
  ASSERT_EQ("double", s.var_def.data_type.type_name);
  ASSERT_EQ("x2", s.var_def.var_name.lexeme());
  s = (VarDeclStmt&)*p.fun_defs[0].stmts[2];
  ASSERT_EQ("bool", s.var_def.data_type.type_name);
  ASSERT_EQ("x3", s.var_def.var_name.lexeme());
  s = (VarDeclStmt&)*p.fun_defs[0].stmts[3];
  ASSERT_EQ("string", s.var_def.data_type.type_name);
  ASSERT_EQ("x4", s.var_def.var_name.lexeme());
  s = (VarDeclStmt&)*p.fun_defs[0].stmts[4];
  ASSERT_EQ("char", s.var_def.data_type.type_name);
  ASSERT_EQ("x5", s.var_def.var_name.lexeme());
}

TEST(BasicASTParserTests, ArrrayVarDecl) {
  stringstream in(build_string({
        "void main() {", 
        "  array int x1 = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs[0].stmts.size());
  VarDeclStmt& s = (VarDeclStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_TRUE(s.var_def.data_type.is_array);
  ASSERT_EQ("int", s.var_def.data_type.type_name);
  ASSERT_EQ("x1", s.var_def.var_name.lexeme());
}

TEST(BasicASTParserTests, IdVarDecl) {
  stringstream in(build_string({
        "void main() {", 
        "  S s = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  ASSERT_EQ(1, p.fun_defs[0].stmts.size());
  VarDeclStmt& s = (VarDeclStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_FALSE(s.var_def.data_type.is_array);
  ASSERT_EQ("S", s.var_def.data_type.type_name);
  ASSERT_EQ("s", s.var_def.var_name.lexeme());
}

//----------------------------------------------------------------------
// Assignment Statements
//----------------------------------------------------------------------

TEST(BasicASTParserTests, SimpleAssignment) {
  stringstream in(build_string({
        "void main() {", 
        "  x = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  AssignStmt& s = (AssignStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(1, s.lvalue.size());
  ASSERT_EQ("x", s.lvalue[0].var_name.lexeme());
  ASSERT_FALSE(s.lvalue[0].array_expr.has_value());
}

TEST(BasicASTParserTests, SimplePathAssignment) {
  stringstream in(build_string({
        "void main() {", 
        "  x.y = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  AssignStmt& s = (AssignStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(2, s.lvalue.size());
  ASSERT_EQ("x", s.lvalue[0].var_name.lexeme());
  ASSERT_FALSE(s.lvalue[0].array_expr.has_value());
  ASSERT_EQ("y", s.lvalue[1].var_name.lexeme());
  ASSERT_FALSE(s.lvalue[1].array_expr.has_value());
}

TEST(BasicASTParserTests, SimpleArrayAssignment) {
  stringstream in(build_string({
        "void main() {", 
        "  x[0] = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  AssignStmt& s = (AssignStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(1, s.lvalue.size());
  ASSERT_EQ("x", s.lvalue[0].var_name.lexeme());
  ASSERT_TRUE(s.lvalue[0].array_expr.has_value());
}

TEST(BasicASTParserTests, MultiplePathAssignment) {
  stringstream in(build_string({
        "void main() {", 
        "  x1.x2[0].x3.x4[1] = null", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  AssignStmt& s = (AssignStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(4, s.lvalue.size());
  ASSERT_EQ("x1", s.lvalue[0].var_name.lexeme());
  ASSERT_FALSE(s.lvalue[0].array_expr.has_value());
  ASSERT_EQ("x2", s.lvalue[1].var_name.lexeme());
  ASSERT_TRUE(s.lvalue[1].array_expr.has_value());
  ASSERT_EQ("x3", s.lvalue[2].var_name.lexeme());
  ASSERT_FALSE(s.lvalue[2].array_expr.has_value());
  ASSERT_EQ("x4", s.lvalue[3].var_name.lexeme());
  ASSERT_TRUE(s.lvalue[3].array_expr.has_value());
}

//----------------------------------------------------------------------
// If Statements
//----------------------------------------------------------------------

TEST(BasicASTParserTests, SingleIfStatement) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {}", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(0, s.if_part.stmts.size());
  ASSERT_EQ(0, s.else_ifs.size());
  ASSERT_EQ(0, s.else_stmts.size());
}

TEST(BasicASTParserTests, IfStatementWithBody) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {int x = 0}", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(1, s.if_part.stmts.size());
  ASSERT_EQ(0, s.else_ifs.size());
  ASSERT_EQ(0, s.else_stmts.size());
}

TEST(BasicASTParserTests, IfStatementWithElseIF) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {}",
        "  elseif (false) {}",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(0, s.if_part.stmts.size());
  ASSERT_EQ(1, s.else_ifs.size());
  ASSERT_EQ(0, s.else_stmts.size());
}

TEST(BasicASTParserTests, IfStatementWithTwoElseIFs) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {}",
        "  elseif (false) {}",
        "  elseif (false) {}",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(0, s.if_part.stmts.size());
  ASSERT_EQ(2, s.else_ifs.size());
  ASSERT_EQ(0, s.else_stmts.size());
}

TEST(BasicASTParserTests, IfStatementWithElse) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {}",
        "  else {}", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(0, s.if_part.stmts.size());
  ASSERT_EQ(0, s.else_ifs.size());
  ASSERT_EQ(0, s.else_stmts.size());
}

TEST(BasicASTParserTests, FullIfStatement) {
  stringstream in(build_string({
        "void main() {", 
        "  if (true) {x = 5}",
        "  elseif (false) {x = 6}",
        "  else {x=7}",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  IfStmt& s = (IfStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(1, s.if_part.stmts.size());
  ASSERT_EQ(1, s.else_ifs.size());
  ASSERT_EQ(1, s.else_ifs[0].stmts.size());
  ASSERT_EQ(1, s.else_stmts.size());
}

//----------------------------------------------------------------------
// While Statements
//----------------------------------------------------------------------

TEST(BasicASTParserTests, EmptyWhileStatement) {
  stringstream in(build_string({
        "void main() {", 
        "  while (true) {}",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  WhileStmt& s = (WhileStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(0, s.stmts.size());
}

TEST(BasicASTParserTests, WhileStatementWithBody) {
  stringstream in(build_string({
        "void main() {", 
        "  while (true) {x = 5}",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  WhileStmt& s = (WhileStmt&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ(1, s.stmts.size());
}

//----------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------

TEST(BasicASTParserTests, Literals) {
  stringstream in(build_string({
        "void main() {", 
        "  x = true",
        "  x = false",
        "  x = 0",
        "  x = 0.0",
        "  x = \"a\"",
        "  x = 'a'",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  ASSERT_FALSE(e.negated);
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("true", v.value.lexeme());
  e = ((AssignStmt&)*p.fun_defs[0].stmts[1]).expr;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("false", v.value.lexeme());
  e = ((AssignStmt&)*p.fun_defs[0].stmts[2]).expr;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("0", v.value.lexeme());
  e = ((AssignStmt&)*p.fun_defs[0].stmts[3]).expr;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("0.0", v.value.lexeme());
  e = ((AssignStmt&)*p.fun_defs[0].stmts[4]).expr;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("a", v.value.lexeme());
  e = ((AssignStmt&)*p.fun_defs[0].stmts[5]).expr;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("a", v.value.lexeme());
}

TEST(BasicASTParserTests, SimpleBooleanExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = true and false",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  ASSERT_FALSE(e.negated);
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("true", v.value.lexeme());
  ASSERT_EQ("and", e.op.value().lexeme());
  e = *e.rest;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("false", v.value.lexeme());
  ASSERT_FALSE(e.op.has_value());
  ASSERT_EQ(nullptr, e.rest);
}

TEST(BasicASTParserTests, SimpleNegatedBooleanExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = not true and false",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  ASSERT_TRUE(e.negated);
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("true", v.value.lexeme());
  ASSERT_EQ("and", e.op.value().lexeme());
  e = *e.rest;
  ASSERT_FALSE(e.negated);
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("false", v.value.lexeme());
  ASSERT_FALSE(e.op.has_value());
  ASSERT_EQ(nullptr, e.rest);
}

TEST(BasicASTParserTests, SimpleParenExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = (1 + 2)", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e1 = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  ASSERT_FALSE(e1.negated);
  Expr& e2 = ((ComplexTerm&)*e1.first).expr;
  ASSERT_FALSE(e2.negated);
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e2.first).rvalue;
  ASSERT_EQ("1", v.value.lexeme());
  ASSERT_EQ("+", e2.op.value().lexeme());
  e2 = *e2.rest;  
  v = (SimpleRValue&)*((SimpleTerm&)*e2.first).rvalue;
  ASSERT_EQ("2", v.value.lexeme());
  ASSERT_FALSE(e1.op.has_value());
  ASSERT_EQ(nullptr, e1.rest);
}

TEST(BasicASTParserTests, ExpressionAfterParenExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = (1 + 2) + 3", 
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  ASSERT_EQ("+", e.op.value().lexeme());
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*(*e.rest).first).rvalue;
  ASSERT_EQ("3", v.value.lexeme());
}

TEST(BasicASTParserTests, ExpressionBeforeParenExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = 1 + (2 + 3)",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;  
  ASSERT_EQ("1", v.value.lexeme());
  ASSERT_EQ("+", e.op.value().lexeme());
  e = ((ComplexTerm&)*(*e.rest).first).expr;
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("2", v.value.lexeme());
  ASSERT_EQ("+", e.op.value().lexeme());
  e = *e.rest;
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;    
  ASSERT_EQ("3", v.value.lexeme());
}

TEST(BasicASTParserTests, ExpressionWithTwoOps) {
  stringstream in(build_string({
        "void main() {", 
        "  x = 1 + 2 + 3",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;  
  ASSERT_EQ("1", v.value.lexeme());
  ASSERT_EQ("+", e.op.value().lexeme());
  e = *e.rest;
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;  
  ASSERT_EQ("2", v.value.lexeme());
  ASSERT_EQ("+", e.op.value().lexeme());  
  e = *e.rest;
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("3", v.value.lexeme());
}

TEST(BasicASTParserTests, EmptyCallExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  f()",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  CallExpr& e = (CallExpr&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ("f", e.fun_name.lexeme());
  ASSERT_EQ(0, e.args.size());
}

TEST(BasicASTParserTests, OneArgCallExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  f(0)",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  CallExpr& c = (CallExpr&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ("f", c.fun_name.lexeme());
  ASSERT_EQ(1, c.args.size());
  Expr& e = c.args[0];
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("0", v.value.lexeme());
}

TEST(BasicASTParserTests, TwoArgCallExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  f(0, 1)",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  CallExpr& c = (CallExpr&)*p.fun_defs[0].stmts[0];
  ASSERT_EQ("f", c.fun_name.lexeme());
  ASSERT_EQ(2, c.args.size());
  Expr& e = c.args[0];
  SimpleRValue& v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("0", v.value.lexeme());
  e = c.args[1];
  v = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("1", v.value.lexeme());
}

TEST(BasicASTParserTests, SimpleNewExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = new S",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  NewRValue& v = (NewRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("S", v.type.lexeme());
  ASSERT_FALSE(v.array_expr.has_value());
  ASSERT_FALSE(e.op.has_value());
  ASSERT_EQ(nullptr, e.rest);
}

TEST(BasicASTParserTests, NewArrayExpression) {
  stringstream in(build_string({
        "void main() {", 
        "  x = new S[10]",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  NewRValue& v1 = (NewRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("S", v1.type.lexeme());
  ASSERT_FALSE(e.op.has_value());
  ASSERT_EQ(nullptr, e.rest);
  e = v1.array_expr.value();
  SimpleRValue& v2 = (SimpleRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ("10", v2.value.lexeme());
  ASSERT_FALSE(e.op.has_value());
  ASSERT_EQ(nullptr, e.rest);
}

TEST(BasicASTParserTests, SimpleVarValue) {
  stringstream in(build_string({
        "void main() {", 
        "  x = y",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  VarRValue& v = (VarRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ(1, v.path.size());
  ASSERT_EQ("y", v.path[0].var_name.lexeme());
  ASSERT_FALSE(v.path[0].array_expr.has_value());
}

TEST(BasicASTParserTests, SimpleArrayValue) {
  stringstream in(build_string({
        "void main() {", 
        "  x = y[0]",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  VarRValue& v = (VarRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ(1, v.path.size());
  ASSERT_EQ("y", v.path[0].var_name.lexeme());
  ASSERT_TRUE(v.path[0].array_expr.has_value());
}

TEST(BasicASTParserTests, TwoVarPathValue) {
  stringstream in(build_string({
        "void main() {", 
        "  x = y.z",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  VarRValue& v = (VarRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ(2, v.path.size());
  ASSERT_EQ("y", v.path[0].var_name.lexeme());
  ASSERT_FALSE(v.path[0].array_expr.has_value());
  ASSERT_EQ("z", v.path[1].var_name.lexeme());
  ASSERT_FALSE(v.path[1].array_expr.has_value());
}

TEST(BasicASTParserTests, MultipleVarPathValue) {
  stringstream in(build_string({
        "void main() {", 
        "  x = u[0].v.w[1].y",
        "}"
      }));
  Program p = ASTParser(Lexer(in)).parse();
  Expr& e = ((AssignStmt&)*p.fun_defs[0].stmts[0]).expr;
  VarRValue& v = (VarRValue&)*((SimpleTerm&)*e.first).rvalue;
  ASSERT_EQ(4, v.path.size());
  ASSERT_EQ("u", v.path[0].var_name.lexeme());
  ASSERT_TRUE(v.path[0].array_expr.has_value());
  ASSERT_EQ("v", v.path[1].var_name.lexeme());
  ASSERT_FALSE(v.path[1].array_expr.has_value());
  ASSERT_EQ("w", v.path[2].var_name.lexeme());
  ASSERT_TRUE(v.path[2].array_expr.has_value());
  ASSERT_EQ("y", v.path[3].var_name.lexeme());
  ASSERT_FALSE(v.path[3].array_expr.has_value());
}



//----------------------------------------------------------------------
// TODO: Add at least 10 of your own tests below. Define at least two
// tests for statements, one test for return statements, five tests
// for expressions, and two tests that are more involved combining
// multiple constructs.
// ----------------------------------------------------------------------

// Two for statement tests

// One return statement teste

// Five expression tests

// Three more involved tests that involve multiple constructs


//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

