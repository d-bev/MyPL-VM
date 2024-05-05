//----------------------------------------------------------------------
// FILE: ast_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: Creating AST from parser
//----------------------------------------------------------------------

#include "ast_parser.h"
#include <iostream>

using namespace std;

const bool debug = false;

ASTParser::ASTParser(const Lexer &a_lexer)
    : lexer{a_lexer}
{
}

void ASTParser::advance()
{
  curr_token = lexer.next_token();
}

void ASTParser::eat(TokenType t, const string &msg)
{
  if (!match(t))
    error(msg);
  advance();
}

bool ASTParser::match(TokenType t)
{
  return curr_token.type() == t;
}

bool ASTParser::match(initializer_list<TokenType> types)
{
  for (auto t : types)
    if (match(t))
      return true;
  return false;
}

void ASTParser::error(const string &msg)
{
  string s = msg + " found '" + curr_token.lexeme() + "' ";
  s += "at line " + to_string(curr_token.line()) + ", ";
  s += "column " + to_string(curr_token.column());
  throw MyPLException::ParserError(s);
}

bool ASTParser::bin_op()
{
  return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
                TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
                TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
                TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}

Program ASTParser::parse()
{
  if (debug)
    cout << "parse" << endl;

  Program p;
  advance();
  while (!match(TokenType::EOS))
  {
    if (match(TokenType::STRUCT))
    {
      struct_def(p);
    }
    else
    {
      fun_def(p);
    }
  }
  eat(TokenType::EOS, "expecting end-of-file");
  return p;
}

/*
  HELPER FUNCTIONS
*/

bool ASTParser::isBaseType()
{
  return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::CHAR_TYPE, TokenType::STRING_TYPE});
}

bool ASTParser::isBaseValue()
{
  return match({TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL});
}

bool ASTParser::isExpr()
{
  return match({TokenType::NOT, TokenType::LPAREN, TokenType::NEW, TokenType::ID, TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL, TokenType::NULL_VAL});
}

/*
  RECURSIVE DESCENT FUNCTIONS
*/

// <struct_def> ::= STRUCT ID LBRACE <fields> RBRACE
void ASTParser::struct_def(Program &p)
{
  if (debug)
  {
    std::cout << "struct_def" << std::endl;
  }
  eat(TokenType::STRUCT, "expecting STRUCT");
  StructDef sd;
  sd.struct_name = curr_token;
  eat(TokenType::ID, "expecting ID");
  eat(TokenType::LBRACE, "expecting LBRACE");
  if (!match(TokenType::RBRACE))
    fields(sd);
  eat(TokenType::RBRACE, "expcting RBRACE");

  p.struct_defs.push_back(sd);
}

// <fun_def> ::= ( <data_type> | VOID_TYPE ) ID LPAREN <params> RPAREN LBRACE ( <stmt> )∗ RBRACE
void ASTParser::fun_def(Program &s)
{
  if (debug)
  {
    std::cout << "fun_def" << std::endl;
  }

  FunDef fd;
  VarDef vd;

  if (match(TokenType::VOID_TYPE))
  {
    fd.return_type.type_name = curr_token.lexeme();
    advance();
  }
  else
  {
    data_type(vd);
    fd.return_type = vd.data_type;
  }

  fd.fun_name = curr_token;
  eat(TokenType::ID, "expecting ID");
  eat(TokenType::LPAREN, "expecting LPAREN");

  if (!match(TokenType::RPAREN))
  {
    params(fd);
  }

  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  while (!match(TokenType::RBRACE))
  {
    stmt(fd.stmts);
  }

  eat(TokenType::RBRACE, "expecting RBRACE");

  s.fun_defs.push_back(fd);
}

// <params> ::= <data_type> ID ( COMMA <data_type> ID )* | ϵ
void ASTParser::params(FunDef &fd)
{
  if (debug)
  {
    std::cout << "params" << std::endl;
  }
  VarDef param;
  // FIXME: get param type (I think)
  data_type(param);

  param.var_name = curr_token;
  eat(TokenType::ID, "expecting ID");
  fd.params.push_back(param);

  while (match(TokenType::COMMA))
  {
    advance();
    VarDef param2;
    // FIXME: get param type (I think)
    data_type(param2);
    param2.var_name = curr_token;
    eat(TokenType::ID, "expecting ID");
    fd.params.push_back(param2);
  }
}

// <fields> ::= <data_type> ID ( COMMA <data_type> ID )* | ϵ
void ASTParser::fields(StructDef &sd)
{
  // vector<VarDef> fields
  VarDef vd;

  if (debug)
  {
    std::cout << "fields" << std::endl;
  }

  if (match(TokenType::ARRAY))
  {
    vd.data_type.is_array = true;
    advance();
  }

  if (isBaseType() || isExpr())
  {
    data_type(vd);
    vd.var_name = curr_token;
    eat(TokenType::ID, "expecting ID");
    sd.fields.push_back(vd);

    while (match(TokenType::COMMA))
    {
      VarDef vd2;
      eat(TokenType::COMMA, "expecting Comma");
      data_type(vd2);
      vd2.var_name = curr_token;
      eat(TokenType::ID, "expecting ID");
      sd.fields.push_back(vd2);
    }
  }
}

// <data_type> ::= <base_type> | ID | ARRAY ( <base_type> | ID )
void ASTParser::data_type(VarDef &vd)
{
  if (debug)
  {
    std::cout << "data_type" << std::endl;
  }

  if (match(TokenType::NEW))
  {
    cout << "DEBUG : NEW";

    advance();
  }

  if (match(TokenType::ID))
  {
    vd.data_type.type_name = curr_token.lexeme();
    advance();
  }
  else if (match(TokenType::ARRAY))
  {
    vd.data_type.is_array = true;
    advance();
    if (match(TokenType::ID))
    {
      vd.data_type.type_name = curr_token.lexeme();
      advance();
    }
    else
    {
      if (match(TokenType::VOID_TYPE))
      {
        vd.data_type.type_name = curr_token.lexeme();
        advance();
      }
      else
      {
        base_type(vd.data_type);
      }
    }
  }
  else
  {
    if (match(TokenType::VOID_TYPE))
    {
      vd.data_type.type_name = curr_token.lexeme();
      advance();
    }
    else
    {
      base_type(vd.data_type);
    }
  }
}

// <base_type> ::= INT_TYPE | DOUBLE_TYPE | BOOL_TYPE | CHAR_TYPE | STRING_TYPE
void ASTParser::base_type(DataType &dt)
{
  if (debug)
  {
    std::cout << "base_type" << std::endl;
  }
  if (match(TokenType::INT_TYPE))
  {
    dt.type_name = curr_token.lexeme();
    advance();
  }
  else if (match(TokenType::DOUBLE_TYPE))
  {
    dt.type_name = curr_token.lexeme();
    advance();
  }
  else if (match(TokenType::BOOL_TYPE))
  {
    dt.type_name = curr_token.lexeme();
    advance();
  }
  else if (match(TokenType::CHAR_TYPE))
  {
    dt.type_name = curr_token.lexeme();
    advance();
  }
  else if (match(TokenType::STRING_TYPE))
  {
    dt.type_name = curr_token.lexeme();
    advance();
  }
  else
  {
    error("END OF CONTROL : base_type");
  }
}

// <stmt> ::= <vdecl_stmt> | <assign_stmt> | <if_stmt> | <while_stmt> | <for_stmt> | <call_expr> | <ret_stmt>
void ASTParser::stmt(std::vector<std::shared_ptr<Stmt>> &stmts)
{
  if (debug)
  {
    std::cout << "stmt" << std::endl;
  }

  if (match(TokenType::FOR))
  {
    std::shared_ptr<ForStmt> x = make_shared<ForStmt>();
    for_stmt(*x);
    stmts.push_back(x);
  }
  else if (match(TokenType::WHILE))
  {
    std::shared_ptr<WhileStmt> x = make_shared<WhileStmt>();
    while_stmt(*x);
    stmts.push_back(x);
  }
  else if (match(TokenType::IF))
  {
    std::shared_ptr<IfStmt> x = make_shared<IfStmt>();
    if_stmt(*x);
    stmts.push_back(x);
  }
  else if (match(TokenType::RETURN))
  {
    std::shared_ptr<ReturnStmt> x = make_shared<ReturnStmt>();
    ret_stmt(*x);
    stmts.push_back(x);
  }
  else if (match(TokenType::ID))
  {
    shared_ptr<VarDeclStmt> vds = make_shared<VarDeclStmt>();
    VarDef vd;
    Token name = curr_token;

    vds->var_def.data_type.type_name = curr_token.lexeme();

    eat(TokenType::ID, "expecting ID");

    // call_expr, vdecl_stmt, or assign_stmt all can start with ID
    if (match(TokenType::LPAREN))
    {
      std::shared_ptr<CallExpr> x = make_shared<CallExpr>();
      x->fun_name = name;
      call_expr(*x);
      stmts.push_back(x);
    }
    else if (match({TokenType::ASSIGN, TokenType::DOT, TokenType::LBRACKET}))
    {
      std::shared_ptr<AssignStmt> x = make_shared<AssignStmt>();

      if(name.type() == TokenType::ASSIGN)
        error("stmt->assign_stmt!");

      assign_stmt(*x, name);
      stmts.push_back(x);
    }
    else
    {
      std::shared_ptr<VarDeclStmt> x = make_shared<VarDeclStmt>();

      x->var_def.data_type.type_name = name.lexeme();
      x->var_def.var_name = curr_token;
      eat(TokenType::ID, "expecting ID");
      eat(TokenType::ASSIGN, "expecting ASSIGN");
      expr(x->expr);
      stmts.push_back(x);
    }
  }
  else if (match(TokenType::ARRAY) || isBaseType())
  {
    std::shared_ptr<VarDeclStmt> x = make_shared<VarDeclStmt>();
    vdecl_stmt(*x);
    stmts.push_back(x);
  }
  else
  {
    error("stmt failure!");
  }
}

// <vdecl_stmt> ::= <data_type> ID ASSIGN <expr>
void ASTParser::vdecl_stmt(VarDeclStmt &vds)
{

  if (debug)
  {
    std::cout << "vdecl_stmt" << std::endl;
  }

  if (match(TokenType::ARRAY))
  {
    vds.var_def.data_type.is_array = true;
    advance();

    if (isBaseType() or match(TokenType::ID))
      vds.var_def.data_type.type_name = curr_token.lexeme();
    else
      error("vdecl: not base type or ID");
  }
  else if (isBaseType() or match(TokenType::ID))
  {
    vds.var_def.data_type.type_name = curr_token.lexeme();
  }
  else
  {
    error("vdecl: not base type or ID");
  }

  advance();

  vds.var_def.var_name = curr_token;
  eat(TokenType::ID, "expecting ID");
  eat(TokenType::ASSIGN, "expecting ASSIGN");
  expr(vds.expr);
}

// <assign_stmt> ::= <lvalue> ASSIGN <expr>
void ASTParser::assign_stmt(AssignStmt &aStmt, Token prevToken)
{
  if (debug)
  {
    std::cout << "assign_stmt" << std::endl;
  }
  
  if(prevToken.type() == TokenType::ASSIGN)
    error("assign_stmt!");
  
  lvalue(aStmt, prevToken);
  eat(TokenType::ASSIGN, "expecting ASSIGN");
  expr(aStmt.expr);
}

// <lvalue> ::= ID ( DOT ID | LBRACKET <expr> RBRACKET )∗
void ASTParser::lvalue(AssignStmt &aStmt, Token prevToken)
{
  if (debug)
  {
    std::cout << "lvalue" << std::endl;
  }

  VarRef vr;
  vr.var_name = prevToken;

  // already ate ID

  while (!match(TokenType::EOS))
  {
    if (match(TokenType::DOT))
    {
      aStmt.lvalue.push_back(vr);
      eat(TokenType::DOT, "expecting DOT");

      if(vr.array_expr.has_value())
        vr.array_expr.reset();

      vr.var_name = curr_token;
      eat(TokenType::ID, "expecting ID");
    }
    else if (match(TokenType::LBRACKET))
    {
      eat(TokenType::LBRACKET, "expecting LBRACKET");
      Expr e;
      expr(e);
      vr.array_expr = e;
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
    else
    {
      aStmt.lvalue.push_back(vr);
      break;
    }
  }
}

// <if_stmt> ::= IF LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE <if_stmt_t>
void ASTParser::if_stmt(IfStmt &ifStmt)
{
  if (debug)
  {
    std::cout << "if_stmt" << std::endl;
  }

  BasicIf basic;
  Expr e;

  eat(TokenType::IF, "expecting IF");
  eat(TokenType::LPAREN, "expecting LPAREN");
  expr(e);
  basic.condition = e;
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  while (!match(TokenType::RBRACE))
  {
    stmt(basic.stmts);
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
  ifStmt.if_part = basic;
  if_stmt_t(ifStmt);
}

// <if_stmt_t> ::= ELSEIF LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE <if_stmt_t> | ELSE LBRACE ( <stmt> )∗ RBRACE | ϵ
void ASTParser::if_stmt_t(IfStmt &ifStmt)
{
  if (debug)
  {
    std::cout << "if_stmt_t" << std::endl;
  }
  if (match(TokenType::ELSE))
  {
    advance();
    eat(TokenType::LBRACE, "expecting LBRACE");

    while (!match(TokenType::RBRACE))
    {
      stmt(ifStmt.else_stmts);
    }
    eat(TokenType::RBRACE, "expecting RBRACE");
  }
  else if (match(TokenType::ELSEIF))
  {
    advance();
    BasicIf elseif;
    Expr e;

    eat(TokenType::LPAREN, "expecting LPAREN");
    expr(e);
    elseif.condition = e;
    eat(TokenType::RPAREN, "expecting RPAREN");
    eat(TokenType::LBRACE, "expecting LBrace");
    while (!match(TokenType::RBRACE))
    {
      stmt(elseif.stmts);
    }
    eat(TokenType::RBRACE, "expecting RBrace");
    ifStmt.else_ifs.push_back(elseif);
    if_stmt_t(ifStmt);
  }
}

// <while_stmt> ::= WHILE LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE
void ASTParser::while_stmt(WhileStmt &ws)
{
  if (debug)
  {
    std::cout << "while_stmt" << std::endl;
  }
  Expr e;
  eat(TokenType::WHILE, "expecting WHILE");
  eat(TokenType::LPAREN, "expecting LPAREN");
  expr(e);
  ws.condition = e;
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  // get 0+ statements
  while (!match(TokenType::RBRACE))
  {
    stmt(ws.stmts);
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
}

// <for_stmt> ::= FOR LPAREN <vdecl_stmt> SEMICOLON <expr> SEMICOLON <assign_stmt> <RPAREN> LBRACE ( <stmt> )∗ RBRACE
void ASTParser::for_stmt(ForStmt &fs)
{
  if (debug){
    std::cout << "for_stmt" << std::endl;
  }
  eat(TokenType::FOR, "expecting FOR");
  eat(TokenType::LPAREN, "expecting LPAREN");
  vdecl_stmt(fs.var_decl);
  eat(TokenType::SEMICOLON, "expecting SEMICOLON");
  expr(fs.condition);
  eat(TokenType::SEMICOLON, "expecting SEMICOLON");
  Token name = curr_token;
  eat(TokenType::ID, "expecting ID");
  assign_stmt(fs.assign_stmt, name);
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  // get 0+ statements
  while (!match(TokenType::RBRACE))
  {
    stmt(fs.stmts);
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
}

// <call_expr> ::= ID LPAREN ( <expr> ( COMMA <expr> )∗ | ϵ ) RPAREN
void ASTParser::call_expr(CallExpr &ce)
{
  if (debug)
  {
    std::cout << "call_expr" << std::endl;
  }
  // already ate ID!
  eat(TokenType::LPAREN, "expecting LPAREN");

  // is there an expression?
  if (isExpr())
  {
    Expr e;
    expr(e);
    ce.args.push_back(e);
    // is there another expression?
    while (match(TokenType::COMMA))
    {
      eat(TokenType::COMMA, "expecting COMMA");
      Expr e2;
      expr(e2);
      ce.args.push_back(e2);
    }
  }
  eat(TokenType::RPAREN, "expecting RPAREN");
}

// <ret_stmt> ::= RETURN <expr>
void ASTParser::ret_stmt(ReturnStmt &rStmt)
{
  if (debug)
  {
  std:
    cout << "ret_stmt" << std::endl;
  }
  eat(TokenType::RETURN, "expecting return");
  Expr e;
  expr(e);
  rStmt.expr = e;
}

// <rvalue> ::= <base_rvalue> | NULL_VAL | <new_rvalue> | <var_rvalue> | <call_expr>
void ASTParser::rvalue(SimpleTerm &st)
{
  if (debug)
  {
    std::cout << "rvalue" << std::endl;
  }
  VarRef vr;

  if (match(TokenType::ID))
  {
    Token id = curr_token;
    eat(TokenType::ID, "expecting ID");

    if (match(TokenType::LPAREN))
    {
      shared_ptr<CallExpr> ce = make_shared<CallExpr>();
      ce->fun_name = id;
      call_expr(*ce);
      st.rvalue = ce;
    }
    else
    {
      shared_ptr<VarRValue> vrv = make_shared<VarRValue>();
      vr.var_name = id;
      vrv->path.push_back(vr);
      var_rvalue(*vrv);
      st.rvalue = vrv;
    }
  }
  else if (isBaseValue() || match(TokenType::NULL_VAL))
  {
    shared_ptr<SimpleRValue> srv = make_shared<SimpleRValue>();
    srv->value = curr_token;
    st.rvalue = srv;
    advance();
  }
  else if (match(TokenType::NEW))
  {
    shared_ptr<NewRValue> nrv = make_shared<NewRValue>();
    advance();
    new_rvalue(*nrv);
    st.rvalue = nrv;
  }
  else
  {
    error("expecting rvalue");
  }
}

// <new_rvalue> ::= NEW ID ( LBRACKET <expr> RBRACKET | ϵ ) | NEW <base_type> LBRACKET <expr> RBRACKET
void ASTParser::new_rvalue(NewRValue &nrv)
{
  if (debug)
  {
    std::cout << "new_rvalue" << std::endl;
  }

  Expr e;

  // already ate 'new'

  if (match(TokenType::ID))
  {
    nrv.type = curr_token;
    eat(TokenType::ID, "expecting ID");
    if (match(TokenType::LBRACKET))
    {
      advance();
      expr(e);
      nrv.array_expr = e;
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
  }
  else
  {
    nrv.type = curr_token;
    DataType dt = DataType{false, nrv.type.lexeme()};
    base_type(dt);
    eat(TokenType::LBRACKET, "expecting LBRACKET");
    expr(e);
    nrv.array_expr = e;
    eat(TokenType::RBRACKET, "expecting RBRACKET");
  }
}

// <base_rvalue> ::= INT_VAL | DOUBLE_VAL | BOOL_VAL | CHAR_VAL | STRING_VAL
// worthless !
void ASTParser::base_rvalue()
{
  if (debug)
  {
    std::cout << "base_rvalue" << std::endl;
  }
  if (match(TokenType::INT_VAL))
  {
    advance();
  }
  else if (match(TokenType::DOUBLE_VAL))
  {
    advance();
  }
  else if (match(TokenType::BOOL_VAL))
  {
    advance();
  }
  else if (match(TokenType::CHAR_VAL))
  {
    advance();
  }
  else if (match(TokenType::STRING_VAL))
  {
    advance();
  }
  else
  {
    error("expecting base_rvalue");
  }
}

// <var_rvalue> ::= ID ( DOT ID | LBRACKET <expr> RBRACKET )∗
void ASTParser::var_rvalue(VarRValue &vrv)
{
  if (debug)
  {
    std::cout << "var_rvalue" << std::endl;
  }

  // already ate ID !!

  while (match({TokenType::DOT, TokenType::LBRACKET}))
  {
    if (match(TokenType::DOT))
    {
      eat(TokenType::DOT, "expecting DOT");
      VarRef vr;
      vr.var_name = curr_token;
      eat(TokenType::ID, "expecting ID");
      vrv.path.push_back(vr);
    }
    else
    {
      eat(TokenType::LBRACKET, "expecting LBRACKET");
      VarRef vr;
      Expr e;

      expr(e);
      vrv.path.back().array_expr = e;
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
    // vrv.path.push_back(vr);
  }
}

// <expr> ::= ( <rvalue> | NOT <expr> | LPAREN <expr> RPAREN ) ( <bin_op> <expr> | ϵ )
void ASTParser::expr(Expr &e)
{
  if (debug)
  {
    std::cout << "expr" << std::endl;
  }

  if (match(TokenType::LPAREN))
  {
    shared_ptr<ComplexTerm> cT = make_shared<ComplexTerm>();
    eat(TokenType::LPAREN, "expecting LPAREN");
    expr(cT->expr);
    eat(TokenType::RPAREN, "expecting RPAREN");
    e.first = cT;
  }
  else if (match(TokenType::NOT))
  {
    e.negated = !e.negated;
    eat(TokenType::NOT, "expecting NOT");
    expr(e);
  }
  else
  {
    shared_ptr<SimpleTerm> sT = make_shared<SimpleTerm>();
    rvalue(*sT);
    e.first = sT;
  }

  if (bin_op())
  {
    // FIXME
    e.op = curr_token;
    advance();
    e.rest = make_shared<Expr>();
    expr(*e.rest);
  }
}