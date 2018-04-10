//
// Created by ehley on 4/5/18.
//

#include <iostream>
#include <iomanip>
#include "Variable.h"



void Variable::build(SymTab * table) {
    //inserts the count, and whatever table data was passed in from treenode.cpp
    map.insert(make_pair(count, table));
    count++;
}

void Variable::print(string str) {

    unordered_map<int, SymTab*>::iterator it = map.begin(); // initializing the map iterator

    //checking if we are in the global table or not
    if(str != "global") {
        // if we arent, print out the Local Variable label
        cout << "Local Variables" << endl;
        cout << "Name        Type        Size        Scope       Address" << endl;
    }

        // iterating through the variable map
        while (it != map.end()){

            //checking to make sure the scope passed in matches with the one the iterator is on
            if(str == it->second->scope){

                //print out the row with formatting added
                cout << left << setw(12)<< it->second->name << left << setw(12) << it->second->type
                     << left << setw(12)<< it->second->size << left << setw(12)<< it->second->scope
                     << left << setw(12) << "0" << endl;

        }

            it++; // advance the iterator
    }

    //added spacing
    cout<<"\n\n";
}




