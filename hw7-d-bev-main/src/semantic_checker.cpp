//----------------------------------------------------------------------
// FILE: semantic_checker.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Dominic Bevilacqua
// DESC: HW5, Type-Checking
//----------------------------------------------------------------------

#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <string>
#include <algorithm>
#include <vector>
#include "mypl_exception.h"
#include "semantic_checker.h"

using namespace std;

// hash table of names of the base data types and built-in functions
const unordered_set<string> BASE_TYPES{"int", "double", "char", "string", "bool"};
const unordered_set<string> BUILT_INS{"print", "input", "to_string", "to_int",
                                      "to_double", "length", "get", "concat"};

// helper functions

optional<VarDef> SemanticChecker::get_field(const StructDef &struct_def,
                                            const string &field_name)
{
  for (const VarDef &var_def : struct_def.fields)
    if (var_def.var_name.lexeme() == field_name)
      return var_def;
  return nullopt;
}

void SemanticChecker::error(const string &msg, const Token &token)
{
  string s = msg;
  s += " near line " + to_string(token.line()) + ", ";
  s += "column " + to_string(token.column());
  throw MyPLException::StaticError(s);
}

void SemanticChecker::error(const string &msg)
{
  throw MyPLException::StaticError(msg);
}

// visitor functions

void SemanticChecker::visit(Program& p)
{
  // record each struct def
  for (StructDef& d : p.struct_defs) {
    string name = d.struct_name.lexeme();
    if (struct_defs.contains(name))
      error("multiple definitions of '" + name + "'", d.struct_name);
    struct_defs[name] = d;
  }
  // record each function def (need a main function)
  bool found_main = false;
  for (FunDef& f : p.fun_defs) {
    string name = f.fun_name.lexeme();
    if (BUILT_INS.contains(name))
      error("redefining built-in function '" + name + "'", f.fun_name);
    if (fun_defs.contains(name))
      error("multiple definitions of '" + name + "'", f.fun_name);
    if (name == "main") {
      if (f.return_type.type_name != "void")
        error("main function must have void type", f.fun_name);
      if (f.params.size() != 0)
        error("main function cannot have parameters", f.params[0].var_name);
      found_main = true;
    }
    fun_defs[name] = f;
  }
  if (!found_main)
    error("program missing main function");
  // check each struct
  for (StructDef& d : p.struct_defs)
    d.accept(*this);
  // check each function
  for (FunDef& d : p.fun_defs)
    d.accept(*this);
}

void SemanticChecker::visit(SimpleRValue &v)
{
  if (v.value.type() == TokenType::INT_VAL)
    curr_type = DataType{false, "int"};
  else if (v.value.type() == TokenType::DOUBLE_VAL)
    curr_type = DataType{false, "double"};
  else if (v.value.type() == TokenType::CHAR_VAL)
    curr_type = DataType{false, "char"};
  else if (v.value.type() == TokenType::STRING_VAL)
    curr_type = DataType{false, "string"};
  else if (v.value.type() == TokenType::BOOL_VAL)
    curr_type = DataType{false, "bool"};
  else if (v.value.type() == TokenType::NULL_VAL)
    curr_type = DataType{false, "void"};
}

void SemanticChecker::visit(FunDef &f)
{
  symbol_table.push_environment();
  
  // check if function's return type is valid
  if( !(BASE_TYPES.contains(f.return_type.type_name) || struct_defs.contains(f.return_type.type_name) || f.return_type.type_name == "void"))
    error("FunDef: invalid return type");

  // add the return type to function's symbol table
  symbol_table.add("return", f.return_type);

  // evaluate each function parameter
  for(auto p : f.params)
  {
    // checking for params with the same name
    if(symbol_table.name_exists_in_curr_env(p.var_name.lexeme()))
      error("FunDef: duplicate param name");

    // checking that each param's type is a valid type
    if(BASE_TYPES.contains(p.data_type.type_name) || struct_defs.contains(p.data_type.type_name))
      symbol_table.add(p.var_name.lexeme(), p.data_type);
    else
      error("FunDef: invalid param type");
  }

  // have stmts accept themselves
  for(auto s : f.stmts)
    s->accept(*this);

  symbol_table.pop_environment();
}

void SemanticChecker::visit(StructDef &s)
{
  unordered_set<string> field_names = unordered_set<string>();

  // evaluate each field
  for(auto f : s.fields)
  {
    // check for fields sharing the same name
    if(field_names.contains(f.var_name.lexeme()))
      error("StructDef: duplicate field names");
    else  
      field_names.insert(f.var_name.lexeme());

    if(f.data_type.type_name == "void")
      error("StructDef: fields cannot have type 'void'");

    // ensure each field is of a valid data type
    if( !(BASE_TYPES.contains(f.data_type.type_name)))
      if( !(symbol_table.name_exists(f.data_type.type_name) || struct_defs.contains(f.data_type.type_name)))
        error("StructDef: invalid field type");
  }  
}

void SemanticChecker::visit(ReturnStmt &s)
{
  DataType ret_type = symbol_table.get("return").value();

  s.expr.accept(*this);
  
  if (curr_type.type_name != ret_type.type_name && curr_type.type_name != "void")
    error("Return statement's type of (" + ret_type.type_name + ") does not match function return type (" + curr_type.type_name + ")");

  if(ret_type.type_name == "void" && curr_type.type_name != "void")
    error("Void functions cannot have return statements");

  // checking if data type is valid
  if(!(BASE_TYPES.contains(curr_type.type_name) || struct_defs.contains(curr_type.type_name) || curr_type.type_name == "void"))
    error("ReturnStmt: invalid return type");
}

void SemanticChecker::visit(WhileStmt &s)
{
  symbol_table.push_environment();

  // accept the condition to validate its type
  s.condition.accept(*this);

  if(curr_type.type_name != "bool")
    error("WhileStmt: condition is not bool");

  // accept the loop stmts
  for (auto stmt : s.stmts)
    stmt->accept(*this);

  symbol_table.pop_environment();
}

void SemanticChecker::visit(ForStmt &s)
{
  symbol_table.push_environment();

  s.var_decl.accept(*this);
  DataType type1 = curr_type;

  s.condition.accept(*this);

  if(curr_type.type_name != "bool")
    error("ForStmt: condition must be bool");

  DataType type2 = curr_type;

  s.assign_stmt.accept(*this);

  for (auto stmt : s.stmts)
    stmt->accept(*this);
    
  symbol_table.pop_environment();
}

void SemanticChecker::visit(IfStmt &s)
{
  // if part

  
  s.if_part.condition.accept(*this);

  if (curr_type.type_name != "bool")
    error("if-condition should be bool", s.if_part.condition.first_token());

  if(curr_type.is_array == true)
    error("if-condition cannot be an array", s.if_part.condition.first_token());

  symbol_table.push_environment();
  for (auto stmt : s.if_part.stmts)
    stmt->accept(*this);
  symbol_table.pop_environment();

  // elseif part

  for (auto i : s.else_ifs)
  {
    
    i.condition.accept(*this);

    if (curr_type.type_name != "bool")
      error("else-if condition should be bool", i.condition.first_token());

    symbol_table.push_environment();
    for (auto stmt : i.stmts)
      stmt->accept(*this);

    symbol_table.pop_environment();
  }

  // else part
  if(s.else_stmts.size() != 0){
    symbol_table.push_environment();
  
    for (auto stmt : s.else_stmts)
      stmt->accept(*this);

    symbol_table.pop_environment();
  }
}

void SemanticChecker::visit(VarDeclStmt &s)
{
  // Does a variable with this name already exist?
  if (symbol_table.name_exists_in_curr_env(s.var_def.var_name.lexeme()))
    error("VarDecl: duplicate variable declaration");

  // accepting rhs
  s.expr.accept(*this); 

  DataType lhs_type = s.var_def.data_type;
  
  // compatibility check 
  if(curr_type.type_name != lhs_type.type_name && curr_type.type_name != "void" || (lhs_type.is_array != curr_type.is_array && !(curr_type.type_name == "void")))
    error("VarDecl: compatibility error on RHS");
  
  symbol_table.add(s.var_def.var_name.lexeme(), lhs_type);
}

void SemanticChecker::visit(AssignStmt &s)
{
  bool is_struct = false;
  DataType lhs_type;
  StructDef sd;

  for(auto l : s.lvalue){
    if(is_struct)
    {
      if(get_field(sd, l.var_name.lexeme()).has_value())
      {
        lhs_type = get_field(sd, l.var_name.lexeme()).value().data_type;
        if(!struct_defs.contains(lhs_type.type_name))
          is_struct = false;
        else
          sd = struct_defs[lhs_type.type_name];
      }
      else 
        error("AssignStmt: field does not exist in struct!");

      if(l.array_expr.has_value())
      {
        l.array_expr.value().accept(*this);
        if(curr_type.type_name != "int")
          error("array indeces must be of type int");
        lhs_type.is_array = false;
      }
    } 
    else 
    {
      if(!symbol_table.name_exists(l.var_name.lexeme()))
        error("AssignStmt: Var doesn't exist!", l.var_name);

      lhs_type = symbol_table.get(l.var_name.lexeme()).value();

      if(struct_defs.contains(lhs_type.type_name))
      {
        is_struct = true;
        sd = struct_defs[lhs_type.type_name];
      }

      if(l.array_expr.has_value())
      {
        l.array_expr.value().accept(*this);
        if(curr_type.type_name != "int")
          error("array indeces must be of type int");
        lhs_type.is_array = false;
      }
    }
  }
  s.expr.accept(*this);

  if(((curr_type.type_name != lhs_type.type_name) && curr_type.type_name != "void")
      || (curr_type.is_array != lhs_type.is_array))
   error("AssignStmt: type mismatch with " + curr_type.type_name);

  // if((curr_type.type_name != lhs_type.type_name) || (curr_type.is_array != lhs_type.is_array))
  //   error("AssignStmt: type mismatch with " + curr_type.type_name);
}

void SemanticChecker::visit(CallExpr &e)
{
  string fun_name = e.fun_name.lexeme();

  // if it's any of these built-ins, the return type should already be set
  if (fun_name == "print")
  {
    if (e.args.size() != 1)
      error("CallExpr: print() only takes 1 argument", e.fun_name);

    e.args[0].accept(*this);
    if(!BASE_TYPES.contains(curr_type.type_name) || curr_type.is_array == true)
      error("print() only works on base types; " + curr_type.type_name + " is an invalid type");

    curr_type = DataType{false, "void"};
  }
  else if (fun_name == "input")
  { // 0 params! returns string from input stream?

    if (e.args.size() != 0)
      error("CallExpr: input() takes no arguments");

    // skip accepting?
    curr_type = DataType{false, "string"};
  }
  else if (fun_name == "to_string")
  { // 1 param : int double char vars
    if (e.args.size() != 1)
      error("CallExpr: to_string() takes 1 argument");

    Expr ex = e.args[0];
    ex.accept(*this);
    if (curr_type.type_name != "int" && curr_type.type_name != "double" && curr_type.type_name != "char")
      error("CallExpr: to_string requires type int, double or char");

    curr_type = DataType{false, "string"};
  }
  else if (fun_name == "to_int")
  { // 1 param : double string vars
    if (e.args.size() != 1)
      error("CallExpr: to_int() takes 1 argument");
      
    Expr ex = e.args[0];
    ex.accept(*this);
    if (curr_type.type_name != "string" && curr_type.type_name != "double")
      error("CallExpr: to_int requires type string or double");

    curr_type = DataType{false, "int"};
  }
  else if (fun_name == "to_double")
  { // 1 param : int string vars
    if (e.args.size() != 1)
      error("CallExpr: to_double() takes 1 argument");

    Expr ex = e.args[0];
    ex.accept(*this);
    if (curr_type.type_name != "int" && curr_type.type_name != "string")
      error("CallExpr: to_double requires type int or string");

    curr_type = DataType{false, "double"};
  }
  else if (fun_name == "length")
  { // 1 param : string array <type> vars
    if (e.args.size() != 1)
      error("CallExpr: length() takes 1 argument");

    e.args[0].accept(*this);

    // if array -> return length of array
    // else MUST be string

    if( (!curr_type.is_array) && (curr_type.type_name != "string"))
      error("CallExpr: expecting string in non-array length", e.args[0].first_token());
    else
      curr_type = DataType{false, "int"};
  }
  else if (fun_name == "get")
  { // 2 params : int (index), string
    if (e.args.size() != 2)
      error("CallExpr: get expects two arguments", e.fun_name);

    Expr e1 = e.args[0];
    e1.accept(*this);
    DataType firstArgType = curr_type;
    Expr e2 = e.args[1];
    e2.accept(*this);
    DataType secondArgType = curr_type;

    if (firstArgType.type_name != "int")
      error("CallExpr: first arg should be int for get", e1.first_token());

    if (secondArgType.type_name != "string")
      error("CallExpr: second arg should be string for get", e2.first_token());

    curr_type = DataType{false, "char"};
  }
  else if (fun_name == "concat")
  { // 2 params : string, string
    if (e.args.size() != 2)
      error("CallExpr: concat() takes 2 arguments");
      
    Expr str1 = e.args[0];
    str1.accept(*this);
    DataType firstArgType = curr_type;
    Expr str2 = e.args[1];
    str2.accept(*this);
    DataType secondArgType = curr_type;

    if (firstArgType.type_name != secondArgType.type_name || firstArgType.type_name != "string")
      error("CallExpr: concat needs two strings");
      
    curr_type = DataType{false, "string"};
  }
  else // isn't a built-in function
  {
    if (!fun_defs.contains(fun_name))
    error("CallExpr: function called is not defined", e.fun_name);

    // make sure same number of args as params
    if (e.args.size() != fun_defs[fun_name].params.size())
      error("CallExpr: arg size of call expr and function params do not matchs");

    // check each arg type :
    for (int i = 0; i < e.args.size(); ++i)
    {
      e.args[i].accept(*this);
      
      FunDef& f = fun_defs[fun_name];
      DataType param_type = f.params[i].data_type;

      if ((curr_type.type_name != "void") && (param_type.type_name != curr_type.type_name))
      {
        error("CallExpr: invalid type, expected " + curr_type.type_name + " but found " + param_type.type_name);
      }
    }
    curr_type = fun_defs[fun_name].return_type;
  }  
}

void SemanticChecker::visit(Expr &e)
{
  if(e.first != nullptr)
    e.first->accept(*this);
  else 
    error("Expr: e.first = nullptr ?");

  DataType firstType = curr_type;

  if (e.negated && curr_type.type_name != "bool" && curr_type.type_name != "int")
    error("Expr: for logically negated expressions, expression needs to be bool or int", e.first->first_token());

  DataType restType;

  if (e.rest != nullptr)
  {
    e.rest->accept(*this);
    restType = curr_type;
  }

  string op;

  if (e.op != nullopt)
  {
    op = e.op.value().lexeme();

    if (op == "-" || op == "/" || op == "*")
    {
      if (firstType.type_name != restType.type_name)
        error("Expr: mismatching types for -, /, *", e.first->first_token());

      if (firstType.type_name != "int" && firstType.type_name != "double")
        error("Expr: left hand type should be int or double for -, /, *", e.first->first_token());
        
      if (restType.type_name != "int" && restType.type_name != "double")
        error("Expr: right hand type should be int or double for -, /, *", e.first->first_token());
        
      curr_type = firstType;
    }

    if (op == "%")
    {
      if (firstType.type_name != "int")
        error("Expr: type for left side of %s should be int", e.first->first_token());
        
      if (restType.type_name != "int")
        error("Expr: type for right side of %s should be int", e.first->first_token());
        
      curr_type.type_name = "int";
    }

    if (op == "+")
    {
      if ((firstType.type_name == "int" || firstType.type_name == "double") 
          &&
          (restType.type_name == "int" || restType.type_name == "double"))
      {
        if (firstType.is_array != restType.is_array || firstType.type_name != restType.type_name)
          error("Expr: for a plus statement with ints and doubles, both sides have to be of same type", e.first->first_token());

        curr_type = firstType;
      }
      else if (
          (firstType.type_name == "string" || firstType.type_name == "char")
          &&
          (restType.type_name == "string" || restType.type_name == "char"))
      {
        if (firstType.type_name == "string" && restType.type_name == "string")
          curr_type.type_name = "string";
        else if (firstType.type_name == "char" && restType.type_name == "string")
          curr_type.type_name = "string";
        else if (firstType.type_name == "string" && restType.type_name == "char")
          curr_type.type_name = "string";
        else
          error("Expr: cannot do string concat on two chars", e.first->first_token());
      }
      else
      {
        error("Expr: cannot do addition on the current types", e.first->first_token());
      }
    }

    if (op == "and" || op == "or")
    {
      if (firstType.type_name != "bool" && restType.type_name != "bool")
        error("Expr: for using and/or, both sides need to be bool", e.first->first_token());

      curr_type = DataType{false, "bool"};
    }

    if (op == "<" || op == "<=" || op == ">" || op == ">=")
    {
      if(firstType.type_name != restType.type_name)
        error("Expr: for comparators, both sides need to be the same type", e.first->first_token());

      
      if( firstType.type_name != "int" 
          && firstType.type_name != "double"
          && firstType.type_name != "string"
          && firstType.type_name != "char")
        error("Expr: comparators require both sides to be either int, double, string, or char");

      if( firstType.is_array == true || restType.is_array == true)
        error("Expr: comparators cannot be used between arrays");

      curr_type = DataType{false, "bool"};
    }

    if (op == "!=" || op == "==")
    {
      // Checking if types are compatible. Either term (or both terms) can be null!

      if ((firstType.type_name != restType.type_name) && (firstType.is_array != restType.is_array))
      {
        // checking for null values
        if(restType.type_name != "void" && firstType.type_name != "void")
          error("Expr: incompatible types for == and !=", e.first->first_token());
      }

      curr_type = DataType{false, "bool"};
    }
  }
}

void SemanticChecker::visit(SimpleTerm &t)
{
  t.rvalue->accept(*this);
}

void SemanticChecker::visit(ComplexTerm &t)
{
  t.expr.accept(*this);
}

void SemanticChecker::visit(NewRValue &v)
{
  bool is_array = v.array_expr.has_value();
  
  if(is_array && v.type.lexeme() == "void")
    error("NewRValue: arrays can't be of type void");

  if(is_array){
    v.array_expr.value().accept(*this);
    if(curr_type.type_name != "int")
      error("NewRValue: type mismatch");
  }

  curr_type = DataType {is_array, v.type.lexeme()};
}

void SemanticChecker::visit(VarRValue &v)
{
  bool is_struct = false;
  StructDef sd;
  DataType l_type;

  if(v.path.size() <= 0)
    error("VarRValue: path does not exist?");

  
  for(auto l : v.path){
    if(is_struct){
      if(get_field(sd, l.var_name.lexeme()).has_value())
      {
        l_type = get_field(sd, l.var_name.lexeme()).value().data_type;
        if(!struct_defs.contains(l_type.type_name))
          is_struct = false;
        else
          sd = struct_defs[l_type.type_name];
      } else {
        error("VarRVal: struct field does not exist!");
      }
    }
    else {
      if (!symbol_table.name_exists(l.var_name.lexeme()))
        error("VarRValue: variable does not exist", v.path[0].var_name);

      l_type = symbol_table.get(l.var_name.lexeme()).value();

      if(struct_defs.contains(l_type.type_name)){
        is_struct = true;
        sd = struct_defs[l_type.type_name];
      }

      if(l.array_expr.has_value()){
        l.array_expr->accept(*this);
        if(curr_type.type_name != "int")
          error("array indeces must be of type integer");
        
        l_type.is_array = false;
      }
    }
  }
  curr_type = l_type;
}
