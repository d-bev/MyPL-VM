//----------------------------------------------------------------------
// FILE: code_generator.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: Code Generator using Visitor Pattern
//----------------------------------------------------------------------

#include <iostream> // for debugging
#include "code_generator.h"

using namespace std;

// helper function to replace all occurrences of old string with new
void replace_all(string &s, const string &old_str, const string &new_str)
{
  while (s.find(old_str) != string::npos)
    s.replace(s.find(old_str), old_str.size(), new_str);
}

CodeGenerator::CodeGenerator(VM &vm)
    : vm(vm)
{
}

void CodeGenerator::visit(Program &p)
{
  for (auto &struct_def : p.struct_defs)
    struct_def.accept(*this);
  for (auto &fun_def : p.fun_defs)
    fun_def.accept(*this);
}

void CodeGenerator::visit(FunDef &f)
{
  // - Create a new frame (as curr_frame)
  VMFrameInfo new_frame;
  new_frame.function_name = f.fun_name.lexeme();
  new_frame.arg_count = f.params.size();
  curr_frame = new_frame;

  // - Push a new variable environment (via var_table)
  var_table.push_environment();

  // - Store each argument provided on operand stack (from CALL)
  for (auto &arg : f.params)
  {
    // save var to var_table
    var_table.add(arg.var_name.lexeme());

    // add STORE instruction
    int index = var_table.get(arg.var_name.lexeme());
    VMInstr instr = VMInstr::STORE(index);
    curr_frame.instructions.push_back(instr);
  }

  // - Visit each statement node to generate its code
  for (auto stmt : f.stmts)
  {
    stmt->accept(*this);
  }

  // - Add a return if last statement WASN'T a return
  if (curr_frame.instructions.size() > 0)
  {
    if (curr_frame.instructions[curr_frame.instructions.size() - 1].opcode() != OpCode::RET)
    {
      curr_frame.instructions.push_back(VMInstr::PUSH("null"));
      curr_frame.instructions.push_back(VMInstr::RET());
    }
  }
  else
  { // no instructions, need to initiate the return
    curr_frame.instructions.push_back(VMInstr::PUSH("null"));
    curr_frame.instructions.push_back(VMInstr::RET());
  }

  // - Pop the variable environment
  var_table.pop_environment();

  // - Add the frame to the VM
  vm.add(curr_frame);
}

void CodeGenerator::visit(StructDef &s)
{
  // remember the struct def for later
  struct_defs[s.struct_name.lexeme()] = s;
}

void CodeGenerator::visit(ReturnStmt &s)
{
  s.expr.accept(*this);
  curr_frame.instructions.push_back(VMInstr::RET());
}

void CodeGenerator::visit(WhileStmt &s)
{
  // grab starting index of first instruction (to jump back to
  int start = curr_frame.instructions.size();

  // call the while condition visitor
  s.condition.accept(*this);

  // add a JMPF instruction with a temp operand of -1
  curr_frame.instructions.push_back(VMInstr::JMPF(-1));
  int jmpf = curr_frame.instructions.size() - 1;

  // push an environment in var table
  var_table.push_environment();

  // visit all statements
  for (auto stmt : s.stmts)
    stmt->accept(*this);

  // pop the environment
  var_table.pop_environment();

  // add a JMP to the starting index
  curr_frame.instructions.push_back(VMInstr::JMP(start));

  // add a NOP instr (for jmpf to refer to)
  curr_frame.instructions.push_back(VMInstr::NOP());
  int nop = curr_frame.instructions.size() - 1;

  // update previous JMPF instr to refer to NOP
  curr_frame.instructions.at(jmpf).set_operand(nop);
}

void CodeGenerator::visit(ForStmt &s)
{
  // push an environment first for var decl, and generate the vardecl code
  var_table.push_environment();

  s.var_decl.accept(*this);

  int start = curr_frame.instructions.size();

  s.condition.accept(*this);

  curr_frame.instructions.push_back(VMInstr::JMPF(10000));

  int jmpf_index = curr_frame.instructions.size() - 1;

  // "rest similar as while (within another pushed and popped env)"
  var_table.push_environment();

  for (auto &stmt : s.stmts)
  {
    stmt->accept(*this);
  }

  var_table.pop_environment();

  s.assign_stmt.accept(*this);

  // add the JMP, NOP
  curr_frame.instructions.push_back(VMInstr::JMP(start));
  curr_frame.instructions.push_back(VMInstr::NOP());

  int nop = curr_frame.instructions.size() - 1;

  // update the JMPF
  VMInstr jmpf = VMInstr::JMPF(curr_frame.instructions.size() - 1);
  curr_frame.instructions[jmpf_index] = jmpf; // overwrites the jmpf(10000)

  // pop the environment for vardecl
  var_table.pop_environment();
}

void CodeGenerator::visit(IfStmt &s)
{
  s.if_part.condition.accept(*this);

  curr_frame.instructions.push_back(VMInstr::JMPF(10));
  int jmpf_index = curr_frame.instructions.size() - 1;

  for (auto stmt : s.if_part.stmts)
  {
    stmt->accept(*this);
  }

  curr_frame.instructions.push_back(VMInstr::NOP());
  curr_frame.instructions.push_back(VMInstr::JMP(10));
  int jmp_to_end = curr_frame.instructions.size() - 1;
  curr_frame.instructions.push_back(VMInstr::NOP());

  curr_frame.instructions[jmpf_index] = VMInstr::JMPF(curr_frame.instructions.size() - 1);

  for (auto &elif : s.else_ifs)
  {
    elif.condition.accept(*this);
    curr_frame.instructions.push_back(VMInstr::JMPF(10));
    int loop_index = curr_frame.instructions.size() - 1;

    for (auto &stmt : elif.stmts)
    {
      stmt->accept(*this);
    }

    curr_frame.instructions.push_back(VMInstr::JMP(jmp_to_end));
    curr_frame.instructions.push_back(VMInstr::NOP());
    curr_frame.instructions[loop_index] = VMInstr::JMPF(curr_frame.instructions.size() - 1);
  }

  for (auto stmt : s.else_stmts)
  {
    stmt->accept(*this);
  }
  curr_frame.instructions.push_back(VMInstr::NOP());

  curr_frame.instructions[jmp_to_end] = VMInstr::JMP(curr_frame.instructions.size() - 1);
}

void CodeGenerator::visit(VarDeclStmt &s)
{
  s.expr.accept(*this);
  var_table.add(s.var_def.var_name.lexeme());
  VMInstr instr = VMInstr::STORE(var_table.get(s.var_def.var_name.lexeme()));
  curr_frame.instructions.push_back(instr);
}

void CodeGenerator::visit(AssignStmt &s)
{
  if (s.lvalue.size() > 1)
  {
    int main_oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
    VMInstr instr = VMInstr::LOAD(main_oid);
    curr_frame.instructions.push_back(instr);

    if (s.lvalue.at(0).array_expr.has_value())
    {
      s.lvalue.at(0).array_expr.value().accept(*this);
      curr_frame.instructions.push_back(VMInstr::GETI());
    }

    for (int i = 1; i < s.lvalue.size() - 1; i++)
    {
      string path = s.lvalue.at(i).var_name.lexeme();
      instr = VMInstr::GETF(path);
      curr_frame.instructions.push_back(instr);
      if (s.lvalue.at(i).array_expr.has_value())
      {
        s.lvalue.at(i).array_expr.value().accept(*this);
        curr_frame.instructions.push_back(VMInstr::GETI());
      }
    }

    string path = s.lvalue.at(s.lvalue.size() - 1).var_name.lexeme();

    if (s.lvalue.at(s.lvalue.size() - 1).array_expr.has_value())
    {
      instr = VMInstr::GETF(path);
      curr_frame.instructions.push_back(instr);
      s.lvalue.at(s.lvalue.size() - 1).array_expr.value().accept(*this);
      
      // pushes the val that the path will be set to
      s.expr.accept(*this); 

      curr_frame.instructions.push_back(VMInstr::SETI());
    }
    else
    {
      // pushes the val needed for the path
      s.expr.accept(*this);

      instr = VMInstr::SETF(path);
      curr_frame.instructions.push_back(instr);
    }
  }
  else if (s.lvalue.at(0).array_expr.has_value())
  {
    // get the array
    int oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
    VMInstr instr = VMInstr::LOAD(oid);
    curr_frame.instructions.push_back(instr);

    // push the index that will be modified
    s.lvalue.at(0).array_expr.value().accept(*this);

    // push the rhs
    s.expr.accept(*this);
    curr_frame.instructions.push_back(VMInstr::SETI());
  }
  else // s.lavlue.size() <= 1, and doesn't have an array expr value
  {
    int oid = var_table.get(s.lvalue.at(0).var_name.lexeme());
    s.expr.accept(*this);
    VMInstr instr = VMInstr::STORE(oid);
    curr_frame.instructions.push_back(VMInstr::STORE(oid));
  }
}

void CodeGenerator::visit(CallExpr &e)
{
  string name = e.fun_name.lexeme();

  for (auto arg : e.args)
  {
    arg.accept(*this);
  }

  if (name == "print")
  {
    curr_frame.instructions.push_back(VMInstr::WRITE());
  }
  else if (name == "input")
  {
    curr_frame.instructions.push_back(VMInstr::READ());
  }
  else if (name == "to_string")
  {
    curr_frame.instructions.push_back(VMInstr::TOSTR());
  }
  else if (name == "to_int")
  {
    curr_frame.instructions.push_back(VMInstr::TOINT());
  }
  else if (name == "to_double")
  {
    curr_frame.instructions.push_back(VMInstr::TODBL());
  }
  else if (name == "length")
  {
    curr_frame.instructions.push_back(VMInstr::SLEN());
  }
  else if (name == "length@array")
  {
    curr_frame.instructions.push_back(VMInstr::ALEN());
  }
  else if (name == "get")
  {
    curr_frame.instructions.push_back(VMInstr::GETC());
  }
  else if (name == "concat")
  {
    curr_frame.instructions.push_back(VMInstr::CONCAT());
  }
  else
  {
    curr_frame.instructions.push_back(VMInstr::CALL(e.fun_name.lexeme()));
  }
}

void CodeGenerator::visit(Expr &e)
{
  e.first->accept(*this);

  if (e.negated)
  {
    curr_frame.instructions.push_back(VMInstr::NOT());
  }

  if (e.op.has_value())
  {
    e.rest->accept(*this);
    string op = e.op.value().lexeme();

    if (op == "+")
    {
      curr_frame.instructions.push_back(VMInstr::ADD());
    }
    else if (op == "-")
    {
      curr_frame.instructions.push_back(VMInstr::SUB());
    }
    else if (op == "*")
    {
      curr_frame.instructions.push_back(VMInstr::MUL());
    }
    else if (op == "/")
    {
      curr_frame.instructions.push_back(VMInstr::DIV());
    }
    else if (op == "and")
    {
      curr_frame.instructions.push_back(VMInstr::AND());
    }
    else if (op == "or")
    {
      curr_frame.instructions.push_back(VMInstr::OR());
    }
    else if (op == "<")
    {
      curr_frame.instructions.push_back(VMInstr::CMPLT());
    }
    else if (op == "<=")
    {
      curr_frame.instructions.push_back(VMInstr::CMPLE());
    }
    else if (op == ">")
    {
      curr_frame.instructions.push_back(VMInstr::CMPGT());
    }
    else if (op == ">=")
    {
      curr_frame.instructions.push_back(VMInstr::CMPGE());
    }
    else if (op == "==")
    {
      curr_frame.instructions.push_back(VMInstr::CMPEQ());
    }
    else if (op == "!=")
    {
      curr_frame.instructions.push_back(VMInstr::CMPNE());
    }
  }
}

void CodeGenerator::visit(SimpleTerm &t)
{
  t.rvalue->accept(*this);
}

void CodeGenerator::visit(ComplexTerm &t)
{
  t.expr.accept(*this);
}

void CodeGenerator::visit(SimpleRValue &v)
{
  /*
    convert each lexeme to its associated C++ type (e.g., to an int)
    push the value onto the current frame’s operand stack
    MyPL char values just left as C++ string values (i.e., the lexeme)
  */
  if (v.value.type() == TokenType::INT_VAL)
  {
    int val = stoi(v.value.lexeme());
    curr_frame.instructions.push_back(VMInstr::PUSH(val));
  }
  else if (v.value.type() == TokenType::DOUBLE_VAL)
  {
    double val = stod(v.value.lexeme());
    curr_frame.instructions.push_back(VMInstr::PUSH(val));
  }
  else if (v.value.type() == TokenType::NULL_VAL)
  {
    curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
  }
  else if (v.value.type() == TokenType::BOOL_VAL)
  {
    if (v.value.lexeme() == "true")
      curr_frame.instructions.push_back(VMInstr::PUSH(true));
    else
      curr_frame.instructions.push_back(VMInstr::PUSH(false));
  }
  else if (v.value.type() == TokenType::STRING_VAL)
  {
    string s = v.value.lexeme();
    replace_all(s, "\\n", "\n");
    replace_all(s, "\\t", "\t");
    replace_all(s, "\\r", "\r");
    replace_all(s, "\\\\", "\\");

    // could do more here

    curr_frame.instructions.push_back(VMInstr::PUSH(s));
  }
  else if (v.value.type() == TokenType::CHAR_VAL)
  {
    string s = v.value.lexeme();
    replace_all(s, "\\n", "\n");
    replace_all(s, "\\t", "\t");
    replace_all(s, "\\r", "\r");
    replace_all(s, "\\\\", "\\");

    // could do more here

    curr_frame.instructions.push_back(VMInstr::PUSH(s));
  }
}

void CodeGenerator::visit(NewRValue &v)
{
  if (v.array_expr.has_value())
  {
    v.array_expr.value().accept(*this);

    curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));

    curr_frame.instructions.push_back(VMInstr::ALLOCA());
  }
  else if (struct_defs.contains(v.type.lexeme()))
  {
    curr_frame.instructions.push_back(VMInstr::ALLOCS());

    StructDef sd = struct_defs[v.first_token().lexeme()];

    for (auto &field : sd.fields)
    {
      curr_frame.instructions.push_back(VMInstr::DUP());
      curr_frame.instructions.push_back(VMInstr::ADDF(field.var_name.lexeme()));
      curr_frame.instructions.push_back(VMInstr::DUP());
      curr_frame.instructions.push_back(VMInstr::PUSH(nullptr));
      curr_frame.instructions.push_back(VMInstr::SETF(field.var_name.lexeme()));
    }
  }
}

void CodeGenerator::visit(VarRValue &v)
{
  for (int i = 0; i < v.path.size(); i++)
  {
    if (i > 0)
      curr_frame.instructions.push_back(VMInstr::GETF(v.path[i].var_name.lexeme()));
    else
      curr_frame.instructions.push_back(VMInstr::LOAD(var_table.get(v.path[i].var_name.lexeme())));

    // check if array
    if (v.path[i].array_expr.has_value())
    {
      v.path[i].array_expr->accept(*this);
      curr_frame.instructions.push_back(VMInstr::GETI());
    }
  }
}