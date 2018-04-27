//
// Created by David Ehley on 2/1/18.
//

#ifndef NARY_treenode_H
#define NARY_treenode_H

#include <vector>

using namespace std;

class treenode{

public:
    int value;

    int lineNumber = 0;

    vector<treenode*> child;

   // treenode(int value_arg);  // Constructor

    treenode(int numRule, string ruleText);  // new constructor for building the tree

    void add_child(treenode *node);

    void preorder_print();

    void checkAddress();

    void searchGlobal();

    string operations [4] = {"+", "-", "*", "/"};

    int ruleNum; //int for keeping track of rules

    int intVal;

    int offset;

    int tempOffset;

    string assignmentAddress = ""; //saves address location of a variable we are assigning to

    int assignmentCounter = 0; //keeps address from overwriting itself

    string type; // used for variables, parameters and functions

    string text; // other text

    void buildMapVec(string scope); // function that goes through the tree and creates table entries

    void makeFile(string file);

    void codeGeneration(ofstream &outfile, int &count);

    void mathOps(ofstream &outfile, int &count, string &assignmentAddress);

    void printTable(); // calls FunctionTable to print out the table


};

#endif //NARY_treenode_H
