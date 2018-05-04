// Created by David Ehley, Jesse Karakash, Aaron Diaz, Abel Rodarte on 2/1/18.
// Current Compiler for Stage 1 and Stage 2
// Assignment 5
// 5-4-2018

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

    int checkLineNums(int line);

    int tmpLine = 0;

    string operations [4] = {"+", "-", "*", "/"};

    int ruleNum; //int for keeping track of rules

    int intVal;

    int offset = 9;

    int tempOffset;

    int oneCount = 0;

    int twoCount = 0;

    int maxCount = 0;

    int ifCount;

    int getMax();

    string assignmentAddress = ""; //saves address location of a variable we are assigning to

    int assignmentCounter = 0; //keeps address from overwriting itself

    string type; // used for variables, parameters and functions

    string text; // other text

    void buildMapVec(string scope); // function that goes through the tree and creates table entries

    void makeFile(string file);

    void codeGeneration(ofstream &outfile, int &count);

    void assign(ofstream &outfile, int &count);

    void assignOut(ofstream &outfile, int &count);

    void mathOps(ofstream &outfile, int &count);

    void ifStatement(ofstream &outfile, int &count);

    void printTable(); // calls FunctionTable to print out the table


};

#endif //NARY_treenode_H
