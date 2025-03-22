#pragma once

#include <iomanip>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct SymbolTable;
using std::shared_ptr;

struct Symbol {
    std::string kind;
    std::string type;
    std::string name;
    shared_ptr<SymbolTable> subtable;
    std::vector<int> dimensions;

    Symbol(std::string kind, std::string type, std::string name, shared_ptr<SymbolTable> subtable) : kind(std::move(kind)),
        type(std::move(type)), name(std::move(name)), subtable(std::move(subtable)) {
    };

    [[nodiscard]] std::string to_string() const;
};

struct SymbolTable {
    std::string name;
    int level = 0;
    shared_ptr<SymbolTable> parent;
    std::vector<shared_ptr<Symbol>> symbols;

    SymbolTable(int level, std::string name, shared_ptr<SymbolTable> parent = nullptr) : name(std::move(name)), level(level),
                                                                              parent(std::move(parent)) {
    }

    void add_entry(const shared_ptr<Symbol>& symbol);

    shared_ptr<Symbol> lookup(const std::string &name);

    [[nodiscard]] std::string to_string() const;
};
