#pragma once

#include <iomanip>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "symbol.h"

using std::shared_ptr;

struct Symbol;

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

    [[nodiscard]] virtual std::string to_string() const;
};

// Allow for inheritance
struct ClassSymbolTable : public SymbolTable {
    std::vector<std::shared_ptr<SymbolTable>> parents;

    ClassSymbolTable(int level, std::string name, SymbolTable* parent) : SymbolTable(level, std::move(name), parent) { }

    shared_ptr<Symbol> lookup(const std::string &name) override;
};