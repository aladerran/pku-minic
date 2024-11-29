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
    virtual std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const = 0;
};

// Instruction IR
class InstructionIR : public IRNode {
public:
    virtual ~InstructionIR() = default;
};

// Return instruction
class ReturnIR : public InstructionIR {
public:
    int int_value;
    std::string str_value;
    bool is_constant;

    explicit ReturnIR(int val) : int_value(val), is_constant(true) {}

    explicit ReturnIR(const std::string &val) : str_value(val), is_constant(false) {}

    std::string ToString() const override {
        if (is_constant) {
            return "    ret " + std::to_string(int_value) + "\n";
        } else {
            return "    ret " + str_value + "\n";
        }
    }

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        if (is_constant) {
            asm_code += "    li a0, " + std::to_string(int_value) + "\n";
        } else {
            std::string reg = getRegisterFunc(str_value);
            asm_code += "    mv a0, " + reg + "\n";
        }
        asm_code += "    ret\n";
        return asm_code;
    }
};

// Load immediate instruction
class LoadImmIR : public InstructionIR {
public:
    std::string dest;
    int value;

    LoadImmIR(const std::string &dest, int value)
        : dest(dest), value(value) {}

    std::string ToString() const override {
        return "    " + dest + " = " + std::to_string(value) + "\n";
    }

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        std::string reg = getRegisterFunc(dest);
        asm_code += "    li " + reg + ", " + std::to_string(value) + "\n";
        return asm_code;
    }
};

// Binary operation instruction
class BinaryOpIR : public InstructionIR {
public:
    std::string op;
    std::string dest;
    std::string lhs;
    std::string rhs;

    BinaryOpIR(const std::string &op, const std::string &dest, const std::string &lhs, const std::string &rhs)
        : op(op), dest(dest), lhs(lhs), rhs(rhs) {}

    std::string ToString() const override {
        return "    " + dest + " = " + op + " " + lhs + ", " + rhs + "\n";
    }

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        std::string rd = getRegisterFunc(dest);

        bool lhs_const = (std::isdigit(lhs[0]) && lhs != "0") || (lhs[0] == '-' && lhs.size() > 1);
        bool rhs_const = (std::isdigit(rhs[0]) && rhs != "0") || (rhs[0] == '-' && rhs.size() > 1);

        std::string rs1, rs2;

        if (lhs_const && rhs_const) {
            // Both operands are constants
            rs1 = rd;
            asm_code += "    li " + rs1 + ", " + lhs + "\n";

            std::string temp_reg = getRegisterFunc(rhs);
            rs2 = temp_reg;
            asm_code += "    li " + rs2 + ", " + rhs + "\n";
        }
        else {
            // Handle single constant operand
            if (lhs_const) {
                rs1 = rd;
                asm_code += "    li " + rs1 + ", " + lhs + "\n";
            }
            else {
                rs1 = getRegisterFunc(lhs);
            }

            if (rhs_const) {
                rs2 = rd;
                asm_code += "    li " + rs2 + ", " + rhs + "\n";
            }
            else {
                rs2 = getRegisterFunc(rhs);
            }
        }

        // Generate corresponding operation instruction
        if (op == "add") {
            asm_code += "    add " + rd + ", " + rs1 + ", " + rs2 + "\n";
        }
        else if (op == "sub") {
            asm_code += "    sub " + rd + ", " + rs1 + ", " + rs2 + "\n";
        }
        else if (op == "mul") {
            asm_code += "    mul " + rd + ", " + rs1 + ", " + rs2 + "\n";
        }
        else if (op == "div") {
            asm_code += "    div " + rd + ", " + rs1 + ", " + rs2 + "\n";
        }
        else if (op == "mod") {
            asm_code += "    rem " + rd + ", " + rs1 + ", " + rs2 + "\n";
        }
        else if (op == "eq") {
            asm_code += "    xor " + rd + ", " + rs1 + ", " + rs2 + "\n";
            asm_code += "    seqz " + rd + ", " + rd + "\n";
        }
        else if (op == "ne") {
            asm_code += "    xor " + rd + ", " + rs1 + ", " + rs2 + "\n";
            asm_code += "    snez " + rd + ", " + rd + "\n";
        }
        else if (op == "lt") {
            asm_code += "    sub " + rd + ", " + rs1 + ", " + rs2 + "\n";
            asm_code += "    sltz " + rd + ", " + rd + "\n";
        }
        else if (op == "gt") {
            asm_code += "    slt " + rd + ", " + rs2 + ", " + rs1 + "\n";
        }
        else if (op == "le") {
            // Implement 'le' as !(lhs > rhs)
            std::string temp = getRegisterFunc(dest + "_temp");
            asm_code += "    sub " + temp + ", " + rs2 + ", " + rs1 + "\n";
            asm_code += "    srai " + temp + ", " + temp + ", 31\n";
            asm_code += "    snez " + temp + ", " + temp + "\n";
            asm_code += "    seqz " + rd + ", " + temp + "\n";
        }
        else if (op == "ge") {
            // Implement 'ge' as !(lhs < rhs)
            std::string temp = getRegisterFunc(dest + "_temp");
            asm_code += "    sub " + temp + ", " + rs1 + ", " + rs2 + "\n";
            asm_code += "    srai " + temp + ", " + temp + ", 31\n";
            asm_code += "    snez " + temp + ", " + temp + "\n";
            asm_code += "    seqz " + rd + ", " + temp + "\n"; 
        }
        else if (op == "and") {
            // Implement '&&' as (lhs != 0) && (rhs != 0)
            asm_code += "    snez " + rd + ", " + rs1 + "\n";
            std::string temp = getRegisterFunc(dest + "_temp");
            asm_code += "    snez " + temp + ", " + rs2 + "\n";
            asm_code += "    and " + rd + ", " + rd + ", " + temp + "\n";
        }
        else if (op == "or") {
            // Implement '||' as (lhs != 0) || (rhs != 0)
            asm_code += "    snez " + rd + ", " + rs1 + "\n";
            std::string temp = getRegisterFunc(dest + "_temp");
            asm_code += "    snez " + temp + ", " + rs2 + "\n";
            asm_code += "    or " + rd + ", " + rd + ", " + temp + "\n";
        }
        else {
            std::cerr << "Unsupported binary operator: " << op << std::endl;
            exit(1);
        }

        return asm_code;
    }
};

// Basic block IR
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

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        for (const auto &instr : instructions) {
            asm_code += instr->GenerateAssembly(getRegisterFunc);
        }
        return asm_code;
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

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        asm_code += "    .text\n";
        asm_code += "    .globl " + name + "\n";
        asm_code += name + ":\n";
        for (const auto &block : blocks) {
            asm_code += block->GenerateAssembly(getRegisterFunc);
        }
        return asm_code;
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

    std::string GenerateAssembly(std::function<std::string(const std::string&)> getRegisterFunc) const override {
        std::string asm_code;
        for (const auto &func : functions) {
            asm_code += func->GenerateAssembly(getRegisterFunc);
        }
        return asm_code;
    }
};
