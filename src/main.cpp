#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "ast.hpp"
#include "ir.hpp"

// Declare lexer input and parser function
extern FILE *yyin;
extern int yyparse(std::unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
    // Parse command line arguments.
    assert(argc == 5);
    std::string mode = argv[1];
    const char* input = argv[2];
    const char* output = argv[4];

    // Open the input file and set it as the input for the lexer.
    yyin = fopen(input, "r");
    assert(yyin);

    // Call the parser function, which will in turn call the lexer to parse the input file.
    std::unique_ptr<BaseAST> ast;
    int ret = yyparse(ast);
    assert(!ret);

    // Dump AST
    std::cout << "AST Dump: " << std::endl;
    ast->Dump();
    std::cout << std::endl;

    // Create the code generation visitor
    CodeGenVisitor codegenVisitor;

    // Traverse the AST
    ast->Accept(&codegenVisitor);

    // Open the output file to write the results
    std::ofstream output_file(output);
    assert(output_file && "Failed to open output file");

    if (mode == "-koopa") {
        // Output the generated IR
        output_file << codegenVisitor.program.ToString();
    } else if (mode == "-riscv") {
        // Output the generated assembly code
        output_file << codegenVisitor.program.GenerateAssembly();
    } else {
        std::cerr << "Invalid mode: " << mode << std::endl;
        return 1;
    }

    // Close the output file
    output_file.close();

    std::cout << "Result file has been written to: " << output << std::endl;

    return 0;
}
