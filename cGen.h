//
// Created by ehley on 4/13/18.
//

#ifndef CMCOMPILER_CGEN_H
#define CMCOMPILER_CGEN_H


#include <unordered_map>
#include <vector>
#include "SymTab.h"

using namespace std;

class cGen {

public:


    int count = 0;

    void generate(unordered_map<int, SymTab*> local,  unordered_map<int, SymTab*> function, vector<SymTab*> param, string file);

};


#endif //CMCOMPILER_CGEN_H
