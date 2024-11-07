#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>


class InstructionIR {
public:
    virtual ~InstructionIR() = default;
    virtual std::string GenerateIR() const = 0;
};


class ReturnIR : public InstructionIR {
public:
    int value;
    ReturnIR(int val) : value(val) {}

    std::string GenerateIR() const override {
        return "    ret " + std::to_string(value) + "\n";
    }
};


class BasicBlockIR {
public:
    std::string label;
    std::vector<std::unique_ptr<InstructionIR>> instructions;

    BasicBlockIR(const std::string& block_label) : label(block_label) {}

    void AddInstruction(std::unique_ptr<InstructionIR> instr) {
        instructions.push_back(std::move(instr));
    }

    std::string GenerateIR() const {
        std::string ir = "%" + label + ":\n";
        for (const auto& instr : instructions) {
            ir += instr->GenerateIR();
        }
        return ir;
    }
};


class FunctionIR {
public:
    std::string name;
    std::vector<std::unique_ptr<BasicBlockIR>> blocks;

    FunctionIR(const std::string& func_name) : name(func_name) {}

    void AddBlock(std::unique_ptr<BasicBlockIR> block) {
        blocks.push_back(std::move(block));
    }

    std::string GenerateIR() const {
        std::string ir = "fun @" + name + "(): i32 {\n";
        for (const auto& block : blocks) {
            ir += block->GenerateIR();
        }
        ir += "}\n";
        return ir;
    }
};


class GlobalVarIR {};


class ProgramIR {
public:
    std::vector<std::unique_ptr<FunctionIR>> functions;
    std::vector<std::unique_ptr<GlobalVarIR>> global_vars;

    void AddFunction(std::unique_ptr<FunctionIR> func) {
        functions.push_back(std::move(func));
    }

    std::string GenerateIR() const {
        std::string ir;
        for (const auto& func : functions) {
            ir += func->GenerateIR();
        }
        return ir;
    }
};
