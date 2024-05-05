//----------------------------------------------------------------------
// FILE: lexer_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic lexer tests
//----------------------------------------------------------------------

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "mypl_exception.h"
#include "token.h"
#include "lexer.h"


using namespace std;

//------------------------------------------------------------
// Positive Test Cases
//------------------------------------------------------------

TEST(BasicLexerTest, SimpleSymbol) {
  stringstream in(".");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::DOT, t.type());
  ASSERT_EQ(".", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, EmptyInput) {
  stringstream in;
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
}

TEST(BasicLexerTest, SkipComment) {
  stringstream in("# this is a comment");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(20, t.column());
}

TEST(BasicLexerTest, OneCharacterSymbols) {
  stringstream in(",.;+-*/()[]{}=<>");
  Lexer lexer(in);
  vector<TokenType> types = {TokenType::COMMA, TokenType::DOT, TokenType::SEMICOLON,
    TokenType::PLUS, TokenType::MINUS, TokenType::TIMES, TokenType::DIVIDE,
    TokenType::LPAREN, TokenType::RPAREN, TokenType::LBRACKET,
    TokenType::RBRACKET, TokenType::LBRACE, TokenType::RBRACE,
    TokenType::ASSIGN, TokenType::LESS, TokenType::GREATER};
  for (int i = 0; i < types.size(); ++i) {
    Token t = lexer.next_token();
    ASSERT_EQ(types[i], t.type());
    ASSERT_EQ(1, t.line());
    ASSERT_EQ(i+1, t.column());
  }
  ASSERT_EQ(TokenType::EOS, lexer.next_token().type());
}

TEST(BasicLexerTest, TwoCharacterSymbols) {
  stringstream in("!=>=<===");
  Lexer lexer(in);
  vector<TokenType> types = {TokenType::NOT_EQUAL, TokenType::GREATER_EQ,
    TokenType::LESS_EQ, TokenType::EQUAL};
  for (int i = 0; i < types.size(); ++i) {
    Token t = lexer.next_token();
    ASSERT_EQ(types[i], t.type());
    ASSERT_EQ(1, t.line());
    ASSERT_EQ((i*2)+1, t.column());
  }
  ASSERT_EQ(TokenType::EOS, lexer.next_token().type());
}  

TEST(BasicLexerTest, OneSymbolPerLine) {
  stringstream in(",\n.\n;\n(");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::COMMA, t.type());
  ASSERT_EQ(",", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOT, t.type());
  ASSERT_EQ(".", t.lexeme());
  ASSERT_EQ(2, t.line());
  ASSERT_EQ(1, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::SEMICOLON, t.type());
  ASSERT_EQ(";", t.lexeme());
  ASSERT_EQ(3, t.line());
  ASSERT_EQ(1, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::LPAREN, t.type());
  ASSERT_EQ("(", t.lexeme());
  ASSERT_EQ(4, t.line());
  ASSERT_EQ(1, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, BasicChars) {
  stringstream in("'a' '?' '<' '\\n' '\\t'");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_VAL, t.type());
  ASSERT_EQ("a", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_VAL, t.type());
  ASSERT_EQ("?", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_VAL, t.type());
  ASSERT_EQ("<", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(9, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_VAL, t.type());
  ASSERT_EQ("\\n", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(13, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_VAL, t.type());
  ASSERT_EQ("\\t", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(18, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, BasicStrings) {
  stringstream in("\"abc\" \"><!=\" \"foo bar\" \"foo bar baz \"");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::STRING_VAL, t.type());
  ASSERT_EQ("abc", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::STRING_VAL, t.type());
  ASSERT_EQ("><!=", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(7, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::STRING_VAL, t.type());
  ASSERT_EQ("foo bar", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(14, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::STRING_VAL, t.type());
  ASSERT_EQ("foo bar baz ", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(24, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, BasicInts) {
  stringstream in("0 42 10 9876543210");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("0", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("42", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(3, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("10", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(6, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("9876543210", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(9, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, BasicDoubles) {
  stringstream in("0.0 0.00 3.14 2.0 0.2");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("0.0", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("0.00", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("3.14", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(10, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("2.0", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(15, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("0.2", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(19, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, SpecialValues) {
  stringstream in("null true false");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::NULL_VAL, t.type());
  ASSERT_EQ("null", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::BOOL_VAL, t.type());
  ASSERT_EQ("true", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(6, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::BOOL_VAL, t.type());
  ASSERT_EQ("false", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(11, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, PrimitiveTypes) {
  stringstream in("int double char string bool void");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_TYPE, t.type());
  ASSERT_EQ("int", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_TYPE, t.type());
  ASSERT_EQ("double", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::CHAR_TYPE, t.type());
  ASSERT_EQ("char", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(12, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::STRING_TYPE, t.type());
  ASSERT_EQ("string", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(17, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::BOOL_TYPE, t.type());
  ASSERT_EQ("bool", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(24, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::VOID_TYPE, t.type());
  ASSERT_EQ("void", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(29, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, LogicalOperators) {
  stringstream in("and or not");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::AND, t.type());
  ASSERT_EQ("and", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::OR, t.type());
  ASSERT_EQ("or", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::NOT, t.type());
  ASSERT_EQ("not", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(8, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, ConditionalReservedWords) {
  stringstream in("if elseif else");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::IF, t.type());
  ASSERT_EQ("if", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ELSEIF, t.type());
  ASSERT_EQ("elseif", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(4, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ELSE, t.type());
  ASSERT_EQ("else", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(11, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, LoopingReservedWords) {
  stringstream in("for while");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::FOR, t.type());
  ASSERT_EQ("for", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::WHILE, t.type());
  ASSERT_EQ("while", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, FunctionReservedWords) {
  stringstream in("return");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::RETURN, t.type());
  ASSERT_EQ("return", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, ComplexTypeReservedWords) {
  stringstream in("struct array");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::STRUCT, t.type());
  ASSERT_EQ("struct", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ARRAY, t.type());
  ASSERT_EQ("array", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(8, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, AdditionalReservedWords) {
  stringstream in("new");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::NEW, t.type());
  ASSERT_EQ("new", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, Identifiers) {
  stringstream in("x xs f0_0 foo_bar foo_bar_baz quix__");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("x", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("xs", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(3, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("f0_0", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(6, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("foo_bar", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(11, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("foo_bar_baz", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(19, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("quix__", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(31, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, TokensWithComments) {
  stringstream in("x < 1 # simple assignment \n if 3.14");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("x", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::LESS, t.type());
  ASSERT_EQ("<", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(3, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("1", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::IF, t.type());
  ASSERT_EQ("if", t.lexeme());
  ASSERT_EQ(2, t.line());
  ASSERT_EQ(2, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("3.14", t.lexeme());
  ASSERT_EQ(2, t.line());
  ASSERT_EQ(5, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, NoSpacesKeywords) {
  stringstream in("for(intx)ifelsey=4end1");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::FOR, t.type());
  ASSERT_EQ("for", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());
  t = lexer.next_token();
  ASSERT_EQ(TokenType::LPAREN, t.type());
  ASSERT_EQ("(", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(4, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("intx", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::RPAREN, t.type());
  ASSERT_EQ(")", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(9, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("ifelsey", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(10, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ASSIGN, t.type());
  ASSERT_EQ("=", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(17, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("4", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(18, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::ID, t.type());
  ASSERT_EQ("end1", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(19, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

TEST(BasicLexerTest, NoSpacesNumbers) {
  stringstream in("32.1.42");
  Lexer lexer(in);
  Token t = lexer.next_token();
  ASSERT_EQ(TokenType::DOUBLE_VAL, t.type());
  ASSERT_EQ("32.1", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(1, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::DOT, t.type());
  ASSERT_EQ(".", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(5, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::INT_VAL, t.type());
  ASSERT_EQ("42", t.lexeme());
  ASSERT_EQ(1, t.line());
  ASSERT_EQ(6, t.column());  
  t = lexer.next_token();
  ASSERT_EQ(TokenType::EOS, t.type());
}

//------------------------------------------------------------
// Negative Test Cases
//------------------------------------------------------------

TEST(BasicLexerTest, MultilineString) {
  stringstream in("\"hello \nworld");
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: found end-of-line in string at line 1, column 8", m);
  }
}

TEST(BasicLexerTest, NonTerminatedString) {
  stringstream in("\"");
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: found end-of-file in string at line 1, column 2", m);
  }
}

TEST(BasicLexerTest, MultilineChar) {
  stringstream in("'\n'");
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: found end-of-line in character at line 1, column 2", m);
  }
}

TEST(BasicLexerTest, NonTerminatedChar) {
  stringstream in("'");
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: found end-of-file in character at line 1, column 2", m);
  }
}

TEST(BasicLexerTest, TooManyCharactersInChar) {
  stringstream in("'ab'");
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: expecting ' found b at line 1, column 3", m);
  }
}

TEST(BasicLexerTest, EmptyChar) {
  stringstream in("''");  
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: empty character at line 1, column 2", m);
  }
}

TEST(BasicLexerTest, InvalidNot) {
  stringstream in("!>");  
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: expecting '!=' found '!>' at line 1, column 1", m);
  }
}

TEST(BasicLexerTest, MissingDoubleDigit) {
  stringstream in("32.a");  
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: missing digit in '32.' at line 1, column 4", m);
  }
}

TEST(BasicLexerTest, LeadingZeroInt) {
  stringstream in("02");  
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: leading zero in number at line 1, column 1", m);
  }
}

TEST(BasicLexerTest, InvalidSymbol) {
  stringstream in("?");  
  Lexer lexer(in);
  try {
    lexer.next_token();
    FAIL();
  } catch(MyPLException& e) {
    string m = e.what();
    ASSERT_EQ("Lexer Error: unexpected character '?' at line 1, column 1", m);
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

