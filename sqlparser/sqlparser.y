%{
// typedef char* string;
#define YYSTYPE char*
#include "sqllexical.c"
#include "trivialtree.h"
#include "string.h"
#include "stdio.h"
#include <iostream>

void INSERTANDMOVEACT(char *data);
void INSERTACT(char *data);

// using namespace std;
int yyerror(char* msg);
trivialtree *grammer=NULL, *prev;
%}
%token DROP CREATE SELECT INSERT DELETE
%token TABLE FROM WHERE ORDER_BY INTO VALUES
%token SEP_SEMICOLON SEP_COMMA
%token LOGICOP OPERATOR LEFT_BRACKET RIGHT_BRACKET
%token VALNAME DATATYPE DATA
%%
file: {
        // if (grammer == NULL){
        //     grammer = new trivialtree("root");  
        //     prev = grammer;
        // }
    }script SEP_SEMICOLON file|
    script SEP_SEMICOLON {grammer->print(0);}

script:  {grammer = new trivialtree("root");prev = grammer;INSERTANDMOVEACT("<drop>");} drop_sql {prev=prev->getParent();grammer->print(0);}
|{grammer = new trivialtree("root");prev = grammer;INSERTANDMOVEACT("<delete>");} delete_sql {prev=prev->getParent();grammer->print(0);}
|{grammer = new trivialtree("root");prev = grammer;INSERTANDMOVEACT("<select>");} select_sql {prev=prev->getParent();grammer->print(0);}
|{grammer = new trivialtree("root");prev = grammer;INSERTANDMOVEACT("<create>");} create_sql {prev=prev->getParent();grammer->print(0);}
|{grammer = new trivialtree("root");prev = grammer;INSERTANDMOVEACT("<insert>");} insert_sql {prev=prev->getParent();grammer->print(0);};

drop_sql:  DROP {INSERTACT("drop");} TABLE {INSERTACT("table");} VALNAME {
    INSERTACT($5);
    printf("drop table %s => drop_sql\n", $5);
};

delete_sql: DELETE {INSERTACT("delete");} FROM {INSERTACT("from");} VALNAME {INSERTACT($5);} where_clause {
    printf("delete from <table> where <condition_list> => delete_sql\n");
};

select_sql: SELECT {INSERTACT("select");INSERTANDMOVEACT("<col_list>");} sel_list FROM {prev=prev->getParent();INSERTACT("from");INSERTANDMOVEACT("<from_list>");} val_list {prev=prev->getParent();} where_clause{
    printf("select %s from %s where %s => select_sql\n", "column", "table", "null");
};

insert_sql: INSERT VALNAME VALUES LEFT_BRACKET {INSERTACT("insert");INSERTACT($2);prev=prev->getParent();INSERTACT("values");INSERTANDMOVEACT("<data_list>");} data_list RIGHT_BRACKET{
    prev=prev->getParent();
    printf("insert into %s values %s => select_sql\n", $2, $5);
}
|INSERT VALNAME LEFT_BRACKET {INSERTACT("insert");INSERTACT($2);INSERTANDMOVEACT("<col_list>");} val_list RIGHT_BRACKET VALUES LEFT_BRACKET {prev=prev->getParent();INSERTACT("values");INSERTANDMOVEACT("data_list");} data_list RIGHT_BRACKET{
    prev = prev->getParent();
    printf("insert into %s values %s => select_sql\n", $2, $5);
};

create_sql: CREATE TABLE VALNAME {INSERTACT("create");INSERTACT($3);INSERTANDMOVEACT("<tuple_list>");} LEFT_BRACKET tuple_list RIGHT_BRACKET{
    prev=prev->getParent();
    printf("create table %s ( %s ) => select_sql\n", $3, $4);
};

sel_list: val_list; // TODO select *

val_list: VALNAME {INSERTACT($1);} SEP_COMMA {INSERTANDMOVEACT("<col_list>");} val_list {prev=prev->getParent();}
|VALNAME{INSERTACT($1);};

where_clause: WHERE {INSERTACT("where");INSERTANDMOVEACT("<condition_list>");} condition_list {prev=prev->getParent();}
| ;

condition_list: condition LOGICOP {INSERTACT($2);INSERTANDMOVEACT("<condition_list>");} condition_list {prev=prev->getParent();};
| condition ;

condition: VALNAME {INSERTANDMOVEACT("<condition>");INSERTACT($1);} OPERATOR VALNAME {INSERTACT($3);INSERTACT($4);prev=prev->getParent();}; // TODO too simple

tuple_list: column SEP_COMMA {INSERTACT("<tuple_list>");}tuple_list{prev=prev->getParent();};
| column;

column: VALNAME DATATYPE{INSERTANDMOVEACT("column");INSERTACT($1);INSERTACT($2);prev=prev->getParent();};

data_list: DATA {INSERTACT($1);INSERTANDMOVEACT("<data_list>");}SEP_COMMA data_list{prev=prev->getParent();}
|DATA{INSERTACT($1);};
%%
void INSERTACT(char *data){
    trivialtree *tmp=new trivialtree(std::string(data));
    prev->insertChild(tmp);
}

void INSERTANDMOVEACT(char *data){
    trivialtree *tmp=new trivialtree(std::string(data));
    prev->insertChild(tmp);
    prev = tmp;
}

int yyerror(char* msg)
{
    printf("Error: %s encountered at line number:%d\n", msg, yylineno);
}
int main()
{
    yyparse();
    return 0;
}
