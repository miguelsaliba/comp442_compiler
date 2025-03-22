#include "symtable.h"
#include <sstream>


[[nodiscard]] std::string Symbol::to_string() const {
    std::stringstream ss;
    ss << "| " << std::left << std::setw(12) << kind << " | " << std::setw(12) << type << " | " << std::setw(19) << name << " | ";
    ss << subtable->to_string();

    return ss.str();
}

void SymbolTable::add_entry(const shared_ptr<Symbol> &symbol) {
    symbols.emplace_back(symbol);
}

shared_ptr<Symbol> SymbolTable::lookup(const std::string &name) {
    for (auto symbol: symbols) {
        if (symbol->name == name) {
            return symbol;
        }
    }
    if (parent != nullptr) {
        return parent->lookup(name);
    }
    return nullptr;
}

[[nodiscard]] std::string SymbolTable::to_string() const {
    std::stringstream ss;
    std::string prefix;
    for (int i = 0; i < level; i++) {
        prefix += "|    ";
    }
    ss << std::endl << std::left;
    ss << prefix << "=====================================================" << std::endl;
    ss << prefix << std::setw(52) << ("| table: " + name) << "|" << std::endl;
    ss << prefix << "=====================================================" << std::endl;
    for (auto &symbol: symbols) {
        ss << prefix << symbol->to_string() << std::endl;
    }
    ss << prefix << "=====================================================";

    return ss.str();
}



shared_ptr<Symbol> ClassSymbolTable::lookup(const std::string &name) {
    for (auto symbol: symbols) {
        if (symbol->name == name) {
            return symbol;
        }
    }
    for (auto parent: parents) {
        auto symbol = parent->lookup(name);
        if (symbol != nullptr) {
            return symbol;
        }
    }
    return nullptr;
}
