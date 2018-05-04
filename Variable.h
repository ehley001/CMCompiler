// Created by David Ehley, Jesse Karakash, Aaron Diaz, Abel Rodarte on 2/1/18.
// Current Compiler for Stage 1 and Stage 2
// Assignment 5
// 5-4-2018

#ifndef CMCOMPILER_VARIABLE_H
#define CMCOMPILER_VARIABLE_H

#include <string>
#include <unordered_map>
#include "SymTab.h"

using namespace std;

class Variable {

public:

    int count = 0; // int that keeps track of how many entries we have

    unordered_map<int, SymTab*> map;  // map declaration

    void build(SymTab*); // function that adds the SymTab data to the map


    void print(string str); // function used to print out the variable map

};


#endif //CMCOMPILER_VARIABLE_H
