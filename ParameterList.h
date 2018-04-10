//
// Created by ehley on 3/29/18.
//

#ifndef CMCOMPILER_PARAMETERLIST_H
#define CMCOMPILER_PARAMETERLIST_H

#include <vector>
#include <unordered_map>
#include "SymTab.h"

using namespace std;

class ParameterList{

public:

    vector<SymTab*> vecMap;  //the vector that will the store the parameter entries

    void add(SymTab * node); // function that adds the SymTab data to the vector

    void printVec(string str); // function that prints out the vector as needed

};

#endif //CMCOMPILER_PARAMETERLIST_H
