%{
    #include "parser.tab.h"
    #include <stdlib.h>
    void yyerror(char *);
%}

%%

"int"       { return INT; }
"float"     { return FLOAT; }
"char"      { return CHAR; }
"void"      { return VOID; }

"case"      { return CASE; }
"default"   { return DEFAULT; }
"if"        { return IF; }
"else"      { return ELSE; }
"switch"    { return SWITCH; }
"while"     { return WHILE; }
"do"        { return DO; }
"for"       { return FOR; }
"goto"      { return GOTO; }
"continue"  { return CONTINUE; }
"break"     { return BREAK; }
"return"    { return RETURN; }

"&&"        { return AND_OP; }
"||"        { return OR_OP; }
"=="        { return EQ_OP; }
"!="        { return NE_OP; }
"<="        { return LE_OP; }
">="        { return GE_OP; }
"<<"        { return LEFT_OP; }
">>"        { return RIGHT_OP; }

[a-zA-Z_][a-zA-Z0-9_]*  { return IDENTIFIER; }
[0-9]+                 { return CONSTANT; }

[ \t\r\n]              ; /* Ignore whitespace */
.               yyerror("Skill Issue: learn grammar!");
%%

int yywrap(void) {
    return 1;
}