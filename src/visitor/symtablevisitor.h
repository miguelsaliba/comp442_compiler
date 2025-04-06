#pragma once
#include "visitor.h"


// Creates the symbol table for all AST nodes
class SymTableVisitor : public Visitor {
    std::ostream &error_output;
    SymbolTable *root_table = nullptr;

    void default_visit(AST* node) {
        for (auto child: node->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }
    }

public:
    explicit SymTableVisitor(std::ostream &error_output) : error_output(error_output) {}

    void visitProgram(AST* node) override {
        node->symbol_table = std::make_shared<SymbolTable>(0, "global");
        root_table = node->symbol_table.get();

        for (auto child: node->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }
    }

    void visitClassDef(AST* node) override {
        std::string classname = node->children[0]->str_value;
        auto class_table = find_class_table(classname);
        if (class_table) {
            if (class_table->declared) {
                error_output << "Line " << node->line_number << ": Class " << classname << " already declared" << std::endl;
                return;
            }
        }
        else {
            if (symbol_exists(classname, node->symbol_table.get())) {
                error_output << "Line " << node->line_number << ": Symbol " << classname << " already exists" << std::endl;
                return;
            }
            class_table = std::make_shared<ClassSymbolTable>(node->symbol_table->level + 1, classname, node->symbol_table.get());
            node->symbol = std::make_shared<Symbol>("class", classname, classname, std::dynamic_pointer_cast<SymbolTable, ClassSymbolTable>(class_table));
            node->symbol_table->add_entry(node->symbol);
        }

        class_table->declared = true;
        node->symbol_table = class_table; // Use the new table for this class

        for (int i = 1; i < node->children.size(); i++) {
            node->children[i]->symbol_table = node->symbol_table;
            visit(node->children[i]);
        }
    }

    void visitIsa(AST* node) override { default_visit(node); }

    void visitImplDef(AST* node) override {
        auto classname = node->children[0]->str_value;
        auto class_table = find_class_table(classname);
        if (class_table == nullptr) {
            class_table = std::make_shared<ClassSymbolTable>(node->symbol_table->level + 1, classname, node->symbol_table.get());
            node->symbol = std::make_shared<Symbol>("class", classname, classname, std::dynamic_pointer_cast<SymbolTable, ClassSymbolTable>(class_table));
            node->symbol_table->add_entry(node->symbol);
        }
        else if (class_table->implemented) {
            error_output << "Line " << node->line_number << ": Class " << classname << " already implemented" << std::endl;
            return;
        }
        node->symbol_table = class_table;
        class_table->implemented = true;
        for (auto child: node->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }
    }

    void visitMembers(AST* node) override { default_visit(node); }
    void visitVisibility(AST* node) override { default_visit(node); }

    void visitFuncHead(AST* node) override {
        assert(node->parent && node->parent->parent);
        auto type = node->children[2];
        auto name = node->children[0]->str_value;
        const auto params = node->children[1];
        std::vector<std::string> param_types = get_func_params(params);
        shared_ptr<FuncSymbol> func_symbol = node->symbol_table->find_func_child(name, param_types);

        if (func_symbol) {
            if (node->next && node->next->type == ASTType::FUNCBODY) {
                if (func_symbol->defined) {
                    error_output << "Line " << node->line_number << ": Function " << name << " already defined" << std::endl;
                    return;
                }
            }
            else {
                if (func_symbol->declared) {
                    error_output << "Line " << node->line_number << ": Function " << name << " already declared" << std::endl;
                    return;
                }
                func_symbol->is_public = node->firstSibling->str_value == "public";
                func_symbol->declared = true;
                return; // We don't need to do anything other than mark the function as declared and public/private
            }
        }
        else if (auto symbol = node->symbol_table->find_child(name)) {
            error_output << "Line " << node->line_number << ": Warning: Function " << name << " overloaded" << std::endl;
        }

        // This is a free function
        if (node->symbol_table->name == "global") {
            auto symbol_table = std::make_shared<SymbolTable>(node->symbol_table->level + 1, name, node->symbol_table.get());
            auto symbol = std::make_shared<FuncSymbol>("function", type->str_value, name, param_types, true, symbol_table);
            node->symbol_table->add_entry(symbol);
            node->parent->symbol_table = symbol_table;
            node->symbol_table = symbol_table;
            node->symbol = symbol;
            symbol->declared = true;
            symbol->defined = true;
        }
        // Class members
        else if (node->parent->type == ASTType::CLASSMEM) {
            assert(!func_symbol);
            bool visibility = node->firstSibling->str_value == "public";
            func_symbol = std::make_shared<FuncSymbol>("method", type->str_value, name, param_types, visibility);
            node->symbol_table->add_entry(func_symbol);
            node->symbol = func_symbol;
            func_symbol->declared = true;
            return;
        }
        else if (node->parent->parent->type == ASTType::IMPLBODY) {
            if (!func_symbol) {
                func_symbol = std::make_shared<FuncSymbol>("method", type->str_value, name, param_types);
                node->symbol_table->add_entry(func_symbol);
                node->symbol = func_symbol;
            }
            auto func_table = std::make_shared<SymbolTable>(node->symbol_table->level + 1, name, node->symbol_table.get());
            func_symbol->subtable = func_table;
            node->symbol_table = func_table;
            node->parent->symbol_table = func_table;
            func_symbol->defined = true;
        }

        auto return_symbol = std::make_shared<Symbol>("return", type->str_value, "return");
        auto jump_symbol = std::make_shared<Symbol>("jump", "int", "jump");
        node->symbol_table->add_entry(return_symbol);
        node->symbol_table->add_entry(jump_symbol);

        for (auto child: node->children[1]->children) {
            child->symbol_table = node->symbol_table;
            visit(child);
        }
    }

    void visitFuncBody(AST* node) override { default_visit(node); }
    void visitConstructor(AST* node) override { default_visit(node); }
    void visitClassMember(AST* node) override { default_visit(node); }
    void visitImplBody(AST* node) override { default_visit(node); }
    void visitFuncDef(AST* node) override { default_visit(node); }
    void visitFParams(AST* node) override { default_visit(node); }
    void visitFParam(AST* node) override {
        auto name = node->children[0]->str_value;
        auto type = node->children[1]->str_value;
        for (int i = 0; i < node->children[2]->children.size(); i++) {
            type += "[]";
        }
        auto symbol = std::make_shared<Symbol>("param", type, name);
        node->symbol_table->add_entry(symbol);
        node->symbol = symbol;
    }
    void visitType(AST* node) override { default_visit(node); }
    void visitArraySizes(AST* node) override { default_visit(node); }
    void visitArraySize(AST* node) override { default_visit(node); }

    void visitVarDecl(AST* node) override {
        default_visit(node);
        assert(node->parent && node->parent->parent);
        auto name = node->children[0]->str_value;
        auto type = node->children[1]->str_value;
        std::vector<int> dimensions;
        for (int i = 0; i < node->children[2]->children.size(); i++) {
            type += "[]";
        }

        if (symbol_exists(name, node->symbol_table.get())) {
            error_output << "Line " << node->line_number << ": Variable " << name << " already exists" << std::endl;
            return;
        }

        if (node->parent->type == ASTType::CLASSMEM) { // This checks if it's a class member
            bool is_public = node->firstSibling->str_value == "public";
            auto symbol = std::make_shared<VarSymbol>("data", type, name, is_public);
            node->symbol_table->add_entry(symbol);
            node->symbol = symbol;
            node->parent->symbol = symbol;
        }
        else {
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
