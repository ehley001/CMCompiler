//
// Created by ehley on 4/3/18.
//

#include "SymTab.h"

// setting the variables that get passed in when we need a table entry
SymTab::SymTab(string name2, string type2,string size2, string scope2,   string address2) {
    name = name2;
    type = type2;
    scope = scope2;
    address = address2;
    size = size2;
}