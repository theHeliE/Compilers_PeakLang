/////////////////////////////////////// CODE REQUIRES ///////////////////////////////////////

%code requires {

    #include "symbol_table.h"

    // Struct to hold the value of a variable
    typedef struct {
        int intVal;
        float floatVal;
        bool boolVal;
        char* stringVal;
        char charVal;
    } Value;  
}

/////////////////////////////////////// GLOBALS ///////////////////////////////////////

%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"


extern FILE *yyin;
int yylex(void);
void yyerror(char *s);
int sym[26];

symbolTable *SymbolTable = new symbolTable();

typedef struct {
    int intVal;
    float floatVal;
    bool boolVal;
    char* stringVal;
    char charVal;
} Value;  


%}

/////////////////////////////////////// UNION ///////////////////////////////////////
%union {
    int intVal;
    float floatVal;
    char * stringVal;
    char charVal;
    char * variable;
    bool boolVal;
    Value val;
}


/////////////////////////////////////// TOKENS ///////////////////////////////////////
%token IDENTIFIER CONSTANT
%token INT FLOAT CHAR VOID BOOL 
%token TRUE FALSE
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token ASSIGNMENT

/////////////////////////////////////// TYPES ///////////////////////////////////////
%type <stringVal> identifier_list

// FIXME: 3ayzeen n3rf el values bta3 el type
%type <stringVal> expression
%type <stringVal> assignment_expression
%type <stringVal> logical_or_expression
%type <stringVal> logical_and_expression
%type <stringVal> inclusive_or_expression
%type <stringVal> exclusive_or_expression
%type <stringVal> and_expression
%type <stringVal> equality_expression
%type <stringVal> relational_expression
%type <stringVal> shift_expression
%type <stringVal> additive_expression
%type <stringVal> multiplicative_expression
%type <stringVal> unary_expression
%type <stringVal> primary_expression
%type <stringVal> type_specifier



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


/////////////////////////////////////// MAIN PROGRAM ///////////////////////////////////////
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
    | /* empty */
    {
        // if the program is empty, print the symbol table
        if (symbolTable->getParent() == NULL) {
            symbolTable->printTableToFile();
        }
    }
    ;

/////////////////////////////////////// EXTERNAL DECLERATION ///////////////////////////////////////


// FUNCTION & VARIABLE
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


/////////////////////////////////////// VARIABLE DEFINITION ///////////////////////////////////////
variable_definition:
    
    // eg. int a, b, c;
    type_specifier identifier_list ';'
    {
        printf("variable_declaration Rule 2\n");

        symbolTable->insert($2, $1, NULL);
    }

    // #####################################
    
    // eg. int a, b, c = 5;
    | type_specifier identifier_list ASSIGNMENT expression ';'
    {
        // Check if there is a mismatch between the type of the variable and the type of the value
        if($1 != $4[0]->type){
            printf("[ERROR] Type mismatch between variable and value\n");
            ErrorToFile("Type mismatch between variable and value");
        }

        symbolTable->insert($2, $1, $4[0]->value);
        symbolTable->update_Value($2, $4[0]->value, $1); // to make the variable isUsed = true
    }

    // #####################################

    // eg. const int a, b, c = 5, d = 10;
    | CONSTANT type_specifier identifier_list ASSIGNMENT expression ';'
    {
        printf("variable_declaration Rule 3\n");

        symbolTable->insert($3, $2, $5[0]->value, true);
        symbolTable->update_Func($3);
        symbolTable->printTable();
    }
    ;

/////////////////////////////////////// IDENTIFIER ///////////////////////////////////////

identifier_list:

    // eg. a  
    IDENTIFIER
    {
        $$ = $1;
    }

    // #####################################

    // eg. a, b, c
    | identifier_list ',' IDENTIFIER
    {
        $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        sprintf($$, "%s,%s", $1, $3);
    }
    ;

/////////////////////////////////////// TYPE ///////////////////////////////////////

type_specifier:
    INT
    | FLOAT
    | CHAR
    | VOID
    | BOOL
    ;

/////////////////////////////////////// FUNCTION DEFINITION ///////////////////////////////////////

function_definition:
    
    // TODO: add symbolTable here
    // eg. int main() { return 0; }
    type_specifier IDENTIFIER '(' parameter_list ')' compound_statement
    ;



/////////////////////////////////////// PARAMETERS ///////////////////////////////////////

parameter_list
    : /* empty */
    | parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

/////////////////////////////////////// PARAMETERS DECLERATION ///////////////////////////////////////

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



expression_statement
    : ';'
    | expression ';'
    ;

expression
    : assignment_expression
    | expression ',' assignment_expression
    ;


/////////////////////////////////////// STATEMENTS ///////////////////////////////////////
statement
    : expression_statement
    | compound_statement
    | selection_statement
    | loop_statement
    | jump_statement
    ;


/////////////////////////////////////// CONDITIONS ///////////////////////////////////////

selection_statement:
    
    // eg. if (x == 5) printf("x is 5");
    IF '(' expression ')' statement %prec LOWER_THAN_ELSE

    // #####################################

    // eg. if (x == 5) printf("x is 5"); else printf("x is not 5");
    | IF '(' expression ')' statement ELSE statement

    // #####################################

    // eg. switch (x) case 1: printf("x is 1");
    // TODO: idk if it need additional rules
    // it doesn't take statement, it needs its own rules
    | SWITCH '(' expression ')' '{' case_list '}'
    ;

case_list
    : case_item
    | case_list case_item
    ;

case_item:
     CASE CONSTANT ':' statement
    | DEFAULT ':' statement
    ;


/////////////////////////////////////// LOOPS ///////////////////////////////////////

loop_statement:

    // eg. while (x == 5) printf("x is 5");
     WHILE '(' expression ')' statement

    // #####################################

    // eg. do {x++;} while (x < 10);
    | DO statement WHILE '(' expression ')' ';'

    // #####################################

    // eg. for (i = 0; i < 10; i++) printf("i is %d", i);
    | FOR { symbolTable= new SymbolTable(symbolTable);} '(' expression_statement expression_statement expression ')' statement
    ;

/////////////////////////////////////// JUMP ///////////////////////////////////////

// TODO: check if need to add symbolTable here
jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'

    // #####################################
        
    | RETURN ';'
    {
       // check if the function is returning a value with same type
        // TODO: string functionName = (active functions) I would like to get the function name from the symbol table
        // TODO: we can have a vector of strings that contain all active functions (or stack)
        string functionName = "dummy"; // change later
        SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        
        // Im not sure that NULL de haga sah walla la
        if (functionEntry->returnType != NULL) 
        {
            printf("[ERROR] Type mismatch in return statement\n");
            ErrorToFile("Type mismatch in return statement");
        }

    }

    // #####################################

    | RETURN expression ';'
    {
        // check if the function is returning a value with same type
        // TODO: string functionName = (active functions) I would like to get the function name from the symbol table
        // TODO: we can have a vector of strings that contain all active functions (or stack)
        string functionName = "dummy" // change later
        SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        
        if (functionEntry->returnType != $2[0]->type) 
        {
            printf("[ERROR] Type mismatch in return statement\n");
            ErrorToFile("Type mismatch in return statement");
        }
    }
    ;


/////////////////////////////////////// EXPRESSIONS ///////////////////////////////////////
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
    fprintf(stderr, "[ERROR] %s\n", s);
}

int main(void) {
    return yyparse();
}
