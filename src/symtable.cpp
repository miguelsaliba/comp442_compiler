#include "symtable.h"

#include <cassert>
#include <sstream>

void SymbolTable::add_entry(const shared_ptr<Symbol> &symbol) {
    assert(symbol);
    symbols.push_back(symbol);
}

shared_ptr<Symbol> SymbolTable::lookup(const std::string &name) {
    if (auto symbol = find_child(name)) {
        return symbol;
    }
    if (parent != nullptr) {
        return parent->lookup(name);
    }
    return nullptr;
}

shared_ptr<Symbol> SymbolTable::find_child(const std::string &name, const std::string &type) const {
    for (auto &symbol: symbols) {
        if (symbol->name == name) {
            if (type.empty() || symbol->type == type) {
                return symbol;
            }
        }
    }
    return nullptr;
}

shared_ptr<FuncSymbol> SymbolTable::find_func_child(const std::string &name, const std::vector<std::string> &args) {
    for (const auto& symbol: symbols) {
        if (symbol->name == name) {
            auto func_symbol = std::dynamic_pointer_cast<FuncSymbol>(symbol);
            if (func_symbol && func_symbol->args == args) {
                return func_symbol;
            }
        }
    }
    return nullptr;
}


[[nodiscard]] std::string SymbolTable::to_string() const {
    std::stringstream ss;
    std::string prefix;
    for (int i = 0; i < level; i++) {
        prefix += "│      ";
    }
    ss << std::endl << std::left;
    ss << prefix << "┌───────────────────────────────────────────────────────────────────────┐" << std::endl;
    ss << prefix << std::setw(60) << ("│ table: " + name) << std::setw(15) << ("│ size: " + std::to_string(size)) << " │" << std::endl;
    ss << prefix << "├───────────────────────────────────────────────────────────────────────┤" << std::endl;
    for (auto &symbol: symbols) {
        ss << prefix << symbol->to_string() << std::endl;
    }
    ss << prefix << "└───────────────────────────────────────────────────────────────────────┘";

    return ss.str();
}


shared_ptr<Symbol> ClassSymbolTable::lookup(const std::string &name) {
    for (auto symbol: symbols) {
        if (symbol->name == name) {
            return symbol;
        }
    }
    for (const auto& parent: parents) {
        auto symbol = parent->find_child(name);
        if (symbol != nullptr) {
            return symbol;
        }
    }
    if (parent != nullptr) {
        return parent->lookup(name);
    }
    return nullptr;
}

shared_ptr<Symbol> ClassSymbolTable::find_child(const std::string &name, const std::string &kind) const {
    for (auto &symbol: symbols) {
        if (symbol->name == name) {
            if (kind.empty() || symbol->kind == kind) {
                return symbol;
            }
        }
    }
    for (const auto& parent: parents) {
        auto symbol = parent->find_child(name, kind);
        if (symbol != nullptr) {
            return symbol;
        }
    }
    return nullptr;
}

