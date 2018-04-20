//
// Created by ehley on 4/13/18.
//

#include <fstream>
#include "cGen.h"
extern string fileName;

void cGen::generate(string action, string file) {

    ofstream outfile(file);

    outfile << "test";

//    int lineNum = 0;
//
//    unordered_map<int, SymTab*>::iterator it = local.begin(); // initializing the map iterator
//
//    // iterating through the function map
//    while (it != local.end()){
//
//        outfile << lineNum << ": IN 4,0,0\n";
//            lineNum++;
//        outfile << lineNum <<": ST 4, " << it->second->address << "(0)\n";
//            lineNum++;
//        outfile << lineNum<< ": LD 4, " << it->second->address << "(0)\n";
//            lineNum++;
//        outfile << lineNum << ": OUT 4, 0,0\n";
//            lineNum++;
//
//
//        it++; //moving forward in the map after everything has been printed for a function
//    }
//
//    outfile << lineNum <<": HALT 0,0,0" << endl;
    outfile.close();

}
