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

int address = 6;
vector<int> globalVec;

vector<int> lineNums;
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
                lineCount = checkLineNums(lineCount);

                outfile << lineCount << ": ST 4," << it->second->address << "(0)\n";
                lineCount++;
            }
            it++;
        }

    }

    // finds mathops to do stuff
    if(ruleNum == 180 && (//child[0]->type == "+" || child[0]->type == "-" || child[0]->type == "*" || child[0]->type == "/" ||
                           child[1]->type == "+" || child[1]->type == "-" || child[1]->type == "*" || child[1]->type == "/")){

        assign(outfile, lineCount);
    }


    // handles output -> arglist -> x and output -> arglist -> x, y
    // does not handle mathops
    if(ruleNum == 262 && type == "output" && child[0]->child[0]->child.size() == 0){
       // cout << child[0]->child.size();


        for(int i = 0; i < child[0]->child.size(); i++){

            unordered_map<int, SymTab*>::iterator it = var.map.begin();

            while (it != var.map.end()) {
                if (it->second->name == child[0]->child[i]->type) {
                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": OUT 4,0,0\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                }
                it++;
            }
        }
    }

    // same as above statement, exept checking for mathops

    if(ruleNum == 262 && type == "output" && child[0]->child[0]->child.size() == 2){
         cout << "in the out";


        child[0]->assignOut(outfile,lineCount);

        for(int i = 0; i < child[0]->child.size(); i++){

            unordered_map<int, SymTab*>::iterator it = var.map.begin();

            while (it != var.map.end()) {
                if (it->second->name == child[0]->child[i]->type) {
                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": OUT 4,0,0\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                }
                it++;
            }
        }
    }

    if (ruleNum == 200){
        int tempCount;
        cout<<lineCount<<"if\n";
        tempCount = lineCount;
        lineCount++;

        ifStatement(outfile, lineCount);
    }


    //recursively going through the tree
    for(int count = 0; count < child.size(); count++){

        child[count]->codeGeneration(outfile, lineCount);

    }
}

// gens code for if statements
// child[0] == relop (>, =>, <, <=, ==), child[1] and child[2] vars/constants
void treenode::ifStatement(ofstream &outfile, int &lineCount){
    unordered_map<int, SymTab*>::iterator it = var.map.begin();

    // keeps count of how many variables exist in the statement
    int tempCounter = 0;

    // stores the address of the first variable, if it exists
    string firstAddress = "";
    // stores the type of the first variable, used if this is a constant
    string firstType = "";
    // stores the address of the second variable, if it exists
    string secondAddress = "";
    // stores the type of the second variable, used if this is a constant
    string secondType = "";
    while (it != var.map.end()) {
        if (it->second->name == child[1]->type) {
            firstAddress = it->second->address;
            tempCounter++;
        }

        else if( it->second->name == child[2]->type){
            secondAddress = it->second->address;
            tempCounter++;
        }
        it++;
    }

    // if address of first is not found, then it is constant to be checked
    if(firstAddress == ""){
        firstType = child[1]->type;
    }

    // if address of second is not found, then it is constant to be checked
    if(secondAddress == ""){
        secondType = child[2]->type;
    }

    if(tempCounter == 2){
        // here because very good reasons
        lineCount--;
        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": LD 4,"<< firstAddress <<"(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": LD 2,"<< secondAddress <<"(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": SUB 4,4,2\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": JLT 4,2(7)\n";
        lineNums.push_back(lineCount);
        lineCount++;

    }
    // handles ifs with 1 variable in table and 1 constant
    if(tempCounter ==1){
        // here because very good reasons
        lineCount--;

        // the two if statements below mirror each other, to handle x > 2 and 2 > x

        // first child has no type set, meaning it was found in our table. Second is guaranteed to be our constant here
        if(firstType == ""){
            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": LD 4,"<< firstAddress <<"(0)\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": LDC 2,"<< secondType <<"(0)\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": SUB 4,4,2\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": JLT 4,2(7)\n";
            lineNums.push_back(lineCount);
            lineCount++;
        }

        // second child has no type set, meaning it was found in our table. First is guaranteed to be our constant here
        else if(secondType == ""){
            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": LDC 4,"<< firstType <<"(0)\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": LD 2,"<< secondAddress <<"(0)\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": SUB 4,4,2\n";
            lineNums.push_back(lineCount);
            lineCount++;

            lineCount = checkLineNums(lineCount);
            outfile << lineCount << ": JLT 4,2(7)\n";
            lineNums.push_back(lineCount);
            lineCount++;
        }
    }

    // occurs if error happens or if both are constants
    if(tempCounter == 0){
        // here because very good reasons
        lineCount--;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": LDC 4,"<< firstType <<"(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": LDC 2,"<< secondType <<"(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": SUB 4,4,2\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": JLT 4,2(7)\n";
        lineNums.push_back(lineCount);
        lineCount++;
    }

}

void treenode::assignOut(ofstream &outfile, int &lineCount){

    string tempAddress = "";
    unordered_map<int, SymTab*>::iterator it = var.map.begin();
    while (it != var.map.end()) {
        if (it->second->name == child[0]->child[0]->type) {
            tempAddress = it->second->address;
        }
        it++;
    }

    if(child[0]->child[0]->ruleNum == 220 || 240){
        mathOps(outfile, lineCount);
        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": ST 4," << tempAddress << "(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": OUT 4,0,0\n";
        lineNums.push_back(lineCount);
        lineCount++;
    }

}

void treenode::assign(ofstream &outfile, int &lineCount){

    string tempAddress = "";
    unordered_map<int, SymTab*>::iterator it = var.map.begin();
    while (it != var.map.end()) {
        if (it->second->name == child[0]->type) {
            tempAddress = it->second->address;
        }
        it++;
    }

    if(child[1]->ruleNum == 220 || 240){

        mathOps(outfile, lineCount);
        lineCount = checkLineNums(lineCount);
        outfile << lineCount << ": ST 4," << tempAddress << "(0)\n";
        lineNums.push_back(lineCount);
        lineCount++;

    }

}

//generate code for mathops
void treenode::mathOps(ofstream &outfile, int &lineCount) {

    //stack operations for later, push and pop
//    outfile << lineCount << ": LDA 5,1(5)\n";
//    lineCount++;
//    outfile << lineCount << ": LDA 5,-1(5)\n";
//    lineCount++;

    // stores the assignment address, this is passed in as an argument so its not lost recursively


    for(int i = 0; i < child.size(); i++){

        if(child[i]->type == "+"){

            string address = "";
            // address of right side
            string address2 = "";
            unordered_map<int, SymTab*>::iterator it = var.map.begin();


            bool isConstant = false;
            int doubleConstant = 0;
            bool first = false, second = false;
            string doubleAddress = "";
            int doubleVar = 2;

            while (it != var.map.end()) {

                if(child[i]->child[0]->type == child[i]->child[1]->type && (child[i]->ruleNum == 220 || child[i]->ruleNum == 240)){
                    lineCount = checkLineNums(lineCount);
                    if(doubleVar < 4){
                        doubleAddress = it->second->address;
                    }
                    outfile << lineCount << ": LD " << doubleVar << "," << doubleAddress << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    doubleVar*=2;
                    first = true;
                    second = true;
                }

                else if (it->second->name == child[i]->child[0]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                 else if (it->second->name == child[i]->child[1]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    string address2 = it->second->address;
                    second = true;

                }

                else{
                    string address2 = it->second->address;
                    isConstant = true;


                }
                if(it->second->name == child[0]->type) {
                    doubleAddress = it->second->address;
                }
                it++;
            }

            // variables so we dont have to write out child 1million and a half times
            string one = child[i]->child[0]->type;
            string two = child[i]->child[1]->type;
            //if they are both constants, and they are not an operation
            if ((first == false && second == false) && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "-"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true && (one != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false && (two != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if ((first == false && second == false) && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "-"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true && (one != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false && (two != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if(isConstant == true && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "-") {

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ADD 4,2,1\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ST 4,0(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDA 5, 1(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

            }else if (one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "-"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ADD 4,4,2\n";
                lineNums.push_back(lineCount);
                lineCount++;



            }
            else if(isConstant == true && (one == "*" || one == "-" || one == "+" || one == "-" || two == "*" || two == "-" || two == "+" || two != "-")){
                offset = 9;
                // these statements pop our stack

                tmpLine = lineCount;


                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 1,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5<< ": ADD 4,2,1\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;


                offset++;


            }
            else{
                offset = 5;

                outfile << lineCount + offset << ": ADD 4,4,2\n";
                lineNums.push_back(lineCount + offset );
                tempOffset = lineCount + 1;

                offset++;

            }

        }

        if(child[i]->type == "-"){
            // address of left side, can do checks if this is empty string, then you know its a constant or another mathop

            string address = "";
            // address of right side
            string address2 = "";
            unordered_map<int, SymTab*>::iterator it = var.map.begin();



            bool isConstant = false;
            int doubleConstant = 0;
            bool first = false, second = false;
            string doubleAddress = "";
            int doubleVar = 2;

            while (it != var.map.end()) {

                if(child[i]->child[0]->type == child[i]->child[1]->type){
                    lineCount = checkLineNums(lineCount);
                    if(doubleVar < 4){
                        doubleAddress = it->second->address;
                    }
                    outfile << lineCount << ": LD " << doubleVar << "," << doubleAddress << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    doubleVar*=2;
                    first = true;
                    second = true;
                }

                else if (it->second->name == child[i]->child[0]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                else if (it->second->name == child[i]->child[1]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    string address2 = it->second->address;
                    second = true;

                } else{ // this is working


                    string address2 = it->second->address;
                    isConstant = true;


                }
                if(it->second->name == child[0]->type)
                    doubleAddress = it->second->address;
                it++;
            }

            // variables so we dont have to write out child 1million and a half times
            string one = child[i]->child[0]->type;
            string two = child[i]->child[1]->type;


//            if (one == "*" || one == "/" ||  one == "+" || one == "-" ){
//                oneCount = lineCount;
//                cout << "one:" << lineCount;
//            }
//
//            if( two == "*" || two == "/" ||  two == "+" || two == "-"){
//                twoCount++;
//                cout << "two:" << twoCount;
//            }
            //if they are both constants, and they are not an operation
            if ((first == false && second == false) && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "-"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true && (one != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false && (two != "*" || "/" || "+" || "-")){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if(isConstant == true && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "/") {

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": SUB 4,2,1\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ST 4,0(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDA 5, 1(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

            }else if (one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": SUB 4,4,2\n";
                lineNums.push_back(lineCount);

                lineCount++;
            }
            else if(isConstant == true && (one == "*" || one == "-" || one == "+" || one == "-" || two == "*" || two != "-" || two == "+" || two == "/")){
                offset = 14;
                // these statements pop our stack

                tmpLine = lineCount;


                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 1,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5<< ": SUB 4,2,1\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;


                offset++;


            }
            else{
                offset = 10;

                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 4,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5 << ": SUB 4,4,2\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;

                if(ruleNum == 180){
                    outfile << lineCount+offset+6 << ": ST 4," << doubleAddress << "(0)\n";
                    lineNums.push_back(lineCount+offset+6);
                    tempOffset = lineCount + 1;
                }

                offset++;

            }

        }


        // multiplication
        if(child[i]->type == "*"){
            // address of left side, can do checks if this is empty string, then you know its a constant or another mathop


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
            int doubleVar = 2;
            bool first = false, second = false;
            string doubleAddress = "";

            while (it != var.map.end()) {

                if(child[i]->child[0]->type == child[i]->child[1]->type){
                    lineCount = checkLineNums(lineCount);
                    if(doubleVar < 4){
                        doubleAddress = it->second->address;
                    }
                    outfile << lineCount << ": LD " << doubleVar << "," << doubleAddress << "(0)\n";
                    outfile << "* I AM IN DOUBLES\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    doubleVar*=2;
                    first = true;
                    second = true;
                }

                else if (it->second->name == child[i]->child[0]->type) {


                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);

                   // lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                else if (it->second->name == child[i]->child[1]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    //lineCount++;
                    string address2 = it->second->address;
                    second = true;

                } else{ // this is working


                    string address2 = it->second->address;
                    isConstant = true;


                }
                if(it->second->name == child[0]->type) {
                    doubleAddress = it->second->address;
                }
                it++;
            }

            // variables so we dont have to write out child 1million and a half times
            string one = child[i]->child[0]->type;
            string two = child[i]->child[1]->type;

            //if they are both constants, and they are not an operation
            if ((first == false && second == false) && one != "*" && one != "-" && one != "+" && one != "/" && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true && one != "*" && one != "-" && one != "+" && one != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            if(isConstant == true && one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "/") {

                // pushes on our stack
                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": MUL 4,2,1\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ST 4,0(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDA 5, 1(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

            }else if (one != "*" && one != "-" && one != "+" && one != "-" && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": MUL 4,4,2\n";
                lineNums.push_back(lineCount);

                lineCount++;
            }
            else if(isConstant == true && (one == "*" || one == "-" || one == "+" || one == "-" || two != "*" || two == "-" || two == "+" || two == "/")){
                offset = 14;
                // these statements pop our stack

                tmpLine = lineCount;


                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 1,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5<< ": MUL 4,2,1\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;


                offset++;


            }
            else{
                offset = 10;

                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 4,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5 << ": MUL 4,4,2\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;

                if(ruleNum == 180){
                    outfile << lineCount+offset+6 << ": ST 4," << doubleAddress << "(0)\n";
                    lineNums.push_back(lineCount+offset+6);
                    tempOffset = lineCount + 1;
                }

                offset++;

            }
        }


        // DIVISION rule
        if(child[i]->type == "/"){
            // address of left side, can do checks if this is empty string, then you know its a constant or another mathop


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
            int doubleVar = 2;
            string doubleAddress = "";

            while (it != var.map.end()) {

                if (it->second->name == child[i]->child[0]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 4," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount = checkLineNums(lineCount);
                    //lineCount++;
                    string address = it->second->address;
                    first = true;

                }
                else if (it->second->name == child[i]->child[1]->type) {

                    lineCount = checkLineNums(lineCount);
                    outfile << lineCount << ": LD 2," << it->second->address << "(0)\n";
                    lineNums.push_back(lineCount);
                    lineCount++;
                    string address2 = it->second->address;
                    second = true;

                } else{ // this is working


                    string address2 = it->second->address;
                    isConstant = true;


                }
                if(it->second->name == child[0]->type) {
                    doubleAddress = it->second->address;
                }
                it++;
            }

            // variables so we dont have to write out child 1million and a half times
            string one = child[i]->child[0]->type;
            string two = child[i]->child[1]->type;
            //if they are both constants, and they are not an operation
            if ((first == false && second == false) && one != "*" && one != "-" && one != "+" && one != "/" && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 2," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                //if only the first child is a constant
            } else if(first == false && second == true && one != "*" && one != "-" && one != "+" && one != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[0]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
                //if only the second child is a constant
            } else if (first == true && second == false && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDC 1," << child[i]->child[1]->type << "(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;
            }

            //the only time reg 1 is used is when there are constants
            //the only time reg 1 is used is when there are constants
            if(isConstant == true && one != "*" && one != "-" && one != "+" && one != "/" && two != "*" && two != "-" && two != "+" && two != "/") {

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": DIV 4,2,1\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": ST 4,0(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": LDA 5, 1(5)\n";
                lineNums.push_back(lineCount);
                lineCount++;

            }else if (one != "*" && one != "-" && one != "+" && one != "/" && two != "*" && two != "-" && two != "+" && two != "/"){

                lineCount = checkLineNums(lineCount);
                outfile << lineCount << ": DIV 4,4,2\n";
                lineNums.push_back(lineCount);

                lineCount++;
            }
            else if(isConstant == true && (one == "*" || one == "-" || one == "+" || one == "/" || two == "*" || two == "-" || two == "+" || two != "/")){
                offset = 14;
                // these statements pop our stack

                tmpLine = lineCount;


                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 1,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5<< ": DIV 4,2,1\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;


                offset++;


            }
            else{
                offset = 10;

                outfile << lineCount + offset + 1<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 1);

                outfile << lineCount + offset + 2<< ": LD 2,0(5)\n";
                lineNums.push_back(lineCount + offset + 2);

                outfile << lineCount + offset + 3<< ": LDA 5,-1(5)\n";
                lineNums.push_back(lineCount + offset + 3);

                outfile << lineCount + offset + 4<< ": LD 4,0(5)\n";
                lineNums.push_back(lineCount + offset + 4);

                outfile << lineCount + offset + 5 << ": DIV 4,4,2\n";
                lineNums.push_back(lineCount + offset + 5);
                tempOffset = lineCount + 1;

                if(ruleNum == 180){
                    outfile << lineCount+offset+6 << ": ST 4," << doubleAddress << "(0)\n";
                    lineNums.push_back(lineCount+offset+6);
                    tempOffset = lineCount + 1;
                }

                offset++;

            }
        }
        child[i]->mathOps(outfile, lineCount);
    }


    getMax();
}

int treenode::checkLineNums(int line) {

    bool checker = false;
 //   cout<<"passed in:" << line <<endl;
    while(find(lineNums.begin(), lineNums.end(), line) != lineNums.end()){
        line++;
        checker = true;

    }

    if (checker == true){
        lineNums.push_back(line);
    }

    return  line;
}


int treenode::getMax() {

    if(maxCount <= tempOffset) {
        maxCount = tempOffset;
    }



}


//resets the address once it gets to the max register
void treenode::checkAddress() {
//    if(address == 15){
//        address = -1;
//    }
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