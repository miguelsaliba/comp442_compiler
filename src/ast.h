#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>

enum class ASTType {
    EMPTY, PROGRAM, CLASSDEF, ISA, IMPLDEF, MEMBERS, MEMBERDECL, VISIBILITY, FUNCHEAD, CONSTRUCTOR, CLASSMEM, IMPLBODY,
    FUNCDEF, FPARAMS, FPARAM, TYPE, ARRAYSIZES, ARRAYSIZE, VARDECL, FUNCBODY, STATEMENT, SIGN, FACTOR, NOT, RELOP,
    STATBLOCK, IF, STATEMENTS, SELF, APARAMS, FUNCALL, EXPR, DOT, WHILE, INDICES, ASSIGN, VARIABLE, INDICE, DATAMEMBER,
    READ, WRITE, RETURN, VARORFUNCALL, MULTOP, ADDOP, TERM,
    INTLIT, FLOATLIT, ID,
};

const std::unordered_map<ASTType, std::string> type_map {
    {ASTType::PROGRAM, "Program"},
    {ASTType::CLASSDEF, "ClassDef"},
    {ASTType::ISA, "Isa"},
    {ASTType::IMPLDEF, "ImplDef"},
    {ASTType::MEMBERS, "Members"},
    {ASTType::MEMBERDECL, "MemberDecl"},
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
    AST* firstChild = nullptr;
    AST* firstSibling = nullptr;
    AST* next = nullptr;

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

    /**
     * Adopt a child node and all its siblings
     * @param child
     */
    AST * adopt(AST *child) {
        child->parent = this;
        if (firstChild) {
            AST *sibling = firstChild;
            while (sibling->next) {
                sibling = sibling->next;
            }
            sibling->next = child;
            child->firstSibling = firstChild;
        } else {
            firstChild = child;
        }

        return child;
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
        AST *left = this;
        while (left->next) {
            left = left->next;
        }
        AST *right = sibling;
        if (sibling->firstSibling) {
            right = sibling->firstSibling;
        }
        left->next = right;

        right->firstSibling = left->firstSibling;
        right->parent = left->parent;

        while (right->next) {
            right = right->next;
            right->firstSibling = left->firstSibling;
            right->parent = left->parent;
        }

        return right;
    }

    void recPrint(std::ostream &o, int depth = 0) {
        for (int i = 0; i < depth; i++) {
            o << "| ";
        }
        print(o);
        if (firstChild) {
            firstChild->recPrint(o, depth + 1);
        }
        if (next) {
            next->recPrint(o, depth);
        }
    }

    virtual void print(std::ostream &o) {
        try {
            o << type_map.at(type);
        } catch (std::out_of_range &e) {
            std::cerr << "Unknown type: " << static_cast<int>(type) << std::endl;
            return;
        }
        if (!str_value.empty()) {
            o << ": (" << str_value << ')';
        }
        o << std::endl;
    }

    void free() {
        if (firstChild) {
            firstChild->free();
        }
        if (next) {
            next->free();
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



