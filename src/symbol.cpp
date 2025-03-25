#include "symbol.h"


[[nodiscard]] std::string Symbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << std::setw(8) << kind << " │ " << std::setw(38) << name << " │ " << std::setw(17) << type
            << " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}

[[nodiscard]] std::string VarSymbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << std::setw(8) << kind << " │ " << std::setw(29) << name << " │ " << std::setw(16) << type
            << " │ " << std::setw(7) << (is_public ? "public" : "private") << " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}

[[nodiscard]] std::string FuncSymbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << std::setw(8) << kind << " │ " << std::setw(11) << name << " │ " << std::setw(9) << type << " │ ";
    std::string args_str;
    args_str = '(';
    for (int i = 0; i < args.size(); i++) {
        if (i != 0) {
            args_str += ", ";
        }
        args_str += args[i];
    }
    args_str += ')';
    ss << std::setw(22) << args_str;
    ss << " │ " << std::setw(7) << (is_public ? "public" : "private") << " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}
