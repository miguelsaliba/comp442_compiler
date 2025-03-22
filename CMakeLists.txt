cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(compiler
        src/main.cpp
        src/lexer.cpp
        src/lexer.h
        src/parser.cpp
        src/parser.h
        src/lexer.cpp
        src/ast.h
        src/visitor.h
        src/symtable.h
        src/symtable.cpp
        src/ast.cpp)
