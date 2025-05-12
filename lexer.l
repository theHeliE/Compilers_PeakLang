%{
    #include "parser.tab.h"
    #include "value_helpers.h"
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <fcntl.h>
    #include <io.h>

    #define isatty(x) 0 
    

    void yyerror(const char *);
%}

%%

"int"       { yylval.val.type = INT_TYPE; return INT; }
"float"     { yylval.val.type = FLOAT_TYPE; return FLOAT; }
"char"      { yylval.val.type = CHAR_TYPE; return CHAR; }
"void"      { yylval.val.type = VOID_TYPE; return VOID; }
"bool"      { yylval.val.type = BOOL_TYPE; return BOOL; }
"tmam"      { yylval.val.type = BOOL_TYPE; yylval.val.boolVal = true; return TRUE; }
"faks"     { yylval.val.type = BOOL_TYPE; yylval.val.boolVal = false; return FALSE; }

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
"thabet"    { return THABET; }


"="         { return ASSIGNMENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }

"&&"        { return AND_OP; }
"||"        { return OR_OP; }
"=="        { return EQ_OP; }
"!="        { return NE_OP; }
"<="        { return LE_OP; }
">="        { return GE_OP; }
"<<"        { return LEFT_OP; }
">>"        { return RIGHT_OP; }
"<"         { return L_OP; }
">"         { return G_OP; }

";"         { return ';'; }
","         { return ','; }
"("         { return '('; }
")"         { return ')'; }
"{"         { return '{'; }
"}"         { return '}'; }

'[^']' {
    yylval.val.type = CHAR_TYPE;
    yylval.val.charVal = yytext[1];
    return CONSTANT; 
}

[a-zA-Z_][a-zA-Z0-9_]*  { 
    yylval.val.type = UNKNOWN_TYPE;
    yylval.val.stringVal = strdup(yytext);
    return IDENTIFIER; 
}

[0-9]+\.[0-9]+ {
    yylval.val.type = FLOAT_TYPE;
    yylval.val.floatVal = atof(yytext);
    return CONSTANT; // Return same token as integer for simplicity?
}

[0-9]+      { 
    yylval.val.type = INT_TYPE;
    yylval.val.intVal = atoi(yytext);
    return CONSTANT; 
}

[ \t\r\n]              ; /* Ignore whitespace */
.               yyerror("Skill Issue: learn grammar!");
%%

int yywrap(void) {
    return 1;
}