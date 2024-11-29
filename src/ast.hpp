// ast.hpp
#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Forward declarations for AST classes
class CompUnitAST;
class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class ExprAST;
class BinaryOpAST;
class UnaryExprAST;
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
    virtual void Visit(ExprAST *node) = 0;
    virtual void Visit(BinaryOpAST *node) = 0;
    virtual void Visit(UnaryExprAST *node) = 0;
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
    std::unique_ptr<BaseAST> expr;

    void Dump() const override {
        std::cout << "StmtAST { return ";
        expr->Dump();
        std::cout << "; }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// ExprAST
class ExprAST : public BaseAST {
public:
    // Expression base class
};

// BinaryOpAST
class BinaryOpAST : public BaseAST {
public:
    std::string op;
    std::unique_ptr<BaseAST> lhs;
    std::unique_ptr<BaseAST> rhs;

    BinaryOpAST(const std::string &op, std::unique_ptr<BaseAST> lhs, std::unique_ptr<BaseAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void Dump() const override {
        std::cout << "BinaryOpAST { " << op << " ";
        lhs->Dump();
        std::cout << ", ";
        rhs->Dump();
        std::cout << " }";
    }

    void Accept(ASTVisitor *visitor) override {
        visitor->Visit(this);
    }
};

// UnaryExprAST
class UnaryExprAST : public BaseAST {
public:
    std::string op;
    std::unique_ptr<BaseAST> operand;

    UnaryExprAST(const std::string &op, std::unique_ptr<BaseAST> operand)
        : op(op), operand(std::move(operand)) {}

    void Dump() const override {
        std::cout << "UnaryExprAST { " << op << " ";
        operand->Dump();
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
