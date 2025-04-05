#pragma once

#include <string>
#include <memory>
#include <symtable.h>

using std::shared_ptr;

struct SymbolTable;

struct Symbol {
    std::string kind;
    std::string type;
    std::string name;
    shared_ptr<SymbolTable> subtable;
    std::vector<int> dimensions;

    int base_size = 0;
    int size = 0;
    int offset = 0;

    Symbol(std::string kind, std::string type, std::string name, shared_ptr<SymbolTable> subtable = nullptr) : kind(std::move(kind)),
        type(std::move(type)), name(std::move(name)), subtable(std::move(subtable)) {
    };

    void calculate_size() {
        // Doing this to avoid considering a class like "integer" as an int type
        if (type == "int" || type.rfind("int[]", 0) == 0) {
            base_size = 4;
        }
        else if (type == "float" || type.rfind("float[]", 0) == 0) {
            base_size = 8;
        }
        else if (type == "bool") {
            base_size = 1;
        }
        else {
            base_size = 0;
        }
        size = base_size;
        for (int dim : dimensions) {
            size *= dim;
        }
    }

    int get_array_offset(const std::vector<int> &dims) const {
        int off = 0;
        for (int i = 0; i < dims.size(); i++) {
            int temp = dims[i] * base_size;
            for (int j = i+1; j < dimensions.size(); j++) {
                temp *= dimensions[j];
            }
            off += temp;
        }
        return off;
    }

    virtual ~Symbol() = default;

    [[nodiscard]] virtual std::string to_string() const;
};

struct VarSymbol : public Symbol {
    bool is_public = false;

    VarSymbol(std::string kind, std::string type, std::string name, bool is_public, shared_ptr<SymbolTable> subtable = nullptr) :
        Symbol(std::move(kind), std::move(type), std::move(name), std::move(subtable)), is_public(is_public) { }

    [[nodiscard]] std::string to_string() const override;
};


struct FuncSymbol : public Symbol {
    bool is_public = true;
    bool declared = false;
    bool defined = false;
    std::vector<std::string> args;

    FuncSymbol(std::string kind, std::string type, std::string name, std::vector<std::string> args, bool is_public = true, shared_ptr<SymbolTable> subtable = nullptr) :
        Symbol(std::move(kind), std::move(type), std::move(name), std::move(subtable)), is_public(is_public), args(std::move(args)) { }

    [[nodiscard]] std::string to_string() const override;
};