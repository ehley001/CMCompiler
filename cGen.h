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

    ofstream outfile;

    int count = 0;

    void generate(string action, string file);

};


#endif //CMCOMPILER_CGEN_H
