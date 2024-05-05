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

using namespace std;


void usage(){
  cout << "Usage: ./mypl [option] [script-file]" << endl;
  cout << "Options:" << endl;
  cout << "  --help\tprints this message" << endl;
  cout << "  --lex\tdisplays token information" << endl;
  cout << "  --parse\tchecks for syntax errors" << endl;
  cout << "  --print\tpretty prints program" << endl;
  cout << "  --check\tstatically checks program" << endl;
  cout << "  --ir\tprint intermediate (code) representation" << endl;
}

int main(int argc, char* argv[])
{
  istream* input = &cin;
  string filename;
  string option;
  string line;
  vector<string> v;
  vector<string>::iterator it;

  Lexer lexer(*input);

  // Check input type

  switch (argc){
    case 1:
      break;
    case 2:
      // If argc = 2, need to distinguish if argv[1] is an option or a filename

      // Check if argv[1] is a valid option, otherwise, check if it's a valid file
      option = argv[1];

      if (option.compare("--help") == 0) {
        // print Usage message
        usage();
      } 
      else if (option.compare("--lex") == 0) {

        // HW2: calls lexer and prints corresponding tokens     
        try {
          Token t = lexer.next_token();
          cout << to_string(t) << endl;
          while (t.type() != TokenType::EOS) {
            t = lexer.next_token();
            cout << to_string(t) << endl;
          }
        } catch (MyPLException& ex) {
          cerr << ex.what() << endl;
        }
        return 1;
      }
      else if (option.compare("--parse") == 0) {

        // HW3: calls parser and builds AST
        try {
          SimpleParser parser(lexer);
          parser.parse();
        } catch (MyPLException& ex) {
          cerr << ex.what() << endl;
        }
      } 
      else if (option.compare("--print") == 0) {
        // print first word of input
        cout << "[PRINT MODE]" << endl;
        while( !isspace(input->peek())){
          cout << char(input->get());
        }
        cout << endl;
      } 
      else if (option.compare("--check") == 0) {
        // print first line of input
        cout << "[CHECK MODE]" << endl;
        getline(cin, line);
        cout << line << endl;
      } 
      else if (option.compare("--ir") == 0) {
        // print first two lines of input
        string line2;
        cout << "[IR MODE]" << endl;
        getline(cin, line);
        getline(cin, line2);
        line.append("\n");
        cout << line.append(line2) << endl;
      }
      else { 
          // Alter to use file input
          filename = argv[1];
          input = new ifstream(filename);

          if( input->fail()){
            cout << "Could not find file: " << filename << endl;
            return 1;
          }

          // Print whole file
          cout << "[Normal Mode]" << endl;
          
          while(input->peek() != EOF){
            cout << char(input->get());
          }
          cout << endl;
        }
      return 0;
      break;
    case 3:
      // If argc = 3, using both an option and a file
      option = argv[1];
      filename = argv[2];
      input = new ifstream(filename);

      if( input->fail()){
        cout << "Could not find file: " << filename << endl;
        return 1;
      }

      if (option.compare("--help") == 0) {
        // print Usage message
        usage();
      } 
      else if (option.compare("--lex") == 0) {

        // HW2: calls lexer and prints corresponding tokens
        Lexer lexer(*input);

        try {
          Token t = lexer.next_token();
          cout << to_string(t) << endl;
          while (t.type() != TokenType::EOS) {
            t = lexer.next_token();
            cout << to_string(t) << endl;
          }
        } catch (MyPLException& ex) {
          cerr << ex.what() << endl;
        }
      }
      else if (option.compare("--parse") == 0) {

        // HW3: calls parser and builds AST
        try {
          SimpleParser parser(lexer);
          parser.parse();
        } catch (MyPLException& ex) {
          cerr << ex.what() << endl;
        }
      } 
      else if (option.compare("--print") == 0) {
        // print first word of input
        cout << "[PRINT MODE]" << endl;
        while( !isspace(input->peek()) && input->peek() != '\r'){
          cout << char(input->get());
        }
        cout << endl;
      } 
      else if (option.compare("--check") == 0) {
        // print first line of input
        cout << "[CHECK MODE]" << endl;
        while (input->peek() != '\n'){
          cout << char(input->get());
        }
        cout << endl;
      } 
      else if (option.compare("--ir") == 0) {
        // print first two lines of input
        cout << "[IR MODE]" << endl;
        while (input->peek() != '\n'){
          cout << char(input->get());
        }
        cout << endl;
        // Remove newline
        input->get();
        while (input->peek() != '\n'){
          cout << char(input->get());
        }
        cout << endl;
      } 
      else {
        cout << "Error: Option not found!" << endl;
        usage();
        return 1;
      } 
      return 0;
      break;
    default:
      usage();
      return 1;
  }

  // if no options and no file

  while(getline(cin, line)){
      if (line.empty()){
        break;
      }
      v.push_back(line);
  }

  for (it = v.begin(); it != v.end(); it++){
      cout << *it << '\n';
  }

  return 0;
}