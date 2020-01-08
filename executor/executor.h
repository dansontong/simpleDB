#ifndef EXECUTOR_INTERFACE_H_INCLUDE
#define EXECUTOR_INTERFACE_H_INCLUDE
#include <iostream>
#include <vector>

#include "config.h"
#include "database.h"
#include "log.h"
#include "optimize.h"

#include "trivialtree.h"

// grammar2plan.cpp
/*--------------------------------------------------------------------
 * 内部结构定义
 *------------------------------------------------------------------*/
enum SQL_TYPE{
    CREATE_SQL,
    DROP_SQL,
    DELETE_SQL,
    INSERT_SQL,
    SELECT_SQL,
    UPDATE_SQL
};

typedef struct PerformPlan{
    char* table_name;
    SQL_TYPE sql_type;


    // hardcode了，没有经历去debug了
    char*   val_list[10];
    char*   val_type[10];
    int val_index;
    // std::vector<char*> val_list;
    // std::vector<char*> val_type;
};


extern Selectnode* select2plan(trivialtree * root);
extern PerformPlan *create2plan(trivialtree * root);
extern PerformPlan *drop2plan(trivialtree * root);
extern PerformPlan *delete2plan(trivialtree * root);
extern PerformPlan *insert2plan(trivialtree * root);
extern Selectnode *InitSelectNode();

// executo.cpp
// 执行指定的sql语句
void execute_sql(char *sql, int strlen);

#endif