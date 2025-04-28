%{
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
int yylex(void);
void yyerror(char *s);
int sym[26];
%}

/* Tokens */
%token IDENTIFIER CONSTANT
%token INT FLOAT CHAR VOID
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

/* Logical operators */
%token AND_OP OR_OP
%token EQ_OP NE_OP
%token LE_OP GE_OP
%token LEFT_OP RIGHT_OP

/* Operator precedence */
%right '='
%right '?' ':'
%left OR_OP
%left AND_OP
%left '|'
%left '^'
%left '&'
%left EQ_OP NE_OP
%left '<' '>' LE_OP GE_OP
%left LEFT_OP RIGHT_OP
%left '+' '-'
%left '*' '/' '%'
%right UMINUS UPLUS '!' '~'

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program
    : external_declaration
    | program external_declaration
    ;

external_declaration
    : function_definition
    | variable_definition
    ;

variable_definition
    : type_specifier identifier_list ';'
    ;

identifier_list
    : IDENTIFIER
    | identifier_list ',' IDENTIFIER
    ;

type_specifier
    : INT
    | FLOAT
    | CHAR
    | VOID
    ;

function_definition
    : type_specifier IDENTIFIER '(' parameter_list ')' compound_statement
    ;

parameter_list
    : /* empty */
    | parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

parameter_declaration
    : type_specifier IDENTIFIER
    ;

compound_statement
    : '{' '}'
    | '{' scope '}'
    ;

scope
    : scope_item
    | scope scope_item
    ;

scope_item
    : variable_definition
    | statement
    ;

statement
    : expression_statement
    | compound_statement
    | selection_statement
    | loop_statement
    | jump_statement
    ;

expression_statement
    : ';'
    | expression ';'
    ;

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;

loop_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;

jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'
    | RETURN ';'
    | RETURN expression ';'
    ;

expression
    : assignment_expression
    | expression ',' assignment_expression
    ;

assignment_expression
    : logical_or_expression
    | IDENTIFIER '=' assignment_expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR_OP logical_and_expression
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression AND_OP inclusive_or_expression
    ;

inclusive_or_expression
    : exclusive_or_expression
    | inclusive_or_expression '|' exclusive_or_expression
    ;

exclusive_or_expression
    : and_expression
    | exclusive_or_expression '^' and_expression
    ;

and_expression
    : equality_expression
    | and_expression '&' equality_expression
    ;

equality_expression
    : relational_expression
    | equality_expression EQ_OP relational_expression
    | equality_expression NE_OP relational_expression
    ;

relational_expression
    : shift_expression
    | relational_expression '<' shift_expression
    | relational_expression '>' shift_expression
    | relational_expression LE_OP shift_expression
    | relational_expression GE_OP shift_expression
    ;

shift_expression
    : additive_expression
    | shift_expression LEFT_OP additive_expression
    | shift_expression RIGHT_OP additive_expression
    ;

additive_expression
    : multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

multiplicative_expression
    : unary_expression
    | multiplicative_expression '*' unary_expression
    | multiplicative_expression '/' unary_expression
    | multiplicative_expression '%' unary_expression
    ;

unary_expression
    : primary_expression
    | '-' unary_expression %prec UMINUS
    | '+' unary_expression %prec UPLUS
    | '!' unary_expression
    | '~' unary_expression
    ;

primary_expression
    : IDENTIFIER
    | CONSTANT
    | '(' expression ')'
    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    return yyparse();
}
