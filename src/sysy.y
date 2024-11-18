%debug

%code requires {
  #include <memory>
  #include <string>
}

%{
#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"

extern int yylineno;

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;
%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  std::unique_ptr<BaseAST> *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> CompUnit FuncDef FuncType Block Stmt Number

%%

CompUnit
  : FuncDef {
    auto comp_unit = std::make_unique<CompUnitAST>();
    comp_unit->func_def = std::move(*($1));
    ast = std::move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto func_def = std::make_unique<FuncDefAST>();
    func_def->func_type = std::move(*$1);
    func_def->ident = *$2;
    func_def->block = std::move(*$5);
    $$ = new std::unique_ptr<BaseAST>(std::move(func_def));
  }
  ;

FuncType
  : INT {
    auto func_type = std::make_unique<FuncTypeAST>();
    func_type->return_type = "int";
    $$ = new std::unique_ptr<BaseAST>(std::move(func_type));
  }
  ;

Block
  : '{' Stmt '}' {
    auto block = std::make_unique<BlockAST>();
    block->stmts.push_back(std::move(*$2));
    $$ = new std::unique_ptr<BaseAST>(std::move(block));
  }
  ;

Stmt
  : RETURN Number ';' {
    auto stmt = std::make_unique<StmtAST>();
    stmt->number = std::move(*$2);
    $$ = new std::unique_ptr<BaseAST>(std::move(stmt));
  }
  ;

Number
  : INT_CONST {
    auto number = std::make_unique<NumberAST>();
    number->value = $1;
    $$ = new std::unique_ptr<BaseAST>(std::move(number));
  }
  ;

%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
    std::cerr << "error: " << s << " at line " << yylineno << std::endl;
}
