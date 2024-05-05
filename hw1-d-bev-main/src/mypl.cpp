//----------------------------------------------------------------------
// FILE: mypl.cpp
// DATE: Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: OPL HW #1, reading input from a file or from CLI
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

  // TODO: implement hw-1 as per the instructions in hw-1.pdf
  //   In addition:
  //   -- add your information to the file header
  //   -- comment your source code
  //   -- ensure your code is "clean" (see instructions)
  //   -- test your solution, including development of new test cases
  //   -- create hw1-writeup.pdf
  //   -- add, commit, and push your files
  //   -- double check your code has been uploaded to GitHub
  //   -- remove this comment block when you finish :-)  

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

  // Check input type

  switch (argc){
    case 1:
      break;
    case 2:
      // If argc = 2, need to distinguish if argv[1] is an option or a filename
      
      option = argv[1];

      if (option.compare("--help") == 0) {
        // print Usage message
        usage();
      } 
      else if (option.compare("--lex") == 0) {
        // print first char of input
        cout << "[LEX MODE]" << endl;
        cout << char(input->get()) << endl;
      }
      else if (option.compare("--parse") == 0) {
        // print first 2 chars of input
        cout << "[PARSE MODE]" << endl;
        cout << char(input->get()) << char(input->get()) << endl;
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

          cout << "[Normal Mode]" << endl;
          
          // Print whole file
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

      // Alter to use file input
      input = new ifstream(filename);
      if( input->fail()){
        cout << "Could not find file: " << filename << endl;
      }

      if (option.compare("--help") == 0) {
        // print Usage message
        usage();
      } 
      else if (option.compare("--lex") == 0) {
        // print first char of input
        cout << "[LEX MODE]" << endl;
        cout << char(input->get()) << endl;
      }
      else if (option.compare("--parse") == 0) {
        // print first 2 chars of input
        cout << "[PARSE MODE]" << endl;
        cout << char(input->get()) << char(input->get()) << endl;
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

