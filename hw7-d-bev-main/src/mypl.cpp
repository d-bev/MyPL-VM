//----------------------------------------------------------------------
// FILE: mypl.cpp
// DATE: Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: OPL Semester HW Project
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"
#include "mypl_exception.h"
#include "simple_parser.h"
#include "lexer.h"
#include "print_visitor.h"
#include "ast_parser.h"
#include "ast.h"
#include "semantic_checker.h"
#include "code_generator.h"

using namespace std;

bool debug = true;

void usage();
void lex(string filename);
void parse(string filename);
void print(string filename);
void check(string filename);
void ir(string filename);
void normal(string filename);

int main(int argc, char *argv[])
{
  string filename = "";
  string option = "";

  // Check input type

  switch (argc)
  {
    case 1: // If argc = 1, manual entry ("Normal mode")
    {
      if (debug)
        cout << "Case 1: normal" << endl;
      normal(filename);
      break;
    }
    case 2: // If argc = 2, need to distinguish if argv[1] is an option or a filename
    {
      option = argv[1];

      // Check if argv[1] is a valid option, otherwise, check if it's a valid file

      if (option.compare("--help") == 0)
      {
        if (debug)
          cout << "Case 2: help" << endl;
        usage();
      }
      else if (option.compare("--lex") == 0)
      {
        if (debug)
          cout << "Case 2: lex" << endl;
        lex(filename);
      }
      else if (option.compare("--parse") == 0)
      {
        if (debug)
          cout << "Case 2: parse" << endl;
        parse(filename);
      }
      else if (option.compare("--print") == 0)
      {
        if (debug)
          cout << "Case 2: print" << endl;
        print(filename);
      }
      else if (option.compare("--check") == 0)
      {
        if (debug)
          cout << "Case 2: check" << endl;
        check(filename);
      }
      else if (option.compare("--ir") == 0)
      {
        if (debug)
          cout << "Case 2: ir" << endl;
        ir(filename);
      }
      else
      {
        // if here, argv[1] isn't a valid option: should be a filename
        filename = argv[1];

        if (debug)
          cout << "Case 2: Normal Mode" << endl;

        normal(filename);
      }
      break;
    }
    case 3: // If argc = 3, using both an option and a file
    {
      option = argv[1];
      filename = argv[2];

      if (option.compare("--help") == 0)
      {
        if (debug)
          cout << "Case 3: help" << endl;
        usage();
      }
      else if (option.compare("--lex") == 0)
      {
        if (debug)
          cout << "Case 3: lex" << endl;
        lex(filename);
      }
      else if (option.compare("--parse") == 0)
      {
        if (debug)
          cout << "Case 3: parse" << endl;
        parse(filename);
      }
      else if (option.compare("--print") == 0)
      {
        if (debug)
          cout << "Case 3: print" << endl;
        print(filename);
      }
      else if (option.compare("--check") == 0)
      {
        if (debug)
          cout << "Case 3: check" << endl;
        check(filename);
      }
      else if (option.compare("--ir") == 0)
      {
        if (debug)
          cout << "Case 3: ir" << endl;
        ir(filename);
      }
      else
      {
        //  detected "./mypl [option] [file]", but the option was invalid
        cout << "Case 3 Error: Option '" << option << "' not found!" << endl;
        usage();
      }
      break;
    }
    default:
    {
      if (debug)
        cout << "Case Default: help" << endl;
      usage();
      break;
    }
  }

  return 0;
}

/*******************
 * HELPER FUNCTIONS *
 *******************/

istream *swapInput(string filename)
{
  if (debug)
    cout << "testing input..." << endl;

  istream *input = new ifstream(filename);
  return input;
}

void usage()
{
  cout << "Usage: ./mypl [option] [script-file]" << endl;
  cout << "Options:" << endl;
  cout << "  --help \tprints this message" << endl;
  cout << "  --lex  \tdisplays token information" << endl;
  cout << "  --parse\tchecks for syntax errors" << endl;
  cout << "  --print\tpretty prints program" << endl;
  cout << "  --check\tstatically checks program" << endl;
  cout << "  --ir   \tprint intermediate (code) representation" << endl;
}

void lex(string filename)
{
  // HW2: calls lexer and prints corresponding tokens

  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    Token t = lexer.next_token();
    cout << to_string(t) << endl;
    while (t.type() != TokenType::EOS)
    {
      t = lexer.next_token();
      cout << to_string(t) << endl;
    }
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }

  if (input->eof())
    delete input;
}

void parse(string filename)
{
  // HW3: calls parser and builds AST

  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    SimpleParser parser(lexer);
    parser.parse();
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }

  if (input->eof())
    delete input;
}

void print(string filename)
{
  // HW4: building the AST

  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    ASTParser parser(lexer);
    Program p = parser.parse();
    PrintVisitor v(cout);
    p.accept(v);
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }

  if (input->eof())
    delete input;
}

void check(string filename)
{
  // HW5: type-checking

  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    ASTParser parser(lexer);
    Program p = parser.parse();
    SemanticChecker v;
    p.accept(v);
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }

  if (input->eof())
    delete input;
}

void ir(string filename)
{
  // HW7: Code Generation

  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    ASTParser parser(lexer);
    Program p = parser.parse();
    SemanticChecker t;
    p.accept(t);
    VM vm;
    CodeGenerator g(vm);
    p.accept(g);
    cout << to_string(vm) << endl;
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }

  if (input->eof())
    delete input;
}

void normal(string filename)
{
  istream *input = &cin;

  // checks if filename isn't empty
  if (filename.compare(""))
  {
    input = swapInput(filename);

    if (input->fail())
    {
      cout << "Input Error: Could not find file '" << filename << "'" << endl;
      return;
    }
  }

  // *input should now be &cin (if no filename) or the new ifstream (if there's a valid filename)
  Lexer lexer(*input);

  try
  {
    ASTParser parser(lexer);
    Program p = parser.parse();
    SemanticChecker t;
    p.accept(t);
    VM vm;
    CodeGenerator g(vm);
    p.accept(g);
    vm.run();
  }
  catch (MyPLException &ex)
  {
    cerr << ex.what() << endl;
  }
}