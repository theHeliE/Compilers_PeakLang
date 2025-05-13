
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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




/* Line 189 of yacc.c  */
#line 308 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 3 "parser.y"

    #include "value_helpers.h"



/* Line 209 of yacc.c  */
#line 338 "parser.tab.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     CONSTANT = 259,
     INT = 260,
     FLOAT = 261,
     CHAR = 262,
     VOID = 263,
     BOOL = 264,
     TRUE = 265,
     FALSE = 266,
     CASE = 267,
     DEFAULT = 268,
     IF = 269,
     ELSE = 270,
     SWITCH = 271,
     WHILE = 272,
     DO = 273,
     FOR = 274,
     GOTO = 275,
     CONTINUE = 276,
     BREAK = 277,
     RETURN = 278,
     THABET = 279,
     ASSIGNMENT = 280,
     AND_OP = 281,
     OR_OP = 282,
     EQ_OP = 283,
     NE_OP = 284,
     LE_OP = 285,
     GE_OP = 286,
     LEFT_OP = 287,
     RIGHT_OP = 288,
     L_OP = 289,
     G_OP = 290,
     UPLUS = 291,
     UMINUS = 292,
     LOWER_THAN_ELSE = 293
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 244 "parser.y"

    int intVal;
    float floatVal;
    char * stringVal;
    char charVal;
    char * variable;
    bool boolVal;
    Value val;

    void* ptr;



/* Line 214 of yacc.c  */
#line 407 "parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 419 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   244

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  93
/* YYNRULES -- Number of states.  */
#define YYNSTATES  180

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    41,     2,     2,     2,    51,    40,     2,
      54,    55,    49,    47,    53,    48,     2,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    52,
       2,    46,     2,    36,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    39,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,    38,    57,    42,     2,     2,     2,
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
      35,    43,    44,    45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    14,    16,    18,
      22,    28,    35,    37,    41,    43,    45,    47,    49,    51,
      53,    57,    64,    66,    70,    73,    79,    81,    84,    86,
      88,    90,    93,    95,    97,    99,   101,   103,   109,   117,
     125,   127,   130,   135,   139,   145,   153,   161,   165,   168,
     171,   174,   178,   180,   184,   186,   190,   192,   196,   198,
     202,   204,   208,   210,   214,   216,   220,   224,   226,   230,
     234,   238,   242,   244,   248,   252,   254,   258,   262,   264,
     268,   272,   276,   278,   281,   284,   287,   290,   292,   294,
     296,   298,   302,   303
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      59,     0,    -1,    60,    -1,     1,    -1,    61,    -1,    60,
      61,    -1,    66,    -1,    62,    -1,    69,    -1,    64,    63,
      52,    -1,    64,    63,    25,    65,    52,    -1,    24,    64,
      63,    25,    65,    52,    -1,     3,    -1,    63,    53,     3,
      -1,     5,    -1,     6,    -1,     7,    -1,     8,    -1,     9,
      -1,    79,    -1,    65,    53,    79,    -1,    64,     3,    54,
      67,    55,    69,    -1,    68,    -1,    67,    53,    68,    -1,
      64,     3,    -1,    56,    92,    70,    93,    57,    -1,    71,
      -1,    70,    71,    -1,    62,    -1,    73,    -1,    52,    -1,
      65,    52,    -1,    72,    -1,    69,    -1,    74,    -1,    77,
      -1,    78,    -1,    14,    54,    65,    55,    69,    -1,    14,
      54,    65,    55,    69,    15,    69,    -1,    16,    54,    65,
      55,    56,    75,    57,    -1,    76,    -1,    75,    76,    -1,
      12,     4,    37,    73,    -1,    13,    37,    73,    -1,    17,
      54,    65,    55,    73,    -1,    18,    73,    17,    54,    65,
      55,    52,    -1,    19,    54,    72,    72,    65,    55,    73,
      -1,    20,     3,    52,    -1,    21,    52,    -1,    22,    52,
      -1,    23,    52,    -1,    23,    65,    52,    -1,    80,    -1,
       3,    25,    79,    -1,    81,    -1,    80,    27,    81,    -1,
      82,    -1,    81,    26,    82,    -1,    83,    -1,    82,    38,
      83,    -1,    84,    -1,    83,    39,    84,    -1,    85,    -1,
      84,    40,    85,    -1,    86,    -1,    85,    28,    86,    -1,
      85,    29,    86,    -1,    87,    -1,    86,    34,    87,    -1,
      86,    35,    87,    -1,    86,    30,    87,    -1,    86,    31,
      87,    -1,    88,    -1,    87,    32,    88,    -1,    87,    33,
      88,    -1,    89,    -1,    88,    47,    89,    -1,    88,    48,
      89,    -1,    90,    -1,    89,    49,    90,    -1,    89,    50,
      90,    -1,    89,    51,    90,    -1,    91,    -1,    48,    90,
      -1,    47,    90,    -1,    41,    90,    -1,    42,    90,    -1,
       3,    -1,     4,    -1,    10,    -1,    11,    -1,    54,    65,
      55,    -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   329,   329,   333,   340,   344,   356,   360,   364,   377,
     392,   418,   452,   460,   471,   472,   473,   474,   475,   479,
     482,   492,   501,   502,   509,   513,   521,   522,   526,   527,
     533,   534,   542,   543,   544,   545,   546,   555,   586,   631,
     635,   636,   640,   641,   650,   655,   661,   668,   669,   670,
     674,   693,   712,   713,   738,   739,   763,   764,   788,   789,
     793,   794,   798,   799,   803,   804,   826,   851,   852,   873,
     894,   915,   939,   940,   941,   945,   946,   947,   951,   952,
     953,   954,   966,   967,   972,   975,   979,   987,  1028,  1029,
    1030,  1031,  1036,  1065
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "CONSTANT", "INT", "FLOAT",
  "CHAR", "VOID", "BOOL", "TRUE", "FALSE", "CASE", "DEFAULT", "IF", "ELSE",
  "SWITCH", "WHILE", "DO", "FOR", "GOTO", "CONTINUE", "BREAK", "RETURN",
  "THABET", "ASSIGNMENT", "AND_OP", "OR_OP", "EQ_OP", "NE_OP", "LE_OP",
  "GE_OP", "LEFT_OP", "RIGHT_OP", "L_OP", "G_OP", "'?'", "':'", "'|'",
  "'^'", "'&'", "'!'", "'~'", "UPLUS", "UMINUS", "LOWER_THAN_ELSE", "'='",
  "'+'", "'-'", "'*'", "'/'", "'%'", "';'", "','", "'('", "')'", "'{'",
  "'}'", "$accept", "mainProgram", "program", "external_declaration",
  "variable_definition", "identifier_list", "type_specifier", "expression",
  "function_definition", "parameter_list", "parameter_declaration",
  "compound_statement", "scope", "scope_item", "expression_statement",
  "statement", "selection_statement", "case_list", "case_item",
  "loop_statement", "jump_statement", "assignment_expression",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "primary_expression", "ENTER_SCOPE", "LEAVE_SCOPE", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,    63,    58,   124,    94,
      38,    33,   126,   291,   292,   293,    61,    43,    45,    42,
      47,    37,    59,    44,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    58,    59,    59,    60,    60,    61,    61,    61,    62,
      62,    62,    63,    63,    64,    64,    64,    64,    64,    65,
      65,    66,    67,    67,    68,    69,    70,    70,    71,    71,
      72,    72,    73,    73,    73,    73,    73,    74,    74,    74,
      75,    75,    76,    76,    77,    77,    77,    78,    78,    78,
      78,    78,    79,    79,    80,    80,    81,    81,    82,    82,
      83,    83,    84,    84,    85,    85,    85,    86,    86,    86,
      86,    86,    87,    87,    87,    88,    88,    88,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    91,    91,    91,
      91,    91,    92,    93
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     3,
       5,     6,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     6,     1,     3,     2,     5,     1,     2,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     5,     7,     7,
       1,     2,     4,     3,     5,     7,     7,     3,     2,     2,
       2,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     2,     2,     2,     2,     1,     1,     1,
       1,     3,     0,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     3,    14,    15,    16,    17,    18,     0,    92,     0,
       2,     4,     7,     0,     6,     8,     0,     0,     1,     5,
      12,     0,    12,     0,    87,    88,    89,    90,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,    28,     0,     0,    33,    93,    26,    32,
      29,    34,    35,    36,    19,    52,    54,    56,    58,    60,
      62,    64,    67,    72,    75,    78,    82,     0,     0,     9,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
      49,    50,     0,    87,    85,    86,    84,    83,     0,    31,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     0,    13,     0,    53,     0,     0,
       0,     0,     0,    47,    51,    91,    20,    25,    55,    57,
      59,    61,    63,    65,    66,    70,    71,    68,    69,    73,
      74,    76,    77,    79,    80,    81,    24,     0,     0,    10,
      11,     0,     0,     0,     0,     0,    23,    21,    37,     0,
      44,     0,     0,     0,     0,     0,     0,    40,     0,     0,
      38,     0,     0,    39,    41,    45,    46,     0,    43,    42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    10,    11,    12,    21,    13,    45,    14,   112,
     113,    46,    47,    48,    49,    50,    51,   166,   167,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    17,    92
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -64
static const yytype_int16 yypact[] =
{
      11,   -64,   -64,   -64,   -64,   -64,   -64,    99,   -64,    26,
      47,   -64,   -64,    39,   -64,   -64,    48,    77,   -64,   -64,
     -43,     5,   -64,     8,   -10,   -64,   -64,   -64,    10,    35,
      68,   150,    76,    59,    50,    75,    18,   174,   174,   174,
     174,   -64,   190,   -64,    48,   -21,   -64,    77,   -64,   -64,
     -64,   -64,   -64,   -64,   -64,   119,   123,   121,   126,   122,
      19,    15,    41,    31,    64,   -64,   -64,    99,   190,   -64,
     171,   190,   190,   190,   190,   190,   166,   134,   135,   -64,
     -64,   -64,    83,   -64,   -64,   -64,   -64,   -64,   -17,   -64,
     190,   -64,   142,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   200,    37,   -64,    88,   -64,   127,   -64,    73,    79,
      97,   151,   134,   -64,   -64,   -64,   -64,   -64,   123,   121,
     126,   122,    19,    15,    15,    41,    41,    41,    41,    31,
      31,    64,    64,   -64,   -64,   -64,   -64,    99,   152,   -64,
     -64,   152,   155,   150,   190,   190,   -64,   -64,   192,   104,
     -64,   102,   103,   152,   208,   176,    12,   -64,   162,   150,
     -64,   180,   150,   -64,   -64,   -64,   -64,   150,   -64,   -64
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -64,   -64,   -64,   209,    -3,   202,    -4,   -34,   -64,   -64,
      78,     0,   -64,   173,   -54,   -30,   -64,   -64,    57,   -64,
     -64,   -63,   -64,   131,   132,   138,   133,   130,    91,     9,
     105,    89,   -33,   -64,   -64,   -64
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      15,    76,    82,    16,    84,    85,    86,    87,    88,   117,
      15,    67,     1,    44,    43,    72,     2,     3,     4,     5,
       6,    24,    25,   122,   164,   165,    18,   126,    26,    27,
      68,    89,    90,    71,   114,     7,    90,   116,   125,   118,
     119,   120,    20,    44,    43,   100,   101,    98,    99,   102,
     103,    22,     2,     3,     4,     5,     6,    69,    70,    37,
      38,    70,    78,   111,    73,    39,    40,     8,   155,   173,
      81,     7,    42,   104,   105,   143,   144,   145,   106,   107,
      24,    25,     2,     3,     4,     5,     6,    26,    27,    74,
     147,    28,   148,    29,    30,    31,    32,    33,    34,    35,
      36,     7,    79,     8,     2,     3,     4,     5,     6,   135,
     136,   137,   138,   108,   109,   110,   164,   165,    37,    38,
     161,   162,    75,   160,    39,    40,    90,    80,   151,    41,
      77,    42,    90,     8,   152,   124,    90,    24,    25,   176,
     149,    90,   178,   111,    26,    27,    93,   179,   157,    94,
      90,   158,   153,    24,    25,    90,    90,   168,   169,    95,
      26,    27,    97,   170,    28,    96,    29,    30,    31,    32,
      33,    34,    35,    36,   115,    37,    38,    83,    25,   150,
      90,    39,    40,   121,    26,    27,    41,   123,    42,   133,
     134,    37,    38,    24,    25,   141,   142,    39,    40,   127,
      26,    27,    41,   146,    42,   154,     8,   163,     8,   139,
     140,   159,   171,   172,   175,    37,    38,   177,    23,    19,
      91,    39,    40,   174,   128,   156,   129,   132,    42,   131,
       0,    37,    38,   130,     0,     0,     0,    39,    40,     0,
       0,     0,     0,     0,    42
};

static const yytype_int16 yycheck[] =
{
       0,    31,    36,     7,    37,    38,    39,    40,    42,    72,
      10,    54,     1,    17,    17,    25,     5,     6,     7,     8,
       9,     3,     4,    77,    12,    13,     0,    90,    10,    11,
      25,    52,    53,    25,    68,    24,    53,    71,    55,    73,
      74,    75,     3,    47,    47,    30,    31,    28,    29,    34,
      35,     3,     5,     6,     7,     8,     9,    52,    53,    41,
      42,    53,     3,    67,    54,    47,    48,    56,   122,    57,
      52,    24,    54,    32,    33,   108,   109,   110,    47,    48,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    54,
      53,    14,    55,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    52,    56,     5,     6,     7,     8,     9,   100,
     101,   102,   103,    49,    50,    51,    12,    13,    41,    42,
     154,   155,    54,   153,    47,    48,    53,    52,    55,    52,
      54,    54,    53,    56,    55,    52,    53,     3,     4,   169,
      52,    53,   172,   147,    10,    11,    27,   177,   148,    26,
      53,   151,    55,     3,     4,    53,    53,    55,    55,    38,
      10,    11,    40,   163,    14,    39,    16,    17,    18,    19,
      20,    21,    22,    23,     3,    41,    42,     3,     4,    52,
      53,    47,    48,    17,    10,    11,    52,    52,    54,    98,
      99,    41,    42,     3,     4,   106,   107,    47,    48,    57,
      10,    11,    52,     3,    54,    54,    56,    15,    56,   104,
     105,    56,     4,    37,    52,    41,    42,    37,    16,    10,
      47,    47,    48,   166,    93,   147,    94,    97,    54,    96,
      -1,    41,    42,    95,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    54
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     5,     6,     7,     8,     9,    24,    56,    59,
      60,    61,    62,    64,    66,    69,    64,    92,     0,    61,
       3,    63,     3,    63,     3,     4,    10,    11,    14,    16,
      17,    18,    19,    20,    21,    22,    23,    41,    42,    47,
      48,    52,    54,    62,    64,    65,    69,    70,    71,    72,
      73,    74,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    54,    25,    52,
      53,    25,    25,    54,    54,    54,    73,    54,     3,    52,
      52,    52,    65,     3,    90,    90,    90,    90,    65,    52,
      53,    71,    93,    27,    26,    38,    39,    40,    28,    29,
      30,    31,    34,    35,    32,    33,    47,    48,    49,    50,
      51,    64,    67,    68,    65,     3,    65,    79,    65,    65,
      65,    17,    72,    52,    52,    55,    79,    57,    81,    82,
      83,    84,    85,    86,    86,    87,    87,    87,    87,    88,
      88,    89,    89,    90,    90,    90,     3,    53,    55,    52,
      52,    55,    55,    55,    54,    72,    68,    69,    69,    56,
      73,    65,    65,    15,    12,    13,    75,    76,    55,    55,
      69,     4,    37,    57,    76,    52,    73,    37,    73,    73
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 330 "parser.y"
    { 
        printf("Main program\n"); 
    ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 334 "parser.y"
    { 
        // ErrorToFile("Syntax Error"); YYABORT; 
        printf("Feh errors ya man\n");  YYABORT; 
    ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 341 "parser.y"
    {
        printf("Program\n");
    ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 345 "parser.y"
    {
        printf("Program with external declarations\n");
    ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 357 "parser.y"
    {
        printf("Function definition\n");
    ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 361 "parser.y"
    {
        printf("Variable definition\n");
    ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 365 "parser.y"
    {
        printf("Expression statement\n");
        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[(1) - (1)].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);
    ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 378 "parser.y"
    {
        printf("variable_declaration\n");
        symbolTable->insert((yyvsp[(2) - (3)].val), (yyvsp[(1) - (3)].val), NULL);

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[(2) - (3)].val)), "", "");


    ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 393 "parser.y"
    {
           if (getValueName((yyvsp[(1) - (5)].val))!=enumToString((yyvsp[(4) - (5)].val).type)) {
            yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
        }
        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[(4) - (5)].val)); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert((yyvsp[(2) - (5)].val), (yyvsp[(1) - (5)].val), allocated_val_ptr); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName((yyvsp[(2) - (5)].val)))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[(2) - (5)].val))).c_str());
             symbolTable->insert((yyvsp[(2) - (5)].val), (yyvsp[(1) - (5)].val), nullptr); 
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[(2) - (5)].val)), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString((yyvsp[(4) - (5)].val)), "", getValueName((yyvsp[(2) - (5)].val)));
    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 419 "parser.y"
    {
        printf("variable_declaration Rule 3\n");
            if (getValueName((yyvsp[(2) - (6)].val))!=enumToString((yyvsp[(5) - (6)].val).type)) {
                yyerror("Elet adab walahy :/, Type mismatch in variable declaration");
            }

         // $2 is type_specifier (Value), $3 is identifier_list (Value), $5 is expression (Value)
        
        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[(5) - (6)].val)); 

        if (allocated_val_ptr != nullptr) { 
            symbolTable->insert((yyvsp[(3) - (6)].val), (yyvsp[(2) - (6)].val), allocated_val_ptr,true); 
            // check if there is an error in the symbol table
            if (symbolTable->get_SingleEntry(getValueName((yyvsp[(3) - (6)].val)))->isError) {
                yyerror("btnadeeny tany leeeeh");
            }
        } else {
             yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[(2) - (6)].val))).c_str());
             symbolTable->insert((yyvsp[(2) - (6)].val), (yyvsp[(1) - (6)].val), nullptr); 
        }

        // handle currentQuadruple
        currentQuadruple->addQuadruple("DECLARE", getValueName((yyvsp[(3) - (6)].val)), "", "");
        currentQuadruple->addQuadruple("ASSIGN", valueToString((yyvsp[(5) - (6)].val)), "", getValueName((yyvsp[(3) - (6)].val)));

    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 453 "parser.y"
    {
        (yyval.val) = (yyvsp[(1) - (1)].val);
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 461 "parser.y"
    {
        // $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
        // sprintf($$, "%s,%s", $1, $3);
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 471 "parser.y"
    { (yyval.val).stringVal = "INT"; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 472 "parser.y"
    { (yyval.val).stringVal = "FLOAT"; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 473 "parser.y"
    { (yyval.val).stringVal = "CHAR"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 474 "parser.y"
    { (yyval.val).stringVal = "VOID";;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 475 "parser.y"
    { (yyval.val).stringVal = "BOOL"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 479 "parser.y"
    {
        printf("Assignment Expression\n");
    ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 501 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 502 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (3)].val); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 513 "parser.y"
    {
        printf("Compound statement\n");

        (yyval.ptr) = (yyvsp[(4) - (5)].ptr);
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 556 "parser.y"
    {
        // $3 is the expression (condition)
        // $5 is the statement to execute if the condition is true

        Value* end_if_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[(3) - (5)].val)), valueToString(*end_if_label), "");
        printf("Debug: Adding JF Quadruple for IF statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[(3) - (5)].val)).c_str(), valueToString(*end_if_label).c_str());

        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[(5) - (5)].ptr);
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
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 586 "parser.y"
    {
        printf("Selection statement with ELSE\n");

         Value* else_label = getLabel(); // Label to jump to if condition is false (i.e., after the statement)

        // Quadruple: If condition ($3) is false, jump to end_if_label
        currentQuadruple->addQuadruple("JF", valueToString((yyvsp[(3) - (7)].val)), valueToString(*else_label), "");
        printf("Debug: Adding JF Quadruple to else statement. Condition: %s, Target Label: %s\n", valueToString((yyvsp[(3) - (7)].val)).c_str(), valueToString(*else_label).c_str());

        // The currentQuadruple for the 'statement' ($5) are generated when $5 is reduced.

        Quadruples * ScopeQuadruples = (Quadruples*)(yyvsp[(5) - (7)].ptr);
        currentQuadruple = ScopeQuadruples->merge(currentQuadruple);

        // Quadruple: Define the label that marks the end of the IF block
        currentQuadruple->addQuadruple("LABEL", valueToString(*else_label), "", "");
        printf("Debug: Adding LABEL Quadruple: %s\n", valueToString(*else_label).c_str());

        // Jump to end of if block after executing the else statement
        Value* end_if_label = getLabel(); // Label to jump to after the else statement

        currentQuadruple->addQuadruple("GOTO", "", valueToString(*end_if_label), "");
        printf("Debug: Adding GOTO Quadruple to end of IF block. Target Label: %s\n", valueToString(*end_if_label).c_str());

        Quadruples * elseScopeQuadruples = (Quadruples*)(yyvsp[(7) - (7)].ptr);
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
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 675 "parser.y"
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

    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 694 "parser.y"
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
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 713 "parser.y"
    {
        printf("Assignment expression\n");
        // $1 is IDENTIFIER, $3 is assignment_expression (Value)


        void* allocated_val_ptr = allocateValueFromExpression((yyvsp[(3) - (3)].val));
        if (allocated_val_ptr != nullptr) {
            symbolTable->update_Value(getValueName((yyvsp[(1) - (3)].val)), allocated_val_ptr , (yyvsp[(3) - (3)].val).type);

        // check if there is an error in the symbol table
        if (symbolTable->get_SingleEntry(getValueName((yyvsp[(1) - (3)].val)))->isError) {
            yyerror("Matrakez ya 3aaam (pay attention noob)");
        }
        } else {
            yyerror(("Failed to create value for assignment to variable " + getValueName((yyvsp[(1) - (3)].val))).c_str());
        }
        printf("Debug: Assigning value to variable %s\n", getValueName((yyvsp[(1) - (3)].val)).c_str());
        currentQuadruple->addQuadruple("ASSIGN", valueToString((yyvsp[(3) - (3)].val)), "", getValueName((yyvsp[(1) - (3)].val)));
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 739 "parser.y"
    {
         (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first || right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first || right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '||' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 764 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first && right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first && right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '&&' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 804 "parser.y"
    {

        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first == right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first == right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '==' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 827 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first != right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first != right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '!=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 851 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 852 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f < %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first < right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d < %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first < right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '<' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false; 
            }
        }
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 873 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f > %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first > right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d > %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first > right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '>' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 894 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f <= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first <= right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d <= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first <= right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '<=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 915 "parser.y"
    {
        (yyval.val) = Value();
        (yyval.val).type = BOOL_TYPE;
        std::pair<float, bool> left_f = resolve_operand_to_float((yyvsp[(1) - (3)].val), symbolTable);
        std::pair<float, bool> right_f = resolve_operand_to_float((yyvsp[(3) - (3)].val), symbolTable);

        if (left_f.second && right_f.second) {
            // printf("Debug: Comparing (float) %f >= %f\n", left_f.first, right_f.first);
            (yyval.val).boolVal = left_f.first >= right_f.first;
        } else {
            std::pair<int, bool> left_i = resolve_operand_to_int((yyvsp[(1) - (3)].val), symbolTable);
            std::pair<int, bool> right_i = resolve_operand_to_int((yyvsp[(3) - (3)].val), symbolTable);
            if (left_i.second && right_i.second) {
                // printf("Debug: Comparing (int) %d >= %d\n", left_i.first, right_i.first);
                (yyval.val).boolVal = left_i.first >= right_i.first;
            } else {
                yyerror("Type error or unresolved identifier in '>=' comparison. Operands not comparable as float or int.");
                (yyval.val).boolVal = false;
            }
        }
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 966 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 967 "parser.y"
    {
          // Create a new Value and apply negation
          (yyval.val) = (yyvsp[(2) - (2)].val);
          (yyval.val).intVal = -(yyval.val).intVal;
      ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 972 "parser.y"
    {
          (yyval.val) = (yyvsp[(2) - (2)].val);
      ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 975 "parser.y"
    {
          (yyval.val) = (yyvsp[(2) - (2)].val);
          (yyval.val).boolVal = !(yyval.val).boolVal;
      ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 979 "parser.y"
    {
          (yyval.val) = (yyvsp[(2) - (2)].val);
          (yyval.val).intVal = ~(yyval.val).intVal;
      ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 987 "parser.y"
    { 
         (yyval.val) = Value();
        SingleEntry* entry = symbolTable->get_SingleEntry(getValueName((yyvsp[(1) - (1)].val)));
        if (entry) {
             (yyval.val).type = entry->type;
             // If the variable has an initialized value in the symbol table
            if (entry->value != nullptr) { // Assuming 'entry->value' is the void* to the actual data
                switch(entry->type) {
                    case INT_TYPE:
                        (yyval.val).intVal = *(static_cast<int*>(entry->value));
                        printf("Debug: Resolved identifier '%s' to %d\n", getValueName((yyvsp[(1) - (1)].val)), (yyval.val).intVal);
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
                         if ((yyvsp[(1) - (1)].val).stringVal) { // Check if stringVal is not null
                           yyerror(("Unhandled type for identifier '" + std::string((yyvsp[(1) - (1)].val).stringVal) + "'").c_str());
                       } else {
                           yyerror("Unhandled type for identifier (name not available).");
                       }
                        break;
                }
            }
        } else {
            yyerror(("Identifier '" + string((yyvsp[(1) - (1)].val).stringVal) + "' not found.").c_str());
            (yyval.val) = Value(); // Return an empty value on error
        }
    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 1028 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 1029 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 1030 "parser.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 1031 "parser.y"
    { (yyval.val) = (yyvsp[(2) - (3)].val); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 1036 "parser.y"
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
    ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 1065 "parser.y"
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

        (yyval.ptr) = currentQuadruple;
        // remove the last Quadruple
        currentQuadruple = quadrupleStack.back();
        quadrupleStack.pop_back();
  ;}
    break;



/* Line 1455 of yacc.c  */
#line 2562 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1082 "parser.y"



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

