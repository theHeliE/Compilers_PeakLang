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
#include <stdbool.h>

using namespace std;

extern FILE *yyin;
int yylex(void);
void yyerror(const char *s);
// void ErrorToFile(string);
int sym[26];

SymbolTable *symbolTable = new SymbolTable();
int isError = false;
int isFuncCall = false;
int isLoop = false;


// Helper function to resolve an operand to an integer value
// Returns a pair: {value, success_flag}
std::pair<int, bool> resolve_operand_to_int(const Value& operand, SymbolTable* table) {
    // Check if it's an identifier that needs lookup
    if (operand.stringVal != nullptr && 
        (operand.type != INT_TYPE && operand.type != FLOAT_TYPE && 
         operand.type != BOOL_TYPE && operand.type != CHAR_TYPE && 
         operand.type != STRING_TYPE /* Added STRING_TYPE to avoid trying to resolve it as a number directly */)) {
        std::string varName(operand.stringVal);
        SingleEntry* entry = table->get_SingleEntry(varName);
        if (entry && entry->value) { // Check entry and value exist
            if (entry->type == INT_TYPE) {
                return {*(static_cast<int*>(entry->value)), true};
            } else if (entry->type == FLOAT_TYPE) {
                return {(int)(*(static_cast<float*>(entry->value))), true}; // Truncate float
            } else if (entry->type == CHAR_TYPE) {
                return {(int)(*(static_cast<char*>(entry->value))), true}; // ASCII value
            }
            else if (entry->type == BOOL_TYPE) {
                return {*(static_cast<bool*>(entry->value)) ? 1 : 0, true}; // Convert bool to int
            }
            return {0, false}; // Identifier found but not a convertible type
        }
        return {0, false}; // Resolution failed (not found or no value)
    } else if (operand.type == INT_TYPE) {
        return {operand.intVal, true};
    } else if (operand.type == FLOAT_TYPE) {
        return {(int)operand.floatVal, true}; // Truncate float
    } else if (operand.type == CHAR_TYPE) {
        return {(int)operand.charVal, true}; // ASCII value
    } else if (operand.type == BOOL_TYPE) {
        return {operand.boolVal ? 1 : 0, true}; // Convert bool to int
    } else if (operand.type == UNKNOWN_TYPE && operand.intVal != 0 && operand.stringVal == nullptr) { 
        // This fallback is currently returning false as per existing code.
        // If enabled, it would treat UNKNOWN_TYPE with intVal as an int.
        // return {operand.intVal, true}; 
        return {0, false}; 
    }
    return {0, false}; // Not an int, recognized identifier pattern, or directly convertible literal
}

// NEW: Helper function to resolve an operand to a float value
// Returns a pair: {value, success_flag}
std::pair<float, bool> resolve_operand_to_float(const Value& operand, SymbolTable* table) {
    // Check if it's an identifier that needs lookup

    if (operand.stringVal != nullptr && 
        (operand.type != INT_TYPE && operand.type != FLOAT_TYPE && 
         operand.type != BOOL_TYPE && operand.type != CHAR_TYPE && 
         operand.type != STRING_TYPE)) {
        std::string varName(operand.stringVal);
        SingleEntry* entry = table->get_SingleEntry(varName);
        if (entry && entry->value) { // Check entry and value exist
            if (entry->type == FLOAT_TYPE) {
                return {*(static_cast<float*>(entry->value)), true};
            } else if (entry->type == INT_TYPE) {
                return {(float)(*(static_cast<int*>(entry->value))), true}; // Promote int
            } else if (entry->type == CHAR_TYPE) {
                return {(float)(*(static_cast<char*>(entry->value))), true}; // Promote char (ASCII value to float)
            }
            else if (entry->type == BOOL_TYPE) {
                return {*(static_cast<bool*>(entry->value)) ? 1.0f : 0.0f, true}; // Convert bool to float
            }
            return {0.0f, false}; // Identifier found but not a convertible type
        }
        return {0.0f, false}; // Resolution failed (not found or no value)
    } else if (operand.type == FLOAT_TYPE) {
        return {operand.floatVal, true};
    } else if (operand.type == INT_TYPE) {
        return {(float)operand.intVal, true}; // Promote int
    } else if (operand.type == CHAR_TYPE) {
        return {(float)operand.charVal, true}; // Promote char (ASCII value to float)
    }
    else if (operand.type == BOOL_TYPE) {
        return {operand.boolVal ? 1.0f : 0.0f, true}; // Convert bool to float
    }
     else if (operand.type == UNKNOWN_TYPE && operand.stringVal == nullptr) {
        // Fallback for UNKNOWN_TYPE, attempting to use floatVal or intVal
        // This depends on how the lexer populates Value for unknown numeric constants
        if (operand.floatVal != 0.0f) { // Check floatVal first
            // return {operand.floatVal, true};
            return {0.0f, false}; // Disabled for consistency with resolve_operand_to_int's UNKNOWN handling
        } else if (operand.intVal != 0) { // Then check intVal and promote
            // return {(float)operand.intVal, true};
            return {0.0f, false}; // Disabled
        }
        return {0.0f, false};
    }
    return {0.0f, false}; // Not a float, recognized identifier pattern, or directly convertible literal
}

std::pair<char, bool> resolve_operand_to_char(const Value& operand, SymbolTable* table) {
    // Check if it's an identifier that needs lookup
    if (operand.stringVal != nullptr && 
        (operand.type != INT_TYPE && operand.type != FLOAT_TYPE && 
         operand.type != BOOL_TYPE && operand.type != CHAR_TYPE && 
         operand.type != STRING_TYPE)) {
        std::string varName(operand.stringVal);
        SingleEntry* entry = table->get_SingleEntry(varName);
        if (entry && entry->value) { // Check entry and value exist
            if (entry->type == CHAR_TYPE) {
                return {*(static_cast<char*>(entry->value)), true};
            } else if (entry->type == INT_TYPE) {
                // Assuming int value represents an ASCII code or similar small integer
                return {(char)(*(static_cast<int*>(entry->value))), true}; 
            }
            // Add other potential promotions to char if needed, e.g., from a small float? Unlikely.
            return {'\0', false}; // Identifier found but not a convertible type
        }
        return {'\0', false}; // Resolution failed (not found or no value)
    } else if (operand.type == CHAR_TYPE) {
        return {operand.charVal, true};
    } else if (operand.type == INT_TYPE) {
        // Direct int literal, cast to char
        return {(char)operand.intVal, true};
    }
    // No UNKNOWN_TYPE fallback for char for now, as it's less clear how that would be represented
    // unless operand.charVal was explicitly set by the lexer for an UNKNOWN_TYPE.
    return {'\0', false}; // Not a char, recognized identifier pattern, or directly convertible literal
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
vector<Quadruples*> quadrupleStack;
 
 // Pointer to current quadruple
Quadruples * currentQuadruple = quadruples;


Value* getLabel(){
    printf("New Label\n");

    char temp[20];
    sprintf(temp, "L%d", currentQuadruple->labelCounter++);

    Value * result = (Value*)malloc(sizeof(Value));
    result->type = STRING_TYPE;

    char* tempStr = strdup(temp);
    result->stringVal = tempStr;
     
     printf("Label: created");

    return result;
}

Value* handleExpression(const char* op, Value op1, Value op2) {
        printf("Expression with %s operator\n", op);

        // Generate result in a temporary variable
        char temp[20];
        snprintf(temp, sizeof(temp), "t%d", quadruples->resultCounter++);

        // Add quadruple with the temporary variable as result
        currentQuadruple->addQuadruple(op, valueToString(op1), valueToString(op2), temp);

        // Create new TypeAndValue for the temporary result
        Value* result = (Value*)malloc(sizeof(Value));
        result->type = STRING_TYPE;
        char* tempStr = strdup(temp);
        result->stringVal = tempStr;

        return result;

}


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

    void* ptr;
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
%type <val> expression_statement
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
%type <val> loop_statement
%type <val> selection_statement
%type <ptr> LEAVE_SCOPE
%type <ptr> compound_statement



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
    | compound_statement
    {
        printf("Expression statement\n");
        Quadruples * ScopeQuadruples = (Quadruples*)$1;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);
    }
    ;


/////////////////////////////////////// VARIABLE DEFINITION ///////////////////////////////////////
variable_definition:
    
    // eg. int a, b, c;
    type_specifier identifier_list ';'
    {
        printf("variable_declaration\n");
        symbolTable->insert($2, $1, NULL);

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName($2), "", "");


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

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName($2), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString($4), "", getValueName($2));
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

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName($3), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString($5), "", getValueName($3));

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
    : '{' ENTER_SCOPE scope LEAVE_SCOPE'}' {
        printf("Compound statement\n");

        $$ = $4;
    }
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
    | compound_statement { 
        printf("Compound statement\n");
        Quadruples * ScopeQuadruples = (Quadruples*)$1;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);
    }
    | selection_statement
    | loop_statement
    | jump_statement
    ;


/////////////////////////////////////// CONDITIONS ///////////////////////////////////////

selection_statement:
    
    // eg. if (x == 5) printf("x is 5");
 IF '(' expression ')' compound_statement %prec LOWER_THAN_ELSE
    {
        // $3 is the expression (condition)
        // $5 is the statement to execute if the condition is true

        Value* end_if_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString($3), valueToString(*end_if_label), "");
        printf("Debug: Adding JF Quadruple for IF statement. Condition: %s, Target Label: %s\n", valueToString($3).c_str(), valueToString(*end_if_label).c_str());

        Quadruples * ScopeQuadruples = (Quadruples*)$5;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        // The quadruples for the 'statement' ($5) are generated when $5 is reduced.

        // Quadruple: Define the label that marks the end of the IF block
        currentQuadruple->addQuadruple("LABEL", valueToString(*end_if_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*end_if_label).c_str());

        // The IF statement itself doesn't produce a value in this context,
        // but you might need to assign to $$ if selection_statement is expected to have one.
        // $$ = Value(); // Default value, or specific if needed.
        printf("Selection statement (IF) processed\n");
        free(end_if_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(end_if_label);
    }

    // #####################################

    // eg. if (x == 5) printf("x is 5"); else printf("x is not 5");
    | IF '(' expression ')' compound_statement ELSE compound_statement{
        printf("Selection statement with ELSE\n");

         Value* else_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString($3), valueToString(*else_label), "");
        printf("Debug: Adding JF Quadruple to else statement. Condition: %s, Target Label: %s\n", valueToString($3).c_str(), valueToString(*else_label).c_str());

        // The currentQuadruple for the 'statement' ($5) are generated when $5 is reduced.

        Quadruples * ScopeQuadruples = (Quadruples*)$5;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        // Quadruple: Define the label that marks the end of the IF block
        currentQuadruple->addQuadruple("LABEL", valueToString(*else_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*else_label).c_str());

        // Jump to end of if block after executing the else statement
        Value* end_if_label = getLabel(); // Label to jump to after the else statement

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*end_if_label), "");
        printf("Debug: Adding GOTO Quadruple to end of IF block. Target Label: %s\n", valueToString(*end_if_label).c_str());

        Quadruples * elseScopeQuadruples = (Quadruples*)$7;
        currentQuadruple = elseScopeQuadruples->merge(currentQuadruple);
        // Quadruple: Define the label that marks the end of the IF block
        currentQuadruple->addQuadruple("LABEL", valueToString(*end_if_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*end_if_label).c_str());

        // The IF statement itself doesn't produce a value in this context,
        // but you might need to assign to $$ if selection_statement is expected to have one.
        // $$ = Value(); // Default value, or specific if needed.
        printf("Selection statement (IF) processed\n");
        free(end_if_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(end_if_label);
        free(else_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(else_label);
    }

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
     WHILE '(' ENTER_SCOPE expression LEAVE_SCOPE ')' compound_statement
     {
        printf("WHILE LOOP\n");

        // label el barg3lo kol marra
        Value* top_while_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*top_while_label), "", "");

        Value* end_while_label = getLabel();


        Quadruples * ScopeQuadruples = (Quadruples*)$5;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("JF", valueToString($4), valueToString(*end_while_label), "");

        Quadruples * statementQuadruples = (Quadruples*)$7;
        currentQuadruple = statementQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*top_while_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*end_while_label), "", "");

     }

    // #####################################

    // eg. do {x++;} while (x < 10);
    | DO compound_statement WHILE '(' ENTER_SCOPE expression LEAVE_SCOPE ')' ';'
    {
        printf("DO WHILE LOOP\n");

        Value* top_while_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*top_while_label), "", "");

        Value* end_while_label = getLabel();

        Quadruples * statementQuadruples = (Quadruples*)$2;
        currentQuadruple = statementQuadruples->merge(currentQuadruple);


        Quadruples * ScopeQuadruples = (Quadruples*)$7;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("JF", valueToString($6), valueToString(*end_while_label), "");

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*top_while_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*end_while_label), "", "");
        

    }

    // #####################################

    // eg. for (i = 0; i < 10; i++) printf("i is %d", i);
    /* | FOR { symbolTable= new SymbolTable(symbolTable);} '(' expression_statement expression_statement expression ')' statement */
    | FOR '(' variable_definition expression_statement ENTER_SCOPE expression LEAVE_SCOPE ')' compound_statement
    {
        // LABEL
        // JF
        // ADD

        // ASSIGN
        // ADD
        // ASSIGN
        // JMP
        // LABEL

        printf("FOR LOOP\n");


        // label el barg3lo kol marra
        Value* JMP_LOOP_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*JMP_LOOP_label), "", "");



        // label el hakhrog mn el loop
        Value* JF_label = getLabel();
        currentQuadruple->addQuadruple("JF", valueToString($4), valueToString(*JF_label), "");

        // EXPR
        Quadruples * LEAVESCOPEQuadruples = (Quadruples*)$7;
        currentQuadruple = LEAVESCOPEQuadruples->merge(currentQuadruple);
        

        // momken yb2a feh merge tany
        Quadruples * ScopeQuadruples = (Quadruples*)$9;
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);


        // return back to loop top
        currentQuadruple->addQuadruple("GOTO", "", valueToString(*JMP_LOOP_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*JF_label), "", "");


        free(JF_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(JF_label);
        free(JMP_LOOP_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(JMP_LOOP_label);



    }
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
        printf("Debug: Assigning value to variable %s\n", getValueName($1).c_str());
        int resultCounter = currentQuadruple->resultCounter;
        string result = "t" + to_string(resultCounter);

        currentQuadruple->addQuadruple("ASSIGN", result, "", getValueName($1));
    }
            


    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR_OP logical_and_expression {
         $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first || right_f.first;
            handleExpression("OR", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first || right_i.first;
                handleExpression("OR", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '||' comparison. Operands not comparable as float or int.");
                $$.boolVal = false; 
            }
        }
    }
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression AND_OP inclusive_or_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first && right_f.first;
            handleExpression("AND", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first && right_i.first;
                handleExpression("AND", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '&&' comparison. Operands not comparable as float or int.");
                $$.boolVal = false; 
            }
        }
    }
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

        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first == right_f.first;
            handleExpression("EQ", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first == right_i.first;
                handleExpression("EQ", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '==' comparison. Operands not comparable as float or int.");
                $$.boolVal = false; 
            }
        }
    }
    | equality_expression NE_OP relational_expression
    {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first != right_f.first;
            handleExpression("NE", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first != right_i.first;
                handleExpression("NE", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '!=' comparison. Operands not comparable as float or int.");
                $$.boolVal = false; 
            }
        }
    }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression L_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first < right_f.first;
            handleExpression("LT", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first < right_i.first;
                handleExpression("LT", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '<' comparison. Operands not comparable as float or int.");
                $$.boolVal = false; 
            }
        }
    }
    | relational_expression G_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f > %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first > right_f.first;
            handleExpression("GT", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d > %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first > right_i.first;
                handleExpression("GT", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '>' comparison. Operands not comparable as float or int.");
                $$.boolVal = false;
            }
        }
    }
    | relational_expression LE_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f <= %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first <= right_f.first;
            handleExpression("LE", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d <= %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first <= right_i.first;
                handleExpression("LE", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '<=' comparison. Operands not comparable as float or int.");
                $$.boolVal = false;
            }
        }
    }
    | relational_expression GE_OP shift_expression {
        $$ = Value();
        $$.type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f >= %f\n", left_f.first, right_f.first);
            $$.boolVal = left_f.first >= right_f.first;
            handleExpression("GE", $1, $3);
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.boolVal = left_i.first >= right_i.first;
                handleExpression("GE", $1, $3);
            } else {
                yyerror("Type error or unresolved identifier in '>=' comparison. Operands not comparable as float or int.");
                $$.boolVal = false;
            }
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
    {
       $$ = Value();
        
        // check if type is int
        if ($1.type == INT_TYPE && $3.type == INT_TYPE) {
            $$.type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.intVal = left_i.first + right_i.first;
                $$.type = INT_TYPE;
                handleExpression("ADD", $1, $3);
                printf("Debug: Adding (int) %d + %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", $$.intVal);
            } else {
                yyerror("Type error or unresolved '+' operator. Operands not comparable as float or int.");
                $$.intVal = 0;
                $$.floatVal = 0.0;
                $$.type = UNKNOWN_TYPE;
            }
        }
        else if ($1.type == FLOAT_TYPE && $3.type == FLOAT_TYPE) {
            $$.type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

            $$.floatVal = left_f.first + right_f.first;
            $$.type = FLOAT_TYPE;
            handleExpression("ADD", $1, $3);
            printf("Debug: Adding (float) %f + %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", $$.floatVal);

        }
        
    }
    | additive_expression '-' multiplicative_expression
    {
        $$ = Value();
        
        // check if type is int
        if ($1.type == INT_TYPE && $3.type == INT_TYPE) {
            $$.type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.intVal = left_i.first - right_i.first;
                $$.type = INT_TYPE;
                handleExpression("SUB", $1, $3);
                printf("Debug: Adding (int) %d - %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", $$.intVal);
            } else {
                yyerror("Type error or unresolved '-' operator. Operands not comparable as float or int.");
                $$.intVal = 0;
                $$.floatVal = 0.0;
                $$.type = UNKNOWN_TYPE;
            }
        }
        else if ($1.type == FLOAT_TYPE && $3.type == FLOAT_TYPE) {
            $$.type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

            $$.floatVal = left_f.first - right_f.first;
            $$.type = FLOAT_TYPE;
            handleExpression("SUB", $1, $3);
            printf("Debug: Adding (float) %f - %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", $$.floatVal);

        }
    }
    ;

multiplicative_expression
    : unary_expression
    | multiplicative_expression '*' unary_expression
    {
        $$ = Value();
        
        // check if type is int
        if ($1.type == INT_TYPE && $3.type == INT_TYPE) {
            $$.type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.intVal = left_i.first * right_i.first;
                $$.type = INT_TYPE;
                handleExpression("MUL", $1, $3);
                printf("Debug: Adding (int) %d + %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", $$.intVal);
            } else {
                yyerror("Type error or unresolved '*' operator. Operands not comparable as float or int.");
                $$.intVal = 0;
                $$.floatVal = 0.0;
                $$.type = UNKNOWN_TYPE;
            }
        }
        else if ($1.type == FLOAT_TYPE && $3.type == FLOAT_TYPE) {
            $$.type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

            $$.floatVal = left_f.first * right_f.first;
            $$.type = FLOAT_TYPE;
            handleExpression("MUL", $1, $3);
            printf("Debug: Adding (float) %f * %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", $$.floatVal);

        }
    }
    | multiplicative_expression '/' unary_expression
    {
        $$ = Value();
        
        // check if type is int
        if ($1.type == INT_TYPE && $3.type == INT_TYPE) {
            $$.type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.intVal = left_i.first / right_i.first;
                $$.type = INT_TYPE;
                handleExpression("DIV", $1, $3);
                printf("Debug: Adding (int) %d / %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", $$.intVal);
            } else {
                yyerror("Type error or unresolved '/' operator. Operands not comparable as float or int.");
                $$.intVal = 0;
                $$.floatVal = 0.0;
                $$.type = UNKNOWN_TYPE;
            }
        }
        else if ($1.type == FLOAT_TYPE && $3.type == FLOAT_TYPE) {
            $$.type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float($1, symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float($3, symbolTable);

            $$.floatVal = left_f.first / right_f.first;
            $$.type = FLOAT_TYPE;
            handleExpression("DIV", $1, $3);
            printf("Debug: Adding (float) %f / %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", $$.floatVal);

        }
    }
    | multiplicative_expression '%' unary_expression
    {
        $$ = Value();
        
        // check if type is int
        if ($1.type == INT_TYPE && $3.type == INT_TYPE) {
            $$.type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int($1, symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int($3, symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                $$.intVal = left_i.first % right_i.first;
                $$.type = INT_TYPE;
                handleExpression("MOD", $1, $3);
                printf("Debug: Adding (int) %d % %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", $$.intVal);
            } else {
                yyerror("Type error or unresolved '%' operator. Operands not comparable as float or int.");
                $$.intVal = 0;
                $$.floatVal = 0.0;
                $$.type = UNKNOWN_TYPE;
            }
        }
        else if ($1.type == FLOAT_TYPE && $3.type == FLOAT_TYPE) {
            yyerror("DID YOU TAKE MATH COURSES???? da modulus w float ya 3m");

        }
    }
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

 // Scopeeeeeeeeeeeeeeeees
 ENTER_SCOPE :
    {
        printf("Entering new scope\n");

        // check if The scope is for a function call or a loop
        if (!isFuncCall && !isLoop){
            // create a new symbol table
            symbolTable = new SymbolTable(symbolTable);

        }
        else if (isFuncCall){

        isFuncCall = false;

        }
        else if (isLoop){

        isLoop = false;

        }

        quadrupleStack.push_back(currentQuadruple);
        
        // add new Quadruple 
        Quadruples * newQuadruple = new Quadruples();
        currentQuadruple = newQuadruple;
    }
    ;

  LEAVE_SCOPE :  
  {
        printf("Leaving scope\n");
        if (symbolTable){
            // remove the last symbol table
            symbolTable->printTableToFile();
            symbolTable = symbolTable->getParent();
        }
        else{
            printf("Error: No symbol table to remove\n");
        }

        $$ = currentQuadruple;
        // remove the last Quadruple
        currentQuadruple = quadrupleStack.back();
        quadrupleStack.pop_back();
  }

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
            currentQuadruple->printQuadruplesToFile("quadruples.txt");
        }
        return 0;
    } else {
        printf("Parsing failed.\n");
        return 1;
       }

}
