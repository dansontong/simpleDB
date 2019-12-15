%option noyywrap

%{
/*--------------------------------------------------------------------
 * 
 * includes
 * 
 *------------------------------------------------------------------*/
#include "sqlparser.h"   
    
/*--------------------------------------------------------------------
 * 
 * defines
 * 
 *------------------------------------------------------------------*/
#define YY_INPUT(buf,result,max_size)  {\
    result = GetNextChar(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
    }
%}

/*--------------------------------------------------------------------
 * 
 * flex definitions
 * 
 *------------------------------------------------------------------*/
digit [0-9]
keyword create|table|drop|select|from|where|insert\ into|values|delete|order\ by|desc|asc
datatype int|float|double|char|date
logic and|or
separator \;|\,|\(|\)
oprator \>|\<|=
data {digit}*
name [A-Za-z][A-Za-z0-9]*
date {digit}+\-{digit}+(\-{digit}+)?
str \".*\"
%%
[ \t\r\n]+  {
    /* eat up whitespace */
    BeginToken(yytext); 
    }
{datatype} {
    BeginToken(yytext); 
    yylval.string = strdup(yytext);
    return DATATYPE;
}
{oprator} {
    BeginToken(yytext); 
    yylval.string = strdup(yytext);
    return OPERATOR;
}
{logic} {
    BeginToken(yytext); 
    yylval.string = strdup(yytext);
    return LOGICOP;
}
{keyword} {
    BeginToken(yytext);
    yylval.string = strdup(yytext);
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
    }else if (strcmp(yytext, "order by")==0){
        return ORDERBY;
    }else if (strcmp(yytext, "desc")==0){
        return DESC;
    }else if (strcmp(yytext, "asc")==0){
        return ASC;
    }
}
{name} {
    BeginToken(yytext); yylval.string = strdup(yytext);return VALNAME;
}
{separator} {
    BeginToken(yytext); 
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

"." {BeginToken(yytext); yylval.string = strdup(yytext); return SEP_PERIOD;}

{date} {BeginToken(yytext); yytext[strlen(yytext)-1]='\0'; yylval.string = strdup(yytext+1);  return DATA;}
{str} {BeginToken(yytext); yytext[strlen(yytext)-1]='\0'; yylval.string = strdup(yytext+1); return DATA;}
{data} {BeginToken(yytext); yylval.string = strdup(yytext); return DATA;}

"*" { 
    BeginToken(yytext); 
    yylval.string = strdup(yytext);
    return ASTERISK; 
}

%%

/*--------------------------------------------------------------------
 * lex.l
 *------------------------------------------------------------------*/
