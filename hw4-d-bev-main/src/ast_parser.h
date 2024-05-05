//----------------------------------------------------------------------
// FILE: ast_parser.h
// DATE: CPSC 326, Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: Using parser to create AST
//----------------------------------------------------------------------

#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "mypl_exception.h"
#include "lexer.h"
#include "ast.h"


class ASTParser
{
public:

  // crate a new recursive descent parer
  ASTParser(const Lexer& lexer);

  // run the parser
  Program parse();
  
private:
  
  Lexer lexer;
  Token curr_token;
  
  // helper functions
  void advance();
  void eat(TokenType t, const std::string& msg);
  bool match(TokenType t);
  bool match(std::initializer_list<TokenType> types);
  void error(const std::string& msg);
  bool bin_op();

  bool isExpr();
  bool isBaseValue();
  bool isBaseType();

  // recursive descent functions
  void struct_def(Program& p);
  void fun_def(Program& s);
  void fields(StructDef& sdef);
  void params(FunDef& fdef);
  void data_type(VarDef& vdef);
  void base_type(DataType& dtype);
  void stmt(std::vector<std::shared_ptr<Stmt>>& stmts);
  void if_stmt(IfStmt& ifStmt);
  void if_stmt_t(IfStmt& ifStmt);
  void while_stmt(WhileStmt& whileStmt);
  void for_stmt(ForStmt& forStmt);
  void ret_stmt(ReturnStmt& retStmt);
  void expr(Expr& expr);
  void rvalue(SimpleTerm& st);
  void new_rvalue(NewRValue& nrv);
  void base_rvalue();
  void var_rvalue(VarRValue& vrv);
  void call_expr(CallExpr& callExpr);
  void vdecl_stmt(VarDeclStmt& vdStmt);

  void assign_stmt(AssignStmt& assnStmt, Token prevToken);
  void lvalue(AssignStmt& assnStmt, Token prevToken);
};


#endif