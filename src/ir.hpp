// ir.hpp
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>

// Base class: IR Node
class IRNode {
public:
    virtual ~IRNode() = default;
    virtual std::string ToString() const = 0;
};

// Instruction IR
class InstructionIR : public IRNode {
public:
    virtual ~InstructionIR() = default;
};

// Return instruction
class ReturnIR : public InstructionIR {
public:
    int value;
    explicit ReturnIR(int val) : value(val) {}

    std::string ToString() const override {
        return "    ret " + std::to_string(value) + "\n";
    }
};

// Basic Block IR
class BasicBlockIR : public IRNode {
public:
    std::string label;
    std::vector<std::unique_ptr<InstructionIR>> instructions;

    explicit BasicBlockIR(const std::string &block_label) : label(block_label) {}

    void AddInstruction(std::unique_ptr<InstructionIR> instr) {
        instructions.push_back(std::move(instr));
    }

    std::string ToString() const override {
        std::string ir = "%" + label + ":\n";
        for (const auto &instr : instructions) {
            ir += instr->ToString();
        }
        return ir;
    }
};

// Function IR
class FunctionIR : public IRNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BasicBlockIR>> blocks;

    explicit FunctionIR(const std::string &func_name) : name(func_name) {}

    void AddBlock(std::unique_ptr<BasicBlockIR> block) {
        blocks.push_back(std::move(block));
    }

    std::string ToString() const override {
        std::string ir = "fun @" + name + "(): i32 {\n";
        for (const auto &block : blocks) {
            ir += block->ToString();
        }
        ir += "}\n";
        return ir;
    }
};

// Program IR
class ProgramIR : public IRNode {
public:
    std::vector<std::unique_ptr<FunctionIR>> functions;

    void AddFunction(std::unique_ptr<FunctionIR> func) {
        functions.push_back(std::move(func));
    }

    std::string ToString() const override {
        std::string ir;
        for (const auto &func : functions) {
            ir += func->ToString();
        }
        return ir;
    }
};
