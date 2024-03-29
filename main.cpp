#include <iostream>
#include <unordered_map>
#include <fstream>


#include "parser.hpp"
#include "treenode.h"
#include "SymTab.h"
#include "FunctionTable.h"
#include "Variable.h"
#include "ParameterList.h"

// Created by David Ehley, Jesse Karakash, Aaron Diaz, Abel Rodarte on 2/1/18.
// Current Compiler for Stage 1 and Stage 2
// Assignment 5
// 5-4-2018
extern FILE * yyin;
extern treenode * root;
extern FunctionTable func;
extern Variable var;
extern ParameterList param;


using namespace std;

//test by Jesse
// main provided by project description
int main(int argc, char *argv[]) {


    if ( argc > 1) {
        yyin = fopen(argv[1], "r");
    }
    else{
        cout << "Reading from standard input" << endl;
        yyin = stdin;
    }

    yyparse();
    //root->preorder_print();

    //function that adds the data to the needed maps/vector
    root->buildMapVec("global"); //passing in global first, since thats the first section

    //after table has been built, print it
    //root->printTable();

    string fileName = string(argv[1]);
    fileName = fileName.substr(0, fileName.length()-2) + "tm";

    root->makeFile(fileName);


    //gen.generate(var.map,func.map, param.vecMap, fileName);

    return 0;
}

