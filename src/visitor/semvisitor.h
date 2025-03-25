#pragma once
#include "symtable.h"
#include "visitor.h"

class SemanticVisitor : public Visitor {
    std::ostream &error_output;
    SymbolTable *root_table = nullptr;

    void default_visit(AST* node) {
        for (auto child: node->children) {
            visit(child);
        }
    }

public:
    explicit SemanticVisitor(std::ostream &error_output) : error_output(error_output) {}

    void visitProgram(AST* node) override {
        assert(node->symbol_table);
        root_table = node->symbol_table.get();
        default_visit(node);
    }

    void visitClassDef(AST* node) override { default_visit(node); }
    void visitIsa(AST* node) override {
        for (auto child: node->children) {
            auto class_table = find_class_table(child->str_value);
            if (class_table == nullptr) {
                error_output << "Line " << node->line_number << ": Class " << child->str_value << " not defined" << std::endl;
            }
            // Check for circular dependencies
            for (auto inherit_parents : class_table->parents) {
                if (inherit_parents->name == node->symbol_table->name) {
                    error_output << "Line " << node->line_number << ": Circular dependency between " << node->symbol_table->name << " and " << class_table->name << std::endl;
                }
            }

            auto this_table = std::dynamic_pointer_cast<ClassSymbolTable, SymbolTable>(node->symbol_table);
            this_table->parents.push_back(class_table);
        }
    }
    void visitImplDef(AST* node) override { default_visit(node); }
    void visitMembers(AST* node) override { default_visit(node); }
    void visitVisibility(AST* node) override { default_visit(node); }

    void visitFuncHead(AST* node) override {
        if (!node->symbol) return;

        auto func_symbol = std::dynamic_pointer_cast<FuncSymbol>(node->symbol);
        if (func_symbol->name == "build2") {
            std::cout << "";
        }
        if (!func_symbol->declared) {
            error_output << "Line " << node->line_number << ": Function " << func_symbol->name << " not declared" << std::endl;
        }
        if (!func_symbol->defined) {
            error_output << "Line " << node->line_number << ": Function " << func_symbol->name << " not defined" << std::endl;
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

    void visitVarDecl(AST* node) override {
        auto type = node->children[1]->str_value;
        if (node->children[0]->str_value == "a") {
            std::cout << "";
        }
        if (type != "int" && type != "float") {
            auto class_table = find_class_table(type);
            if (class_table == nullptr) {
                error_output << "Line " << node->line_number << ": Type " << type << " not defined" << std::endl;
                return;
            }
        }
        for (int i = 0; i < node->children[2]->children.size(); i++) {
            type += "[]";
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

    void visitExpr(AST* node) override {
        default_visit(node);
        std::string child_type = node->children[0]->data_type;
        // assert(!child_type.empty());
        node->data_type = child_type;
    }

    void visitDot(AST* node) override {
        default_visit(node);
        std::string left_type = node->children[0]->data_type;
        assert(!left_type.empty());
        if (left_type == "type_error") {
            node->data_type = "type_error";
            return;
        }

        // Find the class table of the left side
        auto class_table = find_class_table(left_type);
        if (class_table == nullptr) {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Class " << left_type << " not defined" << std::endl;
            return;
        }

        std::string right_name = node->children[1]->str_value;
        auto symbol = class_table->find_child(right_name);
        if (symbol == nullptr) {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Data member " << right_name << " not defined" << std::endl;
            return;
        }
        node->data_type = symbol->type;
    }

    void visitWhile(AST* node) override { default_visit(node); }
    void visitIndices(AST* node) override { default_visit(node); }
    void visitAssign(AST* node) override {
        default_visit(node);
        std::string left_type = node->children[0]->data_type;
        std::string right_type = node->children[1]->data_type;
        assert(!left_type.empty());
        // assert(!right_type.empty());
        if (left_type == right_type) {
            node->data_type = left_type;
        }
        else {
            if (left_type == "type_error" || right_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Assign type error: " << left_type << " := " << right_type << std::endl;
        }
    }
    void visitVariable(AST* node) override { default_visit(node); }
    void visitIndice(AST* node) override { default_visit(node); }

    void visitDataMember(AST* node) override {
        assert(node->children[0]);
        if (node->children[0]->type == ASTType::DOT) {
            default_visit(node);
            node->data_type = node->children[0]->data_type;
            return;
        }
        auto symbol = node->symbol_table->lookup(node->children[0]->str_value);
        if (symbol == nullptr) {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Identifier " << node->children[0]->str_value << " not defined" << std::endl;
            return;
        }
        node->data_type = symbol->type;
    }

    void visitRead(AST* node) override { default_visit(node); }
    void visitWrite(AST* node) override { default_visit(node); }
    void visitReturn(AST* node) override { default_visit(node); }
    void visitVarOrFunCall(AST* node) override { default_visit(node); }

    void visitMultOp(AST* node) override {
        default_visit(node);
        std::string left_type = node->children[0]->data_type;
        std::string right_type = node->children[1]->data_type;

        assert(!left_type.empty());
        assert(!right_type.empty());

        if (left_type == right_type) {
            node->data_type = left_type;
        }
        else {
            if (left_type == "type_error" || right_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Multop type error: " << left_type << " * " << right_type << std::endl;
        }
    }

    void visitAddOp(AST* node) override {
        default_visit(node);
        std::string left_type = node->children[0]->data_type;
        std::string right_type = node->children[1]->data_type;

        assert(!left_type.empty());
        assert(!right_type.empty());

        if (left_type == right_type) {
            node->data_type = left_type;
        }
        else {
            if (left_type == "type_error" || right_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": AddOp type error: " << left_type << " + " << right_type << std::endl;
        }
    }
    void visitTerm(AST* node) override { default_visit(node); }
    void visitId(AST* node) override { default_visit(node); }

    void visit(ASTIntLit* node) override {
        node->data_type = "int";
    }
    void visit(ASTFloatLit* node) override {
        node->data_type = "float";
    }

    void visit(AST* node) override { Visitor::visit(node); }

private:
    // Find a class table from the root
    shared_ptr<ClassSymbolTable> find_class_table(const std::string &name) {
        assert(root_table);
        auto symbol = root_table->find_child(name);
        if (symbol == nullptr) {
            return nullptr;
        }
        return std::static_pointer_cast<ClassSymbolTable, SymbolTable>(symbol->subtable);
    }

    // Get function params from it's FParams node vector
    std::vector<std::string> get_func_params(const AST* node) {
        std::vector<std::string> params;
        for (auto param: node->children) {
            std::string param_type = param->children[1]->str_value;
            for (int i = 0; i < param->children[2]->children.size(); i++) {
                param_type += "[]";
            }
            params.push_back(param_type);
        }
        return params;
    }

    bool symbol_exists(const std::string &name, const SymbolTable *table) {
        return table->find_child(name) != nullptr;
    }
};
