//
// Created by ehley on 3/29/18.
//

#ifndef CMCOMPILER_FUNCTIONTABLE_H
#define CMCOMPILER_FUNCTIONTABLE_H

#include <unordered_map>
#include "SymTab.h"

using namespace std;

class FunctionTable{

public:

    int count = 0; // int that keeps track of how many entries we have

   unordered_map<int, SymTab*> map;  // map declaration

    void build(SymTab*); // function that adds the SymTab to the map


    void print(); // function used to print out the map
    
};

#endif //CMCOMPILER_FUNCTIONTABLE_H
