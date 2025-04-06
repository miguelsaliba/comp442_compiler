#pragma once
#include <stack>

#include "visitor.h"

using std::endl;

class CodeGenVisitor : public Visitor {
    std::ostream &output;
    std::string indent = "\t";
    std::stack<std::string> register_pool;
    int label_num = 0;

    void default_visit(AST *node) {
        for (auto child: node->children) {
            visit(child);
        }
    }

    std::string get_label(const std::string &name) {
        return name + std::to_string(label_num++);
    }

    std::string pop() {
        if (register_pool.empty()) {
            throw std::runtime_error("No registers available");
        }
        std::string reg = register_pool.top();
        register_pool.pop();
        return reg;
    }

public:
    explicit CodeGenVisitor(std::ostream &output) : output(output) {
        for (int i = 12; i >= 1; --i) {
            register_pool.push("r" + std::to_string(i));
        }
    }

    void visitProgram(AST *node) override {
        default_visit(node);
        output << endl;
        output << "% This is used for printing" << endl;
        output << std::left << std::setw(10) << "buf" << "res 20" << endl;
    }

    void visitFuncDef(AST *node) override {
        output << "% Function: " << node->symbol_table->name << endl;
        auto jump_symbol = node->symbol_table->find_child("jump", "jump");
        assert(jump_symbol);
        if (node->symbol_table->name == "main") {
            output << "entry" << endl;
            output << indent << "addi r14,r0,topaddr % Program starts here" << endl;
        }
        output << node->symbol_table->get_unique_name() << endl; // Label
        output << indent << "sw " << jump_symbol->offset << "(r14), r15" << endl;

        default_visit(node);

        if (node->symbol_table->name == "main") {
            output << indent << "hlt" << endl;
        }
        else {
            output << indent << "lw r15," << jump_symbol->offset << "(r14)" << endl;
            output << indent << "jr r15" << endl;
        }
    }

    // TODO: set the variables to the current scope stack + offset then add the current scope offset to the stack pointer
    void visitFunCall(AST *node) override {
        default_visit(node);
        assert(node->symbol);
        assert(node->symbol->subtable);
        std::string reg = pop();
        output << indent << "% Processing: function call to " << node->symbol->name << endl;
        output << indent << "addi r14, r14," << node->symbol_table->size << endl;
        output << indent << "jl r15," << node->symbol->subtable->get_unique_name() << endl;
        output << indent << "subi r14, r14," << node->symbol_table->size << endl;
        register_pool.push(reg);
    }

    void visitAddOp(AST *node) override {
        default_visit(node);
        auto left_node = node->children[0];
        auto right_node = node->children[1];
        assert(node->symbol);
        assert(left_node->symbol);
        assert(right_node->symbol);
        std::string reg1 = pop();
        std::string reg2 = pop();
        output << indent << "% Processing: " << node->symbol->name << " := " << left_node->symbol->name << ' ' << node->
                str_value << ' ' << right_node->symbol->name << endl;
        output << indent << "lw " << reg1 << "," << left_node->symbol->offset << "(r14)" << endl;
        output << indent << "lw " << reg2 << "," << right_node->symbol->offset << "(r14)" << endl;
        if (node->str_value == "+") {
            output << indent << "add " << reg1 << "," << reg1 << "," << reg2 << endl;
        } else if (node->str_value == "-") {
            output << indent << "sub " << reg1 << "," << reg1 << "," << reg2 << endl;
        } else {
            throw std::runtime_error("Unknown operator: " + node->str_value);
        }
        output << indent << "sw " << node->symbol->offset << "(r14)," << reg1 << endl;
        register_pool.push(reg1);
        register_pool.push(reg2);
    }

    void visitMultOp(AST *node) override {
        default_visit(node);
        auto left_node = node->children[0];
        auto right_node = node->children[1];
        assert(node->symbol);
        assert(left_node->symbol);
        assert(right_node->symbol);
        std::string reg1 = pop();
        std::string reg2 = pop();
        output << indent << "% Processing: " << node->symbol->name << " := " << left_node->symbol->name << ' ' << node->
                str_value << ' ' << right_node->symbol->name << endl;
        output << indent << "lw " << reg1 << "," << left_node->symbol->offset << "(r14)" << endl;
        output << indent << "lw " << reg2 << "," << right_node->symbol->offset << "(r14)" << endl;
        if (node->str_value == "*") {
            output << indent << "mul " << reg1 << "," << reg1 << "," << reg2 << endl;
        } else if (node->str_value == "/") {
            output << indent << "div " << reg1 << "," << reg1 << "," << reg2 << endl;
        } else {
            throw std::runtime_error("Unknown operator: " + node->str_value);
        }
        output << indent << "sw " << node->symbol->offset << "(r14)," << reg1 << endl;
        register_pool.push(reg1);
        register_pool.push(reg2);
    }

    void visitRelop(AST *node) override {
        default_visit(node);
        auto left_node = node->children[0];
        auto right_node = node->children[1];
        assert(node->symbol);
        assert(left_node->symbol);
        assert(right_node->symbol);
        std::string reg1 = pop();
        std::string reg2 = pop();
        output << indent << "% Processing: " << node->symbol->name << " := " << left_node->symbol->name << ' ' << node->
                str_value << ' ' << right_node->symbol->name << endl;
        output << indent << "lw " << reg1 << "," << left_node->symbol->offset << "(r14)" << endl;
        output << indent << "lw " << reg2 << "," << right_node->symbol->offset << "(r14)" << endl;
        if (node->str_value == "==") {
            output << indent << "ceq " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else if (node->str_value == "<>") {
            output << indent << "cne " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else if (node->str_value == "<") {
            output << indent << "clt " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else if (node->str_value == ">") {
            output << indent << "cle " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else if (node->str_value == "<=") {
            output << indent << "cgt " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else if (node->str_value == ">=") {
            output << indent << "cge " << reg1 << "," << reg1 << "," << reg2 << endl;
        }
        else {
            throw std::runtime_error("Unknown operator: " + node->str_value);
        }
        output << indent << "sw " << node->symbol->offset << "(r14)," << reg1 << endl;
        register_pool.push(reg1);
        register_pool.push(reg2);
    }


    void visit(ASTIntLit *node) override {
        assert(node->symbol);
        std::string reg = pop();
        output << indent << "% Processing: " << node->symbol->name << " := " << node->value << endl;
        output << indent << "addi " << reg << ",r0," << node->value << endl;
        output << indent << "sw " << node->symbol->offset << "(r14)," << reg << endl;
        register_pool.push(reg);
    }

    void visitAssign(AST *node) override {
        default_visit(node);
        auto left_node = node->children[0];
        auto right_node = node->children[1];
        assert(left_node->symbol);
        assert(right_node->symbol);
        std::string reg = pop();
        output << indent << "% Processing: " << left_node->symbol->name << " := " << right_node->symbol->name << endl;
        output << indent << "lw " << reg << "," << right_node->symbol->offset << "(r14)" << endl;
        output << indent << "sw " << left_node->symbol->offset << "(r14)," << reg << endl;
        register_pool.push(reg);
    }

    void visitWrite(AST *node) override {
        default_visit(node);
        if (node->children[0]->data_type != "int") {
            throw std::runtime_error("Write only supports int type");
        }
        std::string reg1 = pop();
        std::string reg2 = pop();
        output << indent << "% Processing: write " << node->children[0]->symbol->name << endl;
        output << indent << "lw " << reg1 << "," << node->children[0]->symbol->offset << "(r14)" << endl;
        output << indent << "addi r14,r14," << node->symbol_table->size << endl;
        output << indent << "sw -8(r14)," << reg1 << endl;
        output << indent << "addi " << reg1 << ",r0, buf" << endl;
        output << indent << "sw -12(r14)," << reg1 << endl;
        output << indent << "jl r15, intstr" << endl;
        output << indent << "sw -8(r14),r13" << endl;
        output << indent << "jl r15, putstr" << endl;
        output << indent << "subi r14,r14," << node->symbol_table->size << endl;
        register_pool.push(reg1);
    }

    void visitRead(AST *node) override {
        default_visit(node);
        if (node->children[0]->data_type != "int") {
            throw std::runtime_error("Read only supports int type");
        }
        std::string reg = pop();
        output << indent << "% Processing: read " << node->children[0]->symbol->name << endl;
        output << indent << "addi r14,r14," << node->symbol_table->size << endl;
        output << indent << "addi " << reg << ",r0, buf" << endl;
        output << indent << "sw -8(r14)," << reg << endl;
        output << indent << "jl r15, getstr" << endl;
        output << indent << "addi " << reg << ",r0, buf" << endl;
        output << indent << "sw -8(r14)," << reg << endl;
        output << indent << "jl r15, strint" << endl;
        output << indent << "subi r14,r14," << node->symbol_table->size << endl;
        output << indent << "sw " << node->children[0]->symbol->offset << "(r14),r13" << endl;
        register_pool.push(reg);
    }

    void visitIf(AST *node) override {
        assert(node->children.size() == 3);
        auto condition = node->children[0];
        auto then_block = node->children[1];
        auto else_block = node->children[2];
        assert(condition->symbol);

        auto else_label = get_label("else");
        auto end_if_label = get_label("end_if");

        std::string reg = pop();
        output << indent << "% Processing: if statement" << endl;
        visit(condition);
        output << indent << "lw " << reg << "," << condition->symbol->offset << "(r14)" << endl;
        output << indent << "bz " << reg << "," << else_label << endl;
        visit(then_block);
        output << indent << "j " << end_if_label << endl;
        output << else_label << endl;
        visit(else_block);
        output << end_if_label << endl;
        register_pool.push(reg);
    }

    void visitWhile(AST *node) override {
        assert(node->children.size() == 2);
        auto condition = node->children[0];
        auto then_block = node->children[1];
        assert(condition->symbol);

        auto while_label = get_label("while");
        auto end_while_label = get_label("end_while");

        std::string reg = pop();
        output << indent << "% Processing: while loop" << endl;
        output << while_label << endl;
        visit(condition);
        output << indent << "lw " << reg << "," << condition->symbol->offset << "(r14)" << endl;
        output << indent << "bz " << reg << "," << end_while_label << endl;
        visit(then_block);
        output << indent << "j " << while_label << endl;
        output << end_while_label << endl;
        register_pool.push(reg);
    }

    void visitClassDef(AST *node) override { default_visit(node); }
    void visitIsa(AST *node) override { default_visit(node); }
    void visitImplDef(AST *node) override { default_visit(node); }
    void visitMembers(AST *node) override { default_visit(node); }
    void visitVisibility(AST *node) override { default_visit(node); }
    void visitFuncHead(AST *node) override { default_visit(node); }
    void visitConstructor(AST *node) override { default_visit(node); }
    void visitClassMember(AST *node) override { default_visit(node); }
    void visitImplBody(AST *node) override { default_visit(node); }
    void visitFParams(AST *node) override { default_visit(node); }
    void visitFParam(AST *node) override { default_visit(node); }
    void visitType(AST *node) override { default_visit(node); }
    void visitArraySizes(AST *node) override {}
    void visitArraySize(AST *node) override {}
    void visitVarDecl(AST *node) override { default_visit(node); }
    void visitFuncBody(AST *node) override { default_visit(node); }
    void visitStatement(AST *node) override { default_visit(node); }
    void visitSign(AST *node) override { default_visit(node); }
    void visitFactor(AST *node) override { default_visit(node); }
    void visitNot(AST *node) override { default_visit(node); }
    void visitStatblock(AST *node) override { default_visit(node); }
    void visitStatements(AST *node) override { default_visit(node); }
    void visitSelf(AST *node) override { default_visit(node); }
    void visitAParams(AST *node) override { default_visit(node); }
    void visitExpr(AST *node) override { default_visit(node); }
    void visitDot(AST *node) override { default_visit(node); }
    void visitIndices(AST *node) override { default_visit(node); }
    void visitVariable(AST *node) override { default_visit(node); }
    void visitIndice(AST *node) override { default_visit(node); }
    void visitDataMember(AST *node) override { default_visit(node); }
    void visitReturn(AST *node) override { default_visit(node); }
    void visitVarOrFunCall(AST *node) override { default_visit(node); }
    void visitTerm(AST *node) override { default_visit(node); }
    void visitId(AST *node) override { default_visit(node); }
    void visit(ASTFloatLit *node) override { default_visit(node); }
    void visit(AST *node) override { Visitor::visit(node); }
};
