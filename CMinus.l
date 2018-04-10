/****************************************************/
/* File: CMinus.l                                   */
/* Lex specification for C- language from           */
/* Compiler Construction: Principles and Practice   */
/*                                                  */
/* To compile and run:                              */
/*   flex CMinus.l                                  */
/*   g++ lex.yy.c                                   */
/*   ./a.out < cminusfilename                       */
/* Note the redirection arrow, < , is needed.       */
/****************************************************/


/* CMinus file with declarations added by David Ehley */

%{
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "common.h"
#include "parser.hpp"


using namespace std;
/*
   This enum will be replaced in more advanced versions
   with the generated enum from bison
*/
/*typedef enum

   {
       /* book-keeping tokens */
     /*  ENDFILE, ERROR,*/

    /* reserved words */
    /*ELSE, IF, INT, RETURN, VOID, WHILE,*/

    /* special symbols 
    PLUS, MINUS, TIMES, OVER, LT, LE, GT, GE, EQ, NE, ASSIGN,
    SEMI, COMMA, DOT, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE,*/

    /* Comment tokens, currently not used */
    /*LCOMMENT, RCOMMENT,*/

    /* multicharacter tokens */
  /*  ID, NUM*/
 /*  } TokenType;     */

/* These methods are defined below and show how to skip
   comments.
*/
void skipcmnts(void);

/* lexeme of identifier or reserved word */
string tokenString;

/* line number counter */
int lineno = 1;
%}

%option noyywrap

/* Added Regular Expressions for C- */
newline     \n
whitespace  [ \t]+
letter      [a-zA-Z] 
identifier    {letter}+
digit       [0-9]
number      {digit}+  

/* Added declarations for keywords and special symbols*/
%%

"if"     {return IF;}
"else"   {return ELSE;}
"int"    {return INT;}
"return" {return RETURN;}
"void"   {return VOID;}
"while"  {return WHILE;}
"+"      {return PLUS;}
"-"      {return MINUS;}
"*"      {return TIMES;}
"/"      {return OVER;}
"<"      {return LT;}
"<="     {return LE;}
">"      {return GT;}
">="     {return GE;}
"=="     {return EQ;}
"!="     {return NE;}
"="      {return ASSIGN;}
";"      {return SEMI;}
","      {return COMMA;}
"."      {return DOT;}
"("      {return LPAREN;}
")"      {return RPAREN;}
"["      {return LBRACK;}
"]"      {return RBRACK;}
"{"      {return LBRACE;}
"}"      {return RBRACE;}



{number}        {yylval = new treenode(NUM, string(yytext));
                 yylval->intVal = atoi(yytext);
                 return NUM;}
{identifier}    { yylval = new treenode(ID, string(yytext)); return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { skipcmnts(); }
.               {return ERROR;}


%%


/* Borrowed mostly from TINY.l */
void skipcmnts (void) {
   char previous = '\0';
   char c;
   c = yyinput();
   do
   {
      if (c == EOF) {
         break;
      }
      if (c == '\n') {
         lineno++;
      }
      previous = c;
      c = yyinput();
   } while (previous != '*' || c != '/');

}

/*
   Our main simply scans for tokens until
   we come to the end of the file.

int main(int argc, char** argv) {
    int tokentype;
    do {
        tokentype = yylex();
        cout << setw(4) << lineno << setw(5) << tokentype << setw(10) << yytext << endl;
    } while ( tokentype != ENDFILE );
    return 0;
}*/
