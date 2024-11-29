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

%token PLUS MINUS NOT MUL DIV MOD
%token LT GT LE GE EQ NE AND OR
%token LPAREN RPAREN LBRACE RBRACE SEMI

%type <ast_val> CompUnit FuncDef FuncType Block Stmt Exp LOrExp LAndExp EqExp RelExp AddExp MulExp UnaryExp PrimaryExp Number

%%

CompUnit
  : FuncDef {
      auto comp_unit = std::make_unique<CompUnitAST>();
      comp_unit->func_def = std::move(*$1);
      ast = std::move(comp_unit);
    }
  ;

FuncDef
  : FuncType IDENT LPAREN RPAREN Block {
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
  : LBRACE Stmt RBRACE {
      auto block = std::make_unique<BlockAST>();
      block->stmts.push_back(std::move(*$2));
      $$ = new std::unique_ptr<BaseAST>(std::move(block));
    }
  ;

Stmt
  : RETURN Exp SEMI {
      auto stmt = std::make_unique<StmtAST>();
      stmt->expr = std::move(*$2);
      $$ = new std::unique_ptr<BaseAST>(std::move(stmt));
    }
  ;

Exp
  : LOrExp {
      $$ = $1;
    }
  ;

LOrExp
  : LAndExp {
      $$ = $1;
    }
  | LOrExp OR LAndExp {
      auto binary_op = std::make_unique<BinaryOpAST>("or", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

LAndExp
  : EqExp {
      $$ = $1;
    }
  | LAndExp AND EqExp {
      auto binary_op = std::make_unique<BinaryOpAST>("and", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

EqExp
  : RelExp {
      $$ = $1;
    }
  | EqExp EQ RelExp {
      auto binary_op = std::make_unique<BinaryOpAST>("eq", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | EqExp NE RelExp {
      auto binary_op = std::make_unique<BinaryOpAST>("ne", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

RelExp
  : AddExp {
      $$ = $1;
    }
  | RelExp LT AddExp {
      auto binary_op = std::make_unique<BinaryOpAST>("lt", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | RelExp GT AddExp {
      auto binary_op = std::make_unique<BinaryOpAST>("gt", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | RelExp LE AddExp {
      auto binary_op = std::make_unique<BinaryOpAST>("le", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | RelExp GE AddExp {
      auto binary_op = std::make_unique<BinaryOpAST>("ge", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

AddExp
  : MulExp {
      $$ = $1;
    }
  | AddExp PLUS MulExp {
      auto binary_op = std::make_unique<BinaryOpAST>("add", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | AddExp MINUS MulExp {
      auto binary_op = std::make_unique<BinaryOpAST>("sub", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

MulExp
  : UnaryExp {
      $$ = $1;
    }
  | MulExp MUL UnaryExp {
      auto binary_op = std::make_unique<BinaryOpAST>("mul", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | MulExp DIV UnaryExp {
      auto binary_op = std::make_unique<BinaryOpAST>("div", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  | MulExp MOD UnaryExp {
      auto binary_op = std::make_unique<BinaryOpAST>("mod", std::move(*$1), std::move(*$3));
      $$ = new std::unique_ptr<BaseAST>(std::move(binary_op));
    }
  ;

UnaryExp
  : PrimaryExp {
      $$ = $1;
    }
  | PLUS UnaryExp {
      auto expr = std::make_unique<UnaryExprAST>("+", std::move(*$2));
      $$ = new std::unique_ptr<BaseAST>(std::move(expr));
    }
  | MINUS UnaryExp {
      auto expr = std::make_unique<UnaryExprAST>("-", std::move(*$2));
      $$ = new std::unique_ptr<BaseAST>(std::move(expr));
    }
  | NOT UnaryExp {
      auto expr = std::make_unique<UnaryExprAST>("!", std::move(*$2));
      $$ = new std::unique_ptr<BaseAST>(std::move(expr));
    }
  ;

PrimaryExp
  : LPAREN Exp RPAREN {
      $$ = $2;
    }
  | Number {
      $$ = $1;
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
