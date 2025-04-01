#pragma once
#include <functional>

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

    void visitClassDef(AST* node) override {
        default_visit(node);
        auto class_table = std::dynamic_pointer_cast<ClassSymbolTable, SymbolTable>(node->symbol_table);
        if (!class_table) {
            return;
        }
        for (auto symbol : class_table->symbols) {
            if (symbol->kind == "data") {
                for (auto parent : class_table->parents) {
                    if (auto parent_symbol = parent->find_child(symbol->name, "data")) {
                        error_output << "Line " << node->line_number << ": Warning: Symbol " << symbol->name << " shadows parent symbol " << parent_symbol->name << " in class " << parent->name << std::endl;
                    }
                }
            }
        }
    }

    void visitIsa(AST* node) override {
        if (!node->symbol_table) return;

        std::string classname = node->symbol_table->name;
        auto this_table = std::dynamic_pointer_cast<ClassSymbolTable, SymbolTable>(node->symbol_table);

        for (auto child : node->children) {
            auto class_table = find_class_table(child->str_value);
            if (class_table == nullptr) {
                error_output << "Line " << node->line_number << ": Class " << child->str_value << " not defined" << std::endl;
                continue;
            }
            this_table->parents.push_back(class_table);
        }
        std::unordered_set<std::string> visited;
        if (dfs(this_table, classname, visited)) {
            error_output << "Line " << node->line_number << ": Circular dependency detected in class " << classname << std::endl;
            this_table->parents.clear(); // Removing them to avoid infinite loops
        }
    }
    void visitImplDef(AST* node) override { default_visit(node); }
    void visitMembers(AST* node) override { default_visit(node); }
    void visitVisibility(AST* node) override { default_visit(node); }

    void visitFuncHead(AST* node) override {
        if (!node->symbol) return;

        auto func_symbol = std::dynamic_pointer_cast<FuncSymbol>(node->symbol);
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
    void visitSign(AST* node) override {
        default_visit(node);
        assert(node->children.size() == 1);
        node->data_type = node->children[0]->data_type;
    }
    void visitFactor(AST* node) override { default_visit(node); }
    void visitNot(AST* node) override { default_visit(node); }
    void visitRelop(AST* node) override { default_visit(node); }
    void visitStatblock(AST* node) override { default_visit(node); }
    void visitIf(AST* node) override { default_visit(node); }
    void visitStatements(AST* node) override { default_visit(node); }
    void visitSelf(AST* node) override { default_visit(node); }
    void visitAParams(AST* node) override { default_visit(node); }

    void visitFunCall(AST* node) override {
        assert(node->children[0]);
        assert(node->children[1]->type == ASTType::APARAMS);
        auto first_child = node->children[0];
        default_visit(node); // Set the data type of the parameters
        std::vector<std::string> params;
        for (auto &param: node->children[1]->children) {
            if (param->data_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            params.push_back(param->data_type);
        }

        if (first_child->type == ASTType::ID) {
            auto func = root_table->find_func_child(first_child->str_value, params);
            if (!func) {
                func = std::dynamic_pointer_cast<FuncSymbol, Symbol>(root_table->find_child(first_child->str_value, "function"));
                if (func == nullptr)
                    error_output << "Line " << node->line_number << ": Function " << first_child->str_value << " does not exist" << std::endl;
                else
                    error_output << "Line " << node->line_number << ": Function " << first_child->str_value << " called with incorrect parameters" << std::endl;
                node->data_type = "type_error";
                return;
            }
            node->symbol = func;
        }
        else if (first_child->type == ASTType::DOT) {
            if (first_child->data_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            assert(first_child->children[0]);
            assert(first_child->children[1]);
            auto left_type = first_child->children[0]->data_type;
            auto class_table = find_class_table(left_type);
            assert(class_table != nullptr);
            auto func = class_table->find_func_child(first_child->children[1]->str_value, params);
            if (func == nullptr) {
                error_output << "Line " << node->line_number << ": Method " << first_child->children[1]->str_value << " called with incorrect parameters (" << vector_to_string(params) << ')' << std::endl;
                node->data_type = "type_error";
                return;
            }
            node->symbol = func;
            node->data_type = func->type;
        }
    }

    void visitExpr(AST* node) override {
        default_visit(node);
        std::string child_type = node->children[0]->data_type;
        // assert(!child_type.empty());
        node->data_type = child_type;
    }

    void visitDot(AST* node) override {
        default_visit(node);
        const std::string left_type = node->children[0]->data_type;
        assert(node->parent);
        assert(!left_type.empty());
        if (left_type == "type_error") {
            node->data_type = "type_error";
            return;
        }
        if (left_type == "int" || left_type == "float") {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Cannot use dot operator on type " << left_type << std::endl;
            return;
        }

        // Find the class table of the left side
        auto class_table = find_class_table(left_type);
        if (class_table == nullptr) {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Class " << left_type << " not defined" << std::endl;
            return;
        }

        const std::string right_name = node->children[1]->str_value;
        const auto symbol = class_table->find_child(right_name);
        if (symbol == nullptr) {
            node->data_type = "type_error";
            error_output << "Line " << node->line_number << ": Data member " << right_name << " not defined" << std::endl;
            return;
        }
        node->data_type = symbol->type;
        node->symbol = symbol;
    }

    void visitWhile(AST* node) override { default_visit(node); }
    void visitIndices(AST* node) override {
        default_visit(node);
        for (auto &child: node->children) {
            if (child->data_type == "type_error") {
                node->data_type = "type_error";
                return;
            }
            if (child->data_type != "int") {
                node->data_type = "type_error";
                error_output << "Line " << node->line_number << ": Indices type error: expected int, found " << child->data_type << std::endl;
                return;
            }
        }
    }

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
        default_visit(node);
        if (node->children[0]->type == ASTType::DOT) {
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
        // Check for array access
        for (int i = 0; i < node->children[1]->children.size(); i++) {
            if (node->data_type.back() != ']') {
                node->data_type = "type_error";
                error_output << "Line " << node->line_number << ": Identifier " << node->children[0]->str_value << " incorrect depth" << std::endl;
                return;
            }
            node->data_type.pop_back();
            assert(node->data_type.back() == '[');
            node->data_type.pop_back();
        }
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

    // Depth first search to avoid circular dependencies in inheritance using a set to keep track of visited nodes
    bool dfs(const std::shared_ptr<ClassSymbolTable>& class_table, const std::string &original_name, std::unordered_set<std::string> &visited) {
        if (visited.contains(class_table->name)) {
            return false;
        }
        visited.insert(class_table->name);
        for (const auto& parent : class_table->parents) {
            if (parent->name == original_name) {
                return true;
            }
            if (dfs(parent, original_name, visited)) {
                return true;
            }
        }
        return false;
    }

    // std::vector<std::string> to string function
    static std::string vector_to_string(const std::vector<std::string>& vec) {
        std::string result;
        for (const auto& str : vec) {
            result += str + " ";
        }
        return result;
    }
};
