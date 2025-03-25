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