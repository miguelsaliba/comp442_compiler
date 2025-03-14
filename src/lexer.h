#pragma once

#include <map>
#include <string>
#include <iostream>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <utility>

enum class TokenType {
    EOF_TOKEN = 1,
    IDENTIFIER,
    INTLIT,
    FLOATLIT,
    INVALID_NUMBER,
    INVALID_ID,
    INVALID_CHAR,

    // Keywords
    INT, WHILE, FLOAT, LOCAL, IF, VOID, CONSTRUCTOR,
    THEN, CLASS, ELSE, ATTRIBUTE, FUNCTION, SELF, READ,
    PUBLIC, ISA, WRITE, IMPLEMENTATION, RETURN, PRIVATE,

    // Operators
    EQ, NEQ, LT, GT, LTEQ, GTEQ, ADD, SUB, MUL, DIV, ASSIGN, OR, AND, NOT,

    // Punctuation
    COMMA, SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, DOT, COLON, ARROW,

    EPSILON
};

struct Token {
    TokenType type;
    std::string value;
    int line = 0;

    Token(TokenType type, std::string str, int line) : type(type), value(std::move(str)), line(line) {};

    Token(TokenType type, int line) : type(type), line(line) {};

    Token() = default;
};

class Lexer {
public:
    explicit Lexer(std::istream &input);

    Token nextToken();


private:
    std::istream &input;
    char currentChar{};
    int line = 1;

    bool advance();

    char peek();

    void backup();

    void skipWhitespace();

    Token comment();

    Token multiline_comment();

    Token number();

    Token identifierOrKeyword();

    Token punctuationOrOperator();

    bool isKeyword(const std::string &str);

    bool isPunctuation(const std::string &str);

    bool isOperator(const std::string &str);


    std::unordered_map<std::string, TokenType> keywords = {
            {"int",            TokenType::INT},
            {"while",          TokenType::WHILE},
            {"float",          TokenType::FLOAT},
            {"local",          TokenType::LOCAL},
            {"if",             TokenType::IF},
            {"void",           TokenType::VOID},
            {"constructor",    TokenType::CONSTRUCTOR},
            {"then",           TokenType::THEN},
            {"class",          TokenType::CLASS},
            {"else",           TokenType::ELSE},
            {"attribute",      TokenType::ATTRIBUTE},
            {"function",       TokenType::FUNCTION},
            {"self",           TokenType::SELF},
            {"read",           TokenType::READ},
            {"public",         TokenType::PUBLIC},
            {"isa",            TokenType::ISA},
            {"write",          TokenType::WRITE},
            {"implementation", TokenType::IMPLEMENTATION},
            {"return",         TokenType::RETURN},
            {"private",        TokenType::PRIVATE},
    };

    std::unordered_map<std::string, TokenType> operators = {
            {"==",  TokenType::EQ},
            {"<>",  TokenType::NEQ},
            {"<",   TokenType::LT},
            {">",   TokenType::GT},
            {"<=",  TokenType::LTEQ},
            {">=",  TokenType::GTEQ},
            {"+",   TokenType::ADD},
            {"-",   TokenType::SUB},
            {"*",   TokenType::MUL},
            {"/",   TokenType::DIV},
            {":=",  TokenType::ASSIGN},
            {"or",  TokenType::OR},
            {"and", TokenType::AND},
            {"not", TokenType::NOT},
    };

    std::unordered_map<std::string, TokenType> punctuation = {
            {",",  TokenType::COMMA},
            {";",  TokenType::SEMICOLON},
            {"(",  TokenType::LPAREN},
            {")",  TokenType::RPAREN},
            {"{",  TokenType::LBRACE},
            {"}",  TokenType::RBRACE},
            {"[",  TokenType::LBRACKET},
            {"]",  TokenType::RBRACKET},
            {".",  TokenType::DOT},
            {":",  TokenType::COLON},
            {"=>", TokenType::ARROW},
    };
};

// Combination of punctuation and operators and keywords
static std::unordered_map<std::string, TokenType> allBuiltins = {
        {"int",            TokenType::INT},
        {"while",          TokenType::WHILE},
        {"float",          TokenType::FLOAT},
        {"local",          TokenType::LOCAL},
        {"if",             TokenType::IF},
        {"void",           TokenType::VOID},
        {"constructor",    TokenType::CONSTRUCTOR},
        {"then",           TokenType::THEN},
        {"class",          TokenType::CLASS},
        {"else",           TokenType::ELSE},
        {"attribute",      TokenType::ATTRIBUTE},
        {"function",       TokenType::FUNCTION},
        {"self",           TokenType::SELF},
        {"identifier",     TokenType::IDENTIFIER},
        {"read",           TokenType::READ},
        {"public",         TokenType::PUBLIC},
        {"isa",            TokenType::ISA},
        {"write",          TokenType::WRITE},
        {"implementation", TokenType::IMPLEMENTATION},
        {"return",         TokenType::RETURN},
        {"private",        TokenType::PRIVATE},
        {"==",             TokenType::EQ},
        {"<>",             TokenType::NEQ},
        {"<",              TokenType::LT},
        {">",              TokenType::GT},
        {"<=",             TokenType::LTEQ},
        {">=",             TokenType::GTEQ},
        {"+",              TokenType::ADD},
        {"-",              TokenType::SUB},
        {"*",              TokenType::MUL},
        {"/",              TokenType::DIV},
        {":=",             TokenType::ASSIGN},
        {"or",             TokenType::OR},
        {"and",            TokenType::AND},
        {"not",            TokenType::NOT},
        {",",              TokenType::COMMA},
        {";",              TokenType::SEMICOLON},
        {"(",              TokenType::LPAREN},
        {")",              TokenType::RPAREN},
        {"{",              TokenType::LBRACE},
        {"}",              TokenType::RBRACE},
        {"[",              TokenType::LBRACKET},
        {"]",              TokenType::RBRACKET},
        {".",              TokenType::DOT},
        {":",              TokenType::COLON},
        {"=>",             TokenType::ARROW},
        {"epsilon",        TokenType::EPSILON},
};

static std::string find_builtin(TokenType type) {
    for (const auto &pair : allBuiltins) {
        if (pair.second == type) {
            return pair.first;
        }
    }
    return "Undefined";
}
