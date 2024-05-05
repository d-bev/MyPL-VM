//----------------------------------------------------------------------
// FILE: mypl_exception.h
// DATE: CPSC 326, Spring 2023
// NAME: S. Bowers
// DESC: Exception class for the MyPL implementation. Supports four
//       types of exceptions: Lexer Errors, Parser Errors, Static
//       Analysis Errors, and VM Errors. To obtain the result message
//       of an exception object use the what() method.
//----------------------------------------------------------------------

#ifndef MYPL_EXCEPTION
#define MYPL_EXCEPTION

#include <string>

class MyPLException : public std::exception
{
public:
  
  // construct a "generic" error exception
  MyPLException(const std::string& msg);

  // helpers to create specific type of error
  static MyPLException LexerError(const std::string& msg);
  static MyPLException ParserError(const std::string& msg);
  static MyPLException StaticError(const std::string& msg);
  static MyPLException VMError(const std::string& msg);
  
  // return a string representation of the error (for printing)
  const char* what() const noexcept;
  
private:

  // the exeception message
  std::string message;

};



#endif
