//
// Created by ehley on 3/28/18.
//


#include <iostream>
#include "FunctionTable.h"
#include "Variable.h"
#include "ParameterList.h"

extern Variable var;
extern ParameterList param;

void FunctionTable::build(SymTab * table) {
    //inserts the count, and whatever table data was passed in from treenode.cpp
    map.insert(make_pair(count, table));
    count++;
}

void FunctionTable::print() {
    unordered_map<int, SymTab*>::iterator it = map.begin(); // initializing the map iterator

    // since the global table is always first, print that out first
    cout << "Global Table" << endl;
    cout << "Name        Type        Size        Scope       Address" << endl;
    var.print("global");

    cout<<"Function Table" << endl;

    // iterating through the function map
    while (it != map.end()){
        //grabbing the function name and type
        cout << "Function:  "<< it->second->name << " " << it->second->type << " " << it->second->address << endl;

        param.printVec(it->second->scope); // printing all the parameters for a given scope
        var.print(it->second->scope);      // printing all the variables for a given scope


        it++; //moving forward in the map after everything has been printed for a function
    }
}

