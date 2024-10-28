#pragma once

#include <iostream>
#include <memory>
#include <vector>



class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
};


class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override {
        std::cout << "CompUnitAST { ";
        func_def->Dump();
        std::cout << " }";
    }
};


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
};


class FuncTypeAST : public BaseAST {
public:
    std::string return_type;

    void Dump() const override {
        std::cout << "FuncTypeAST { " << return_type << " }";
    }
};


class BlockAST : public BaseAST {
public:
    std::vector<std::unique_ptr<BaseAST>> stmts;

    void Dump() const override {
        std::cout << "BlockAST { ";
        for (const auto& stmt : stmts) {
            stmt->Dump();
            std::cout << "; ";
        }
        std::cout << " }";
    }
};


class StmtAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> number;

    void Dump() const override {
        std::cout << "StmtAST { return ";
        number->Dump();
        std::cout << " }";
    }
};


class NumberAST : public BaseAST {
public:
    int value;

    void Dump() const override {
        std::cout << value;
    }
};
