// Created by David Ehley, Jesse Karakash, Aaron Diaz, Abel Rodarte on 2/1/18.
// Current Compiler for Stage 1 and Stage 2
// Assignment 5
// 5-4-2018

#include <iostream>

#ifndef CMCOMPILER_SYMTAB_H
#define CMCOMPILER_SYMTAB_H

using namespace std;


class SymTab {

public:

   string name, type, scope, address, size; // the different variables needed for our table

    //constructor
    SymTab( string name, string type,string  scope,string  address,string size);

};


#endif //CMCOMPILER_SYMTAB_H
