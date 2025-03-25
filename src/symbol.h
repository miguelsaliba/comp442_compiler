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

    Symbol(std::string kind, std::string type, std::string name, shared_ptr<SymbolTable> subtable = nullptr) : kind(std::move(kind)),
        type(std::move(type)), name(std::move(name)), subtable(std::move(subtable)) {
    };

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