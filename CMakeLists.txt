cmake_minimum_required(VERSION 3.16)
project(A2)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(A2
        src/main.cpp
        src/lexer.cpp
        src/lexer.h
        src/parser.cpp
        src/parser.h
        src/lexer.cpp
        src/ast.h)
