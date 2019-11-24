%{
/*--------------------------------------------------------------------
 * 
 * Includes
 * 
 *------------------------------------------------------------------*/
// #define YYSTYPE char*
#include "sqlparser.h"
#define YYERROR_VERBOSE 1

#include <string.h>
#include <stdio.h>


/*--------------------------------------------------------------------
 * 
 * global variables
 * 
 *------------------------------------------------------------------*/
trivialtree *curGT;

#define BUILDANDINSERT(root, data)  do{\
        trivialtree *tmp=new trivialtree(std::string(data));\
        ((trivialtree *)root)->insertChild(tmp);\
    }while(0)
#define ONLYINSERT(root, cnode)  do{\
        if (cnode){\
            ((trivialtree *)root)->insertChild((trivialtree *)cnode);\
        }\
    }while(0)
%}

/*--------------------------------------------------------------------
 * 
 * definitions
 * 
 *------------------------------------------------------------------*/
/* generate include-file with symbols and types */
%defines

/* a more advanced semantic type */
%union {
  double      value;
  char        *string;
  void        *tnode;
}


/*--------------------------------------------------------------------
 *
 * terminal-symbols
 *
 *------------------------------------------------------------------*/
%token<string> DROP CREATE SELECT INSERT DELETE
%token<string> TABLE FROM WHERE ORDERBY INTO VALUES
%token<string> SEP_SEMICOLON SEP_COMMA ASTERISK
%token<string> LOGICOP OPERATOR LEFT_BRACKET RIGHT_BRACKET
%token<string> VALNAME DATATYPE DATA ASC DESC

%type<tnode>    drop_sql delete_sql select_sql create_sql insert_sql
%type<tnode>    where_clause sel_list condition_list val_list data_list tuple_list order_clause
%type<tnode>    condition column
/*------------------------------------------------------------------------------
 *
 * start of grammar
 *
 *----------------------------------------------------------------------------*/
%start sql

%%

/*------------------------------------------------------------------------------
 * 
 * rules
 * 
 *----------------------------------------------------------------------------*/
sql: scripts sql| scripts; 

scripts: drop_sql SEP_SEMICOLON {PrintGrammar($1);}
| delete_sql SEP_SEMICOLON {PrintGrammar($1);}
| select_sql SEP_SEMICOLON {PrintGrammar($1);}
| create_sql SEP_SEMICOLON {PrintGrammar($1);}
| insert_sql SEP_SEMICOLON {PrintGrammar($1);};

drop_sql:  DROP TABLE VALNAME {
    $$ = (void *) new trivialtree("<DROP>");
    BUILDANDINSERT($$, $1);
    BUILDANDINSERT($$, $2);
    BUILDANDINSERT($$, $3);
    if ( debug ){
        // printf("[reduce]: DROP TABLE %s => drop_sql\n", ((trivialtree *)$3)->data().c_str());
        printf("debug: %p\n", &@3);
    }
};

delete_sql: DELETE FROM VALNAME where_clause {
    $$ = (void *) new trivialtree("<DELETE>");
    BUILDANDINSERT($$, $1);
    BUILDANDINSERT($$, $2);
    BUILDANDINSERT($$, $3);
    ONLYINSERT($$,$4);
    if ( debug ){
        // printf("[reduce]: DELETE FROM %s WHERE => drop_sql\n", ((trivialtree *)$3)->data().c_str());
        ;
    }
};

select_sql: SELECT sel_list FROM val_list where_clause order_clause{
    $$ = (void *) new trivialtree("<SELECT>");
    BUILDANDINSERT($$,$1);
    ONLYINSERT( ((trivialtree*)$$)->getChild(), $2); // sel_list插入到select之后
    BUILDANDINSERT($$,$3);
    ONLYINSERT(((trivialtree*)$$)->getChild(), $4); // val_list插入到from之后
    ONLYINSERT($$,$5);
    ONLYINSERT($$,$6);

    if ( debug ){
        // printf("[reduce]: SELECT FROM %s WHERE => select_sql\n", ((trivialtree *)$3)->data().c_str());
        ;
    }
};

insert_sql: INSERT VALNAME VALUES LEFT_BRACKET data_list RIGHT_BRACKET{
    $$ = (void *) new trivialtree("<INSERT>");
    BUILDANDINSERT($$,$1);
    BUILDANDINSERT($$,$2);
    BUILDANDINSERT($$,$3);
    ONLYINSERT($$,$5);
    if ( debug ){
        // printf("[reduce]: insert into values => insert_sql\n", ((trivialtree *)$3)->data().c_str());
        ;
    }
}|INSERT VALNAME LEFT_BRACKET val_list RIGHT_BRACKET VALUES LEFT_BRACKET data_list RIGHT_BRACKET{
    $$ = (void *) new trivialtree("<INSERT>");
    BUILDANDINSERT($$,$1); // INSERT 
    BUILDANDINSERT($$,$2); // VALNAME
    ONLYINSERT($$,$4); // val_list
    BUILDANDINSERT($$,$6); // VALUES
    ONLYINSERT($$,$8); // data_list
    if ( debug ){
        // printf("[reduce]: insert into values => insert_sql\n", ((trivialtree *)$3)->data().c_str());
        ;
    }
};

create_sql: CREATE TABLE VALNAME LEFT_BRACKET tuple_list RIGHT_BRACKET{
    $$ = (void *) new trivialtree("<CREATE>");
    BUILDANDINSERT($$,$1); // CREATE
    BUILDANDINSERT($$,$2); // TABLE
    BUILDANDINSERT($$,$3); // VALNAME
    ONLYINSERT($$, $5);
    if ( debug ){
        // printf("[reduce]: CREATE TABLE  => create_sql\n", ((trivialtree *)$3)->data().c_str());
        ;
    }
};

sel_list: val_list{
    $$ = $1;
    ((trivialtree *)$$)->setdata("<SEL_LIST>");
}|ASTERISK {
    $$ = (void *) new trivialtree("<SEL_LIST>");
    BUILDANDINSERT($$,$1);
};

val_list: VALNAME SEP_COMMA val_list {
    $$ = (void *) new trivialtree("<VAL_LIST>");
    BUILDANDINSERT($$,$1);
    ONLYINSERT($$,$3);
}
| VALNAME{
    $$ = (void *) new trivialtree("<VAL_LIST>");
    BUILDANDINSERT($$,$1);
};

where_clause: WHERE condition_list {
    $$ = (void *) new trivialtree("<WHERE>");
    BUILDANDINSERT($$, $1);
    ONLYINSERT($$, $2);
}
| {$$ = NULL;};

order_clause: ORDERBY val_list ASC{
    $$ = (void *) new trivialtree("<ORDERBY>");
    BUILDANDINSERT($$, $1);
    ONLYINSERT($$, $2);
    BUILDANDINSERT($$, $3);
}| ORDERBY val_list DESC{
    $$ = (void *) new trivialtree("<ORDERBY>");
    BUILDANDINSERT($$, $1);
    ONLYINSERT($$, $2);
    BUILDANDINSERT($$, $3);
}| ORDERBY val_list{
    $$ = (void *) new trivialtree("<ORDERBY>");
    BUILDANDINSERT($$, $1);
    ONLYINSERT($$, $2);
}| {$$=NULL;}

condition_list: condition LOGICOP condition_list {
    $$ = (void *) new trivialtree("<CONDITION_LIST>");
    ONLYINSERT($$, $1);
    BUILDANDINSERT($$, $2);
    ONLYINSERT($$, $3);
}
| condition {
    $$ = (void *) new trivialtree("<CONDITION_LIST>");
    ONLYINSERT($$, $1);
};

// TODO too simple
condition: VALNAME OPERATOR VALNAME {    
    $$ = (void *) new trivialtree("<CONDITION>");
    BUILDANDINSERT($$,$1);
    BUILDANDINSERT($$,$2);
    BUILDANDINSERT($$,$3);
}; 

tuple_list: column SEP_COMMA tuple_list{
    $$ = (void *) new trivialtree("<TUPLE_LIST>");
    ONLYINSERT($$, $1);
    ONLYINSERT($$, $3);
};
| column{
    $$ = (void *) new trivialtree("<TUPLE_LIST>");
    ONLYINSERT($$, $1);
};

column: VALNAME DATATYPE{
    $$ = (void *) new trivialtree("<TUPLE>");
    BUILDANDINSERT($$, $1);
    BUILDANDINSERT($$, $2);
};

data_list: DATA SEP_COMMA data_list{
    $$ = (void *) new trivialtree("<DATA_LIST>");
    BUILDANDINSERT($$, $1);
    ONLYINSERT($$, $3);
}|DATA{
    $$ = (void *) new trivialtree("<DATA_LIST>");
    BUILDANDINSERT($$, $1);
};

%%
/*------------------------------------------------------------------------------
 * 
 * functions
 * 
 *----------------------------------------------------------------------------*/
extern
void yyerror(char *s) {
//  a more sophisticated error-function
  PrintError(s);
}

/*--------------------------------------------------------------------
 * parse.y
 *------------------------------------------------------------------*/