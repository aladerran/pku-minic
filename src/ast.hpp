// ast.hpp
#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "ir.hpp"

// Forward declarations
class CompUnitAST;
class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class NumberAST;

// AST Visitor base class
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void Visit(CompUnitAST *node) = 0;
    virtual void Visit(FuncDefAST *node) = 0;
    virtual void Visit(FuncTypeAST *node) = 0;
    virtual void Visit(BlockAST *node) = 0;
    virtual void Visit(StmtAST *node) = 0;
    virtual void Visit(NumberAST *node) = 0;
};

// Base AST class
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    virtual void Accept(ASTVisitor *visitor) = 0;
};

// CompUnitAST
class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override {
        std::cout << "CompUnitAST { ";
        func_def->Dump();
        std::cout << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// FuncDefAST
class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override {
        std::cout << "FuncDefAST { ";
        func_type->Dump();
        std::cout << ", " << ident << ", ";
        block->Dump();
        std::cout << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// FuncTypeAST
class FuncTypeAST : public BaseAST {
public:
    std::string return_type;

    void Dump() const override {
        std::cout << "FuncTypeAST { " << return_type << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// BlockAST
class BlockAST : public BaseAST {
public:
    std::vector<std::unique_ptr<BaseAST>> stmts;

    void Dump() const override {
        std::cout << "BlockAST { ";
        for (const auto &stmt : stmts) {
            stmt->Dump();
            std::cout << "; ";
        }
        std::cout << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// StmtAST
class StmtAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> number;

    void Dump() const override {
        std::cout << "StmtAST { return ";
        number->Dump();
        std::cout << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// NumberAST
class NumberAST : public BaseAST {
public:
    int value;

    void Dump() const override {
        std::cout << value;
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// CodeGenVisitor
class CodeGenVisitor : public ASTVisitor {
public:
    ProgramIR program;

    void Visit(CompUnitAST *node) override;
    void Visit(FuncDefAST *node) override;
    void Visit(FuncTypeAST *node) override;
    void Visit(BlockAST *node) override;
    void Visit(StmtAST *node) override;
    void Visit(NumberAST *node) override;

private:
    FunctionIR *current_function = nullptr;
    BasicBlockIR *current_block = nullptr;
    int last_value = 0;
};

// Implementations of CodeGenVisitor methods
inline void CodeGenVisitor::Visit(CompUnitAST *node) {
    if (node->func_def) {
        node->func_def->Accept(this);
    }
}

inline void CodeGenVisitor::Visit(FuncDefAST *node) {
    // Visit the function type
    node->func_type->Accept(this);

    auto func_ir = std::make_unique<FunctionIR>(node->ident);
    current_function = func_ir.get();

    auto entry_block = std::make_unique<BasicBlockIR>("entry");
    current_block = entry_block.get();

    if (node->block) {
        node->block->Accept(this);
    }

    current_function->AddBlock(std::move(entry_block));
    program.AddFunction(std::move(func_ir));

    current_function = nullptr;
    current_block = nullptr;
}

inline void CodeGenVisitor::Visit(FuncTypeAST *node) {
    // Currently, no action needed for function type
}

inline void CodeGenVisitor::Visit(BlockAST *node) {
    for (const auto &stmt : node->stmts) {
        stmt->Accept(this);
    }
}

inline void CodeGenVisitor::Visit(StmtAST *node) {
    if (node->number) {
        node->number->Accept(this);
        auto return_instr = std::make_unique<ReturnIR>(last_value);
        current_block->AddInstruction(std::move(return_instr));
    }
}

inline void CodeGenVisitor::Visit(NumberAST *node) {
    last_value = node->value;
}
