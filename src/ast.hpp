#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "ir.hpp"


class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    virtual void GenerateIR(ProgramIR &program) const {}
    virtual void GenerateIR(FunctionIR &function, BasicBlockIR &block) const {}
    virtual std::unique_ptr<InstructionIR> GenerateIR() const {return nullptr;}
};


class CompUnitAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override {
        std::cout << "CompUnitAST { ";
        func_def->Dump();
        std::cout << " }";
    }

    void GenerateIR(ProgramIR &program) const override {
        if (func_def) {
            func_def->GenerateIR(program);
        }
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

    void GenerateIR(ProgramIR &program) const override {
        auto function = std::make_unique<FunctionIR>(ident);
        auto entry_block = std::make_unique<BasicBlockIR>("entry");

        if (block) {
            block->GenerateIR(*function, *entry_block);
        }

        function->AddBlock(std::move(entry_block));
        program.AddFunction(std::move(function));
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

    void GenerateIR(FunctionIR &function, BasicBlockIR &block) const override {
        for (const auto& stmt : stmts) {
            stmt->GenerateIR(function, block);
        }
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

    void GenerateIR(FunctionIR &function, BasicBlockIR &block) const override {
        if (number) {
            auto return_instr = number->GenerateIR();
            if (return_instr) {
                block.AddInstruction(std::move(return_instr));
            }
        }
    }
};


class NumberAST : public BaseAST {
public:
    int value;

    void Dump() const override {
        std::cout << value;
    }

    std::unique_ptr<InstructionIR> GenerateIR() const override {
        return std::make_unique<ReturnIR>(value);
    }
};
