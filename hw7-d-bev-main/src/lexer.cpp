//----------------------------------------------------------------------
// FILE: lexer.cpp
// DATE: CPSC 326, Spring 2023
// NAME: Dominic Bevilacqua
// DESC: HW2 - Implementation of Lexer
//----------------------------------------------------------------------

#include "lexer.h"
#include "token.h"

using namespace std;


Lexer::Lexer(istream& input_stream)
  : input {input_stream}, column {0}, line {1}
{}


char Lexer::read()
{
  ++column;
  return input.get();
}


char Lexer::peek()
{
  return input.peek();
}


void Lexer::error(const string& str, int line, int column) const
{
  throw MyPLException::LexerError(str + " at line " + to_string(line) +
                                  ", column " + to_string(column));
}


Token Lexer::next_token()
{
  string msg;
  char ch = read();


  // STEP 1: Read over all newlines, whitespace, and comments (respectively)


  while((ch == '#') or isspace(ch)){
    if(ch == '\n'){
    column = 0;
    line += 1;
    }
    else if(ch == '#'){
      while((ch != '\n') and (ch != EOF)){
        ch = read();

      }
      if(ch == '\n'){
        column = 0;
        line++;
      } 
      else if (ch == EOF){
        return Token(TokenType::EOS, "EOS", line, column);
      }
    }
    ch = read();
  }


  // STEP 2: Check for EOF


  if(ch == EOF) {
    return Token(TokenType::EOS, "EOS", line, column);
  }


  // STEP 3: Check for single-character tokens


  if(ch == ',') {
    return Token(TokenType::COMMA, ",", line, column);
  }
  if(ch == '.') {
    return Token(TokenType::DOT, ".", line, column);
  }
  if(ch == ';') {
    return Token(TokenType::SEMICOLON, ";", line, column);
  }

  if(ch =='+') {
    return Token(TokenType::PLUS, "+", line, column);
  }
  if(ch == '-') {
    return Token(TokenType::MINUS, "-", line, column);
  }
  if(ch == '*') {
    return Token(TokenType::TIMES, "*", line, column);
  }
  if(ch == '/') {
    return Token(TokenType::DIVIDE, "/", line, column);
  }

  if(ch == '(') {
    return Token(TokenType::LPAREN, "(", line, column);
  }
  if(ch == ')') {
    return Token(TokenType::RPAREN, ")", line, column);
  }
  if(ch == '[') {
    return Token(TokenType::LBRACKET, "[", line, column);
  }
  if(ch == ']') {
    return Token(TokenType::RBRACKET, "]", line, column);
  }
  if(ch == '{') {
    return Token(TokenType::LBRACE, "{", line, column);
  }
  if(ch == '}') {
    return Token(TokenType::RBRACE, "}", line, column);
  }
  

  // STEP 4: Check for 2-character tokens


  if(ch == '<') {
    ch = peek();
    if(ch == '=') {
      ch = read();
      return Token(TokenType::LESS_EQ, "<=", line, column - 1);
    }
    else {
      return Token(TokenType::LESS, "<", line, column);
    }
  }

  if(ch == '>') {
    ch = peek();
    if(ch == '=') {
      ch = read();
      return Token(TokenType::GREATER_EQ, ">=", line, column - 1);
    }
    else {
      return Token(TokenType::GREATER, ">", line, column);
    }
  }

  if(ch == '=') {
    ch = peek();
    if(ch == '=') {
      ch = read();
      return Token(TokenType::EQUAL, "==", line, column - 1);
    }
    else {
      return Token(TokenType::ASSIGN, "=", line, column);
    }
  }

  if(ch == '!') {
    ch = peek();
    if(ch == '=') {
      ch = read();
      return Token(TokenType::NOT_EQUAL, "!=", line, column - 1);
    } else if(ch == '\n' || ch == EOF) {
      error("unexpected character '" + ch, line, column);
    } else {
      msg = "expecting '!=' found '!";
      msg.push_back(ch);
      msg += "\'";
      error(msg, line, column);
      return Token();
    }
  }


  // STEP 5: Check for character values

  
  if(ch == '\'') {
    int counter = column;
    ch = read();

    // check if empty character
    if(ch == '\'') { 
      error("empty character", line, column);
      return Token();
    }

    // check for tomfoolery
    if(ch == '\n'){
      error("found end-of-line in character", line, column);
    } else if (ch == EOF){
      error("found end-of-file in character", line, column);
    }

    // check for escaped chars
    if(ch == '\\'){
      ch = read();
      if(ch == 'n'){
        ch = read();
        return Token(TokenType::CHAR_VAL, "\\n", line, counter);
      } else if (ch == 't'){
        ch = read();
        return Token(TokenType::CHAR_VAL, "\\t", line, counter);
      } else if (ch == 'r'){
        ch = read();
        return Token(TokenType::CHAR_VAL, "\\r", line, counter);
      } else {
        if(ch != EOF){
          msg = "expecting \' found ";
          msg.push_back(ch);
          error(msg, line, column);
        }
      }
    }

    // check if char is missing single quote
    if(peek() != '\'') {
      if(peek() != EOF){
        ch = read();
        msg = "expecting \' found ";
        msg.push_back(ch);
        error(msg, line, column);
        return Token();
      } 
      else {
        error("found end-of-file in character", line, counter);
      }
    } else {
      msg.push_back(ch);
      Token token(TokenType::CHAR_VAL, msg, line, counter);
      ch = read();
      return token;
    }

    return Token(TokenType::STRING_VAL, msg, line, counter);
  }


  // STEP 6: Check for string values


  if(ch == '\"') {
    int counter = column;
    ch = read();

    // check if empty string
    if(ch == '\"') {
      return Token(TokenType::STRING_VAL, "", line, counter);
    }

    // grab rest of string
    while(ch != '\"') {
      if(ch == EOF) { 
        // non-terminated string
        error("found end-of-file in string", line, column);
      }
      if(ch == '\n') {
        // newlines aren't allowed in strings
        error("found end-of-line in string", line, column);
      }
      msg += ch;
      ch = read();
    }
    return Token(TokenType::STRING_VAL, msg, line, counter);
  }


  // STEP 7: Check for ints and doubles


  if(isdigit(ch)) {
    bool decimalFlag = false;
    int counter = column;
    msg += ch;
    while(isdigit(peek()) or (peek() == '.')) {
      if((peek() == '.') && decimalFlag) { 
        break;
        //error("too many decimal points in double value '" + msg + "'", line, counter);
      }
      ch = read();
      if(ch == '.' || isdigit(ch)) {
        if(ch == '.') {
          decimalFlag = true;
        }
        msg += ch;
      }

    }

    if(decimalFlag) {
      // leading zero in double
      if(msg.at(0) == '0' && isdigit(msg.at(1))) { 
        error("leading zero in number", line, counter);
        return Token();
      }
      if(msg.back() == '.'){
        error("missing digit in '" + msg + "'", line, column + 1);    
      }
      return Token(TokenType::DOUBLE_VAL, msg, line, counter);
    }
    else {
      // leading zero in string
      if(msg.at(0) == '0' && msg.length() > 1 && !decimalFlag) { 
        error("leading zero in number", line, counter);
        return Token();
      }
      return Token(TokenType::INT_VAL, msg, line, counter);
    }
  }


  // STEP 8: Build lexeme and check for reserved words and IDs


  if(isalpha(ch)){
    int counter = column;
    
    while(isdigit(ch) or (ch == '_') or isalpha(ch)){
      msg += ch;
      ch = peek();
      if(!isdigit(ch) and !isalpha(ch) and (ch != '_')){
        break;
      } else {
        ch = read();
      }
    }

    if(msg == "true") {
      return Token(TokenType::BOOL_VAL, "true", line, counter);
    }
    else if(msg == "false") {
      return Token(TokenType::BOOL_VAL, "false", line, counter); 
    }
    else if(msg == "null"){
      return Token(TokenType::NULL_VAL, "null", line, counter); 
    }
    else if(msg == "and") {
      return Token(TokenType::AND, "and", line, counter); 
    }
    else if(msg == "or") {
      return Token(TokenType::OR, "or", line, counter); 
    }
    else if(msg == "not") {
      return Token(TokenType::NOT, "not", line, counter); 
    }
    else if(msg == "bool") {
      return Token(TokenType::BOOL_TYPE, "bool", line, counter); 
    }
    else if(msg == "int") {
      return Token(TokenType::INT_TYPE, "int", line, counter); 
    }
    else if(msg == "double") {
      return Token(TokenType::DOUBLE_TYPE, "double", line, counter); 
    }
    else if(msg == "char") {
      return Token(TokenType::CHAR_TYPE, "char", line, counter); 
    }
    else if(msg == "string") {
      return Token(TokenType::STRING_TYPE, "string", line, counter); 
    }
    else if(msg == "void") {
      return Token(TokenType::VOID_TYPE, "void", line, counter); 
    }
    else if(msg == "struct") {
      return Token(TokenType::STRUCT, "struct", line, counter); 
    }
    else if(msg == "array") {
      return Token(TokenType::ARRAY, "array", line, counter); 
    }
    else if(msg == "while") {
      return Token(TokenType::WHILE, "while", line, counter); 
    }
    else if(msg == "for") {
      return Token(TokenType::FOR, "for", line, counter); 
    }
    else if(msg == "if") {
      return Token(TokenType::IF, "if", line, counter); 
    }
    else if(msg == "elseif") {
      return Token(TokenType::ELSEIF, "elseif", line, counter); 
    }
    else if(msg == "else") {
      return Token(TokenType::ELSE, "else", line, counter); 
    }
    else if(msg == "new") {
      return Token(TokenType::NEW, "new", line, counter); 
    }
    else if(msg == "return") {
      return Token(TokenType::RETURN, "return", line, counter); 
    }
    else {
      return Token(TokenType::ID, msg, line, counter);
    }
  }
  
  // If we get here, must be an unexpected token

  if(!isspace(ch) && (ch != EOF)) {
    int counter = column;
    while(!isspace(ch) && (ch != EOF)) {
      msg += ch;
      ch = read();
    }
    error("unexpected character '" + msg + "'", line, counter);
  }

  if(ch == EOF) {
    return Token(TokenType::EOS, "EOS", line, column);
  }


  error(msg, -1, -1);
}