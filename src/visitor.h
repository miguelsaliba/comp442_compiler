#pragma once
#include <assert.h>

#include "ast.h"

// This is not exactly how the visitor pattern works, but I wanted to make something similar while keeping the AST class
// the same
class Visitor {
public:
    Visitor() = default;
    virtual ~Visitor() = default;

    virtual void visit(AST* node) {
        switch (node->type) {
            case ASTType::EMPTY:
                std::cout << "EMPTY AST\n";
                break;
            case ASTType::PROGRAM:
                visitProgram(node);
                break;
            case ASTType::CLASSDEF:
                visitClassDef(node);
                break;
            case ASTType::ISA:
                visitIsa(node);
                break;
            case ASTType::IMPLDEF:
                visitImplDef(node);
                break;
            case ASTType::MEMBERS:
                visitMembers(node);
                break;
            case ASTType::VISIBILITY:
                visitVisibility(node);
                break;
            case ASTType::FUNCHEAD:
                visitFuncHead(node);
                break;
            case ASTType::CONSTRUCTOR:
                visitConstructor(node);
                break;
            case ASTType::CLASSMEM:
                visitClassMember(node);
                break;
            case ASTType::IMPLBODY:
                visitImplBody(node);
                break;
            case ASTType::FUNCDEF:
                visitFuncDef(node);
                break;
            case ASTType::FPARAMS:
                visitFParams(node);
                break;
            case ASTType::FPARAM:
                visitFParam(node);
                break;
            case ASTType::TYPE:
                visitType(node);
                break;
            case ASTType::ARRAYSIZES:
                visitArraySizes(node);
                break;
            case ASTType::ARRAYSIZE:
                visitArraySize(node);
                break;
            case ASTType::VARDECL:
                visitVarDecl(node);
                break;
            case ASTType::FUNCBODY:
                visitFuncBody(node);
                break;
            case ASTType::STATEMENT:
                visitStatement(node);
                break;
            case ASTType::SIGN:
                visitSign(node);
                break;
            case ASTType::FACTOR:
                visitFactor(node);
                break;
            case ASTType::NOT:
                visitNot(node);
                break;
            case ASTType::RELOP:
                visitRelop(node);
                break;
            case ASTType::STATBLOCK:
                visitStatblock(node);
                break;
            case ASTType::IF:
                visitIf(node);
                break;
            case ASTType::STATEMENTS:
                visitStatements(node);
                break;
            case ASTType::SELF:
                visitSelf(node);
                break;
            case ASTType::APARAMS:
                visitAParams(node);
                break;
            case ASTType::FUNCALL:
                visitFunCall(node);
                break;
            case ASTType::EXPR:
                visitExpr(node);
                break;
            case ASTType::DOT:
                visitDot(node);
                break;
            case ASTType::WHILE:
                visitWhile(node);
                break;
            case ASTType::INDICES:
                visitIndices(node);
                break;
            case ASTType::ASSIGN:
                visitAssign(node);
                break;
            case ASTType::VARIABLE:
                visitVariable(node);
                break;
            case ASTType::INDICE:
                visitIndice(node);
                break;
            case ASTType::DATAMEMBER:
                visitDataMember(node);
                break;
            case ASTType::READ:
                visitRead(node);
                break;
            case ASTType::WRITE:
                visitWrite(node);
                break;
            case ASTType::RETURN:
                visitReturn(node);
                break;
            case ASTType::VARORFUNCALL:
                visitVarOrFunCall(node);
                break;
            case ASTType::MULTOP:
                visitMultOp(node);
                break;
            case ASTType::ADDOP:
                visitAddOp(node);
                break;
            case ASTType::TERM:
                visitTerm(node);
                break;
            case ASTType::INTLIT: {
                auto* intlit = dynamic_cast<ASTIntLit*>(node);
                if (intlit) {
                    visit(intlit);
                }
                else {
                    std::cerr << "Error: Could not cast to ASTIntLit\n";
                }
                break;
            }
            case ASTType::FLOATLIT: {
                auto* floatlit = dynamic_cast<ASTFloatLit*>(node);
                if (floatlit) {
                    visit(floatlit);
                }
                else {
                    std::cerr << "Error: Could not cast to ASTFloatLit\n";
                }
            }
                break;
            case ASTType::ID:
                visitId(node);
                break;
        }
    };

    virtual void visit(ASTIntLit* node) = 0;
    virtual void visit(ASTFloatLit* node) = 0;
    virtual void visitProgram(AST* node) = 0;
    virtual void visitClassDef(AST* node) = 0;
    virtual void visitIsa(AST* node) = 0;
    virtual void visitImplDef(AST* node) = 0;
    virtual void visitMembers(AST* node) = 0;
    virtual void visitVisibility(AST* node) = 0;
    virtual void visitFuncHead(AST* node) = 0;
    virtual void visitConstructor(AST* node) = 0;
    virtual void visitClassMember(AST* node) = 0;
    virtual void visitImplBody(AST* node) = 0;
    virtual void visitFuncDef(AST* node) = 0;
    virtual void visitFParams(AST* node) = 0;
    virtual void visitFParam(AST* node) = 0;
    virtual void visitType(AST* node) = 0;
    virtual void visitArraySizes(AST* node) = 0;
    virtual void visitArraySize(AST* node) = 0;
    virtual void visitVarDecl(AST* node) = 0;
    virtual void visitFuncBody(AST* node) = 0;
    virtual void visitStatement(AST* node) = 0;
    virtual void visitSign(AST* node) = 0;
    virtual void visitFactor(AST* node) = 0;
    virtual void visitNot(AST* node) = 0;
    virtual void visitRelop(AST* node) = 0;
    virtual void visitStatblock(AST* node) = 0;
    virtual void visitIf(AST* node) = 0;
    virtual void visitStatements(AST* node) = 0;
    virtual void visitSelf(AST* node) = 0;
    virtual void visitAParams(AST* node) = 0;
    virtual void visitFunCall(AST* node) = 0;
    virtual void visitExpr(AST* node) = 0;
    virtual void visitDot(AST* node) = 0;
    virtual void visitWhile(AST* node) = 0;
    virtual void visitIndices(AST* node) = 0;
    virtual void visitAssign(AST* node) = 0;
    virtual void visitVariable(AST* node) = 0;
    virtual void visitIndice(AST* node) = 0;
    virtual void visitDataMember(AST* node) = 0;
    virtual void visitRead(AST* node) = 0;
    virtual void visitWrite(AST* node) = 0;
    virtual void visitReturn(AST* node) = 0;
    virtual void visitVarOrFunCall(AST* node) = 0;
    virtual void visitMultOp(AST* node) = 0;
    virtual void visitAddOp(AST* node) = 0;
    virtual void visitTerm(AST* node) = 0;
    virtual void visitId(AST* node) = 0;
};

// Creates the symbol table for all AST nodes
class SymTableVisitor : public Visitor {
    std::ostream &output;
    std::ostream &error_output;

    void default_visit(AST* node) {
        for (auto child: node->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }
    }

public:
    explicit SymTableVisitor(std::ostream &output, std::ostream &error_output) : output(output), error_output(error_output) {}

    void visitProgram(AST* node) override {
        node->symbol_table = std::make_shared<SymbolTable>(0, "global");

        for (auto child: node->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }

        output << node->symbol_table->to_string();
    }

    void visitClassDef(AST* node) override {
        std::string classname = node->children[0]->str_value;
        if (node->symbol_table->lookup(classname) != nullptr) {
            error_output << "Line " << node->line_number << ": Class " << classname << " already defined" << std::endl;
            return;
        }
        auto local_table = std::make_shared<ClassSymbolTable>(node->symbol_table->level + 1, classname, node->symbol_table.get());
        node->symbol = std::make_shared<Symbol>("class", classname, classname, std::dynamic_pointer_cast<SymbolTable, ClassSymbolTable>(local_table));
        node->symbol_table->add_entry(node->symbol);
        node->symbol_table = local_table; // Use the new table for this class

        // Handle the ISA node
        std::vector<AST*> &bases = node->children[1]->children;

        for (int i = 2; i < node->children.size(); i++) {
            node->children[i]->symbol_table = node->symbol_table;
            visit(node->children[i]);
        }
    }

    void visitIsa(AST* node) override { default_visit(node); }
    void visitImplDef(AST* node) override { default_visit(node); }
    void visitMembers(AST* node) override { default_visit(node); }
    void visitVisibility(AST* node) override { default_visit(node); }

    void visitFuncHead(AST* node) override {
        assert(node->parent && node->parent->parent);
        auto type = node->children[2]->str_value;
        auto name = node->children[0]->str_value;
        const auto params = node->children[1];
        std::vector<std::string> param_types;
        for (const auto param: params->children) {
            std::string param_type = param->children[1]->str_value;
            for (int i = 0; i < param->children[2]->children.size(); i++) {
                param_type += "[]";
            }
            param_types.push_back(param_type);
        }


        // Class members
        if (node->parent->type == ASTType::CLASSMEM) {
            bool visibility = node->firstSibling->str_value == "public";
            auto symbol = std::make_shared<FuncSymbol>("method", type, name, param_types, visibility);
            node->symbol_table->add_entry(symbol);
        }
        else if (node->parent->parent->type == ASTType::IMPLBODY) {

        }
        else {
            // Global functions
            auto symbol_table = std::make_shared<SymbolTable>(node->symbol_table->level + 1, name, node->symbol_table.get());
            auto symbol = std::make_shared<FuncSymbol>("function", type, name, param_types, true, symbol_table);
            node->symbol_table->add_entry(symbol);
            node->parent->symbol_table = symbol_table;
        }
    }

    void visitFuncBody(AST* node) override { default_visit(node); }
    void visitConstructor(AST* node) override { default_visit(node); }
    void visitClassMember(AST* node) override { default_visit(node); }
    void visitImplBody(AST* node) override { default_visit(node); }
    void visitFuncDef(AST* node) override { default_visit(node); }
    void visitFParams(AST* node) override { default_visit(node); }
    void visitFParam(AST* node) override { default_visit(node); }
    void visitType(AST* node) override { default_visit(node); }
    void visitArraySizes(AST* node) override { default_visit(node); }
    void visitArraySize(AST* node) override { default_visit(node); }

    // TODO: non-class member variables
    void visitVarDecl(AST* node) override {
        assert(node->parent && node->parent->parent && node->parent->parent->parent);
        auto type = node->children[1]->str_value;
        auto name = node->children[0]->str_value;

        if (node->parent->type == ASTType::CLASSMEM) { // This checks if it's a class member
            bool is_public = node->firstSibling->str_value == "public";
            auto symbol = std::make_shared<VarSymbol>("data", type, name, is_public);
            node->symbol_table->add_entry(symbol);
            node->symbol = symbol;
        }
        else if (node->parent->parent->parent->type == ASTType::PROGRAM) {
            auto symbol = std::make_shared<Symbol>("local", type, name);
            node->symbol_table->add_entry(symbol);
            node->symbol = symbol;
        }
    }

    void visitStatement(AST* node) override { default_visit(node); }
    void visitSign(AST* node) override { default_visit(node); }
    void visitFactor(AST* node) override { default_visit(node); }
    void visitNot(AST* node) override { default_visit(node); }
    void visitRelop(AST* node) override { default_visit(node); }
    void visitStatblock(AST* node) override { default_visit(node); }
    void visitIf(AST* node) override { default_visit(node); }
    void visitStatements(AST* node) override { default_visit(node); }
    void visitSelf(AST* node) override { default_visit(node); }
    void visitAParams(AST* node) override { default_visit(node); }
    void visitFunCall(AST* node) override { default_visit(node); }
    void visitExpr(AST* node) override { default_visit(node); }
    void visitDot(AST* node) override { default_visit(node); }
    void visitWhile(AST* node) override { default_visit(node); }
    void visitIndices(AST* node) override { default_visit(node); }
    void visitAssign(AST* node) override { default_visit(node); }
    void visitVariable(AST* node) override { default_visit(node); }
    void visitIndice(AST* node) override { default_visit(node); }
    void visitDataMember(AST* node) override { default_visit(node); }
    void visitRead(AST* node) override { default_visit(node); }
    void visitWrite(AST* node) override { default_visit(node); }
    void visitReturn(AST* node) override { default_visit(node); }
    void visitVarOrFunCall(AST* node) override { default_visit(node); }
    void visitMultOp(AST* node) override { default_visit(node); }
    void visitAddOp(AST* node) override { default_visit(node); }
    void visitTerm(AST* node) override { default_visit(node); }
    void visitId(AST* node) override { default_visit(node); }
    void visit(ASTIntLit* node) override { default_visit(node); }
    void visit(ASTFloatLit* node) override { default_visit(node); }

    void visit(AST* node) override { Visitor::visit(node); }
};