//
// Created by David Ehley on 2/1/18.
//

#include <iostream>
#include <algorithm>
#include <fstream>
#include <stack>

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
    outfile.close();
}

void treenode::codeGeneration(ofstream &outfile, int &lineCount) {

    stack<int> stacker;  // initiate stack, not sure on what type it has to be yet, so i just put int for now
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

    // finds mathops to do stuff
    if(ruleNum == 180 && (//child[0]->type == "+" || child[0]->type == "-" || child[0]->type == "*" || child[0]->type == "/" ||
                           child[1]->type == "+" || child[1]->type == "-" || child[1]->type == "*" || child[1]->type == "/")){
        cout << " rulenum180 child[0] type: " << child[0]->type<<endl;

        mathOps(outfile, lineCount);
    }


    // handles output -> arglist -> x and output -> arglist -> x, y
    // does not handle mathops
    if(ruleNum == 262 && type == "output" && child[0]->child[0]->child.size() == 0){
       // cout << child[0]->child.size();


        for(int i = 0; i < child[0]->child.size(); i++){

            unordered_map<int, SymTab*>::iterator it = var.map.begin();

            while (it != var.map.end()) {
                if (it->second->name == child[0]->child[i]->type) {
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": OUT 4,0,0\n";
                    lineCount++;
                }
                it++;
            }
        }
    }


    //recursively going through the tree
    for(int count = 0; count < child.size(); count++){

        child[count]->codeGeneration(outfile, lineCount);

    }
}

//generate code for mathops
void treenode::mathOps(ofstream &outfile, int &lineCount) {

    if(child.size() == 2) {
   //     cout << "child contents " << child[0]->type << endl;
   //     cout << "child contents " << child[1]->type << endl;
    }

    for(int i = 0; i < child.size(); i++){


        // do stuff if +
        // goes through map to match variables to their addresses, if not found in map then assumes it
        // is a constant value -- such as x + 4 would assume 4 is the constant instead of a variable
        // this will be the same code for each operation, with a change in the outfile statements
        if(child[i]->type == "+"){
            // address of left side, can do checks if this is empty string, then you know its a constant or another mathop

            if(child.size() == 2){
                cout<<"****"<<child[i]->child[0]->type;
                cout<<"****"<<child[i]->child[1]->type;
            }

            string address = "";
            // address of right side
            string address2 = "";
            unordered_map<int, SymTab*>::iterator it = var.map.begin();

            // finds the address locations of the two children, if they exist, and sets them both to constants we can
            // use in our outfile.... hopefully
            // running on simpleExpressionc. , this prints out the address of x (0), as it is the only node we go to other
            // than "+", which is not found within the table. statements handling the side with no address (constants,
            // variables, etc.) should be handled after this while loop with recursion. Recursion was taken out due to
            // error listed below

            bool isConstant = false;
            int doubleConstant = 0;
            bool first = false, second = false;


            while (it != var.map.end()) {


                if (it->second->name == child[i]->child[0]->type) {

                    outfile << lineCount << ": LDA 5,1(5)\n";
                    lineCount++;
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": LDA 5,-1(5)\n";
                    lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                 else if (it->second->name == child[i]->child[1]->type) {

                    outfile << lineCount << ": LDA 5,1(5)\n";
                    lineCount++;
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": LDA 5,-1(5)\n";
                    lineCount++;
                    string address2 = it->second->address;
                    second = true;

                } else{ // this is working

                //TODO this works for every case except for adding two constants

//
                    string address2 = it->second->address;
                    isConstant = true;


                }
                it++;
            }

            //if they are both constants
            if (first == false && second == false){
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineCount++;
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true){
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false){
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if(isConstant == true) {
                outfile << lineCount << ": ADD 4,2,1\n";
                lineCount++;
            }else{
                outfile << lineCount << ": ADD 4,4,2\n";
                lineCount++;
            }





            outfile<<lineCount<<": ST 4,0(0)\n";
            lineCount++;
        }

        if(child[i]->type == "*"){
            // address of left side, can do checks if this is empty string, then you know its a constant or another mathop

            if(child.size() == 2){
                cout<<"****"<<child[i]->child[0]->type;
                cout<<"****"<<child[i]->child[1]->type;
            }

            string address = "";
            // address of right side
            string address2 = "";
            unordered_map<int, SymTab*>::iterator it = var.map.begin();

            // finds the address locations of the two children, if they exist, and sets them both to constants we can
            // use in our outfile.... hopefully
            // running on simpleExpressionc. , this prints out the address of x (0), as it is the only node we go to other
            // than "+", which is not found within the table. statements handling the side with no address (constants,
            // variables, etc.) should be handled after this while loop with recursion. Recursion was taken out due to
            // error listed below

            bool isConstant = false;
            int doubleConstant = 0;
            bool first = false, second = false;


            while (it != var.map.end()) {


                if (it->second->name == child[i]->child[0]->type) {

                    outfile << lineCount << ": LDA 5,1(5)\n";
                    lineCount++;
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": LDA 5,-1(5)\n";
                    lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                else if (it->second->name == child[i]->child[1]->type) {

                    outfile << lineCount << ": LDA 5,1(5)\n";
                    lineCount++;
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineCount++;
                    outfile << lineCount << ": LDA 5,-1(5)\n";
                    lineCount++;
                    string address2 = it->second->address;
                    second = true;

                } else{ // this is working


                    string address2 = it->second->address;
                    isConstant = true;


                }
                it++;
            }

            //if they are both constants
            if (first == false && second == false){
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineCount++;
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true){
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false){
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if(isConstant == true) {
                outfile << lineCount << ": MUL 4,2,1\n";
                lineCount++;
            }else{
                outfile << lineCount << ": MUL 4,4,2\n";
                lineCount++;
            }





            outfile<<lineCount<<": ST 4,0(0)\n";
            lineCount++;
        }


        // errors when recursing here, because tries to reach children that do not exist
       // cout << "this is child[i] text: " << child[i]->type << endl;
        child[i]->mathOps(outfile, lineCount);
    }

}


//resets the address once it gets to the max register
void treenode::checkAddress() {
    if(address == 6){
        address = -1;
    }
    address++;

}

//looks to see if current address has already been used by a global
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