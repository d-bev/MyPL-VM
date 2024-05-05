//----------------------------------------------------------------------
// FILE: mypl_exception.cpp
// DATE: Spring 2023
// NAME: S. Bowers
// DESC: ...
//----------------------------------------------------------------------

#include "mypl_exception.h"


MyPLException::MyPLException(const std::string& msg)
  : message {msg}
{
}

MyPLException MyPLException::LexerError(const std::string& msg)
{
  return MyPLException("Lexer Error: " + msg);
}

MyPLException MyPLException::ParserError(const std::string& msg)
{
  return MyPLException("Parser Error: " + msg);
}

MyPLException MyPLException::StaticError(const std::string& msg)
{
  return MyPLException("Static Error: " + msg);  
}

MyPLException MyPLException::VMError(const std::string& msg)
{
  return MyPLException("VM Error: " + msg);    
}
  
const char* MyPLException::what() const noexcept 
{
  return message.c_str();
}
