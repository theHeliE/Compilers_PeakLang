%code requires {

    // Struct to hold the value of a variable
    typedef struct {
        int intVal;
        float floatVal;
        bool boolVal;
        char* stringVal;
        char charVal;
    } Value;  
}

%{
#include <stdio.h>
#include <stdlib.h>


extern FILE *yyin;
int yylex(void);
void yyerror(char *s);
int sym[26];
%}

/* Tokens */
%union {
    int intVal;
    float floatVal;
    char * stringVal;
    char charVal;
    char * variable;
    bool boolVal;
    Value val;
}
%token IDENTIFIER CONSTANT
%token INT FLOAT CHAR VOID
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token ASSIGNMENT

/* Declare the type of identifier_list */
%type <stringVal> identifier_list

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
mainProgram: 
    program 
    { 
        printf("Main program\n"); 
    }
    | error 
    { 
        ErrorToFile("Syntax Error"); YYABORT; 
    };

program
    : external_declaration
    {
        printf("Program\n");
    }
    | program external_declaration
    {
        printf("Program with external declarations\n");
    }
    ;

external_declaration
    : function_definition
    {
        printf("Function definition\n");
    }
    | variable_definition
    {
        printf("Variable definition\n");
    }
    ;

variable_definition
    : type_specifier identifier_list ';'
    {
        printf("variable_declaration Rule 2\n");

        symbolTable->insert($2, $1, NULL);
    }
    | type_specifier identifier_list ASSIGNMENT expression ';'
    {
        // Check if there is a mismatch between the type of the variable and the type of the value
        if($1 != $4[0]->type){
            printf("Error: Type mismatch between variable and value\n");
            ErrorToFile("Type mismatch between variable and value");
        }

        symbolTable->insert($2, $1, $4[0]->value);
        symbolTable->update_Value($2, $4[0]->value, $1); // to make the variable isUsed = true
    }
    | CONSTANT type_specifier identifier_list ASSIGNMENT expression ';'
    {
        printf("variable_declaration Rule 3\n");

        symbolTable->insert($3, $2, $5[0]->value, true);
        symbolTable->update_Func($3);
        symbolTable->printTable();
    }
    ;

identifier_list
    : IDENTIFIER
    {
        $$ = $1;
    }
    | identifier_list ',' IDENTIFIER
    {
        $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        sprintf($$, "%s,%s", $1, $3);
    }
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
