/****************************************************/
/* File: CMinus.y                                   */
/* The CMinus Bison specification file              */
/* Grammar from: Compiler Construction: Principles  */
/* and Practice.                                    */
/*                                                  */
/* There are currently no actions associated with   */
/* any of the rules, so running this program        */
/* on correct C- programs generates no output.      */
/*                                                  */
/* To compile create your parser                    */
/* make                                             */
/*                                                  */
/* ./parse < cminusfilename                         */
/*                                                  */
/* Stuart Hansen                                    */
/* January 2018                                     */
/*    Assignment4 by David Ehley                    */
/****************************************************/
%{
#include <iostream>
#include <string>
#include "common.h"
#include "treenode.h"

treenode * root;
treenode * child;

using namespace std;

int yylex();
int yyerror(string message);
extern char* yytext;
extern int lineno;
%}

/* book-keeping tokens */
%token   ENDFILE ERROR

/* reserved words */
%token    ELSE IF INT RETURN VOID WHILE

/* special symbols */
%token    PLUS MINUS TIMES OVER LT LE GT GE EQ NE ASSIGN
%token    SEMI COMMA DOT LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE

/* Comment tokens, currently not used */
%token    LCOMMENT RCOMMENT

/* multicharacter tokens */
%token    ID NUM

%% /* Grammar for CMinus 
      Added by David Ehley, from Louden's rules
       Actions added to rules to complete the parse tree for assignment 4
       Actions for all 29 rules defined in Louden's book*/

program : declarationlist {root = $1;};

declarationlist : declarationlist declaration
        { $$ = $1; $$->child.push_back($2);}
        | declaration { $$ = new treenode(21, string("decl list"));
        $$ -> child.push_back($1);};

declaration : vardeclaration { $$ = $1; }
| fundeclaration { $$ = $1; };

vardeclaration : typespecifier ID SEMI
        {$$ = new treenode(40, string("vardecl"));
        $$ -> child.push_back($1);
        $$ -> child.push_back($2);}
| typespecifier ID LBRACK NUM RBRACK SEMI
        {$$ = new treenode(41, string("arrayVar"));
        $$ -> child.push_back($1);
        $$ -> child.push_back($2);
        $$ -> child.push_back($4);
        };

typespecifier : INT {$$ = new treenode(262, string(""));
        $$->text="int";}
        | VOID {$$ = new treenode(300, string(""));
        $$->text="void";};

fundeclaration : typespecifier ID LPAREN params RPAREN compoundstmt
        {$$ = $2; $$ -> ruleNum=60; $$ ->text=($1->text);
        $$ -> child.push_back($4);
        $$ -> child.push_back($6);};

params : paramlist {$$ = $1;}
        | VOID {$$ = new treenode(264, string("void"));};

paramlist : paramlist COMMA param
        {$$ = $1;
        $$ -> child.push_back($3);}
        | param {$$ = new treenode(81, string("paramlist"));
        $$ -> child.push_back($1);};

param : typespecifier ID
        {$$ = new treenode(90, string("param"));
        $$ -> child.push_back($1);
        $$ -> child.push_back($2);}
        | typespecifier ID LBRACK RBRACK
        {$$ = new treenode(91, string("paramArray"));
                 $$ -> child.push_back($1);
                 $$ -> child.push_back($2);};

compoundstmt : LBRACE localdeclarations statementlist RBRACE
        {$$ = new treenode(100, string("compoundstmt"));
        $$ -> child.push_back($2);
        $$ -> child.push_back($3);};

localdeclarations : localdeclarations vardeclaration
        {$$ = $1; $$->child.push_back($2);}
        | {$$ = new treenode(111, string("localdeclarations"));};

statementlist : statementlist statement
        {$$ = $1;
        $$ -> child.push_back($2);}
        | {$$ = new treenode(121, string("exp list"));};

statement : expressionstmt {$$ = $1;} |
        compoundstmt {$$ = $1;} |
        selectionstmt {$$ = $1;} |
        iterationstmt {$$ = $1;} |
        returnstmt {$$ = $1;};

expressionstmt : expression SEMI
        {$$ = $1;}
        | SEMI {$$ = $1;};

selectionstmt : IF LPAREN expression RPAREN statement
        {$$ = new treenode(150, string("selectionstmt"));
        $$ -> child.push_back($3);
        $$ -> child.push_back($5);}
        | IF LPAREN expression RPAREN statement ELSE statement
        {$$ = new treenode(151, string ("if..else"));
        $$->child.push_back($3);
        $$->child.push_back($5);
        $$ -> child.push_back($7);};

iterationstmt : WHILE LPAREN expression RPAREN statement
        {$$ = new treenode(160, string("iterationstmt"));
        $$ -> child.push_back($3);
        $$ -> child.push_back($5);};

returnstmt : RETURN SEMI
        {$$ = new treenode(170, string("return"));};|
        RETURN expression SEMI
        {$$ = new treenode(171, string("return with val"));
        $$ -> child.push_back($2);};

expression : var ASSIGN expression
        {$$ = new treenode(180, string("assignment"));
        $$ -> child.push_back($1); $$ -> child.push_back($3);}
        | simpleexpression {$$ = $1; };

var : ID {$$ = $1; }
        | ID LBRACK expression RBRACK
        {$$ = new treenode(191, string("var"));
        $$ -> child.push_back($3);};

simpleexpression : additiveexpression relop additiveexpression
        {$$ = new treenode(200, string("relop"));
        $$ -> child.push_back($2);
        $$ -> child.push_back($1);
        $$ -> child.push_back($3);}|
        additiveexpression {$$ = $1;};

relop : LE   {$$ = new treenode(210, string("<=")) ;}
        | LT {$$ = new treenode(211, string("<")) ;}
        | GT {$$ = new treenode(212, string(">")) ;}|
          GE {$$ = new treenode(213, string(">=")) ;}
        | EQ {$$ = new treenode(214, string("==")) ;}
        | NE {$$ = new treenode(215, string("!=")) ;};

additiveexpression : additiveexpression addop term
        {
        $$ = $2;
        $$ -> child.push_back($1);
        $$ -> child.push_back($3);}|
        term {$$ = $1;};

addop : PLUS {$$ = $1;$$ = new treenode(220, string("+"));}|
        MINUS {$$ = $1; $$ = new treenode(220, string("-"));};

term : term mulop factor
        {
        $$ = $2;
        $$ -> child.push_back($1);$$ -> child.push_back($3);}|
        factor {$$ = $1;};

mulop : TIMES {$$ = $1;$$ = new treenode(240, string("*"));}|
        OVER {$$ = $1;$$ = new treenode(240, string("/"));};

factor : LPAREN expression RPAREN
        {$$ = new treenode(260, string("factor"));
        $$ -> child.push_back($2);}
        |var {$$ = $1; $$ -> ruleNum=261;}
        |call{$$ = $1; $$ -> ruleNum=262;}
        | NUM {$$ = $1; $$ -> ruleNum=263;};

call : ID LPAREN args RPAREN
        {$$ = $1;
        $$->child.push_back($3);};

args : arglist {$$ = $1;}
        | {$$ = new treenode(281, string("args"));};

arglist : arglist COMMA expression
        {
        $$ = $1; $$->child.push_back($3);}
        |expression  {$$ = new treenode(291, string("arglist"));
        $$ -> child.push_back($1);};

/* End of grammar */

%%

/*
  Dumbed down version of yyerror, mostly borrowed from TINY.y
*/
int yyerror(string message)
{
  cout << "Syntax error at line: " << lineno << " " << message << endl;
  return 0;
}



