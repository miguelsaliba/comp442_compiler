#include "lexer.h"
#include <cstdio>
#include <iostream>

// C compiler

using enum TokenType;


Lexer::Lexer(std::istream &in) : input(in), currentChar(0) {
    input >> std::noskipws;
}

bool Lexer::advance()
{
    if (!(input >> currentChar)) {
        return false;
    }
    if (currentChar == '\n') {
        line++;
    }

    return true;
}

char Lexer::peek()
{
    return input.peek();
}

// Go back one char
void Lexer::backup()
{
    input.unget();
    if (currentChar == '\n') {
        line--;
    }
    currentChar = input.peek();
}

void Lexer::skipWhitespace()
{
    while (isspace(currentChar) && advance());
}

Token Lexer::comment()
{
    std::string str;
    do {
        str += currentChar;
    } while (advance() && currentChar != '\n' && currentChar != '\r');
    return nextToken();
}

Token Lexer::multiline_comment()
{
    int startline = line;
    int level = 1;
    std::string str{'/'};

    while (level > 0) {
        if (!advance()) {
            return {EOF_TOKEN, line};
        }
        if (currentChar == '\r')
            continue;

        if (currentChar == '\n')
            str += "\\n";
        else {
            str += currentChar;
        }

        if (currentChar == '/' && input.peek() == '*') {
            advance();
            str += currentChar;
            level++;
        }

        else if (currentChar == '*' && input.peek() == '/') {
            advance();
            str += currentChar;
            level--;
        }
    }

    str += '/';
    return nextToken();
}

bool Lexer::isKeyword(const std::string &str)
{
    return keywords.find(str) != keywords.end();
}

bool Lexer::isPunctuation(const std::string &str)
{
    return punctuation.find(str) != punctuation.end();
}

bool Lexer::isOperator(const std::string &str)
{
    return operators.find(str) != operators.end();
}

Token Lexer::identifierOrKeyword()
{
    std::string str;
    bool invalid_id = false;

    if (currentChar == '_') {
        invalid_id = true;
    }

    do {
        str += currentChar;
    } while (advance() && (isalnum(currentChar) || currentChar == '_'));
    backup();

    if (invalid_id) {
        return {INVALID_ID, str, line};
    }
    if (isKeyword(str)) {
        return {keywords[str], str, line};
    }
    if (isOperator(str)) {
        return {operators[str], str, line};
    }

    return {IDENTIFIER, str, line};
}

Token Lexer::punctuationOrOperator()
{
    std::string str {currentChar};

    char secondChar = input.peek();
    if (secondChar != EOF) {
        std::string doubleCharOp = str + static_cast<char>(input.peek());

        if (isOperator(doubleCharOp)) {
            advance();
            return {operators[doubleCharOp], doubleCharOp, line};
        }
        else if (isPunctuation(doubleCharOp)) {
            advance();
            return {punctuation[doubleCharOp], doubleCharOp, line};
        }
    } 

    if (isOperator(str)) {
        return {operators[str], str, line};
    }
    else if (isPunctuation(str)) {
        return {punctuation[str], str, line};
    }
    return {INVALID_CHAR, str, line};
}

Token Lexer::number()
{
    std::string str;
    bool starts_with_zero = false;
    bool error = false;

    if (currentChar == '0') {
        starts_with_zero = true;
    }

    // Integer part
    int num_digits = 0;
    do {
        str += currentChar;
        num_digits++;
    } while (advance() && isdigit(currentChar));

    if (starts_with_zero && num_digits > 1) {
        error = true;
    }

    if (isalpha(currentChar)) {
        do {
            str += currentChar;
        } while (advance() && isalpha(currentChar));

        return {INVALID_ID, str, line};
    }

    else if (currentChar != '.') {
        backup();
        if (error) {
            return {INVALID_NUMBER, str, line};
        } else {
            return {INTLIT, str, line};
        }
    }

    // Decimal part
    int decimal_places = -1;
    do {
        str += currentChar;
        decimal_places++;
    } while (advance() && isdigit(currentChar));

    if (decimal_places < 1) {
        error = true;
    }

    if (str.back() == '0' && decimal_places != 1) {
        error = true;
    }

    if (currentChar != 'e') {
        backup();
        if (error) {
            return {INVALID_NUMBER, str, line};
        } else {
            return {FLOATLIT, str, line};
        }
    }

    // 'e' part
    str += currentChar;
    advance();
    if (currentChar == '-' || currentChar == '+') {
        str += currentChar;
        advance();
    }

    if (!isdigit(currentChar)) {
        error = true;
    }

    starts_with_zero = currentChar == '0';

    num_digits = 0;
    do {
        str += currentChar;
        num_digits++;
    } while (advance() && isdigit(currentChar));

    if (starts_with_zero && num_digits > 1) {
        error = true;
    }

    backup();

    if (error) {
        return {INVALID_NUMBER, str, line};
    }

    return {FLOATLIT, str, line};
}

Token Lexer::nextToken()
{
    advance();
    if (!input.good())
    {
        return {EOF_TOKEN, line};
    }

    skipWhitespace();
    // Check if space
    if (input.eof()) {
        return {EOF_TOKEN, line};
    }

    if (currentChar == '/') {
        char nextChar = peek();

        // Line is a comment so we get the next token
        if (nextChar == '/') {
            return comment();
        }
        else if (nextChar == '*') {
            return multiline_comment();
        }

        return {DIV, "/", line};
    }
    else if (isdigit(currentChar)) {
        return number();
    }
    else if (isalpha(currentChar) || currentChar == '_') {
        return identifierOrKeyword();
    }
    else {
        return punctuationOrOperator();
    }
}
