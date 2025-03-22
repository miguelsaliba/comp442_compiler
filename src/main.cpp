#include <fstream>

#include "lexer.h"
#include "parser.h"
#include "visitor.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Please enter one parameter which is the filename" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::string extension = filename.substr(filename.length()-4);
    if (extension != ".src") {
        std::cerr << "Please use a file of type '.src'" << std::endl;
    }

    std::ifstream file(filename);
    // filename without the .src extension
    std::string outfilename = filename.substr(0, filename.length()-4);
    std::ofstream derivation_file(outfilename + ".outderivation", std::ios::trunc);
    std::ofstream errors_file(outfilename + ".outsyntaxerrors", std::ios::trunc);
    std::ofstream ast_file(outfilename + ".outast", std::ios::trunc);
    std::ofstream symtable_file(outfilename + ".symtab", std::ios::trunc);
//    Token tok = lexer.nextToken();
//    while (tok.type != EOF_TOKEN) {
//        std::cout << tok.type << " " << tok.value << std::endl;
//        tok = lexer.nextToken();
//    }
//    return 0;

    Lexer lexer(file);
    Parser parser(lexer, derivation_file, errors_file, ast_file);

    AST* root_node = parser.parse();
    SymTableVisitor symtable_visitor(symtable_file);
    root_node->accept(symtable_visitor);

    file.close();
    derivation_file.close();
    errors_file.close();
    return 0;
}
