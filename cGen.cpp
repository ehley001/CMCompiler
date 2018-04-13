//
// Created by ehley on 4/13/18.
//

#include <fstream>
#include "cGen.h"
extern string fileName;


void cGen::generate(unordered_map<int, SymTab*> local,  unordered_map<int, SymTab*> function, vector<SymTab*> param, string file) {

    ofstream outfile (file);


    unordered_map<int, SymTab*>::iterator it = local.begin(); // initializing the map iterator

    // iterating through the function map
    while (it != local.end()){

    outfile << "IN 4,0,0\n";
        outfile << "ST 4, " << it->second->address << "(0)\n";
        outfile << "LD 4, " << it->second->address << "(0)\n";
        outfile << "OUT 4, 0,0\n";


        it++; //moving forward in the map after everything has been printed for a function
    }

    outfile << "HALT 0,0,0" << endl;

}
