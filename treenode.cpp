//
// Created by David Ehley on 2/1/18.
//

#include <iostream>
#include <algorithm>
#include <fstream>

#include "treenode.h"
#include "FunctionTable.h"
#include "Variable.h"
#include "ParameterList.h"
#include "cGen.h"


using namespace std;

string countSpace = ""; // space holder
int numSpace =0;       // space counter

int address = 0;
vector<int> globalVec;
cGen generator;


FunctionTable func;
Variable var;
ParameterList param;

/*treenode::treenode(int value_arg) {
    value = value_arg;     // Initializing value
}*/

treenode::treenode(int numRule, string ruleText) {

    ruleNum = numRule;  // rule number that was passed in
    type = ruleText;    // rule descriptor

}

void treenode::add_child(treenode *node){

   // if(child.size() < 10){ // as long as the vector is less than 10, add the child to it
        child.push_back(node);
   // }
}

void treenode::buildMapVec(string scope) {

    // looking for a function rule
    if(ruleNum == 60){

      searchGlobal();

      scope = type;                                         // saving off the function name
      func.build(new SymTab(type, text,  "1",scope, to_string(address)));  // adding the data to the map
        checkAddress();


    }

    // if parameter array is found, create a vector entry
    if(ruleNum == 91){

        searchGlobal();


        //adding [] since the parameter is an array
        param.add(new SymTab(child[1]->type,child[0]->text+"[]", "1", scope, to_string(address)));
        checkAddress();



    }

    // if parameter is found, create a vector entry
    if(ruleNum == 90){

        searchGlobal();

        param.add(new SymTab(child[1]->type,child[0]->text, "1", scope, to_string(address)));
        checkAddress();


    }


    // if a variable array is found, create a variable map entry
    if(ruleNum == 41){

        //if the address is found in the vector of global address's, move to the next one
       searchGlobal();
        if(scope == "global"){           // if global, push the address so it isnt used again
            globalVec.push_back(address);
        }

     //adding [] since the variable is an array
     var.build(new SymTab(child[1]->type,child[0]->text+"[]", child[2]->type, scope, to_string(address))) ;
        checkAddress();

    }

    // if a variable is found, create a variable map entry
    if(ruleNum == 40){
searchGlobal();

        if(scope == "global"){           // if global, push the address so it isnt used again
            globalVec.push_back(address);
        }

        var.build(new SymTab(child[1]->type,child[0]->text,"1", scope, to_string(address))) ;
        checkAddress();


    }

    // resetting address once it uses 8 registers


    // going thru the tree in a preorder method again, just passing in the scope this time
    for(int count = 0; count < child.size(); count++){

        child[count]->buildMapVec(scope); // call to print the next root

    }

}

void treenode::makeFile(string file){
    ofstream outfile(file);
    int lineCount = 0;
    codeGeneration(outfile, lineCount);
}

void treenode::codeGeneration(ofstream &outfile, int &lineCount) {

    // have to assign this to x and y
    if(ruleNum == 180 && child[1]->type == "input"){
        outfile << lineCount << ": IN 4,0,0\n";
        lineCount++;


        unordered_map<int, SymTab*>::iterator it = var.map.begin();

        while (it != var.map.end()){
            if(it->second->name == child[0]->type){
                outfile << lineCount << ": ST 4," << it->second->address << "(0)\n";
                lineCount++;
            }
            it++;
        }

    }

    // handles output -> arglist -> x and output -> arglist -> x, y
    if(ruleNum == 262 && type == "output"){
        unordered_map<int, SymTab*>::iterator it = var.map.begin();

        for(int i = 0; i < child[0]->child.size(); i++){

            while (it != var.map.end()) {
                if (it->second->name == child[0]->child[i]->type) {
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": OUT 4,0,0\n";
                    lineCount++;
                }
                it++;
            }

            //outfile << child[0]->child[i]->type;
        }
    }

    for(int count = 0; count < child.size(); count++){

        child[count]->codeGeneration(outfile, lineCount);

    }
}

void treenode::checkAddress() {
    if(address == 6){
        address = -1;
    }
    address++;

}

void treenode::searchGlobal() {
    while(find(globalVec.begin(), globalVec.end(), address) != globalVec.end()){
        address++;
    }

}

void treenode::printTable(){
    func.print();
}



// modified preorder print method for Cminus grammar
void treenode::preorder_print() {

    // I use space to make sure that I dont enter two spaces in a row, if type is empty
    string space = " ";
    if (type == ""){
        space = "";
    }

    cout << countSpace << ruleNum << " " << type <<  space << text << endl; // print the rule num & type with the correct spacing

    for(int count = 0; count < child.size(); count++){

            countSpace += "   ";  // add 3 spaces
            numSpace += 3;       // update the counter

            child[count]->preorder_print(); // call to print the next root
        
            countSpace.resize(numSpace - 3); // subtract the spaces
            numSpace -= 3;

    }

}