#include <fstream>

#include "lexer.h"
#include "parser.h"
#include "visitor/codegenvisitor.h"
#include "visitor/memsizevisitor.h"
#include "visitor/semvisitor.h"
#include "visitor/symtablevisitor.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Please enter one parameter which is the filename" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }
    std::string extension = filename.substr(filename.length()-4);
    if (extension != ".src") {
        std::cerr << "Please use a file of type '.src'" << std::endl;
    }

    // filename without the .src extension
    std::string outfilename = filename.substr(0, filename.length()-4);
    std::ofstream derivation_file(outfilename + ".outderivation", std::ios::trunc);
    std::ofstream syntax_errors_file(outfilename + ".outsyntaxerrors", std::ios::trunc);
    std::ofstream ast_file(outfilename + ".outast", std::ios::trunc);
    std::ofstream symtable_file(outfilename + ".outsymboltables", std::ios::trunc);
    std::ofstream symtable_errors_file(outfilename + ".outsemerrors", std::ios::trunc);
    std::ofstream codegen_file(outfilename + ".m", std::ios::trunc);
    std::ofstream errors_file(outfilename + ".outerrors", std::ios::trunc);

    Lexer lexer(file);
    // Parser parser(lexer, derivation_file, syntax_errors_file, ast_file);
    // SymTableVisitor symtable_visitor(symtable_errors_file);
    // SemanticVisitor sem_visitor(symtable_errors_file);
    // MemSizeVisitor memsize_visitor;
    // CodeGenVisitor codegen_visitor(codegen_file, errors_file);

    Parser parser(lexer, derivation_file, errors_file, ast_file);
    SymTableVisitor symtable_visitor(errors_file);
    SemanticVisitor sem_visitor(errors_file);
    MemSizeVisitor memsize_visitor;
    CodeGenVisitor codegen_visitor(codegen_file, errors_file);

    AST* root_node = parser.parse();

    root_node->accept(symtable_visitor);
    root_node->accept(sem_visitor);

    symtable_file << root_node->symbol_table->to_string();
    if (parser.has_error) {
        std::cerr << "Error parsing file" << std::endl;
        return 1;
    }
    if (symtable_visitor.has_error) {
        std::cerr << "Error creating symbol table" << std::endl;
        return 1;
    }
    if (sem_visitor.has_error) {
        std::cerr << "Error in semantic analysis" << std::endl;
        return 1;
    }
    root_node->accept(memsize_visitor);
    root_node->accept(codegen_visitor);
    if (codegen_visitor.has_error) {
        std::cerr << "Error in code generation" << std::endl;
    }

    root_node->free();

    file.close();
    derivation_file.close();
    syntax_errors_file.close();
    ast_file.close();
    symtable_file.close();
    symtable_errors_file.close();
    errors_file.close();
    return 0;
}
