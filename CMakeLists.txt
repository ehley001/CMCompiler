cmake_minimum_required(VERSION 3.9)
project(CMCompiler)

ADD_CUSTOM_COMMAND(
        OUTPUT ../parser.cpp ../parser.hpp
        COMMAND bison -d -o ../parser.cpp ../CMinus.y
        DEPENDS CMinus.y
        COMMENT "Generating Parser.cpp and Parser.hpp"
)

ADD_CUSTOM_COMMAND(
        OUTPUT ../scanner.cpp
        COMMAND flex -o ../scanner.cpp ../CMinus.l
        DEPENDS parser.hpp common.h CMinus.l
        COMMENT "Generating scanner.cpp"
)



set(CMAKE_CXX_STANDARD 11)

add_executable(CMCompiler main.cpp treenode.h treenode.cpp parser.cpp parser.hpp scanner.cpp common.h FunctionTable.cpp ParameterList.cpp ParameterList.h FunctionTable.h SymTab.cpp SymTab.h Variable.cpp Variable.h)