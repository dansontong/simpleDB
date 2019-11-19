%{
#include "sqlparser.h"
 
#include <stdio.h>
#include <string.h>
extern char* yylval;
%}
char [A-Za-z]
num [0-9]
keyword create|table|drop|select|from|where|insert\ into|values|delete
datatype int|float|double
logic and|or
separator \;|\,|\(|\)
oprator \>|\<
name {char}*
data {num}*
%%
{datatype} {
    yylval = strdup(yytext);
    return DATATYPE;
}
{oprator} {
    yylval = strdup(yytext);
    return OPERATOR;
}
{logic} {
    yylval = strdup(yytext);
    return LOGICOP;
}
{keyword} {
    if(strcmp(yytext,"drop")==0){
        return DROP;
    }else if (strcmp(yytext, "delete")==0){
        return DELETE;
    }else if (strcmp(yytext, "select")==0){
        return SELECT;
    }else if (strcmp(yytext, "create")==0){
        return CREATE;
    }else if (strcmp(yytext, "insert into")==0){
        return INSERT;
    }else if (strcmp(yytext, "table")==0){
        return TABLE;
    }else if (strcmp(yytext, "from")==0){
        return FROM;
    }else if (strcmp(yytext, "where")==0){
        return WHERE;
    }else if (strcmp(yytext, "values")==0){
        return VALUES;
    }
}
{name} {yylval = strdup(yytext);return VALNAME;}
{separator} {
    if(strcmp(yytext,";")==0){
        return SEP_SEMICOLON;
    }else if (strcmp(yytext, ",")==0){
        return SEP_COMMA;
    }else if (strcmp(yytext, "(")==0){
        return LEFT_BRACKET;
    }else if (strcmp(yytext, ")")==0){
        return RIGHT_BRACKET;
    }
}
{data} {yylval = strdup(yytext); return DATA;}
%%
int yywrap()
{
 return 1;
}