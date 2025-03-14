#include "lexer.h"
#include "parser.h"
#include <fstream>

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
    std::ofstream derivation_file(filename.substr(0, filename.length()-4) + ".outderivation", std::ios::trunc);
    std::ofstream errors_file(filename.substr(0, filename.length()-4) + ".outsyntaxerrors", std::ios::trunc);
    std::ofstream ast_file(filename.substr(0, filename.length()-4) + ".outast", std::ios::trunc);
    Lexer lexer(file);
//    Token tok = lexer.nextToken();
//    while (tok.type != EOF_TOKEN) {
//        std::cout << tok.type << " " << tok.value << std::endl;
//        tok = lexer.nextToken();
//    }
//    return 0;
    Parser parser(lexer, derivation_file, errors_file, ast_file);
    parser.parse();

    file.close();
    derivation_file.close();
    errors_file.close();
    return 0;
}
