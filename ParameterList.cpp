//
// Created by ehley on 3/28/18.
//

#include <iostream>
#include <iomanip>
#include "ParameterList.h"


/* note a vector is used here instead of a unordered map in order to maintain the
 order of the parameters */

void ParameterList::add(SymTab *node) {
    // add the table data we got from treenode.cpp into the vector
    vecMap.push_back(node);
    }


void ParameterList::printVec(string str) {
    vector<SymTab *>::iterator vecIT = vecMap.begin(); // initializing the vector iterator

    int countVec =0; // keeps track of what vector row we are currently in

    bool checkIsEmpty = true;   //boolean to check if there are parameters or not

    //iterating through the vector
    while (vecIT != vecMap.end()){

            //comparing scopes
            if (str == vecMap[countVec]->scope) {

                //this makes sure the parameter label only gets printed out once per function
                if(checkIsEmpty == true){
                    cout << "Parameters" << endl;
                    cout << "Name        Type        Size        Scope       Address" << endl;

                    //if we got this far, there are parameters so flip the checker
                    checkIsEmpty = false;
                }

                //print out the row with formatting added
                cout << left << setw(12) << vecMap[countVec]->name << left << setw(12) << vecMap[countVec]->type
                     << left << setw(12) << vecMap[countVec]->size << left << setw(12) << vecMap[countVec]->scope
                     << left << setw(12) << "0" << endl;


            }
        countVec++; //move to the next entry

        vecIT++;   // advance the iterator

    }

    // if checkIsEmpty hasnt been flipped by this point, there are no parameters for the function
    if(checkIsEmpty == true){
        cout<< "Parameters   -- Parameter list is empty for this function" << endl;
    }

}