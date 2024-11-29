// visitor.hpp
#pragma once

#include <string>
#include <unordered_map>
#include "ast.hpp"
#include "ir.hpp"

// CodeGenVisitor
class CodeGenVisitor : public ASTVisitor {
public:
    ProgramIR program;

    void Visit(CompUnitAST *node) override;
    void Visit(FuncDefAST *node) override;
    void Visit(FuncTypeAST *node) override;
    void Visit(BlockAST *node) override;
    void Visit(StmtAST *node) override;
    void Visit(ExprAST *node) override;
    void Visit(BinaryOpAST *node) override;
    void Visit(UnaryExprAST *node) override;
    void Visit(NumberAST *node) override;

    std::string getRegister(const std::string &var);
    std::string getOperand(const std::string &operand);

private:
    FunctionIR *current_function = nullptr;
    BasicBlockIR *current_block = nullptr;
    std::string last_value; // Store the result as a string (variable or constant)
    int temp_count = 0;

    std::unordered_map<std::string, std::string> reg_map; // Mapping from temp vars to registers
    int reg_count = 0;
};

// Implementations of CodeGenVisitor methods

void CodeGenVisitor::Visit(CompUnitAST *node) {
    if (node->func_def) {
        node->func_def->Accept(this);
    }
}

void CodeGenVisitor::Visit(FuncDefAST *node) {
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

void CodeGenVisitor::Visit(FuncTypeAST *node) {
    // Currently, no action needed for function type
}

void CodeGenVisitor::Visit(BlockAST *node) {
    for (const auto &stmt : node->stmts) {
        stmt->Accept(this);
    }
}

void CodeGenVisitor::Visit(StmtAST *node) {
    if (node->expr) {
        node->expr->Accept(this);

        // Determine if last_value is a constant or variable
        if (last_value.empty()) {
            std::cerr << "Error: last_value is empty in StmtAST\n";
            exit(1);
        }

        if (std::isdigit(last_value[0]) || (last_value[0] == '-' && last_value.size() > 1)) {
            // last_value is a constant
            int val = std::stoi(last_value);
            auto return_instr = std::make_unique<ReturnIR>(val);
            current_block->AddInstruction(std::move(return_instr));
        } else {
            // last_value is a variable
            auto return_instr = std::make_unique<ReturnIR>(last_value);
            current_block->AddInstruction(std::move(return_instr));
        }
    }
}

void CodeGenVisitor::Visit(ExprAST *node) {
    // Should not be called directly
}

void CodeGenVisitor::Visit(BinaryOpAST *node) {
    node->lhs->Accept(this);
    std::string lhs_val = last_value;

    node->rhs->Accept(this);
    std::string rhs_val = last_value;

    if (node->op == "and" || node->op == "or") {
        // Allocate temporary registers for boolean conversion
        std::string bool1 = "%" + std::to_string(temp_count++);
        std::string bool2 = "%" + std::to_string(temp_count++);
        std::string result = "%" + std::to_string(temp_count++);

        auto ne_lhs = std::make_unique<BinaryOpIR>("ne", bool1, lhs_val, "0");
        current_block->AddInstruction(std::move(ne_lhs));

        auto ne_rhs = std::make_unique<BinaryOpIR>("ne", bool2, rhs_val, "0");
        current_block->AddInstruction(std::move(ne_rhs));

        auto binary_op_ir = std::make_unique<BinaryOpIR>(node->op, result, bool1, bool2);
        current_block->AddInstruction(std::move(binary_op_ir));

        last_value = result;
    }
    else {
        std::string result = "%" + std::to_string(temp_count++);

        auto binary_op_ir = std::make_unique<BinaryOpIR>(node->op, result, lhs_val, rhs_val);
        current_block->AddInstruction(std::move(binary_op_ir));

        last_value = result;
    }
}

void CodeGenVisitor::Visit(UnaryExprAST *node) {
    node->operand->Accept(this);

    std::string operand = last_value;

    std::string result = "%" + std::to_string(temp_count++);

    if (node->op == "+") {
        // Unary plus, no operation needed
        last_value = operand;
    } else if (node->op == "-") {
        // Generate sub 0, operand
        auto instr = std::make_unique<BinaryOpIR>("sub", result, "0", operand);
        current_block->AddInstruction(std::move(instr));
        last_value = result;
    } else if (node->op == "!") {
        // Generate eq operand, 0
        auto instr = std::make_unique<BinaryOpIR>("eq", result, operand, "0");
        current_block->AddInstruction(std::move(instr));
        last_value = result;
    } else {
        std::cerr << "Unsupported unary operator: " << node->op << std::endl;
        exit(1);
    }
}

void CodeGenVisitor::Visit(NumberAST *node) {
    last_value = std::to_string(node->value);
}

std::string CodeGenVisitor::getRegister(const std::string &var) {
    if (reg_map.find(var) != reg_map.end()) {
        return reg_map[var];
    } else {
        // Assign a new register
        if (reg_count < 7) {
            std::string reg = "t" + std::to_string(reg_count++);
            reg_map[var] = reg;
            return reg;
        }
        else if (reg_count < 15) {
            std::string reg = "a" + std::to_string(reg_count - 7);
            reg_count++;
            reg_map[var] = reg;
            return reg;
        }
        else {
            std::cerr << "Error: Register overflow\n";
            exit(1);
        }
    }
}

std::string CodeGenVisitor::getOperand(const std::string &operand) {
    if (operand == "0") {
        // Map zero to the RISC-V zero register x0
        return "x0";
    } else {
        // Operand is a variable or non-zero constant; return its corresponding register
        return getRegister(operand);
    }
}
