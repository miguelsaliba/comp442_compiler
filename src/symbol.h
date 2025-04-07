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

    Symbol *reference = nullptr; // this is used for things like function calls

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
            base_size = 4;
        }

        size = base_size;
        for (int dim : dimensions) {
            size *= dim;
        }
    }

    // Calculates the offset for that specific index in the indices list
    int get_array_offset_multiplier(int index) const {
        int multiplier = base_size;
        for (int i = index + 1; i < dimensions.size(); ++i) {
            multiplier *= dimensions[i];
        }
        return multiplier;
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