/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 3 "parser.y"

    #include "value_helpers.h"

#line 53 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    CONSTANT = 259,                /* CONSTANT  */
    INT = 260,                     /* INT  */
    FLOAT = 261,                   /* FLOAT  */
    CHAR = 262,                    /* CHAR  */
    VOID = 263,                    /* VOID  */
    BOOL = 264,                    /* BOOL  */
    TRUE = 265,                    /* TRUE  */
    FALSE = 266,                   /* FALSE  */
    CASE = 267,                    /* CASE  */
    DEFAULT = 268,                 /* DEFAULT  */
    IF = 269,                      /* IF  */
    ELSE = 270,                    /* ELSE  */
    SWITCH = 271,                  /* SWITCH  */
    WHILE = 272,                   /* WHILE  */
    DO = 273,                      /* DO  */
    FOR = 274,                     /* FOR  */
    GOTO = 275,                    /* GOTO  */
    CONTINUE = 276,                /* CONTINUE  */
    BREAK = 277,                   /* BREAK  */
    RETURN = 278,                  /* RETURN  */
    THABET = 279,                  /* THABET  */
    ASSIGNMENT = 280,              /* ASSIGNMENT  */
    AND_OP = 281,                  /* AND_OP  */
    OR_OP = 282,                   /* OR_OP  */
    EQ_OP = 283,                   /* EQ_OP  */
    NE_OP = 284,                   /* NE_OP  */
    LE_OP = 285,                   /* LE_OP  */
    GE_OP = 286,                   /* GE_OP  */
    LEFT_OP = 287,                 /* LEFT_OP  */
    RIGHT_OP = 288,                /* RIGHT_OP  */
    L_OP = 289,                    /* L_OP  */
    G_OP = 290,                    /* G_OP  */
    UMINUS = 291,                  /* UMINUS  */
    UPLUS = 292,                   /* UPLUS  */
    LOWER_THAN_ELSE = 293          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 237 "parser.y"

    int intVal;
    float floatVal;
    char * stringVal;
    char charVal;
    char * variable;
    bool boolVal;
    Value val;

#line 118 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
