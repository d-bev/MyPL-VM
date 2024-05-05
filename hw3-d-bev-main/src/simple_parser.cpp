//----------------------------------------------------------------------
// FILE: simple_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: Implementing grammars to make a parser
//----------------------------------------------------------------------

#include "simple_parser.h"
#include <iostream>



const bool debug = false;



SimpleParser::SimpleParser(const Lexer& a_lexer)
  : lexer {a_lexer}
{}


void SimpleParser::advance()
{
  curr_token = lexer.next_token();
}


void SimpleParser::eat(TokenType t, const std::string& msg)
{
  if (!match(t))
    error(msg);
  advance();
}


bool SimpleParser::match(TokenType t)
{
  return curr_token.type() == t;
}


bool SimpleParser::match(std::initializer_list<TokenType> types)
{
  for (auto t : types)
    if (match(t))
      return true;
  return false;
}


void SimpleParser::error(const std::string& msg)
{
  std::string s = msg + " found '" + curr_token.lexeme() + "' ";
  s += "at line " + std::to_string(curr_token.line()) + ", ";
  s += "column " + std::to_string(curr_token.column());
  throw MyPLException::ParserError(s);
}


bool SimpleParser::bin_op()
{
  return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
      TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
      TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
      TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}


/*
  HELPER FUNCTIONS
*/

bool SimpleParser::isExpr() {
  return match({TokenType::NOT, TokenType::LPAREN, TokenType::NEW, TokenType::ID, TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL, TokenType::NULL_VAL});
}

bool SimpleParser::isBaseType(){
  return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::CHAR_TYPE, TokenType::STRING_TYPE}); // TokenType::VOID_TYPE});
}

bool SimpleParser::isBaseVal(){
    return match({TokenType::INT_VAL, TokenType::DOUBLE_VAL, TokenType::BOOL_VAL, TokenType::CHAR_VAL, TokenType::STRING_VAL}); // TokenType::NULL_VAL});
}

/*
  RECURSIVE DESCENT FUNCTIONS
*/

void SimpleParser::parse()
{
  advance();
  if (debug){
    std::cout << "start!" << std::endl;
  }
  while (!match(TokenType::EOS)) {
    if (match(TokenType::STRUCT))
      struct_def();
    else
      fun_def();
  }
  eat(TokenType::EOS, "expecting end-of-file");
}

// <struct_def> ::= STRUCT ID LBRACE <fields> RBRACE
void SimpleParser::struct_def()
{
  if (debug){
    std::cout << "struct_def" << std::endl;
  }
  eat(TokenType::STRUCT, "expecting STRUCT");
  eat(TokenType::ID, "expecting ID ");
  eat(TokenType::LBRACE, "expecting LBRACE");
  if(!match(TokenType::RBRACE))
    fields();
  eat(TokenType::RBRACE, "expcting RBRACE");
}


// <fun_def> ::= ( <data_type> | VOID_TYPE ) ID LPAREN <params> RPAREN LBRACE ( <stmt> )∗ RBRACE
void SimpleParser::fun_def()
{
  if (debug){
    std::cout << "fun_def" << std::endl;
  }
  if(match(TokenType::VOID_TYPE)){
    advance();
  } else {
    data_type();
  }

  eat(TokenType::ID, "expecting ID");
  eat(TokenType::LPAREN, "expecting LPAREN");

  if(!match(TokenType::RPAREN))
    params();

  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  while (!match(TokenType::RBRACE)){
    stmt();
  }

  eat(TokenType::RBRACE, "expecting RBRACE");
}


// <fields> ::= <data_type> ID ( COMMA <data_type> ID )∗ | ϵ
void SimpleParser::fields()
{
  if (debug){
    std::cout << "fields" << std::endl;
  }
  if(match(TokenType::ARRAY)){
    advance();
  }

  if(isBaseType() || isExpr()) {
    data_type();
    eat(TokenType::ID, "expecting ID");
    while(match(TokenType::COMMA)) {
      eat(TokenType::COMMA, "expecting Comma");
      data_type();
      eat(TokenType::ID, "expecting ID");
    }
  }
}


// <params> ::= <data_type> ID ( COMMA <data_type> ID )∗ | ϵ
void SimpleParser::params()
{
  if (debug){
    std::cout << "params" << std::endl;
  }
  data_type();
  eat(TokenType::ID, "expecting ID");

  while (match(TokenType::COMMA)){
    advance();
    // eat(TokenType::COMMA, "expecting COMMA");
    data_type();
    eat(TokenType::ID, "expecting ID");
  }
}


// <data_type> ::= <base_type> | ID | ARRAY ( <base_type> | ID )
void SimpleParser::data_type()
{
  if (debug){
    std::cout << "data_type" << std::endl;
  }
  if(match(TokenType::ID)){
    advance();
  } else if(match(TokenType::ARRAY)) {
    advance();
    if(match(TokenType::ID)){
      advance();
    } else {
      if(match(TokenType::VOID_TYPE))
        advance();
      else
        base_type();
    }
  } else {
    if(match(TokenType::VOID_TYPE))
      advance();
    else
      base_type();
  }
}


// <base_type> ::= INT_TYPE | DOUBLE_TYPE | BOOL_TYPE | CHAR_TYPE | STRING_TYPE
void SimpleParser::base_type()
{
  if (debug){
    std::cout << "base_type" << std::endl;
  }
  if(match(TokenType::INT_TYPE)){
    advance();
  } 
  else if (match(TokenType::DOUBLE_TYPE)){
    advance();
  } 
  else if (match(TokenType::BOOL_TYPE)) {
    advance();
  } 
  else if (match(TokenType::CHAR_TYPE)) {
    advance();
  } 
  else if (match(TokenType::STRING_TYPE)) {
    advance();
  } else {
    error("END OF CONTROL : base_type");
  }
}


// <stmt> ::= <vdecl_stmt> | <assign_stmt> | <if_stmt> | <while_stmt> | <for_stmt> | <call_expr> | <ret_stmt>
void SimpleParser::stmt()
{
  if (debug){
    std::cout << "stmt" << std::endl;
  }
  if(match(TokenType::FOR)){
    for_stmt();
  } else if (match(TokenType::WHILE)){
    while_stmt();
  } else if (match(TokenType::IF)) {
    if_stmt();
  } else if (match(TokenType::RETURN)) {
    ret_stmt();
  } else if (match(TokenType::ID)) {
    
    //eat(TokenType::ID, "expecting ID");
    advance();

    // call_expr, vdecl_stmt, or assign_stmt all can start with ID
    if(match(TokenType::LPAREN)) {
      call_expr();
    }
    else if (match(TokenType::ID)){
      
      //eat(TokenType::ID, "expecting ID");
      advance();
      
      if(match(TokenType::ASSIGN)){
        // DEBUG: failing of VarDecls
        //assign_stmt();
        eat(TokenType::ASSIGN, "expecting ASSIGN");
        expr();
      } else {
        vdecl_stmt();
      }
    } else {
      // if(match(TokenType::ASSIGN)){
      //   // DEBUG: failing of VarDecls
      //   //assign_stmt();
      //   eat(TokenType::ASSIGN, "expecting ASSIGN");
      //   expr();
      // } else {
      //   error("END OF CONTROL : stmt");
      // }
      assign_stmt();
    }
  } else {
    // must start with ARRAY or <base_type>
    vdecl_stmt();
  }
}


// <vdecl_stmt> ::= <data_type> ID ASSIGN <expr>
void SimpleParser::vdecl_stmt()
{
  if (debug){
    std::cout << "vdecl_stmt" << std::endl;
  }

  if(match(TokenType::ARRAY)){
    advance();
  }
  
  if (isBaseType() or match(TokenType::ID)){
    advance();
  } else {
    error("vdecl: not base type or ID");
  }

  eat(TokenType::ID, "expecting ID");
  eat(TokenType::ASSIGN, "expecting ASSIGN");
  expr();
  
}


// <assign_stmt> ::= <lvalue> ASSIGN <expr>
void SimpleParser::assign_stmt()
{
  if (debug){
    std::cout << "assign_stmt" << std::endl;
  }
  lvalue();
  eat(TokenType::ASSIGN, "expecting ASSIGN");
  expr();
}


// <lvalue> ::= ID ( DOT ID | LBRACKET <expr> RBRACKET )∗
void SimpleParser::lvalue()
{
  if (debug){
    std::cout << "lvalue" << std::endl;
  }
  //eat(TokenType::ID, "expecting ID");

  while(match({TokenType::DOT, TokenType::LBRACKET})){
    if(match(TokenType::DOT)){
      eat(TokenType::DOT, "expecting DOT");
      eat(TokenType::ID, "expecting ID");
    }
    else {
      eat(TokenType::LBRACKET, "expecting LBRACKET");
      expr();
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
  }
}


// <if_stmt> ::= IF LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE <if_stmt_t>
void SimpleParser::if_stmt()
{
  if (debug){
    std::cout << "if_stmt" << std::endl;
  }
  eat(TokenType::IF, "expecting IF");
  eat(TokenType::LPAREN, "expecting LPAREN");
  expr();
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  // get 0+ statements
  while(!match(TokenType::RBRACE)) 
  {
    stmt();
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
  
  if_stmt_t();
}

// DEBUG
// <if_stmt_t> ::= ELSEIF LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE <if_stmt_t> | ELSE LBRACE ( <stmt> )∗ RBRACE | ϵ
void SimpleParser::if_stmt_t()
{
  if (debug){
    std::cout << "if_stmt_t" << std::endl;
  }
  if(match(TokenType::ELSE)){
    eat(TokenType::ELSE, "expecting ELSE");

    eat(TokenType::LBRACE, "expecting LBRACE");

    // get 0+ statements
    while(!match(TokenType::RBRACE)) 
    {
      stmt();
    }
    eat(TokenType::RBRACE, "expecting RBRACE");
  } else if (match(TokenType::ELSEIF)){
    eat(TokenType::ELSEIF, "expecting ELSEIF");
    eat(TokenType::LPAREN, "expecting LPAREN");
    expr();
    eat(TokenType::RPAREN, "expecting RPAREN");
    eat(TokenType::LBRACE, "expecting LBRACE");

    // get 0+ statements
    while(!match(TokenType::RBRACE)) 
    {
      stmt();
    }
    eat(TokenType::RBRACE, "expecting RBRACE");

    if_stmt_t();
  } 
}


// <while_stmt> ::= WHILE LPAREN <expr> RPAREN LBRACE ( <stmt> )∗ RBRACE
void SimpleParser::while_stmt()
{
  if (debug){
    std::cout << "while_stmt" << std::endl;
  }
  eat(TokenType::WHILE, "expecting WHILE");
  eat(TokenType::LPAREN, "expecting LPAREN");
  expr();
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  // get 0+ statements
  while(!match(TokenType::RBRACE)) 
  {
    stmt();
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
}


// <for_stmt> ::= FOR LPAREN <vdecl_stmt> SEMICOLON <expr> SEMICOLON <assign_stmt> <RPAREN> LBRACE ( <stmt> )∗ RBRACE                                                                         ^ ?!
void SimpleParser::for_stmt()
{
  if (debug){
    std::cout << "for_stmt" << std::endl;
  }
  eat(TokenType::FOR, "expecting FOR");
  eat(TokenType::LPAREN, "expecting LPAREN");
  vdecl_stmt();
  eat(TokenType::SEMICOLON, "expecting SEMICOLON");
  expr();
  eat(TokenType::SEMICOLON, "expecting SEMICOLON");
  eat(TokenType::ID, "expecting ID");
  assign_stmt();
  eat(TokenType::RPAREN, "expecting RPAREN");
  eat(TokenType::LBRACE, "expecting LBRACE");

  // get 0+ statements
  while(!match(TokenType::RBRACE)) 
  {
    stmt();
  }
  eat(TokenType::RBRACE, "expecting RBRACE");
}


// <call_expr> ::= ID LPAREN ( <expr> ( COMMA <expr> )∗ | ϵ ) RPAREN
void SimpleParser::call_expr()
{
  if (debug){
    std::cout << "call_expr" << std::endl;
  }
  //eat(TokenType::ID, "expecting ID");
  eat(TokenType::LPAREN, "expecting LPAREN");
  
  // is there an expression?
  if(isExpr()){
    expr();
    // is there another expression?
    while(match(TokenType::COMMA)){
      eat(TokenType::COMMA, "expecting COMMA");
      expr();
    }
  }
  eat(TokenType::RPAREN, "expecting RPAREN");
}


// <ret_stmt> ::= RETURN <expr>
void SimpleParser::ret_stmt()
{
  if (debug){
    std::cout << "ret_stmt" << std::endl;
  }
  eat(TokenType::RETURN, "expecting RETURN");
  expr();
}


// <expr> ::= ( <rvalue> | NOT <expr> | LPAREN <expr> RPAREN ) ( <bin_op> <expr> | ϵ )
void SimpleParser::expr()
{
  if (debug){
    std::cout << "expr" << std::endl;
  }
  if (match(TokenType::LPAREN)) {
    eat(TokenType::LPAREN, "expecting LPAREN");
    expr();
    eat(TokenType::RPAREN, "expecting RPAREN");
  } else if (match(TokenType::NOT)) {
    eat(TokenType::NOT, "expecting NOT");
    expr();
  } else {
    rvalue();
  }

  if(bin_op()) {
    advance();
    expr();
  }
}


// <rvalue> ::= <base_rvalue> | NULL_VAL | <new_rvalue> | <var_rvalue> | <call_expr>
void SimpleParser::rvalue()
{
  if (debug){
    std::cout << "rvalue" << std::endl;
  }
  
  if (match(TokenType::ID)) {
    eat(TokenType::ID, "expecting ID");
    if (match(TokenType::LPAREN)){
      // DEBUG
      //eat(TokenType::LPAREN, "expecting LPAREN");
      call_expr();
    } else {
      var_rvalue();
    }
  } else if(match(TokenType::NULL_VAL)){
    eat(TokenType::NULL_VAL, "expecting NULL_VAL");
  } 
  else if (isBaseVal()) {
    base_rvalue();
  }
  else if (match(TokenType::NEW)) {
    new_rvalue();
  }
  else {
    error("expecting rvalue");
  }
}


// <new_rvalue> ::= NEW ID ( LBRACKET <expr> RBRACKET | ϵ ) | NEW <base_type> LBRACKET <expr> RBRACKET
void SimpleParser::new_rvalue()
{
  if (debug){
    std::cout << "new_rvalue" << std::endl;
  }
  eat(TokenType::NEW, "expecting NEW");

  if(match(TokenType::ID)) {
    eat(TokenType::ID, "expecting ID");
    if(match(TokenType::LBRACKET)){
      advance();
      expr();
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
  } else {
    base_type();
    eat(TokenType::LBRACKET, "expecting LBRACKET");
    expr();
    eat(TokenType::RBRACKET, "expecting RBRACKET");
  }
}


// <base_rvalue> ::= INT_VAL | DOUBLE_VAL | BOOL_VAL | CHAR_VAL | STRING_VAL
void SimpleParser::base_rvalue()
{
  if (debug){
    std::cout << "base_rvalue" << std::endl;
  }
  if (match(TokenType::INT_VAL)){
    advance();
  } else if (match(TokenType::DOUBLE_VAL)) {
    advance();
  } else if (match(TokenType::BOOL_VAL)) {
    advance();
  } else if (match(TokenType::CHAR_VAL)) {
    advance();
  } else if (match(TokenType::STRING_VAL)) {
    advance();
  } else {
    error("expecting base_rvalue");
  }
}

// <var_rvalue> ::= ID ( DOT ID | LBRACKET <expr> RBRACKET )∗
void SimpleParser::var_rvalue()
{
  if (debug){
    std::cout << "var_rvalue" << std::endl;
  }
  //eat(TokenType::ID, "expecting ID");

  while (match({TokenType::DOT, TokenType::LBRACKET}))
  {
    if(match(TokenType::DOT)){
      eat(TokenType::DOT, "expecting DOT");
      eat(TokenType::ID, "expecting ID");
    } else {
      eat(TokenType::LBRACKET, "expecting LBRACKET");
      expr();
      eat(TokenType::RBRACKET, "expecting RBRACKET");
    }
  }
}