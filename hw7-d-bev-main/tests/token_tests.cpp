//----------------------------------------------------------------------
// FILE: token_tests.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: S. Bowers
// DESC: Basic token tests
//----------------------------------------------------------------------

#include <string>
#include <gtest/gtest.h>
#include "token.h"


TEST(BasicTokenTest, TokenCreation) {
  Token token1(TokenType::INT_TYPE, "int", 0, 0);
  Token token2(TokenType::STRING_TYPE, "string", 1, 1);
}

TEST(BasicTokenTest, TypeGivenIsReturned) {
  Token token1(TokenType::INT_TYPE, "int", 0, 0);
  Token token2(TokenType::STRING_TYPE, "string", 0, 0);
  ASSERT_EQ(TokenType::INT_TYPE, token1.type());
  ASSERT_EQ(TokenType::STRING_TYPE, token2.type());
}

TEST(BasicTokenTest, LexemeGivenIsReturned) {
  Token token1(TokenType::BOOL_TYPE, "bool", 0, 0);
  Token token2(TokenType::DOUBLE_TYPE, "double", 0, 0);
  ASSERT_EQ("bool", token1.lexeme());
  ASSERT_EQ("double", token2.lexeme());
}

TEST(BasicTokenTest, LineGivenIsReturned) {
  Token token1(TokenType::BOOL_TYPE, "bool", 10, 0);
  Token token2(TokenType::BOOL_TYPE, "bool", 20, 0);
  ASSERT_EQ(10, token1.line());
  ASSERT_EQ(20, token2.line());
}

TEST(BasicTokenTest, ColumnGivenIsReturned) {
  Token token1(TokenType::BOOL_TYPE, "bool", 0, 10);
  Token token2(TokenType::BOOL_TYPE, "bool", 0, 20);
  ASSERT_EQ(10, token1.column());
  ASSERT_EQ(20, token2.column());
}

TEST(BasicTokenTest, CorrectlyPrints) {
  Token token(TokenType::BOOL_TYPE, "bool", 4, 2);
  ASSERT_EQ("4, 2: BOOL_TYPE 'bool'", to_string(token));
}


//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

