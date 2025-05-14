/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 12 "parser.y"

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
std:: vector<std::string> functionsOrder;
int isError = false;
bool isFuncCall = false;
bool isLoop = false;



vector<Quadruples*> quadruplesListForSwitch;


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



#line 332 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_CONSTANT = 4,                   /* CONSTANT  */
  YYSYMBOL_INT = 5,                        /* INT  */
  YYSYMBOL_FLOAT = 6,                      /* FLOAT  */
  YYSYMBOL_CHAR = 7,                       /* CHAR  */
  YYSYMBOL_VOID = 8,                       /* VOID  */
  YYSYMBOL_BOOL = 9,                       /* BOOL  */
  YYSYMBOL_TRUE = 10,                      /* TRUE  */
  YYSYMBOL_FALSE = 11,                     /* FALSE  */
  YYSYMBOL_CASE = 12,                      /* CASE  */
  YYSYMBOL_DEFAULT = 13,                   /* DEFAULT  */
  YYSYMBOL_IF = 14,                        /* IF  */
  YYSYMBOL_ELSE = 15,                      /* ELSE  */
  YYSYMBOL_SWITCH = 16,                    /* SWITCH  */
  YYSYMBOL_WHILE = 17,                     /* WHILE  */
  YYSYMBOL_DO = 18,                        /* DO  */
  YYSYMBOL_FOR = 19,                       /* FOR  */
  YYSYMBOL_GOTO = 20,                      /* GOTO  */
  YYSYMBOL_CONTINUE = 21,                  /* CONTINUE  */
  YYSYMBOL_BREAK = 22,                     /* BREAK  */
  YYSYMBOL_RETURN = 23,                    /* RETURN  */
  YYSYMBOL_THABET = 24,                    /* THABET  */
  YYSYMBOL_ASSIGNMENT = 25,                /* ASSIGNMENT  */
  YYSYMBOL_AND_OP = 26,                    /* AND_OP  */
  YYSYMBOL_OR_OP = 27,                     /* OR_OP  */
  YYSYMBOL_EQ_OP = 28,                     /* EQ_OP  */
  YYSYMBOL_NE_OP = 29,                     /* NE_OP  */
  YYSYMBOL_LE_OP = 30,                     /* LE_OP  */
  YYSYMBOL_GE_OP = 31,                     /* GE_OP  */
  YYSYMBOL_LEFT_OP = 32,                   /* LEFT_OP  */
  YYSYMBOL_RIGHT_OP = 33,                  /* RIGHT_OP  */
  YYSYMBOL_L_OP = 34,                      /* L_OP  */
  YYSYMBOL_G_OP = 35,                      /* G_OP  */
  YYSYMBOL_36_ = 36,                       /* '?'  */
  YYSYMBOL_37_ = 37,                       /* ':'  */
  YYSYMBOL_38_ = 38,                       /* '|'  */
  YYSYMBOL_39_ = 39,                       /* '^'  */
  YYSYMBOL_40_ = 40,                       /* '&'  */
  YYSYMBOL_UMINUS = 41,                    /* UMINUS  */
  YYSYMBOL_UPLUS = 42,                     /* UPLUS  */
  YYSYMBOL_43_ = 43,                       /* '!'  */
  YYSYMBOL_44_ = 44,                       /* '~'  */
  YYSYMBOL_LOWER_THAN_ELSE = 45,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_46_ = 46,                       /* '='  */
  YYSYMBOL_47_ = 47,                       /* '+'  */
  YYSYMBOL_48_ = 48,                       /* '-'  */
  YYSYMBOL_49_ = 49,                       /* '*'  */
  YYSYMBOL_50_ = 50,                       /* '/'  */
  YYSYMBOL_51_ = 51,                       /* '%'  */
  YYSYMBOL_52_ = 52,                       /* ';'  */
  YYSYMBOL_53_ = 53,                       /* ','  */
  YYSYMBOL_54_ = 54,                       /* '('  */
  YYSYMBOL_55_ = 55,                       /* ')'  */
  YYSYMBOL_56_ = 56,                       /* '{'  */
  YYSYMBOL_57_ = 57,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 58,                  /* $accept  */
  YYSYMBOL_mainProgram = 59,               /* mainProgram  */
  YYSYMBOL_program = 60,                   /* program  */
  YYSYMBOL_external_declaration = 61,      /* external_declaration  */
  YYSYMBOL_variable_definition = 62,       /* variable_definition  */
  YYSYMBOL_identifier_list = 63,           /* identifier_list  */
  YYSYMBOL_type_specifier = 64,            /* type_specifier  */
  YYSYMBOL_expression = 65,                /* expression  */
  YYSYMBOL_function_definition = 66,       /* function_definition  */
  YYSYMBOL_67_1 = 67,                      /* $@1  */
  YYSYMBOL_parameters = 68,                /* parameters  */
  YYSYMBOL_parameter_list = 69,            /* parameter_list  */
  YYSYMBOL_parameter_declaration = 70,     /* parameter_declaration  */
  YYSYMBOL_compound_statement = 71,        /* compound_statement  */
  YYSYMBOL_scope = 72,                     /* scope  */
  YYSYMBOL_scope_item = 73,                /* scope_item  */
  YYSYMBOL_expression_statement = 74,      /* expression_statement  */
  YYSYMBOL_statement = 75,                 /* statement  */
  YYSYMBOL_selection_statement = 76,       /* selection_statement  */
  YYSYMBOL_case_list = 77,                 /* case_list  */
  YYSYMBOL_case_item = 78,                 /* case_item  */
  YYSYMBOL_loop_statement = 79,            /* loop_statement  */
  YYSYMBOL_jump_statement = 80,            /* jump_statement  */
  YYSYMBOL_assignment_expression = 81,     /* assignment_expression  */
  YYSYMBOL_arguments = 82,                 /* arguments  */
  YYSYMBOL_logical_or_expression = 83,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 84,    /* logical_and_expression  */
  YYSYMBOL_inclusive_or_expression = 85,   /* inclusive_or_expression  */
  YYSYMBOL_exclusive_or_expression = 86,   /* exclusive_or_expression  */
  YYSYMBOL_and_expression = 87,            /* and_expression  */
  YYSYMBOL_equality_expression = 88,       /* equality_expression  */
  YYSYMBOL_relational_expression = 89,     /* relational_expression  */
  YYSYMBOL_shift_expression = 90,          /* shift_expression  */
  YYSYMBOL_additive_expression = 91,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 92, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 93,          /* unary_expression  */
  YYSYMBOL_primary_expression = 94,        /* primary_expression  */
  YYSYMBOL_ENTER_SCOPE = 95,               /* ENTER_SCOPE  */
  YYSYMBOL_ENTER_SCOPE_SEMANTIC = 96,      /* ENTER_SCOPE_SEMANTIC  */
  YYSYMBOL_LEAVE_SCOPE = 97,               /* LEAVE_SCOPE  */
  YYSYMBOL_LEAVE_SCOPE_SEMANTIC = 98       /* LEAVE_SCOPE_SEMANTIC  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  19
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   247

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  104
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  208

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   293


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,     2,     2,     2,    51,    40,     2,
      54,    55,    49,    47,    53,    48,     2,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    52,
       2,    46,     2,    36,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    39,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,    38,    57,    44,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    41,    42,    45
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   367,   367,   371,   378,   382,   394,   398,   402,   415,
     431,   457,   491,   499,   510,   512,   513,   514,   515,   519,
     522,   532,   532,   558,   597,   608,   614,   628,   650,   654,
     658,   659,   663,   664,   665,   671,   672,   680,   681,   686,
     687,   688,   697,   728,   773,   831,   837,   846,   853,   867,
     895,   924,   979,   980,   981,   985,  1002,  1020,  1021,  1044,
    1051,  1052,  1055,  1061,  1062,  1088,  1089,  1115,  1116,  1120,
    1121,  1125,  1126,  1130,  1131,  1155,  1182,  1183,  1206,  1229,
    1252,  1278,  1279,  1280,  1284,  1285,  1325,  1367,  1368,  1407,
    1446,  1487,  1488,  1493,  1496,  1500,  1508,  1550,  1551,  1552,
    1553,  1558,  1569,  1587,  1596
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "CONSTANT", "INT", "FLOAT", "CHAR", "VOID", "BOOL", "TRUE", "FALSE",
  "CASE", "DEFAULT", "IF", "ELSE", "SWITCH", "WHILE", "DO", "FOR", "GOTO",
  "CONTINUE", "BREAK", "RETURN", "THABET", "ASSIGNMENT", "AND_OP", "OR_OP",
  "EQ_OP", "NE_OP", "LE_OP", "GE_OP", "LEFT_OP", "RIGHT_OP", "L_OP",
  "G_OP", "'?'", "':'", "'|'", "'^'", "'&'", "UMINUS", "UPLUS", "'!'",
  "'~'", "LOWER_THAN_ELSE", "'='", "'+'", "'-'", "'*'", "'/'", "'%'",
  "';'", "','", "'('", "')'", "'{'", "'}'", "$accept", "mainProgram",
  "program", "external_declaration", "variable_definition",
  "identifier_list", "type_specifier", "expression", "function_definition",
  "$@1", "parameters", "parameter_list", "parameter_declaration",
  "compound_statement", "scope", "scope_item", "expression_statement",
  "statement", "selection_statement", "case_list", "case_item",
  "loop_statement", "jump_statement", "assignment_expression", "arguments",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "primary_expression", "ENTER_SCOPE", "ENTER_SCOPE_SEMANTIC",
  "LEAVE_SCOPE", "LEAVE_SCOPE_SEMANTIC", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-117)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      14,  -117,  -117,  -117,  -117,  -117,  -117,   166,   -31,    34,
     108,  -117,  -117,    40,  -117,  -117,    71,  -117,  -117,  -117,
    -117,    70,    -1,  -117,     3,    82,  -117,     6,  -117,    91,
       6,   -13,  -117,  -117,  -117,    77,    79,    98,   100,   112,
     151,   116,   124,   138,   184,   184,   184,   184,  -117,     6,
    -117,    -5,  -117,  -117,    82,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,   162,   165,   155,   169,   158,    11,     1,    30,
      35,    94,  -117,  -117,   166,    75,  -117,   107,     6,  -117,
       6,  -117,  -117,   187,   113,   149,  -117,  -117,  -117,   126,
    -117,  -117,  -117,  -117,  -117,    16,  -117,     6,  -117,  -117,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   202,  -117,
     156,  -117,  -117,  -117,  -117,    69,    42,     6,     6,   160,
     159,    71,  -117,  -117,  -117,  -117,   153,   165,   155,   169,
     158,    11,     1,     1,    30,    30,    30,    30,    35,    35,
      94,    94,  -117,  -117,  -117,  -117,  -117,   166,   163,  -117,
     100,   164,   164,  -117,  -117,  -117,   157,  -117,    69,   167,
     200,   168,   170,     6,     6,   100,  -117,  -117,   100,   173,
     100,   164,   164,  -117,  -117,  -117,  -117,   171,   175,   161,
     134,   172,   100,  -117,   134,   215,   183,  -117,  -117,  -117,
    -117,   196,   100,  -117,  -117,   100,  -117,  -117
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     3,    14,    15,    16,    17,    18,     0,   102,     0,
       2,     4,     7,     0,     6,     8,     0,    29,   101,     1,
       5,    12,     0,    12,     0,     0,   101,     0,     9,     0,
       0,    96,    97,    98,    99,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,     0,
      32,     0,    34,    38,   103,    30,    37,    33,    39,    40,
      41,    19,    57,    63,    65,    67,    69,    71,    73,    76,
      81,    84,    87,    91,    24,     0,    13,     0,     0,   101,
       0,   101,   101,     0,     0,     0,    53,    54,    55,     0,
      96,    94,    95,    93,    92,     0,    36,     0,    31,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   103,
      23,    25,    10,    11,    58,    62,     0,     0,     0,     0,
       0,     0,    52,    56,   100,    20,     0,    64,    66,    68,
      70,    72,    74,    75,    79,    80,    77,    78,    82,    83,
      85,    86,    88,    89,    90,    27,    21,     0,   103,    60,
       0,   103,   103,   101,   101,    28,     0,    26,     0,     0,
      42,     0,     0,     0,     0,     0,    61,    59,     0,     0,
       0,   103,   103,    22,    43,   101,    49,     0,     0,   101,
       0,     0,     0,    44,     0,     0,     0,   103,    50,    51,
     103,     0,     0,    45,    46,     0,    48,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -117,  -117,  -117,   211,   -17,   218,    -6,   -16,     5,  -117,
    -117,  -117,    80,   -25,  -117,   181,   106,  -117,  -117,  -117,
      45,  -117,  -117,   -53,  -117,  -117,   140,   141,   139,   142,
     143,    78,     0,    85,    86,   -40,  -107,   -24,  -117,  -116,
    -117
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     9,    10,    11,    12,    22,    13,    51,    14,   166,
     119,   120,   121,    15,    54,    55,    56,    57,    58,   189,
     197,    59,    60,    61,   158,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    25,    18,    99,
     136
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      53,    16,    74,   156,    91,    92,    93,    94,    50,    31,
      32,    75,    78,    83,    77,     1,    33,    34,   159,     2,
       3,     4,     5,     6,    27,   124,    17,    89,    30,    53,
      52,   107,   108,    95,    19,   109,   110,    50,     7,   105,
     106,    79,   169,    21,   135,   171,   172,    96,    97,    44,
      45,    28,    29,    46,    47,   125,    29,   127,   128,    52,
      49,   176,   111,   112,   126,   187,   188,   130,   118,    97,
       8,   134,    90,    32,    23,   152,   153,   154,   131,    33,
      34,   203,   113,   114,   204,    31,    32,     2,     3,     4,
       5,     6,    33,    34,    76,    97,    35,   160,    36,    37,
      38,    39,    40,    41,    42,    43,     7,   144,   145,   146,
     147,   161,   162,     2,     3,     4,     5,     6,     2,     3,
       4,     5,     6,    49,    26,    44,    45,   122,    97,    46,
      47,    80,     7,    81,    48,   170,    49,     7,     8,   173,
     174,    31,    32,   115,   116,   117,   195,   196,    33,    34,
     183,   118,    82,   184,    85,   186,     8,   181,   182,   123,
      97,   190,    31,    32,     8,   194,    84,   199,    86,    33,
      34,     2,     3,     4,     5,     6,    87,   206,   133,    97,
     207,    44,    45,   142,   143,    46,    47,    90,    32,   100,
      88,   101,    49,   102,    33,    34,   148,   149,   104,   150,
     151,   132,    44,    45,   129,   155,    46,    47,   103,   157,
     165,    48,   175,    49,   163,   178,   168,    97,   193,   201,
     202,    20,   177,   179,   198,   180,   191,    44,    45,   185,
     192,    46,    47,   205,    24,    98,   164,   167,    49,   200,
     137,   139,   138,     0,     0,   140,     0,   141
};

static const yytype_int16 yycheck[] =
{
      25,     7,    26,   119,    44,    45,    46,    47,    25,     3,
       4,    27,    25,    38,    30,     1,    10,    11,   125,     5,
       6,     7,     8,     9,    25,    78,    57,    43,    25,    54,
      25,    30,    31,    49,     0,    34,    35,    54,    24,    28,
      29,    54,   158,     3,    97,   161,   162,    52,    53,    43,
      44,    52,    53,    47,    48,    79,    53,    81,    82,    54,
      54,   168,    32,    33,    80,   181,   182,    84,    74,    53,
      56,    55,     3,     4,     3,   115,   116,   117,    84,    10,
      11,   197,    47,    48,   200,     3,     4,     5,     6,     7,
       8,     9,    10,    11,     3,    53,    14,    55,    16,    17,
      18,    19,    20,    21,    22,    23,    24,   107,   108,   109,
     110,   127,   128,     5,     6,     7,     8,     9,     5,     6,
       7,     8,     9,    54,    54,    43,    44,    52,    53,    47,
      48,    54,    24,    54,    52,   160,    54,    24,    56,   163,
     164,     3,     4,    49,    50,    51,    12,    13,    10,    11,
     175,   157,    54,   178,     3,   180,    56,   173,   174,    52,
      53,   185,     3,     4,    56,   189,    54,   192,    52,    10,
      11,     5,     6,     7,     8,     9,    52,   202,    52,    53,
     205,    43,    44,   105,   106,    47,    48,     3,     4,    27,
      52,    26,    54,    38,    10,    11,   111,   112,    40,   113,
     114,    52,    43,    44,    17,     3,    47,    48,    39,    53,
      57,    52,    55,    54,    54,    15,    53,    53,    57,     4,
      37,    10,    55,    55,    52,    55,    55,    43,    44,    56,
      55,    47,    48,    37,    16,    54,   130,   157,    54,   194,
     100,   102,   101,    -1,    -1,   103,    -1,   104
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     5,     6,     7,     8,     9,    24,    56,    59,
      60,    61,    62,    64,    66,    71,    64,    57,    96,     0,
      61,     3,    63,     3,    63,    95,    54,    25,    52,    53,
      25,     3,     4,    10,    11,    14,    16,    17,    18,    19,
      20,    21,    22,    23,    43,    44,    47,    48,    52,    54,
      62,    65,    66,    71,    72,    73,    74,    75,    76,    79,
      80,    81,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    65,     3,    65,    25,    54,
      54,    54,    54,    71,    54,     3,    52,    52,    52,    65,
       3,    93,    93,    93,    93,    65,    52,    53,    73,    97,
      27,    26,    38,    39,    40,    28,    29,    30,    31,    34,
      35,    32,    33,    47,    48,    49,    50,    51,    64,    68,
      69,    70,    52,    52,    81,    95,    65,    95,    95,    17,
      62,    64,    52,    52,    55,    81,    98,    84,    85,    86,
      87,    88,    89,    89,    90,    90,    90,    90,    91,    91,
      92,    92,    93,    93,    93,     3,    97,    53,    82,    94,
      55,    65,    65,    54,    74,    57,    67,    70,    53,    97,
      71,    97,    97,    95,    95,    55,    94,    55,    15,    55,
      55,    65,    65,    71,    71,    56,    71,    97,    97,    77,
      95,    55,    55,    57,    95,    12,    13,    78,    52,    71,
      78,     4,    37,    97,    97,    37,    71,    71
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    58,    59,    59,    60,    60,    61,    61,    61,    62,
      62,    62,    63,    63,    64,    64,    64,    64,    64,    65,
      65,    67,    66,    68,    68,    69,    69,    70,    71,    71,
      72,    72,    73,    73,    73,    74,    74,    75,    75,    75,
      75,    75,    76,    76,    76,    77,    77,    78,    78,    79,
      79,    79,    80,    80,    80,    80,    80,    81,    81,    81,
      82,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    88,    88,    88,    89,    89,    89,    89,
      89,    90,    90,    90,    91,    91,    91,    92,    92,    92,
      92,    93,    93,    93,    93,    93,    94,    94,    94,    94,
      94,    95,    96,    97,    98
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     3,
       5,     6,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     0,     9,     1,     0,     1,     3,     2,     7,     2,
       1,     2,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     5,     7,     9,     3,     4,     4,     3,     7,
       9,     9,     3,     2,     2,     2,     3,     1,     3,     6,
       1,     3,     0,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     1,     2,     2,     2,     2,     1,     1,     1,     1,
       3,     0,     0,     0,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* mainProgram: program  */
#line 368 "parser.y"
    { 
        printf("Main program\n"); 
    }
#line 1573 "parser.tab.c"
    break;

  case 3: /* mainProgram: error  */
#line 372 "parser.y"
    { 
        // ErrorToFile("Syntax Error"); YYABORT; 
        printf("Feh errors ya man\n");  YYABORT; 
    }
#line 1582 "parser.tab.c"
    break;

  case 4: /* program: external_declaration  */
#line 379 "parser.y"
    {
        printf("Program\n");
    }
#line 1590 "parser.tab.c"
    break;

  case 5: /* program: program external_declaration  */
#line 383 "parser.y"
    {
        printf("Program with external declarations\n");
    }
#line 1598 "parser.tab.c"
    break;

  case 6: /* external_declaration: function_definition  */
#line 395 "parser.y"
    {
        printf("Function definition\n");
    }
#line 1606 "parser.tab.c"
    break;

  case 7: /* external_declaration: variable_definition  */
#line 399 "parser.y"
    {
        printf("Variable definition\n");
    }
#line 1614 "parser.tab.c"
    break;

  case 8: /* external_declaration: compound_statement  */
#line 403 "parser.y"
    {
        printf("Expression statement\n");
        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[0].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);
    }
#line 1624 "parser.tab.c"
    break;

  case 9: /* variable_definition: type_specifier identifier_list ';'  */
#line 416 "parser.y"
    {
        printf("variable_declaration\n");
        symbolTable->insert((yyvsp[-1].val), (yyvsp[-2].val), NULL);

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[-1].val)), "", "");


    }
#line 1638 "parser.tab.c"
    break;

  case 10: /* variable_definition: type_specifier identifier_list ASSIGNMENT expression ';'  */
#line 432 "parser.y"
    {
           if (getValueName((yyvsp[-4].val))!=enumToString((yyvsp[-1].val).type)) {
            yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
        }
        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[-1].val)); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert((yyvsp[-3].val), (yyvsp[-4].val), allocated_val_ptr); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName((yyvsp[-3].val)))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[-3].val))).c_str());
             symbolTable->insert((yyvsp[-3].val), (yyvsp[-4].val), nullptr); 
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[-3].val)), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString((yyvsp[-1].val)), "", getValueName((yyvsp[-3].val)));
    }
#line 1664 "parser.tab.c"
    break;

  case 11: /* variable_definition: THABET type_specifier identifier_list ASSIGNMENT expression ';'  */
#line 458 "parser.y"
    {
        printf("variable_declaration Rule 3\n");
            if (getValueName((yyvsp[-4].val))!=enumToString((yyvsp[-1].val).type)) {
                yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
            }

         // $2 is type_specifier (Value), $3 is identifier_list (Value), $5 is expression (Value)
        
        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[-1].val)); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert((yyvsp[-3].val), (yyvsp[-4].val), allocated_val_ptr,true); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName((yyvsp[-3].val)))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[-4].val))).c_str());
             symbolTable->insert((yyvsp[-4].val), (yyvsp[-5].val), nullptr); 
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[-3].val)), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString((yyvsp[-1].val)), "", getValueName((yyvsp[-3].val)));

    }
#line 1695 "parser.tab.c"
    break;

  case 12: /* identifier_list: IDENTIFIER  */
#line 492 "parser.y"
    {
        (yyval.val) = (yyvsp[0].val);
    }
#line 1703 "parser.tab.c"
    break;

  case 13: /* identifier_list: identifier_list ',' IDENTIFIER  */
#line 500 "parser.y"
    {
        // $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        // sprintf($$, "%s,%s", $1, $3);
    }
#line 1712 "parser.tab.c"
    break;

  case 14: /* type_specifier: INT  */
#line 510 "parser.y"
            { (yyval.val).stringVal = "INT"; 
    printf("INT\n");}
#line 1719 "parser.tab.c"
    break;

  case 15: /* type_specifier: FLOAT  */
#line 512 "parser.y"
              { (yyval.val).stringVal = "FLOAT"; }
#line 1725 "parser.tab.c"
    break;

  case 16: /* type_specifier: CHAR  */
#line 513 "parser.y"
              { (yyval.val).stringVal = "CHAR"; }
#line 1731 "parser.tab.c"
    break;

  case 17: /* type_specifier: VOID  */
#line 514 "parser.y"
              { (yyval.val).stringVal = "VOID";}
#line 1737 "parser.tab.c"
    break;

  case 18: /* type_specifier: BOOL  */
#line 515 "parser.y"
              { (yyval.val).stringVal = "BOOL"; }
#line 1743 "parser.tab.c"
    break;

  case 19: /* expression: assignment_expression  */
#line 519 "parser.y"
                            {
        printf("Assignment Expression\n");
    }
#line 1751 "parser.tab.c"
    break;

  case 21: /* $@1: %empty  */
#line 532 "parser.y"
                                                                     {
    symbolTable->getParent()->insert((yyvsp[-4].val), (yyvsp[-5].val), NULL, false, (yyvsp[-1].parameterList), (yyvsp[-5].val).type);
    functionsOrder.push_back((yyvsp[-4].val).stringVal) ;}
#line 1759 "parser.tab.c"
    break;

  case 22: /* function_definition: type_specifier IDENTIFIER '(' ENTER_SCOPE parameters LEAVE_SCOPE $@1 ')' compound_statement  */
#line 534 "parser.y"
                                                                    {
        printf("Function definition\n");
        // $1 is type_specifier (Value), $2 is IDENTIFIER (Value), $4 is parameter_list (Value), $6 is compound_statement (Quadruples)
        // Name of the function is in $2

        // remove function from latest insertion order
        functionsOrder.pop_back();

        currentQuadruple->addQuadruple("LABEL", (yyvsp[-7].val).stringVal, "", "");

        // merging parameters with the current quadruple
        Quadruples * parametersQuadruples = (Quadruples*)(yyvsp[-3].ptr);
        currentQuadruple = parametersQuadruples->reverseMerge(currentQuadruple);
        
        // merging the function body with the current quadruple
        Quadruples * functionBodyQuadruples = (Quadruples*)(yyvsp[0].ptr);
        currentQuadruple = functionBodyQuadruples->merge(currentQuadruple);
    }
#line 1782 "parser.tab.c"
    break;

  case 23: /* parameters: parameter_list  */
#line 559 "parser.y"
{
    printf("Parameters\n");
    (yyval.parameterList) = (yyvsp[0].parameterList);

    // create a symbol table
    symbolTable = new SymbolTable(symbolTable);
    isFuncCall = true;

    //Pushing parameters to the symbol table
    queue<std::pair<dataType,std::string>>paramList = *(yyval.parameterList);
     while (!paramList.empty()) 
        {
            pair<dataType, string> arg = paramList.front();
            paramList.pop();
            void* dummyValue;
            switch (arg.first) {
            case dataType::INT_TYPE:
                dummyValue = new int(0);
                break;
            case dataType::FLOAT_TYPE:
                dummyValue = new float(0.0);
                break;
            case dataType::BOOL_TYPE:
                dummyValue = new bool(false);
                break;
            case dataType::STRING_TYPE:
                dummyValue = strdup("");
                break;
            case dataType::CHAR_TYPE:
                dummyValue = new char('\0');
                break;
            default:
                dummyValue = NULL;
            }
            symbolTable->insert(arg.second, arg.first, dummyValue);
        }
}
#line 1824 "parser.tab.c"
    break;

  case 24: /* parameters: %empty  */
#line 597 "parser.y"
{
    printf("No parameters\n");
    (yyval.parameterList) = new std::queue<std::pair<dataType,std::string>> ();
    // create a symbol table
    symbolTable = new SymbolTable(symbolTable);
    isFuncCall = true;
}
#line 1836 "parser.tab.c"
    break;

  case 25: /* parameter_list: parameter_declaration  */
#line 608 "parser.y"
                                    {
        (yyval.parameterList) = new std::queue<std::pair<dataType,std::string>> ();
        (yyval.parameterList)->push(make_pair((yyvsp[0].parameterDeclaration)->first, (yyvsp[0].parameterDeclaration)->second));

        currentQuadruple->addQuadruple("POP", (yyvsp[0].parameterDeclaration)->second , "", "");
    }
#line 1847 "parser.tab.c"
    break;

  case 26: /* parameter_list: parameter_list ',' parameter_declaration  */
#line 614 "parser.y"
                                               {
        // Pushing paramter to the queue
        printf("Parameter list\n");
        (yyval.parameterList) = (yyvsp[-2].parameterList);
        (yyval.parameterList)->push(make_pair((yyvsp[0].parameterDeclaration)->first, (yyvsp[0].parameterDeclaration)->second));
        currentQuadruple->addQuadruple("POP", (yyvsp[0].parameterDeclaration)->second , "", "");

    }
#line 1860 "parser.tab.c"
    break;

  case 27: /* parameter_declaration: type_specifier IDENTIFIER  */
#line 628 "parser.y"
                                {
        // pushing paramter to the queue
        printf("Parameter declaration\n");
        dataType param_type = (yyvsp[-1].val).type; // Get the dataType from the type_specifier's Value
        std::string param_name;

        if ((yyvsp[0].val).stringVal) {
            param_name = std::string((yyvsp[0].val).stringVal); // Convert char* name to std::string
        } else {
            yyerror("Parameter identifier has no name.");
            param_name = "<unknown_param_name>";
        }

        printf("Parameter declaration: Name='%s', Type='%s'\n", param_name.c_str(), enumToString(param_type).c_str());

        // Create the pair on the heap with the correct types
        (yyval.parameterDeclaration) = new std::pair<dataType, std::string>(param_type, param_name);
    }
#line 1883 "parser.tab.c"
    break;

  case 28: /* compound_statement: '{' ENTER_SCOPE_SEMANTIC ENTER_SCOPE scope LEAVE_SCOPE LEAVE_SCOPE_SEMANTIC '}'  */
#line 650 "parser.y"
                                                                                      {
        printf("Compound statement\n");
        (yyval.ptr) = (yyvsp[-2].ptr);
    }
#line 1892 "parser.tab.c"
    break;

  case 38: /* statement: compound_statement  */
#line 681 "parser.y"
                         { 
        printf("Compound statement\n");
        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[0].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);
    }
#line 1902 "parser.tab.c"
    break;

  case 42: /* selection_statement: IF '(' expression ')' compound_statement  */
#line 698 "parser.y"
    {
        // $3 is the expression (condition)
        // $5 is the statement to execute if the condition is true

        Value* end_if_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[-2].val)), valueToString(*end_if_label), "");
        printf("Debug: Adding JF Quadruple for IF statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[-2].val)).c_str(), valueToString(*end_if_label).c_str());

        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[0].ptr);
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
#line 1933 "parser.tab.c"
    break;

  case 43: /* selection_statement: IF '(' expression ')' compound_statement ELSE compound_statement  */
#line 728 "parser.y"
                                                                      {
        printf("Selection statement with ELSE\n");

         Value* else_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[-4].val)), valueToString(*else_label), "");
        printf("Debug: Adding JF Quadruple to else statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[-4].val)).c_str(), valueToString(*else_label).c_str());

        // The currentQuadruple for the 'statement' ($5) are generated when $5 is reduced.

        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[-2].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        // Quadruple: Define the label that marks the end of the IF block
        currentQuadruple->addQuadruple("LABEL", valueToString(*else_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*else_label).c_str());

        // Jump to end of if block after executing the else statement
        Value* end_if_label = getLabel(); // Label to jump to after the else statement

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*end_if_label), "");
        printf("Debug: Adding GOTO Quadruple to end of IF block. Target Label: %s\n", valueToString(*end_if_label).c_str());

        Quadruples * elseScopeQuadruples = (Quadruples*)(yyvsp[0].ptr);
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
#line 1977 "parser.tab.c"
    break;

  case 44: /* selection_statement: SWITCH '(' ENTER_SCOPE expression LEAVE_SCOPE ')' '{' case_list '}'  */
#line 774 "parser.y"
    {
        printf("SWITCH STATEMENT\n");

        // check if the expression match the case

        /*
            switch(x)
            {
                case 1: {
                    x = 6;
                    break;
                }
                case 2: {
                    x = 7;
                    break;
                }
                
            }

            EQ x, 1
            JF x, label1


            EQ x, 2
            JF x, label2
            GOTO label3
            LABEL label1
            
        
        */

        Value* switch_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*switch_label), "", "");

        Value* end_switch_label = getLabel();
        
        // merge el expression
        Quadruples * exprQuadruples = (Quadruples*)(yyvsp[-4].ptr);
        currentQuadruple = exprQuadruples->merge(currentQuadruple);


    for (Quadruples* quad : quadruplesListForSwitch) {

        Value* caseValue = (Value*)(yyvsp[-1].ptr);
        Value* result = handleExpression("EQ", (yyvsp[-5].val), *caseValue);
        currentQuadruple->addQuadruple("JF", valueToString(*result), valueToString(*end_switch_label), "");
        currentQuadruple = quad->merge(currentQuadruple);
    }


        // if not, jump to the default case
        // if yes, jump to the case

    }
#line 2036 "parser.tab.c"
    break;

  case 45: /* case_list: ENTER_SCOPE case_item LEAVE_SCOPE  */
#line 832 "parser.y"
    {
        printf("CASE LIST\n");
        Quadruples* caseQuad = (Quadruples*)(yyvsp[0].ptr);
        (yyval.ptr) = caseQuad;
    }
#line 2046 "parser.tab.c"
    break;

  case 46: /* case_list: case_list ENTER_SCOPE case_item LEAVE_SCOPE  */
#line 838 "parser.y"
    {
        printf("CASE LIST\n");
        Quadruples* caseQuad = (Quadruples*)(yyvsp[0].ptr);
        (yyval.ptr) = caseQuad;
    }
#line 2056 "parser.tab.c"
    break;

  case 47: /* case_item: CASE CONSTANT ':' compound_statement  */
#line 847 "parser.y"
     {
        printf("CASE ITEM\n");
        Quadruples* caseQuad = (Quadruples*)(yyvsp[0].ptr);
        quadruplesListForSwitch.push_back(caseQuad);
        (yyval.val) = (yyvsp[-2].val);
     }
#line 2067 "parser.tab.c"
    break;

  case 48: /* case_item: DEFAULT ':' compound_statement  */
#line 854 "parser.y"
      {
        Quadruples* caseQuad = (Quadruples*)(yyvsp[0].ptr);
        quadruplesListForSwitch.push_back(caseQuad);
        printf("DEFAULT CASE ITEM\n");
    }
#line 2077 "parser.tab.c"
    break;

  case 49: /* loop_statement: WHILE '(' ENTER_SCOPE expression LEAVE_SCOPE ')' compound_statement  */
#line 868 "parser.y"
     {
        printf("WHILE LOOP\n");

        // label el barg3lo kol marra
        Value* top_while_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*top_while_label), "", "");

        Value* end_while_label = getLabel();


        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[-2].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[-3].val)), valueToString(*end_while_label), "");

        Quadruples * statementQuadruples = (Quadruples*)(yyvsp[0].ptr);
        currentQuadruple = statementQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*top_while_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*end_while_label), "", "");

     }
#line 2105 "parser.tab.c"
    break;

  case 50: /* loop_statement: DO compound_statement WHILE '(' ENTER_SCOPE expression LEAVE_SCOPE ')' ';'  */
#line 896 "parser.y"
    {
        printf("DO WHILE LOOP\n");

        Value* top_while_label = getLabel();
        currentQuadruple->addQuadruple("LABEL", valueToString(*top_while_label), "", "");

        Value* end_while_label = getLabel();

        Quadruples * statementQuadruples = (Quadruples*)(yyvsp[-7].ptr);
        currentQuadruple = statementQuadruples->merge(currentQuadruple);


        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[-2].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[-3].val)), valueToString(*end_while_label), "");

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*top_while_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*end_while_label), "", "");
        

    }
#line 2133 "parser.tab.c"
    break;

  case 51: /* loop_statement: FOR '(' variable_definition expression_statement ENTER_SCOPE expression LEAVE_SCOPE ')' compound_statement  */
#line 925 "parser.y"
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
        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[-5].val)), valueToString(*JF_label), "");

        // EXPR
        Quadruples * LEAVESCOPEQuadruples = (Quadruples*)(yyvsp[-2].ptr);
        currentQuadruple = LEAVESCOPEQuadruples->merge(currentQuadruple);
        

        // momken yb2a feh merge tany
        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[0].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);


        // return back to loop top
        currentQuadruple->addQuadruple("GOTO", "", valueToString(*JMP_LOOP_label), "");

        currentQuadruple->addQuadruple("LABEL", valueToString(*JF_label), "", "");


        free(JF_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(JF_label);
        free(JMP_LOOP_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(JMP_LOOP_label);



    }
#line 2186 "parser.tab.c"
    break;

  case 55: /* jump_statement: RETURN ';'  */
#line 986 "parser.y"
    {
        printf("Return statement\n");
        string functionName = functionsOrder.back();
        // get type of the function from SymbolTable
        SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        if (functionEntry->type != VOID_TYPE) {
            yyerror("Elet adab walahy :/, Type mismatch in return statement");
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("RET", "", "", "");

    }
#line 2204 "parser.tab.c"
    break;

  case 56: /* jump_statement: RETURN expression ';'  */
#line 1003 "parser.y"
    {
        printf("Return statement with expression\n");
        string functionName = functionsOrder.back();
        // get type of the function from SymbolTable
        SingleEntry * functionEntry = symbolTable->get_SingleEntry(functionName);
        if (functionEntry->type != (yyvsp[-1].val).type) {
            yyerror("Elet adab walahy :/, Type mismatch in return statement");
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("RET", valueToString((yyvsp[-1].val)), "", "");
    }
#line 2221 "parser.tab.c"
    break;

  case 58: /* assignment_expression: IDENTIFIER ASSIGNMENT assignment_expression  */
#line 1021 "parser.y"
                                                   {
        printf("Assignment expression\n");
        // $1 is IDENTIFIER, $3 is assignment_expression (Value)


        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[0].val));
        if (allocated_val_ptr != nullptr) {
            symbolTable->update_Value(getValueName((yyvsp[-2].val)), allocated_val_ptr , (yyvsp[0].val).type);

        // check if there is an error in the symbol table
        if (symbolTable->get_SingleEntry(getValueName((yyvsp[-2].val)))->isError) {
            yyerror("Matrakez ya 3aaam (pay attention noob)");
        }
        } else {
            //yyerror(("Failed to create value for assignment to variable " + getValueName($1)).c_str());
        }
        printf("Debug: Assigning value to variable %s\n", getValueName((yyvsp[-2].val)).c_str());
        int resultCounter = currentQuadruple->resultCounter;
        string result = "t" + to_string(resultCounter);

        currentQuadruple->addQuadruple("ASSIGN", result, "", getValueName((yyvsp[-2].val)));
    }
#line 2248 "parser.tab.c"
    break;

  case 59: /* assignment_expression: IDENTIFIER '(' ENTER_SCOPE arguments LEAVE_SCOPE ')'  */
#line 1044 "parser.y"
                                                          {
        printf("Function call assignment expression\n");
        // $1 is IDENTIFIER, $3 is arguments (Value)
    }
#line 2257 "parser.tab.c"
    break;

  case 61: /* arguments: arguments ',' primary_expression  */
#line 1052 "parser.y"
                                       {

    }
#line 2265 "parser.tab.c"
    break;

  case 62: /* arguments: %empty  */
#line 1055 "parser.y"
                  {

    }
#line 2273 "parser.tab.c"
    break;

  case 64: /* logical_or_expression: logical_or_expression OR_OP logical_and_expression  */
#line 1062 "parser.y"
                                                         {
         (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first || right_f.first;
            handleExpression("OR", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first || right_i.first;
                handleExpression("OR", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '||' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 2301 "parser.tab.c"
    break;

  case 66: /* logical_and_expression: logical_and_expression AND_OP inclusive_or_expression  */
#line 1089 "parser.y"
                                                            {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first && right_f.first;
            handleExpression("AND", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first && right_i.first;
                handleExpression("AND", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '&&' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 2329 "parser.tab.c"
    break;

  case 74: /* equality_expression: equality_expression EQ_OP relational_expression  */
#line 1131 "parser.y"
                                                     {

        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first == right_f.first;
            handleExpression("EQ", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first == right_i.first;
                handleExpression("EQ", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '==' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 2358 "parser.tab.c"
    break;

  case 75: /* equality_expression: equality_expression NE_OP relational_expression  */
#line 1156 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first != right_f.first;
            handleExpression("NE", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first != right_i.first;
                handleExpression("NE", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '!=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 2386 "parser.tab.c"
    break;

  case 76: /* relational_expression: shift_expression  */
#line 1182 "parser.y"
                       { (yyval.val) = (yyvsp[0].val); }
#line 2392 "parser.tab.c"
    break;

  case 77: /* relational_expression: relational_expression L_OP shift_expression  */
#line 1183 "parser.y"
                                                  {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first < right_f.first;
            handleExpression("LT", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first < right_i.first;
                handleExpression("LT", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '<' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 2420 "parser.tab.c"
    break;

  case 78: /* relational_expression: relational_expression G_OP shift_expression  */
#line 1206 "parser.y"
                                                  {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f > %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first > right_f.first;
            handleExpression("GT", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d > %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first > right_i.first;
                handleExpression("GT", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '>' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2448 "parser.tab.c"
    break;

  case 79: /* relational_expression: relational_expression LE_OP shift_expression  */
#line 1229 "parser.y"
                                                   {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f <= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first <= right_f.first;
            handleExpression("LE", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d <= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first <= right_i.first;
                handleExpression("LE", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '<=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2476 "parser.tab.c"
    break;

  case 80: /* relational_expression: relational_expression GE_OP shift_expression  */
#line 1252 "parser.y"
                                                   {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f >= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first >= right_f.first;
            handleExpression("GE", (yyvsp[-2].val), (yyvsp[0].val));
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first >= right_i.first;
                handleExpression("GE", (yyvsp[-2].val), (yyvsp[0].val));
            } else {
                yyerror("Type error or unresolved identifier in '>=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2504 "parser.tab.c"
    break;

  case 85: /* additive_expression: additive_expression '+' multiplicative_expression  */
#line 1286 "parser.y"
    {
       (yyval.val) = Value();
        
        // check if type is int
        if ((yyvsp[-2].val).type == INT_TYPE && (yyvsp[0].val).type == INT_TYPE) {
            (yyval.val).type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).intVal = left_i.first + right_i.first;
                (yyval.val).type = INT_TYPE;
                handleExpression("ADD", (yyvsp[-2].val), (yyvsp[0].val));
                printf("Debug: Adding (int) %d + %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", (yyval.val).intVal);
            } else {
                yyerror("Type error or unresolved '+' operator. Operands not comparable as float or int.");
                (yyval.val).intVal = 0;
                (yyval.val).floatVal = 0.0;
                (yyval.val).type = UNKNOWN_TYPE;
            }
        }
        else if ((yyvsp[-2].val).type == FLOAT_TYPE && (yyvsp[0].val).type == FLOAT_TYPE) {
            (yyval.val).type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

            (yyval.val).floatVal = left_f.first + right_f.first;
            (yyval.val).type = FLOAT_TYPE;
            handleExpression("ADD", (yyvsp[-2].val), (yyvsp[0].val));
            printf("Debug: Adding (float) %f + %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", (yyval.val).floatVal);

        }
        
    }
#line 2548 "parser.tab.c"
    break;

  case 86: /* additive_expression: additive_expression '-' multiplicative_expression  */
#line 1326 "parser.y"
    {
        (yyval.val) = Value();
        
        // check if type is int
        if ((yyvsp[-2].val).type == INT_TYPE && (yyvsp[0].val).type == INT_TYPE) {
            (yyval.val).type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).intVal = left_i.first - right_i.first;
                (yyval.val).type = INT_TYPE;
                handleExpression("SUB", (yyvsp[-2].val), (yyvsp[0].val));
                printf("Debug: Adding (int) %d - %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", (yyval.val).intVal);
            } else {
                yyerror("Type error or unresolved '-' operator. Operands not comparable as float or int.");
                (yyval.val).intVal = 0;
                (yyval.val).floatVal = 0.0;
                (yyval.val).type = UNKNOWN_TYPE;
            }
        }
        else if ((yyvsp[-2].val).type == FLOAT_TYPE && (yyvsp[0].val).type == FLOAT_TYPE) {
            (yyval.val).type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

            (yyval.val).floatVal = left_f.first - right_f.first;
            (yyval.val).type = FLOAT_TYPE;
            handleExpression("SUB", (yyvsp[-2].val), (yyvsp[0].val));
            printf("Debug: Adding (float) %f - %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", (yyval.val).floatVal);

        }
    }
#line 2591 "parser.tab.c"
    break;

  case 88: /* multiplicative_expression: multiplicative_expression '*' unary_expression  */
#line 1369 "parser.y"
    {
        (yyval.val) = Value();
        
        // check if type is int
        if ((yyvsp[-2].val).type == INT_TYPE && (yyvsp[0].val).type == INT_TYPE) {
            (yyval.val).type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).intVal = left_i.first * right_i.first;
                (yyval.val).type = INT_TYPE;
                handleExpression("MUL", (yyvsp[-2].val), (yyvsp[0].val));
                printf("Debug: Adding (int) %d + %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", (yyval.val).intVal);
            } else {
                yyerror("Type error or unresolved '*' operator. Operands not comparable as float or int.");
                (yyval.val).intVal = 0;
                (yyval.val).floatVal = 0.0;
                (yyval.val).type = UNKNOWN_TYPE;
            }
        }
        else if ((yyvsp[-2].val).type == FLOAT_TYPE && (yyvsp[0].val).type == FLOAT_TYPE) {
            (yyval.val).type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

            (yyval.val).floatVal = left_f.first * right_f.first;
            (yyval.val).type = FLOAT_TYPE;
            handleExpression("MUL", (yyvsp[-2].val), (yyvsp[0].val));
            printf("Debug: Adding (float) %f * %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", (yyval.val).floatVal);

        }
    }
#line 2634 "parser.tab.c"
    break;

  case 89: /* multiplicative_expression: multiplicative_expression '/' unary_expression  */
#line 1408 "parser.y"
    {
        (yyval.val) = Value();
        
        // check if type is int
        if ((yyvsp[-2].val).type == INT_TYPE && (yyvsp[0].val).type == INT_TYPE) {
            (yyval.val).type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).intVal = left_i.first / right_i.first;
                (yyval.val).type = INT_TYPE;
                handleExpression("DIV", (yyvsp[-2].val), (yyvsp[0].val));
                printf("Debug: Adding (int) %d / %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", (yyval.val).intVal);
            } else {
                yyerror("Type error or unresolved '/' operator. Operands not comparable as float or int.");
                (yyval.val).intVal = 0;
                (yyval.val).floatVal = 0.0;
                (yyval.val).type = UNKNOWN_TYPE;
            }
        }
        else if ((yyvsp[-2].val).type == FLOAT_TYPE && (yyvsp[0].val).type == FLOAT_TYPE) {
            (yyval.val).type = FLOAT_TYPE;
            // check if the operands are of the same type
            std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
            std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

            (yyval.val).floatVal = left_f.first / right_f.first;
            (yyval.val).type = FLOAT_TYPE;
            handleExpression("DIV", (yyvsp[-2].val), (yyvsp[0].val));
            printf("Debug: Adding (float) %f / %f\n", left_f.first, right_f.first);
            printf("Debug: Result (float) %f\n", (yyval.val).floatVal);

        }
    }
#line 2677 "parser.tab.c"
    break;

  case 90: /* multiplicative_expression: multiplicative_expression '%' unary_expression  */
#line 1447 "parser.y"
    {
        (yyval.val) = Value();
        
        // check if type is int
        if ((yyvsp[-2].val).type == INT_TYPE && (yyvsp[0].val).type == INT_TYPE) {
            (yyval.val).type = INT_TYPE;
            // check if the operands are of the same type
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);

            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).intVal = left_i.first % right_i.first;
                (yyval.val).type = INT_TYPE;
                handleExpression("MOD", (yyvsp[-2].val), (yyvsp[0].val));
                printf("Debug: Adding (int) %d % %d\n", left_i.first, right_i.first);
                printf("Debug: Result (int) %d\n", (yyval.val).intVal);
            } else {
                yyerror("Type error or unresolved '%' operator. Operands not comparable as float or int.");
                (yyval.val).intVal = 0;
                (yyval.val).floatVal = 0.0;
                (yyval.val).type = UNKNOWN_TYPE;
            }
        }
        else if ((yyvsp[-2].val).type == FLOAT_TYPE && (yyvsp[0].val).type == FLOAT_TYPE) {
            yyerror("DID YOU TAKE MATH COURSES???? da modulus w float ya 3m");

        }
    }
#line 2711 "parser.tab.c"
    break;

  case 91: /* unary_expression: primary_expression  */
#line 1487 "parser.y"
                                 { (yyval.val) = (yyvsp[0].val); }
#line 2717 "parser.tab.c"
    break;

  case 92: /* unary_expression: '-' unary_expression  */
#line 1488 "parser.y"
                                        {
          // Create a new Value and apply negation
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).intVal = -(yyval.val).intVal;
      }
#line 2727 "parser.tab.c"
    break;

  case 93: /* unary_expression: '+' unary_expression  */
#line 1493 "parser.y"
                                       {
          (yyval.val) = (yyvsp[0].val);
      }
#line 2735 "parser.tab.c"
    break;

  case 94: /* unary_expression: '!' unary_expression  */
#line 1496 "parser.y"
                           {
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).boolVal = !(yyval.val).boolVal;
      }
#line 2744 "parser.tab.c"
    break;

  case 95: /* unary_expression: '~' unary_expression  */
#line 1500 "parser.y"
                           {
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).intVal = ~(yyval.val).intVal;
      }
#line 2753 "parser.tab.c"
    break;

  case 96: /* primary_expression: IDENTIFIER  */
#line 1508 "parser.y"
                            { 
        printf("identifier: %s\n", getValueName((yyvsp[0].val)));
         (yyval.val) = Value();
        SingleEntry* entry = symbolTable->get_SingleEntry(getValueName((yyvsp[0].val)));
        if (entry) {
             (yyval.val).type = entry->type;
             // If the variable has an initialized value in the symbol table
            if (entry->value != nullptr) { // Assuming 'entry->value' is the void* to the actual data
                switch(entry->type) {
                    case INT_TYPE:
                        (yyval.val).intVal = *(static_cast<int*>(entry->value));
                        printf("Debug: Resolved identifier '%s' to %d\n", getValueName((yyvsp[0].val)), (yyval.val).intVal);
                        break;
                    case FLOAT_TYPE:
                        (yyval.val).floatVal = *(static_cast<float*>(entry->value));
                        break;
                    case BOOL_TYPE:
                        (yyval.val).boolVal = *(static_cast<bool*>(entry->value));
                        break;
                    case CHAR_TYPE:
                        (yyval.val).charVal = *(static_cast<char*>(entry->value));
                        break;
                    case STRING_TYPE:
                        // For STRING_TYPE, $$.stringVal should hold the content
                        // for valueToString and allocateValueFromExpression to work correctly.
                        if ((yyval.val).stringVal) free((yyval.val).stringVal); // Free if already allocated by Value() or previous use
                        (yyval.val).stringVal = strdup(static_cast<char*>(entry->value)); // Copy the string content
                        break;
                    default:
                         if ((yyvsp[0].val).stringVal) { // Check if stringVal is not null
                           yyerror(("Unhandled type for identifier '" + std::string((yyvsp[0].val).stringVal) + "'").c_str());
                       } else {
                           yyerror("Unhandled type for identifier (name not available).");
                       }
                        break;
                }
            }
        } else {
            yyerror(("Identifier '" + string((yyvsp[0].val).stringVal) + "' not found.").c_str());
            (yyval.val) = Value(); // Return an empty value on error
        }
    }
#line 2800 "parser.tab.c"
    break;

  case 97: /* primary_expression: CONSTANT  */
#line 1550 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2806 "parser.tab.c"
    break;

  case 98: /* primary_expression: TRUE  */
#line 1551 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2812 "parser.tab.c"
    break;

  case 99: /* primary_expression: FALSE  */
#line 1552 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2818 "parser.tab.c"
    break;

  case 100: /* primary_expression: '(' expression ')'  */
#line 1553 "parser.y"
                            { (yyval.val) = (yyvsp[-1].val); }
#line 2824 "parser.tab.c"
    break;

  case 101: /* ENTER_SCOPE: %empty  */
#line 1558 "parser.y"
    {
        printf("Entering new scope\n");
        quadrupleStack.push_back(currentQuadruple);
        
        // add new Quadruple 
        Quadruples * newQuadruple = new Quadruples();
        currentQuadruple = newQuadruple;
    }
#line 2837 "parser.tab.c"
    break;

  case 102: /* ENTER_SCOPE_SEMANTIC: %empty  */
#line 1569 "parser.y"
    {
        printf("Entering new scope semantic\n");

        // check if The scope is for a function call or a loop
        if (!isFuncCall && !isLoop){
            // create a new symbol table
            symbolTable = new SymbolTable(symbolTable);

        }
        else if (isFuncCall){

        isFuncCall = false;

        }
    }
#line 2857 "parser.tab.c"
    break;

  case 103: /* LEAVE_SCOPE: %empty  */
#line 1587 "parser.y"
  {
        printf("Leaving scope\n");
        (yyval.ptr) = currentQuadruple;
        // remove the last Quadruple
        currentQuadruple = quadrupleStack.back();
        quadrupleStack.pop_back();
  }
#line 2869 "parser.tab.c"
    break;

  case 104: /* LEAVE_SCOPE_SEMANTIC: %empty  */
#line 1596 "parser.y"
  {
        printf("Leaving scope semantic\n");

        if (symbolTable){
            // remove the last symbol table
            symbolTable->printTableToFile();
            symbolTable = symbolTable->getParent();
        }
        else{
            printf("Error: No symbol table to remove\n");
        }
  }
#line 2886 "parser.tab.c"
    break;


#line 2890 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1609 "parser.y"



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

