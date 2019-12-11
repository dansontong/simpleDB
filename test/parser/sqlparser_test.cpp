
#include <stdlib.h>

#include <string.h>

#include "trivialtree.h"
#include "sqlparser.h"


int main() {
    printf("======================= parser_test begin ===========\n");

    char sql[] = "select * from user;";
    printf("sql: %s\n", sql);

    trivialtree *root = sql_parser_single(sql, strlen(sql));
    if (root){
        printf("grammar tree:\n");
        root->print(0);
    }

    printf("======================= parser_test end ============\n");
}
