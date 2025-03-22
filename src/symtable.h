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
    SymbolTable* parent = nullptr;
    std::vector<shared_ptr<Symbol>> symbols;

    SymbolTable(int level, std::string name, SymbolTable* parent = nullptr) : name(std::move(name)), level(level),
                                                                              parent(parent) { }
    virtual ~SymbolTable() = default;

    void add_entry(const shared_ptr<Symbol>& symbol);

    virtual shared_ptr<Symbol> lookup(const std::string &name);

    virtual [[nodiscard]] std::string to_string() const;
};

// Allow for inheritance
struct ClassSymbolTable : public SymbolTable {
    std::vector<std::shared_ptr<SymbolTable>> parents;

    ClassSymbolTable(int level, std::string name, SymbolTable* parent) : SymbolTable(level, name, parent) { }

    shared_ptr<Symbol> lookup(const std::string &name) override;
};