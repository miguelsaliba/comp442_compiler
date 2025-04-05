#include "symbol.h"

using std::setw;

[[nodiscard]] std::string Symbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << setw(8) << kind << " │ " << setw(20) << name << " │ " << setw(13) << type
            << " │ " << setw(8) << size << " │ " << setw(8) << offset << " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}

[[nodiscard]] std::string VarSymbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << setw(8) << kind << " │ " << setw(19) << name << " │ " << setw(10) << type <<
            " │ " << setw(5) << size << " │ " << setw(5) << offset << " │ " << setw(7) << (is_public ? "public" : "private") <<
            " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}

[[nodiscard]] std::string FuncSymbol::to_string() const {
    std::stringstream ss;
    ss << "│ " << std::left << setw(8) << kind << " │ " << setw(11) << name << " │ " << setw(9) << type
            << " │ ";
    std::string args_str;
    args_str = '(';
    for (int i = 0; i < args.size(); i++) {
        if (i != 0) {
            args_str += ", ";
        }
        args_str += args[i];
    }
    args_str += ')';
    ss << setw(22) << args_str;
    ss << " │ " << setw(7) << (is_public ? "public" : "private") << " │";
    if (subtable) {
        ss << subtable->to_string();
    }

    return ss.str();
}
