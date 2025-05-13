/////////////////////////////////////// CODE REQUIRES ///////////////////////////////////////

%code requires {
    #include "value_helpers.h"
}

/////////////////////////////////////// GLOBALS ///////////////////////////////////////

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "quadruples.h"
#include "SymbolTable.h"
#include <utility>
#include <queue>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

extern FILE *yyin;
int yylex(void);
void yyerror(const char *s);
// void ErrorToFile(string);
int sym[26];

SymbolTable *symbolTable = new SymbolTable();
int isError = false;

// Helper function to resolve an operand to an integer value
// Returns a pair: {value, success_flag}
std::pair<int, bool> resolve_operand_to_int(const Value& operand, SymbolTable* table) {
    // Check if it's an identifier that needs lookup
    // Condition: has a name AND type is not one of the directly usable literal types
    if (operand.stringVal != nullptr && 
        (operand.type != INT_TYPE && operand.type != FLOAT_TYPE && 
         operand.type != BOOL_TYPE && operand.type != CHAR_TYPE /* Removed check for non-existent CONSTANT_TYPE */)) {
        std::string varName(operand.stringVal);
        SingleEntry* entry = table->get_SingleEntry(varName);
        if (entry && entry->value && entry->type == INT_TYPE) {
            // printf("Debug: Resolved identifier '%s' to %d\n", varName.c_str(), *(static_cast<int*>(entry->value)));
            return {*(static_cast<int*>(entry->value)), true};
        }
        // yyerror(("Could not resolve identifier '" + varName + "' to INT or not found.").c_str());
        return {0, false}; // Resolution failed or not an int
    } else if (operand.type == INT_TYPE) {
        // Direct integer literal or already resolved constant of int type
        // printf("Debug: Operand is direct INT value %d\n", operand.intVal);
        return {operand.intVal, true};
    } else if (operand.type == UNKNOWN_TYPE && operand.intVal != 0 ) { // Fallback check for integer constants if lexer assigns UNKNOWN_TYPE but fills intVal
        // This is a guess based on the presence of CONSTANT token; adjust if lexer behaves differently.
         // Check if the value corresponds to the CONSTANT token itself, assuming lexer set intVal.
         // A better approach is for the lexer to assign INT_TYPE to integer constants.
         // printf("Debug: Operand is likely CONSTANT token (treated as INT) value %d\n", operand.intVal);
        // return {operand.intVal, true}; // Temporarily disabling this fallback as it's uncertain
         return {0, false}; 
    }
    // yyerror(("Operand type '" + enumToString(operand.type) + "' not resolvable to INT directly.").c_str());
    return {0, false}; // Not an int or recognized identifier pattern
}

// Helper function to allocate memory for a Value's content
void* allocateValueFromExpression(const Value& exprValue) {
    void* allocated_ptr = nullptr;
    switch(exprValue.type) {
        case INT_TYPE:    allocated_ptr = (void*)new int(exprValue.intVal); break;
        case FLOAT_TYPE:  allocated_ptr = (void*)new float(exprValue.floatVal); break;
        case BOOL_TYPE:   allocated_ptr = (void*)new bool(exprValue.boolVal); break;
        case CHAR_TYPE:   allocated_ptr = (void*)new char(exprValue.charVal); break;
        case STRING_TYPE: 
            if (exprValue.stringVal) allocated_ptr = (void*)strdup(exprValue.stringVal); 
            break;
        default:
            fprintf(stderr, "Warning: Cannot allocate value for unhandled expression type %d\n", exprValue.type);
            break;
    }
    return allocated_ptr;
}

string enumToString (dataType enumType) {
    switch (enumType) {
        case dataType::INT_TYPE:
            return "INT";
        case dataType::FLOAT_TYPE:
            return "FLOAT";
        case dataType::BOOL_TYPE:
            return "BOOL";
        case dataType::STRING_TYPE:
            return "STRING";
        case dataType::CHAR_TYPE:
            return "CHAR";
        case dataType::FUNC_TYPE:
            return "FUNC";
        case dataType::VOID_TYPE:
            return "VOID";
        case dataType::UNKNOWN_TYPE:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

// Helper function to convert a Value to its string representation
string valueToString(const Value& val) {
    switch (val.type) {
        case INT_TYPE:    return std::to_string(val.intVal);
        case FLOAT_TYPE:  return std::to_string(val.floatVal);
        case STRING_TYPE: return val.stringVal ? string(val.stringVal) : ""; // Handle potential null
        case BOOL_TYPE:   return val.boolVal ? "True" : "False";
        case CHAR_TYPE:   return string(1, val.charVal); // Create string from char
        default:          return ""; // Return empty string for unknown/unhandled types
    }
}


///// QUADRUPLES /////

Quadruples * quadruples = new Quadruples();

// Notmally the "Main" quadruple is the current quadruple, when we enter a new scope, this temporarily changes.
// Quadruples * currentQuadruple = quadruples;
// vector<Quadruples*> quadrupleStack;

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
%token <val> IDENTIFIER
%token <val> CONSTANT
%token <val> INT 
%token <val> FLOAT 
%token <val> CHAR 
%token <val> VOID 
%token <val> BOOL 
%token <val> TRUE
%token <val> FALSE
%token <val> CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN 
%token <val> THABET

%token <val> ASSIGNMENT

/////////////////////////////////////// TYPES ///////////////////////////////////////



%type <val> expression
%type <val> identifier_list
%type <val> assignment_expression
%type <val> logical_or_expression
%type <val> logical_and_expression
%type <val> inclusive_or_expression
%type <val> exclusive_or_expression
%type <val> and_expression
%type <val> equality_expression
%type <val> relational_expression
%type <val> shift_expression
%type <val> additive_expression
%type <val> multiplicative_expression
%type <val> unary_expression
%type <val> primary_expression
%type <val> type_specifier
%type <val> parameter_list
%type <val> parameter_declaration
%type <val> selection_statement




/* Logical operators */
%token AND_OP OR_OP
%token EQ_OP NE_OP
%token LE_OP GE_OP
%token LEFT_OP RIGHT_OP
%token L_OP G_OP

/* Operator precedence */

%right '?' ':'
%left OR_OP
%left AND_OP
%left '|'
%left '^'
%left '&'
%left EQ_OP NE_OP
%left L_OP G_OP LE_OP GE_OP
%left LEFT_OP RIGHT_OP 

%right UMINUS UPLUS '!' '~' 

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right '='
%left '+' '-'
%left '*' '/' '%'

/////////////////////////////////////// MAIN PROGRAM ///////////////////////////////////////
%%
mainProgram: 
    program 
    { 
        printf("Main program\n"); 
    }
    | error 
    { 
        // ErrorToFile("Syntax Error"); YYABORT; 
        printf("Feh errors ya man\n");  YYABORT; 
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


/////////////////////////////////////// EXTERNAL DECLERATION ///////////////////////////////////////


// FUNCTION & VARIABLE
external_declaration:
    function_definition
    {
        printf("Function definition\n");
    }
    | variable_definition
    {
        printf("Variable definition\n");
    }
    | statement
    {
        printf("Expression statement\n");
    }
    ;


/////////////////////////////////////// VARIABLE DEFINITION ///////////////////////////////////////
variable_definition:
    
    // eg. int a, b, c;
    type_specifier identifier_list ';'
    {
        printf("variable_declaration\n");
        symbolTable->insert($2, $1, NULL);

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName($2), "", "");


    }

    // #####################################
    
    // eg. int a, b, c = 5;

    | type_specifier identifier_list ASSIGNMENT expression ';'
    {
           if (getValueName($1)!=enumToString($4.type)) {
            yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
        }
        void* allocated_val_ptr = allocateValueFromExpression($4); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert($2, $1, allocated_val_ptr); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName($2))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName($2)).c_str());
             symbolTable->insert($2, $1, nullptr); 
        }

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName($2), "", "");
        quadruples->addQuadruple("ASSIGN", valueToString($4), "", getValueName($2));
    }

    // #####################################

    // eg. const int a, b, c = 5, d = 10;
    | THABET type_specifier identifier_list ASSIGNMENT expression ';'
    {
        printf("variable_declaration Rule 3\n");
            if (getValueName($2)!=enumToString($5.type)) {
                yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
            }

         // $2 is type_specifier (Value), $3 is identifier_list (Value), $5 is expression (Value)
        
        void* allocated_val_ptr = allocateValueFromExpression($5); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert($3, $2, allocated_val_ptr,true); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName($3))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName($2)).c_str());
             symbolTable->insert($2, $1, nullptr); 
        }

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName($3), "", "");
        quadruples->addQuadruple("ASSIGN", valueToString($5), "", getValueName($3));

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
        // $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        // sprintf($$, "%s,%s", $1, $3);
    }
    ;


 /////////////////////////////////////// TYPE ///////////////////////////////////////

type_specifier:
    INT     { $$.stringVal = "INT"; }
    | FLOAT   { $$.stringVal = "FLOAT"; }
    | CHAR    { $$.stringVal = "CHAR"; }
    | VOID    { $$.stringVal = "VOID";}
    | BOOL    { $$.stringVal = "BOOL"; }
    ;

expression
    : assignment_expression {
        printf("Assignment Expression\n");
    }
    | expression ',' assignment_expression
    ;


/////////////////////////////////////// FUNCTION DEFINITION ///////////////////////////////////////

function_definition:
    
    // TODO: add symbolTable here
    // eg. int main() { return 0; }
    type_specifier IDENTIFIER '(' parameter_list ')' compound_statement
    ;



/////////////////////////////////////// PARAMETERS ///////////////////////////////////////


parameter_list:
    parameter_declaration           { $$ = $1; }
    | parameter_list ',' parameter_declaration { $$ = $1; } // FIXME: change later de haga khara
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
    {
        printf("Selection statement\n");
    }

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
    /* | FOR { symbolTable= new SymbolTable(symbolTable);} '(' expression_statement expression_statement expression ')' statement */
    | FOR '(' expression_statement expression_statement expression ')' statement
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
        // SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        
        // Im not sure that NULL de haga sah walla la
        // if (functionEntry->returnType != NULL) 
        // {
        //     printf("[ERROR] Type mismatch in return statement\n");
        //     ErrorToFile("Type mismatch in return statement");
        // }

    }

    // #####################################

    | RETURN expression ';'
    {
        // check if the function is returning a value with same type
        // TODO: string functionName = (active functions) I would like to get the function name from the symbol table
        // TODO: we can have a vector of strings that contain all active functions (or stack)
        string functionName = "dummy"; // change later
        // SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        
        // if (functionEntry->returnType != $2[0]->type) 
        // {
        //     printf("[ERROR] Type mismatch in return statement\n");
        //     ErrorToFile("Type mismatch in return statement");
        // }
    }
    ;


/////////////////////////////////////// EXPRESSIONS ///////////////////////////////////////
assignment_expression:
 logical_or_expression
    | IDENTIFIER ASSIGNMENT assignment_expression  {
        printf("Assignment expression\n");
        // $1 is IDENTIFIER, $3 is assignment_expression (Value)


        void* allocated_val_ptr = allocateValueFromExpression($3);
        if (allocated_val_ptr != nullptr) {
            symbolTable->update_Value(getValueName($1), allocated_val_ptr , $3.type);

        // check if there is an error in the symbol table
        if (symbolTable->get_SingleEntry(getValueName($1))->isError) {
            yyerror("Matrakez ya 3aaam (pay attention noob)");
        }
        } else {
            yyerror(("Failed to create value for assignment to variable " + getValueName($1)).c_str());
        }
        quadruples->addQuadruple("ASSIGN", valueToString($3), "", getValueName($1));
    }
            


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
    | equality_expression EQ_OP relational_expression{
        
    }
    | equality_expression NE_OP relational_expression
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression L_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<int, bool> left = resolve_operand_to_int($1, symbolTable);
        std::pair<int, bool> right = resolve_operand_to_int($3, symbolTable);

        if (left.second && right.second) {
            printf("Debug: Comparing %d < %d\n", left.first, right.first);
            $$.boolVal = left.first < right.first;
        } else {
            yyerror("Type error or unresolved identifier in '<' comparison.");
            $$.boolVal = false; // Default to false on error
        }
    }
    | relational_expression G_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<int, bool> left = resolve_operand_to_int($1, symbolTable);
        std::pair<int, bool> right = resolve_operand_to_int($3, symbolTable);

        if (left.second && right.second) {
            printf("Debug: Comparing %d > %d\n", left.first, right.first);
            $$.boolVal = left.first > right.first;
        } else {
            yyerror("Type error or unresolved identifier in '>' comparison.");
            $$.boolVal = false;
        }
    }
    | relational_expression LE_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<int, bool> left = resolve_operand_to_int($1, symbolTable);
        std::pair<int, bool> right = resolve_operand_to_int($3, symbolTable);

        if (left.second && right.second) {
            printf("Debug: Comparing %d <= %d\n", left.first, right.first);
            $$.boolVal = left.first <= right.first;
        } else {
            yyerror("Type error or unresolved identifier in '<=' comparison.");
            $$.boolVal = false;
        }
    }
    | relational_expression GE_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<int, bool> left = resolve_operand_to_int($1, symbolTable);
        std::pair<int, bool> right = resolve_operand_to_int($3, symbolTable);

        if (left.second && right.second) {
            printf("Debug: Comparing %d >= %d\n", left.first, right.first);
            $$.boolVal = left.first >= right.first;
        } else {
            yyerror("Type error or unresolved identifier in '>=' comparison.");
            $$.boolVal = false;
        }
    }
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

/* unary_expression
    : primary_expression
    | '-' unary_expression %prec UMINUS
    | '+' unary_expression %prec UPLUS
    | '!' unary_expression
    | '~' unary_expression
    ; */

unary_expression
    : primary_expression         { $$ = $1; }
    | '-' unary_expression %prec UMINUS {
          // Create a new Value and apply negation
          $$ = $2;
          $$.intVal = -$$.intVal;
      }
    | '+' unary_expression %prec UPLUS {
          $$ = $2;
      }
    | '!' unary_expression {
          $$ = $2;
          $$.boolVal = !$$.boolVal;
      }
    | '~' unary_expression {
          $$ = $2;
          $$.intVal = ~$$.intVal;
      }
    ;

    
primary_expression
    : IDENTIFIER            { 
         $$ = Value();
        SingleEntry* entry = symbolTable->get_SingleEntry(getValueName($1));
        if (entry) {
             $$.type = entry->type;
             // If the variable has an initialized value in the symbol table
            if (entry->value != nullptr) { // Assuming 'entry->value' is the void* to the actual data
                switch(entry->type) {
                    case INT_TYPE:
                        $$.intVal = *(static_cast<int*>(entry->value));
                        printf("Debug: Resolved identifier '%s' to %d\n", getValueName($1), $$.intVal);
                        break;
                    case FLOAT_TYPE:
                        $$.floatVal = *(static_cast<float*>(entry->value));
                        break;
                    case BOOL_TYPE:
                        $$.boolVal = *(static_cast<bool*>(entry->value));
                        break;
                    case CHAR_TYPE:
                        $$.charVal = *(static_cast<char*>(entry->value));
                        break;
                    case STRING_TYPE:
                        // For STRING_TYPE, $$.stringVal should hold the content
                        // for valueToString and allocateValueFromExpression to work correctly.
                        if ($$.stringVal) free($$.stringVal); // Free if already allocated by Value() or previous use
                        $$.stringVal = strdup(static_cast<char*>(entry->value)); // Copy the string content
                        break;
                    default:
                         if ($1.stringVal) { // Check if stringVal is not null
                           yyerror(("Unhandled type for identifier '" + std::string($1.stringVal) + "'").c_str());
                       } else {
                           yyerror("Unhandled type for identifier (name not available).");
                       }
                        break;
                }
            }
        } else {
            yyerror(("Identifier '" + string($1.stringVal) + "' not found.").c_str());
            $$ = Value(); // Return an empty value on error
        }
    }
    | CONSTANT              { $$ = $1; }
    | TRUE                  { $$ = $1; }
    | FALSE                 { $$ = $1; }
    | '(' expression ')'    { $$ = $2; }
    ; 

%%

void yyerror(const char *s) {
    fprintf(stderr, "[ERROR] %s\n", s);
    isError = true;
}

// Function to merge parameters from parameter_list
/* Value merge(Value param1, Value param2) {
    // Simple implementation - in a real compiler this would 
    // properly merge parameter information
    return param2; // Just return the second parameter for now
} */

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
    } else {
        fprintf(stderr, "No input file provided.\n");
        return 1;
    }
    yyparse();

    if (!isError) {
        printf("Parsing completed successfully.\n");
        if (symbolTable->getParent() == NULL) {
            symbolTable->printTableToFile();
            // print quadruples
            quadruples->printQuadruplesToFile("quadruples.txt");
        }
        return 0;
    } else {
        printf("Parsing failed.\n");
        return 1;
       }

}
