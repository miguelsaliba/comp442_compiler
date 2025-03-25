#pragma once

#include <iomanip>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "symbol.h"

using std::shared_ptr;

struct Symbol;
struct FuncSymbol;

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

    virtual shared_ptr<Symbol> find_child(const std::string &name) const;

    virtual shared_ptr<FuncSymbol> find_func_child(const std::string &name, const std::vector<std::string> &args);

    [[nodiscard]] virtual std::string to_string() const;
};

// Allow for inheritance
struct ClassSymbolTable : public SymbolTable {
    bool declared = false;
    bool implemented = false;
    std::vector<std::shared_ptr<ClassSymbolTable>> parents;

    ClassSymbolTable(int level, std::string name, SymbolTable* parent) : SymbolTable(level, std::move(name), parent) { }

    shared_ptr<Symbol> lookup(const std::string &name) override;
};