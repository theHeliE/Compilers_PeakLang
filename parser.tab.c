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
#line 9 "parser.y"

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

Value* getLabel(){
    printf("New Label\n");

    char temp[20];
    sprintf(temp, "L%d", quadruples->labelCounter++);

    Value * result = (Value*)malloc(sizeof(Value));
    result->type = STRING_TYPE;

    char* tempStr = strdup(temp);
    result->stringVal = tempStr;
     
     printf("Label: created");

    return result;
}

// Notmally the "Main" quadruple is the current quadruple, when we enter a new scope, this temporarily changes.
// Quadruples * currentQuadruple = quadruples;
// vector<Quadruples*> quadrupleStack;


#line 298 "parser.tab.c"

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
  YYSYMBOL_parameter_list = 67,            /* parameter_list  */
  YYSYMBOL_parameter_declaration = 68,     /* parameter_declaration  */
  YYSYMBOL_compound_statement = 69,        /* compound_statement  */
  YYSYMBOL_scope = 70,                     /* scope  */
  YYSYMBOL_scope_item = 71,                /* scope_item  */
  YYSYMBOL_expression_statement = 72,      /* expression_statement  */
  YYSYMBOL_statement = 73,                 /* statement  */
  YYSYMBOL_selection_statement = 74,       /* selection_statement  */
  YYSYMBOL_case_list = 75,                 /* case_list  */
  YYSYMBOL_case_item = 76,                 /* case_item  */
  YYSYMBOL_loop_statement = 77,            /* loop_statement  */
  YYSYMBOL_jump_statement = 78,            /* jump_statement  */
  YYSYMBOL_assignment_expression = 79,     /* assignment_expression  */
  YYSYMBOL_logical_or_expression = 80,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 81,    /* logical_and_expression  */
  YYSYMBOL_inclusive_or_expression = 82,   /* inclusive_or_expression  */
  YYSYMBOL_exclusive_or_expression = 83,   /* exclusive_or_expression  */
  YYSYMBOL_and_expression = 84,            /* and_expression  */
  YYSYMBOL_equality_expression = 85,       /* equality_expression  */
  YYSYMBOL_relational_expression = 86,     /* relational_expression  */
  YYSYMBOL_shift_expression = 87,          /* shift_expression  */
  YYSYMBOL_additive_expression = 88,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 89, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 90,          /* unary_expression  */
  YYSYMBOL_primary_expression = 91         /* primary_expression  */
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
#define YYFINAL  78
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   348

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  179

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
       0,   320,   320,   324,   331,   335,   347,   351,   355,   366,
     381,   407,   441,   449,   460,   461,   462,   463,   464,   468,
     471,   481,   490,   491,   498,   502,   503,   507,   508,   512,
     513,   519,   520,   528,   529,   530,   531,   532,   541,   569,
     610,   614,   615,   619,   620,   629,   634,   640,   647,   648,
     649,   653,   672,   691,   692,   716,   717,   741,   742,   766,
     767,   771,   772,   776,   777,   781,   782,   804,   829,   830,
     851,   872,   893,   917,   918,   919,   923,   924,   925,   929,
     930,   931,   932,   944,   945,   950,   953,   957,   965,  1006,
    1007,  1008,  1009
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
  "parameter_list", "parameter_declaration", "compound_statement", "scope",
  "scope_item", "expression_statement", "statement", "selection_statement",
  "case_list", "case_item", "loop_statement", "jump_statement",
  "assignment_expression", "logical_or_expression",
  "logical_and_expression", "inclusive_or_expression",
  "exclusive_or_expression", "and_expression", "equality_expression",
  "relational_expression", "shift_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "primary_expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-51)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      15,   -51,     5,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,     6,    14,    26,    96,    44,    37,     0,    13,    43,
     127,   292,   292,   292,   292,   -51,   294,   160,    73,   270,
     -51,   -51,    90,    75,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,    78,    76,    70,    86,    98,    -1,    54,   109,
     108,    28,   -51,   -51,   294,   294,   294,   294,   132,   143,
     123,   -51,   -51,   -51,   106,   182,   -51,   -51,   -51,   -51,
     -51,   -38,   -51,   -51,   182,   215,   -51,   -51,   -51,   -51,
     140,    23,   -51,   294,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   -51,    -4,    11,    19,   142,   143,   -51,   -51,
     -51,    17,   -51,   -51,   -51,   127,   294,   -51,   195,   -51,
      76,    70,    86,    98,    -1,    54,    54,   109,   109,   109,
     109,   108,   108,    28,    28,   -51,   -51,   -51,    96,   145,
      96,   294,   294,   294,   199,    39,   -51,   120,   -51,   190,
     148,   -51,    48,    56,   136,   -51,   127,   150,   -51,    96,
     207,   176,    -2,   -51,   163,    96,   -51,   -51,   -51,   -51,
     191,    96,   -51,   -51,   -51,   -51,    96,   -51,   -51
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     3,    88,    89,    14,    15,    16,    17,    18,    90,
      91,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     2,
       4,     7,     0,     0,     6,    34,    33,     8,    35,    36,
      37,    19,    53,    55,    57,    59,    61,    63,    65,    68,
      73,    76,    79,    83,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,     0,     0,    88,    86,    87,    85,
      84,     0,    25,    29,     0,     0,    27,    30,     1,     5,
      12,     0,    32,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    54,     0,     0,     0,     0,     0,    48,    52,
      12,     0,    92,    26,    28,     0,     0,     9,     0,    20,
      56,    58,    60,    62,    64,    66,    67,    71,    72,    69,
      70,    74,    75,    77,    78,    80,    81,    82,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,    13,    38,
       0,    45,     0,     0,     0,    24,     0,     0,    10,     0,
       0,     0,     0,    41,     0,     0,    11,    23,    21,    39,
       0,     0,    40,    42,    46,    47,     0,    44,    43
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -51,   -51,   -51,   198,   -25,   165,   -19,   -12,   -51,   -51,
      84,    85,   -51,   166,   -50,   -14,   -51,   -51,    81,   -51,
     -51,   -42,   -51,   161,   159,   162,   164,   158,   103,    29,
     104,   112,   -18,   -51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    28,    29,    30,    31,    81,    32,    33,    34,   145,
     146,    35,    75,    76,    36,    37,    38,   162,   163,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      58,    65,    73,    67,    68,    69,    70,    64,    74,   107,
     160,   161,   102,    77,    71,    83,     1,   112,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    89,    90,    11,
      54,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      60,   119,   143,   103,   104,   105,     2,     3,   116,    83,
      73,   138,    61,     9,    10,   172,    74,   142,    21,    22,
      55,    77,    23,    24,    83,    62,   139,    25,    56,    26,
     118,    27,    83,    78,   140,   117,   118,    99,   100,   101,
      57,   135,   136,   137,    91,    92,    21,    22,    93,    94,
      23,    24,   156,    80,   157,    63,   144,    26,    59,     2,
       3,    83,    85,   164,   147,    84,     9,    10,    86,    83,
      11,   165,    12,    13,    14,    15,    16,    17,    18,    19,
     127,   128,   129,   130,   149,    87,   151,    82,    83,   152,
     153,   154,     4,     5,     6,     7,     8,   144,    88,    21,
      22,    95,    96,    23,    24,   169,     2,     3,    25,   106,
      26,   175,    27,     9,    10,    97,    98,   177,   109,    83,
     160,   161,   178,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   158,    83,    11,   108,    12,    13,    14,    15,
      16,    17,    18,    19,    20,   110,    21,    22,   166,    83,
      23,    24,   125,   126,   115,    25,   141,    26,   148,   131,
     132,   150,   155,    21,    22,   159,    27,    23,    24,   133,
     134,   170,    25,   171,    26,   174,    27,    72,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    79,   176,    11,
     111,    12,    13,    14,    15,    16,    17,    18,    19,    20,
     167,   114,   168,   173,   121,   120,   124,     0,   122,     0,
       0,   123,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,    23,    24,     0,     0,     0,    25,     0,    26,
       0,    27,   113,     2,     3,     4,     5,     6,     7,     8,
       9,    10,     0,     0,    11,     0,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    66,     3,     2,     3,     0,
       0,     0,     9,    10,     9,    10,     0,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,    23,    24,     0,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,    21,    22,    23,
      24,    23,    24,     0,     0,     0,    26,     0,    26
};

static const yytype_int16 yycheck[] =
{
      14,    20,    27,    21,    22,    23,    24,    19,    27,    59,
      12,    13,    54,    27,    26,    53,     1,    55,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    28,    29,    14,
      25,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       3,    83,    25,    55,    56,    57,     3,     4,    25,    53,
      75,    55,    52,    10,    11,    57,    75,   107,    43,    44,
      54,    75,    47,    48,    53,    52,    55,    52,    54,    54,
      53,    56,    53,     0,    55,    52,    53,    49,    50,    51,
      54,    99,   100,   101,    30,    31,    43,    44,    34,    35,
      47,    48,    53,     3,    55,    52,   115,    54,    54,     3,
       4,    53,    26,    55,   116,    27,    10,    11,    38,    53,
      14,    55,    16,    17,    18,    19,    20,    21,    22,    23,
      91,    92,    93,    94,   138,    39,   140,    52,    53,   141,
     142,   143,     5,     6,     7,     8,     9,   156,    40,    43,
      44,    32,    33,    47,    48,   159,     3,     4,    52,    17,
      54,   165,    56,    10,    11,    47,    48,   171,    52,    53,
      12,    13,   176,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    52,    53,    14,    52,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     3,    43,    44,    52,    53,
      47,    48,    89,    90,    54,    52,    54,    54,     3,    95,
      96,    56,     3,    43,    44,    15,    56,    47,    48,    97,
      98,     4,    52,    37,    54,    52,    56,    57,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    29,    37,    14,
      65,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     156,    75,   157,   162,    85,    84,    88,    -1,    86,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      -1,    -1,    47,    48,    -1,    -1,    -1,    52,    -1,    54,
      -1,    56,    57,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    -1,    14,    -1,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     3,     4,     3,     4,    -1,
      -1,    -1,    10,    11,    10,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    -1,    52,    -1,    54,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    43,    44,    47,
      48,    47,    48,    -1,    -1,    -1,    54,    -1,    54
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    14,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    43,    44,    47,    48,    52,    54,    56,    59,    60,
      61,    62,    64,    65,    66,    69,    72,    73,    74,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    25,    54,    54,    54,    73,    54,
       3,    52,    52,    52,    65,    64,     3,    90,    90,    90,
      90,    65,    57,    62,    64,    70,    71,    73,     0,    61,
       3,    63,    52,    53,    27,    26,    38,    39,    40,    28,
      29,    30,    31,    34,    35,    32,    33,    47,    48,    49,
      50,    51,    79,    65,    65,    65,    17,    72,    52,    52,
       3,    63,    55,    57,    71,    54,    25,    52,    53,    79,
      81,    82,    83,    84,    85,    86,    86,    87,    87,    87,
      87,    88,    88,    89,    89,    90,    90,    90,    55,    55,
      55,    54,    72,    25,    64,    67,    68,    65,     3,    73,
      56,    73,    65,    65,    65,     3,    53,    55,    52,    15,
      12,    13,    75,    76,    55,    55,    52,    68,    69,    73,
       4,    37,    57,    76,    52,    73,    37,    73,    73
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    58,    59,    59,    60,    60,    61,    61,    61,    62,
      62,    62,    63,    63,    64,    64,    64,    64,    64,    65,
      65,    66,    67,    67,    68,    69,    69,    70,    70,    71,
      71,    72,    72,    73,    73,    73,    73,    73,    74,    74,
      74,    75,    75,    76,    76,    77,    77,    77,    78,    78,
      78,    78,    78,    79,    79,    80,    80,    81,    81,    82,
      82,    83,    83,    84,    84,    85,    85,    85,    86,    86,
      86,    86,    86,    87,    87,    87,    88,    88,    88,    89,
      89,    89,    89,    90,    90,    90,    90,    90,    91,    91,
      91,    91,    91
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     3,
       5,     6,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     6,     1,     3,     2,     2,     3,     1,     2,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     5,     7,
       7,     1,     2,     4,     3,     5,     7,     7,     3,     2,
       2,     2,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     2,     2,     1,     1,
       1,     1,     3
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
#line 321 "parser.y"
    { 
        printf("Main program\n"); 
    }
#line 1536 "parser.tab.c"
    break;

  case 3: /* mainProgram: error  */
#line 325 "parser.y"
    { 
        // ErrorToFile("Syntax Error"); YYABORT; 
        printf("Feh errors ya man\n");  YYABORT; 
    }
#line 1545 "parser.tab.c"
    break;

  case 4: /* program: external_declaration  */
#line 332 "parser.y"
    {
        printf("Program\n");
    }
#line 1553 "parser.tab.c"
    break;

  case 5: /* program: program external_declaration  */
#line 336 "parser.y"
    {
        printf("Program with external declarations\n");
    }
#line 1561 "parser.tab.c"
    break;

  case 6: /* external_declaration: function_definition  */
#line 348 "parser.y"
    {
        printf("Function definition\n");
    }
#line 1569 "parser.tab.c"
    break;

  case 7: /* external_declaration: variable_definition  */
#line 352 "parser.y"
    {
        printf("Variable definition\n");
    }
#line 1577 "parser.tab.c"
    break;

  case 8: /* external_declaration: statement  */
#line 356 "parser.y"
    {
        printf("Expression statement\n");
    }
#line 1585 "parser.tab.c"
    break;

  case 9: /* variable_definition: type_specifier identifier_list ';'  */
#line 367 "parser.y"
    {
        printf("variable_declaration\n");
        symbolTable->insert((yyvsp[-1].val), (yyvsp[-2].val), NULL);

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName((yyvsp[-1].val)), "", "");


    }
#line 1599 "parser.tab.c"
    break;

  case 10: /* variable_definition: type_specifier identifier_list ASSIGNMENT expression ';'  */
#line 382 "parser.y"
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

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName((yyvsp[-3].val)), "", "");
        quadruples->addQuadruple("ASSIGN", valueToString((yyvsp[-1].val)), "", getValueName((yyvsp[-3].val)));
    }
#line 1625 "parser.tab.c"
    break;

  case 11: /* variable_definition: THABET type_specifier identifier_list ASSIGNMENT expression ';'  */
#line 408 "parser.y"
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

        // handle quadruples
        quadruples->addQuadruple("DECLARE", getValueName((yyvsp[-3].val)), "", "");
        quadruples->addQuadruple("ASSIGN", valueToString((yyvsp[-1].val)), "", getValueName((yyvsp[-3].val)));

    }
#line 1656 "parser.tab.c"
    break;

  case 12: /* identifier_list: IDENTIFIER  */
#line 442 "parser.y"
    {
        (yyval.val) = (yyvsp[0].val);
    }
#line 1664 "parser.tab.c"
    break;

  case 13: /* identifier_list: identifier_list ',' IDENTIFIER  */
#line 450 "parser.y"
    {
        // $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        // sprintf($$, "%s,%s", $1, $3);
    }
#line 1673 "parser.tab.c"
    break;

  case 14: /* type_specifier: INT  */
#line 460 "parser.y"
            { (yyval.val).stringVal = "INT"; }
#line 1679 "parser.tab.c"
    break;

  case 15: /* type_specifier: FLOAT  */
#line 461 "parser.y"
              { (yyval.val).stringVal = "FLOAT"; }
#line 1685 "parser.tab.c"
    break;

  case 16: /* type_specifier: CHAR  */
#line 462 "parser.y"
              { (yyval.val).stringVal = "CHAR"; }
#line 1691 "parser.tab.c"
    break;

  case 17: /* type_specifier: VOID  */
#line 463 "parser.y"
              { (yyval.val).stringVal = "VOID";}
#line 1697 "parser.tab.c"
    break;

  case 18: /* type_specifier: BOOL  */
#line 464 "parser.y"
              { (yyval.val).stringVal = "BOOL"; }
#line 1703 "parser.tab.c"
    break;

  case 19: /* expression: assignment_expression  */
#line 468 "parser.y"
                            {
        printf("Assignment Expression\n");
    }
#line 1711 "parser.tab.c"
    break;

  case 22: /* parameter_list: parameter_declaration  */
#line 490 "parser.y"
                                    { (yyval.val) = (yyvsp[0].val); }
#line 1717 "parser.tab.c"
    break;

  case 23: /* parameter_list: parameter_list ',' parameter_declaration  */
#line 491 "parser.y"
                                               { (yyval.val) = (yyvsp[-2].val); }
#line 1723 "parser.tab.c"
    break;

  case 38: /* selection_statement: IF '(' expression ')' statement  */
#line 542 "parser.y"
    {
        // $3 is the expression (condition)
        // $5 is the statement to execute if the condition is true

        Value* end_if_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        quadruples->addQuadruple("JF", valueToString((yyvsp[-2].val)), valueToString(*end_if_label), "");
        printf("Debug: Adding JF Quadruple for IF statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[-2].val)).c_str(), valueToString(*end_if_label).c_str());

        // The quadruples for the 'statement' ($5) are generated when $5 is reduced.

        // Quadruple: Define the label that marks the end of the IF block
        quadruples->addQuadruple("LABEL", valueToString(*end_if_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*end_if_label).c_str());

        // The IF statement itself doesn't produce a value in this context,
        // but you might need to assign to $$ if selection_statement is expected to have one.
        // $$ = Value(); // Default value, or specific if needed.
        printf("Selection statement (IF) processed\n");
        free(end_if_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(end_if_label);
    }
#line 1751 "parser.tab.c"
    break;

  case 39: /* selection_statement: IF '(' expression ')' statement ELSE statement  */
#line 569 "parser.y"
                                                    {

         Value* else_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        quadruples->addQuadruple("JF", valueToString((yyvsp[-4].val)), valueToString(*else_label), "");
        printf("Debug: Adding JF Quadruple to else statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[-4].val)).c_str(), valueToString(*else_label).c_str());

        // The quadruples for the 'statement' ($5) are generated when $5 is reduced.

        // Quadruple: Define the label that marks the end of the IF block
        quadruples->addQuadruple("LABEL", valueToString(*else_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*else_label).c_str());

        // Jump to end of if block after executing the else statement
        Value* end_if_label = getLabel(); // Label to jump to after the else statement

        quadruples->addQuadruple("GOTO", "", valueToString(*end_if_label), "");
        printf("Debug: Adding GOTO Quadruple to end of IF block. Target Label: %s\n", valueToString(*end_if_label).c_str());

        // Quadruple: Define the label that marks the end of the IF block
        quadruples->addQuadruple("LABEL", valueToString(*end_if_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*end_if_label).c_str());

        // The IF statement itself doesn't produce a value in this context,
        // but you might need to assign to $$ if selection_statement is expected to have one.
        // $$ = Value(); // Default value, or specific if needed.
        printf("Selection statement (IF) processed\n");
        free(end_if_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(end_if_label);
        free(else_label->stringVal); // Assuming getLabel strdup'd and valueToString doesn't take ownership
        free(else_label);

        printf("Selection statement\n");
    }
#line 1791 "parser.tab.c"
    break;

  case 51: /* jump_statement: RETURN ';'  */
#line 654 "parser.y"
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
#line 1811 "parser.tab.c"
    break;

  case 52: /* jump_statement: RETURN expression ';'  */
#line 673 "parser.y"
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
#line 1829 "parser.tab.c"
    break;

  case 54: /* assignment_expression: IDENTIFIER ASSIGNMENT assignment_expression  */
#line 692 "parser.y"
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
            yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[-2].val))).c_str());
        }
        quadruples->addQuadruple("ASSIGN", valueToString((yyvsp[0].val)), "", getValueName((yyvsp[-2].val)));
    }
#line 1852 "parser.tab.c"
    break;

  case 56: /* logical_or_expression: logical_or_expression OR_OP logical_and_expression  */
#line 717 "parser.y"
                                                         {
         (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first || right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first || right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '||' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 1878 "parser.tab.c"
    break;

  case 58: /* logical_and_expression: logical_and_expression AND_OP inclusive_or_expression  */
#line 742 "parser.y"
                                                            {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first && right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first && right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '&&' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 1904 "parser.tab.c"
    break;

  case 66: /* equality_expression: equality_expression EQ_OP relational_expression  */
#line 782 "parser.y"
                                                     {

        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first == right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first == right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '==' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 1931 "parser.tab.c"
    break;

  case 67: /* equality_expression: equality_expression NE_OP relational_expression  */
#line 805 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first != right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first != right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '!=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 1957 "parser.tab.c"
    break;

  case 68: /* relational_expression: shift_expression  */
#line 829 "parser.y"
                       { (yyval.val) = (yyvsp[0].val); }
#line 1963 "parser.tab.c"
    break;

  case 69: /* relational_expression: relational_expression L_OP shift_expression  */
#line 830 "parser.y"
                                                  {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first < right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first < right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '<' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    }
#line 1989 "parser.tab.c"
    break;

  case 70: /* relational_expression: relational_expression G_OP shift_expression  */
#line 851 "parser.y"
                                                  {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f > %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first > right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d > %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first > right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '>' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2015 "parser.tab.c"
    break;

  case 71: /* relational_expression: relational_expression LE_OP shift_expression  */
#line 872 "parser.y"
                                                   {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f <= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first <= right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d <= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first <= right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '<=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2041 "parser.tab.c"
    break;

  case 72: /* relational_expression: relational_expression GE_OP shift_expression  */
#line 893 "parser.y"
                                                   {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[-2].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[0].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f >= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first >= right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[-2].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[0].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first >= right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '>=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    }
#line 2067 "parser.tab.c"
    break;

  case 83: /* unary_expression: primary_expression  */
#line 944 "parser.y"
                                 { (yyval.val) = (yyvsp[0].val); }
#line 2073 "parser.tab.c"
    break;

  case 84: /* unary_expression: '-' unary_expression  */
#line 945 "parser.y"
                                        {
          // Create a new Value and apply negation
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).intVal = -(yyval.val).intVal;
      }
#line 2083 "parser.tab.c"
    break;

  case 85: /* unary_expression: '+' unary_expression  */
#line 950 "parser.y"
                                       {
          (yyval.val) = (yyvsp[0].val);
      }
#line 2091 "parser.tab.c"
    break;

  case 86: /* unary_expression: '!' unary_expression  */
#line 953 "parser.y"
                           {
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).boolVal = !(yyval.val).boolVal;
      }
#line 2100 "parser.tab.c"
    break;

  case 87: /* unary_expression: '~' unary_expression  */
#line 957 "parser.y"
                           {
          (yyval.val) = (yyvsp[0].val);
          (yyval.val).intVal = ~(yyval.val).intVal;
      }
#line 2109 "parser.tab.c"
    break;

  case 88: /* primary_expression: IDENTIFIER  */
#line 965 "parser.y"
                            { 
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
#line 2155 "parser.tab.c"
    break;

  case 89: /* primary_expression: CONSTANT  */
#line 1006 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2161 "parser.tab.c"
    break;

  case 90: /* primary_expression: TRUE  */
#line 1007 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2167 "parser.tab.c"
    break;

  case 91: /* primary_expression: FALSE  */
#line 1008 "parser.y"
                            { (yyval.val) = (yyvsp[0].val); }
#line 2173 "parser.tab.c"
    break;

  case 92: /* primary_expression: '(' expression ')'  */
#line 1009 "parser.y"
                            { (yyval.val) = (yyvsp[-1].val); }
#line 2179 "parser.tab.c"
    break;


#line 2183 "parser.tab.c"

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

#line 1012 "parser.y"


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
