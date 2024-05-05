//----------------------------------------------------------------------
// FILE: hw6.cpp
// DATE: CPSC326, Spring 2023
// AUTH: 
// DESC:
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <memory>
#include "mypl_exception.h"
#include "vm.h"


using namespace std;


void vm_test_1(VM& vm);         // example program


void usage(const string& command, ostream& out)
{
  out << "Usage: " << command << " [option] " << endl;
  out << "Options:" << endl;
  out << "  --ir       print intermediate (code) representation" << endl;
}


int main(int argc, char** argv)
{
  // convert arguments to strings
  string args[argc];
  for (int i = 0; i < argc; ++i)
    args[i] = string(argv[i]);
  
  // check for too many command line args
  if (argc > 2) {
    usage(args[0], cerr);
    return 1;
  }

  bool inter_mode = false;
  
  // check if in lexer or print mode
  if (argc > 1 && args[1] == "--ir")
    inter_mode = true;
  else if (argc > 1) {
    usage(args[0], cerr);
    return 1;
  }
    
  if (inter_mode) {
    try {
      VM vm;
      vm_test_1(vm);
      cout << to_string(vm) << endl;
    } catch (MyPLException& ex) {
      cerr << ex.what() << endl;            
    }
  }
  else {
    try {
      VM vm;
      vm_test_1(vm);
      vm.run();
    } catch (MyPLException& ex) {
      cerr << ex.what() << endl;            
    }
  }

}

void vm_test_1(VM& vm)
{
  //----------------------------------------------------------------------
  // TODO: Implement the following MyPL program as VM instructions.
  //----------------------------------------------------------------------

  //  bool is_prime(int n) {
  //    var m = n / 2
  //    var v = 2
  //    while (v <= m) {
  //      var r = n / v
  //      var p = r * v
  //      if (p == n) {
  //        return false
  //      }
  //      v = v + 1
  //    }
  //    return true
  //  }

  VMFrameInfo is_prime {"is_prime", 1};
 
  /*0*/   is_prime.instructions.push_back(VMInstr::STORE(0));     // n should already be on stack -> store at 0
  /*1*/   is_prime.instructions.push_back(VMInstr::LOAD(0));      // load n
  /*2*/   is_prime.instructions.push_back(VMInstr::PUSH(2));
  /*3*/   is_prime.instructions.push_back(VMInstr::DIV());        // n / 2
  /*4*/   is_prime.instructions.push_back(VMInstr::STORE(1));     // store at 1 (m = n / 2)
  /*5*/   is_prime.instructions.push_back(VMInstr::PUSH(2));
  /*6*/   is_prime.instructions.push_back(VMInstr::STORE(2));     // store at 2 (v = 2)
  /*7*/   is_prime.instructions.push_back(VMInstr::LOAD(2));      // load v
  /*8*/   is_prime.instructions.push_back(VMInstr::LOAD(1));      // load m
  /*9*/   is_prime.instructions.push_back(VMInstr::CMPLE());      // (v <= m)
  /*10*/  is_prime.instructions.push_back(VMInstr::JMPF(  30  ));   // v > m; jump to returning true
  /*11*/  is_prime.instructions.push_back(VMInstr::LOAD(0));      // load n
  /*12*/  is_prime.instructions.push_back(VMInstr::LOAD(2));      // load v
  /*13*/  is_prime.instructions.push_back(VMInstr::DIV());        // n / v
  /*14*/  is_prime.instructions.push_back(VMInstr::DUP());        // duplicate r (n/v) for next calc
  /*15*/  is_prime.instructions.push_back(VMInstr::STORE(3));     // store at 3 (r = n / v)
  /*16*/  is_prime.instructions.push_back(VMInstr::LOAD(2));      // load v (r already on stack)
  /*17*/  is_prime.instructions.push_back(VMInstr::MUL());        // r * v
  /*18*/  is_prime.instructions.push_back(VMInstr::DUP());        // duplicate, one for cmp and one for store
  /*19*/  is_prime.instructions.push_back(VMInstr::STORE(4));     // store at 4 (p = r*v)
  /*20*/  is_prime.instructions.push_back(VMInstr::LOAD(0));      // load n
  /*21*/  is_prime.instructions.push_back(VMInstr::CMPEQ());      // p == n
  /*22*/  is_prime.instructions.push_back(VMInstr::JMPF( 25 ));     // if false, dont return
  /*23*/  is_prime.instructions.push_back(VMInstr::PUSH(false));
  /*24*/  is_prime.instructions.push_back(VMInstr::RET());
  /*25*/  is_prime.instructions.push_back(VMInstr::LOAD(2));      // load v
  /*26*/  is_prime.instructions.push_back(VMInstr::PUSH(1));
  /*27*/  is_prime.instructions.push_back(VMInstr::ADD());        // v + 1
  /*28*/  is_prime.instructions.push_back(VMInstr::STORE(2));     // store at 2 ( v = v + 1 )
  /*29*/  is_prime.instructions.push_back(VMInstr::JMP( 7 ));     // redo while-loop comparison      
  /*30*/  is_prime.instructions.push_back(VMInstr::PUSH(true));
  /*31*/  is_prime.instructions.push_back(VMInstr::RET());

  
  //  void main() {
  //    print("Please enter integer values to sum (prime number to quit)\n")
  //    int sum = 0
  //    print(">> Enter an int: ")
  //    int val = to_int(read())
  //    while (not is_prime(val)) {
  //      sum = sum + val
  //      print(">> Enter an int: ")
  //      val = to_int(read())
  //    }
  //    print(The sum is: ")
  //    print(sum)
  //    print("\nGoodbye!\n")  
  //  }

  VMFrameInfo main {"main", 0};  

  /*0*/  main.instructions.push_back(VMInstr::PUSH("Please enter integer values to sum (prime number to quit)\n"));
  /*1*/  main.instructions.push_back(VMInstr::WRITE());
  /*2*/  main.instructions.push_back(VMInstr::PUSH(0));
  /*3*/  main.instructions.push_back(VMInstr::STORE(0));          // store at 0 (sum = 0)
  /*4*/  main.instructions.push_back(VMInstr::PUSH(">> Enter an int: "));
  /*5*/  main.instructions.push_back(VMInstr::WRITE());
  /*6*/  main.instructions.push_back(VMInstr::READ());            // gets value as a string, pushes it onto stack
  /*7*/  main.instructions.push_back(VMInstr::TOINT());           // convert it to an integer
  /*8*/  main.instructions.push_back(VMInstr::STORE(1));          // store at 1 (val = to_int(read()))
  /*9*/  main.instructions.push_back(VMInstr::LOAD(1));          // load val for call
  /*10*/  main.instructions.push_back(VMInstr::CALL("is_prime")); // returns true if val prime, false otherwise
  /*11*/  main.instructions.push_back(VMInstr::NOT());            // negate the returned bool
  /*12*/  main.instructions.push_back(VMInstr::JMPF( 24 ));         // skip loop and jump to end
  /*13*/  main.instructions.push_back(VMInstr::LOAD(0));    // load sum
  /*14*/  main.instructions.push_back(VMInstr::LOAD(1));    // load val
  /*15*/  main.instructions.push_back(VMInstr::ADD());
  /*16*/  main.instructions.push_back(VMInstr::STORE(0));   // store at 0 (sum = sum + val)

  /*17*/  main.instructions.push_back(VMInstr::PUSH(">> Enter an int: "));
  /*18*/  main.instructions.push_back(VMInstr::WRITE());
  /*19*/  main.instructions.push_back(VMInstr::READ());   // gets value as a string, pushes it onto stack
  /*20*/  main.instructions.push_back(VMInstr::TOINT());  // convert it to an integer
  /*21*/  main.instructions.push_back(VMInstr::STORE(1)); // update at 1 (val = new integer input)
  /*22*/  main.instructions.push_back(VMInstr::LOAD(1));  // push val onto stack for next loop comparison
  /*23*/  main.instructions.push_back(VMInstr::JMP( 9 ));

  /*24*/  main.instructions.push_back(VMInstr::PUSH("\nGoodbye!\n"));
  /*25*/  main.instructions.push_back(VMInstr::LOAD(0));
  /*26*/  main.instructions.push_back(VMInstr::PUSH("The sum is: "));

  /*27*/  main.instructions.push_back(VMInstr::WRITE());
  /*28*/  main.instructions.push_back(VMInstr::WRITE());
  /*29*/  main.instructions.push_back(VMInstr::WRITE());

  // DEBUG: For some reason, the program always runs twice before exiting?

  vm.add(is_prime);
  vm.add(main);
  vm.run();
}

