//
// Created by ehley on 4/5/18.
//

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
