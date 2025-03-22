#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>

#include "symtable.h"

class Visitor; // Forward declaration

using std::shared_ptr;

enum class ASTType {
    EMPTY, PROGRAM, CLASSDEF, ISA, IMPLDEF, MEMBERS, VISIBILITY, FUNCHEAD, CONSTRUCTOR, CLASSMEM, IMPLBODY, FUNCDEF,
    FPARAMS, FPARAM, TYPE, ARRAYSIZES, ARRAYSIZE, VARDECL, FUNCBODY, STATEMENT, SIGN, FACTOR, NOT, RELOP, STATBLOCK,
    IF, STATEMENTS, SELF, APARAMS, FUNCALL, EXPR, DOT, WHILE, INDICES, ASSIGN, VARIABLE, INDICE, DATAMEMBER, READ,
    WRITE, RETURN, VARORFUNCALL, MULTOP, ADDOP, TERM,
    INTLIT, FLOATLIT, ID,
};

const std::unordered_map<ASTType, std::string> type_map {
    {ASTType::PROGRAM, "Program"},
    {ASTType::CLASSDEF, "ClassDef"},
    {ASTType::ISA, "Isa"},
    {ASTType::IMPLDEF, "ImplDef"},
    {ASTType::MEMBERS, "Members"},
    {ASTType::VISIBILITY, "Visibility"},
    {ASTType::FUNCHEAD, "FuncHead"},
    {ASTType::CONSTRUCTOR, "Constructor"},
    {ASTType::CLASSMEM, "ClassMember"},
    {ASTType::IMPLBODY, "ImplBody"},
    {ASTType::FUNCDEF, "FuncDef"},
    {ASTType::FPARAMS, "FParams"},
    {ASTType::FPARAM, "FParam"},
    {ASTType::TYPE, "Type"},
    {ASTType::ARRAYSIZES, "ArraySizes"},
    {ASTType::ARRAYSIZE, "ArraySize"},
    {ASTType::VARDECL, "VarDecl"},
    {ASTType::FUNCBODY, "FuncBody"},
    {ASTType::STATEMENT, "Statement"},
    {ASTType::SIGN, "Sign"},
    {ASTType::FACTOR, "Factor"},
    {ASTType::NOT, "Not"},
    {ASTType::RELOP, "Relop"},
    {ASTType::STATBLOCK, "Statblock"},
    {ASTType::IF, "If"},
    {ASTType::STATEMENTS, "Statements"},
    {ASTType::SELF, "Self"},
    {ASTType::APARAMS, "AParams"},
    {ASTType::FUNCALL, "FunCall"},
    {ASTType::EXPR, "Expr"},
    {ASTType::DOT, "Dot"},
    {ASTType::WHILE, "While"},
    {ASTType::INDICES, "Indices"},
    {ASTType::ASSIGN, "Assign"},
    {ASTType::VARIABLE, "Variable"},
    {ASTType::INDICE, "Indice"},
    {ASTType::DATAMEMBER, "DataMember"},
    {ASTType::READ, "Read"},
    {ASTType::WRITE, "Write"},
    {ASTType::RETURN, "Return"},
    {ASTType::VARORFUNCALL, "VarOrFunCall"},
    {ASTType::MULTOP, "MultOp"},
    {ASTType::ADDOP, "AddOp"},
    {ASTType::TERM, "Term"},

    {ASTType::INTLIT, "IntLit"},
    {ASTType::FLOATLIT, "FloatLit"},
    {ASTType::ID, "Id"}
};


struct AST {
    ASTType type = ASTType::EMPTY;
    std::string str_value;
    int line_number = -1;

    AST* parent = nullptr;
    std::vector<AST*> children;
    AST* firstSibling = nullptr;
    AST* next = nullptr;

    shared_ptr<SymbolTable> symbol_table;
    shared_ptr<Symbol> symbol;

    AST() = default;

    explicit AST(int line_number) : line_number(line_number) {};

    /**
     * Creates a new node and adopts all given children
     * @param type type of this node
     * @param line_number line number in the source code
     * @param children (Optional)
     */
    explicit AST(ASTType type, int line_number, std::initializer_list<AST *> children = {}) : type(type), line_number(line_number) {
        for (const auto& child: children) {
            adopt(child);
        }
    };

    void accept(Visitor &v);

    /**
     * Adopt a child node and all its siblings
     * @param child
     */
    AST * adopt(AST *child) {
        child->parent = this;
        if (!children.empty()) {
            children.back()->next = child;
            child->firstSibling = children.front();
        }
        children.emplace_back(child);

        return this;
    }

    void adopt(std::initializer_list<AST *> children) {
        for ( auto child: children) {
            adopt(child);
        }
    }

    /**
     * Add a sibling and all of its siblings to the end of the list
     * @param sibling
     * @return the last sibling
     */
    AST *addSibling(AST *sibling) {
        if (parent != nullptr) {
            parent->adopt(sibling);
            return this;
        }
        std::cerr << "Cannot add sibling to root node" << std::endl;
        return this;
    }

    void recPrint(std::ostream &o, int depth = 0) {
        for (int i = 0; i < depth; i++) {
            o << "| ";
        }
        print(o);
        for (auto child: children) {
            child->recPrint(o, depth + 1);
        }
    }

    virtual void print(std::ostream &o) {
        try {
            o << type_map.at(type);
        } catch (std::out_of_range) {
            std::cerr << "Unknown type: " << static_cast<int>(type) << std::endl;
            return;
        }
        if (!str_value.empty()) {
            o << ": (" << str_value << ')';
        }
        o << std::endl;
    }

    void free() {
        for (auto child: children) {
            child->free();
        }
        delete this;
    }

    virtual ~AST() = default;
};

struct ASTIntLit : AST {
    ASTIntLit(int value, int line) : AST(ASTType::INTLIT, line), value(value) {};
    explicit ASTIntLit(int line) : AST(ASTType::INTLIT, line) {};
    int value = -1;

    void print(std::ostream &o) override {
        o << type_map.at(type) << ": (" << value << ')' << std::endl;
    }
};

struct ASTFloatLit : AST {
    ASTFloatLit(float value, int line) : AST(ASTType::FLOATLIT, line), value(value) {};
    explicit ASTFloatLit(int line) : AST(ASTType::FLOATLIT, line) {};
    float value = -1;

    void print(std::ostream &o) override {
        o << type_map.at(type) << ": (" << value << ')' << std::endl;
    }
};



