#pragma once

#include <set>

#include "visitor.h"

// TODO: add arraysizes to allow for arrays
// TODO: create a hashmap for the offset of the inherited table

class MemSizeVisitor : public Visitor {
    AST* root_node = nullptr;

    void default_visit(AST* node) {
        for (auto child: node->children) {
            visit(child);
        }
    }

    int get_size(const std::string& type) {
        assert(root_node);
        if (type == "int") return 4;
        if (type == "float") return 8;
        if (type == "void") return 0;
        if (const auto class_symbol = root_node->symbol_table->find_child(type, "class")) {
            return class_symbol->subtable->size;
        }
        return 0;
    }

    std::set<std::string> var_types = { "data", "param", "local" };

public:
    MemSizeVisitor() = default;

    void visitProgram(AST* node) override {
        root_node = node;
        default_visit(node);
    }

    void visitClassDef(AST* node) override {
        assert(node->symbol_table);
        assert(std::dynamic_pointer_cast<ClassSymbolTable>(node->symbol_table));
        default_visit(node);
        for (auto symbol : node->symbol_table->symbols) {
            if (symbol) {
                symbol->offset = node->symbol_table->size - symbol->size;
                node->symbol_table->size -= symbol->size;
            }
        }
    }

    void visitFuncBody(AST* node) override {
        assert(node->symbol_table);
        default_visit(node);
        for (auto symbol : node->symbol_table->symbols) {
            if (symbol) {
                symbol->calculate_size();
                symbol->offset = node->symbol_table->size - symbol->size;
                node->symbol_table->size -= symbol->size;
            }
        }
    }

    void visitMultOp(AST* node) override {
        default_visit(node);
        node->symbol->calculate_size();
    }

    void visitAddOp(AST* node) override {
        default_visit(node);
        node->symbol->calculate_size();
    }

    void visitRelop(AST* node) override {
        default_visit(node);
        node->symbol->calculate_size();
    }

    void visitVarDecl(AST* node) override {
        default_visit(node);
        assert(node->symbol);
        node->symbol->calculate_size();
    }

    void visit(ASTIntLit* node) override {
        assert(node->symbol);
        node->symbol->calculate_size();
    }
    void visit(ASTFloatLit* node) override {
        assert(node->symbol);
        node->symbol->calculate_size();
    }
    void visitFParam(AST* node) override {
        if (node->symbol) {
            node->symbol->calculate_size();
        }
    }

    void visitIndices(AST* node) override {
        default_visit(node);
        if (node->symbol) {
            node->symbol->calculate_size();
        }
    }

    void visitIsa(AST* node) override { default_visit(node); }
    void visitImplDef(AST* node) override { default_visit(node); }
    void visitMembers(AST* node) override { default_visit(node); }
    void visitVisibility(AST* node) override { default_visit(node); }
    void visitFuncHead(AST* node) override { default_visit(node); }
    void visitConstructor(AST* node) override { default_visit(node); }
    void visitClassMember(AST* node) override { default_visit(node); }
    void visitImplBody(AST* node) override { default_visit(node); }
    void visitFuncDef(AST* node) override { default_visit(node); }
    void visitFParams(AST* node) override { default_visit(node); }
    void visitType(AST* node) override { default_visit(node); }
    void visitArraySizes(AST* node) override {}
    void visitArraySize(AST* node) override {}
    void visitStatement(AST* node) override { default_visit(node); }
    void visitSign(AST* node) override { default_visit(node); }
    void visitFactor(AST* node) override { default_visit(node); }
    void visitNot(AST* node) override { default_visit(node); }
    void visitStatblock(AST* node) override { default_visit(node); }
    void visitIf(AST* node) override { default_visit(node); }
    void visitStatements(AST* node) override { default_visit(node); }
    void visitSelf(AST* node) override { default_visit(node); }
    void visitAParams(AST* node) override { default_visit(node); }
    void visitFunCall(AST* node) override { default_visit(node); }
    void visitExpr(AST* node) override { default_visit(node); }
    void visitDot(AST* node) override { default_visit(node); }
    void visitWhile(AST* node) override { default_visit(node); }
    void visitAssign(AST* node) override { default_visit(node); }
    void visitVariable(AST* node) override { default_visit(node); }
    void visitIndice(AST* node) override { default_visit(node); }
    void visitDataMember(AST* node) override { default_visit(node); }
    void visitRead(AST* node) override { default_visit(node); }
    void visitWrite(AST* node) override { default_visit(node); }
    void visitReturn(AST* node) override { default_visit(node); }
    void visitVarOrFunCall(AST* node) override { default_visit(node); }
    void visitTerm(AST* node) override { default_visit(node); }
    void visitId(AST* node) override { default_visit(node); }
    void visit(AST* node) override { Visitor::visit(node); }
};