#ifndef SQL_PARSER
#define SQL_PARSER

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>

#include "parse.h"
#include "trivialtree.h"

/*
 * global variable
 */
extern int parser_debug;
extern trivialtree *groot;

/*
 * lex & parse
 */
extern int yylex(void);
extern int yyparse(void);
extern void yyerror(char*);

/*
 * ccalc.c
 */
extern void DumpRow(void); 
extern int GetNextChar(char *b, int maxBuffer);
extern void BeginToken(char*);
extern void PrintError(char *s, ...);

extern void PrintGrammar(trivialtree *root);
extern trivialtree * sql_parser_single(char *sql, int sqllen);

#endif /*SQL_PARSER*/