//
// Created by ehley on 4/3/18.
//

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
