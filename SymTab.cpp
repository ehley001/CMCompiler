// Created by David Ehley, Jesse Karakash, Aaron Diaz, Abel Rodarte on 2/1/18.
// Current Compiler for Stage 1 and Stage 2
// Assignment 5
// 5-4-2018

#include "SymTab.h"

// setting the variables that get passed in when we need a table entry
SymTab::SymTab(string name2, string type2,string size2, string scope2,   string address2) {
    name = name2;
    type = type2;
    scope = scope2;
    address = address2;
    size = size2;
}