#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"

using namespace std;

// Declare lexer input and parser function
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  // Parse command line arguments.
  // The script/platform requires your compiler to accept the following arguments:
  // compiler <mode> <input file> -o <output file>
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // Open the input file and set it as the input for the lexer.
  yyin = fopen(input, "r");
  assert(yyin);

  // Call the parser function, which will in turn call the lexer to parse the input file.
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  // dump AST
  ast->Dump();
  cout << endl;
}
