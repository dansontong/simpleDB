#ifndef EXECUTOR_INTERFACE_H_INCLUDE
#define EXECUTOR_INTERFACE_H_INCLUDE
#include <iostream>
#include <vector>

#include "config.h"
#include "database.h"
#include "log.h"

#include "trivialtree.h"

// grammar2plan.cpp
/* author hrc start */
// select需要的数据结构
struct Selectnode
{                
    char tablename[10][100];           //from 后的关系名
    char attribute[10][100];           //where 后的属性名 where a.a1 = b.a1 and b1 = 1 就是 a.a1 b.a1 b1 1
    char attrioftable[10][100];        //属性所属的表名 接上一行 a b b null(null表示这个是单纯的一个数值，不是列名)
    char op[10][100];                  //运算符：= =
    char projectionattribute[10][100]; //select 关键词后的投影的属性名select *  就是*
    int num_tab;                       //tablename的总数量
    int num_attri;                     //where中属性的总数量
    int num_attri_tab;                 //和num_attri值相同
    int num_op;                        //运算符的总数量
    int num_pro;                       //投影的属性的总数量
    int subselect;                     //子查询的总数  量  只写了一个子查询的情况
    int subtablename[10];              //主查询与子查询 在表名的下标，subtablename[0]=0 subtablename[1]=2
    int subattribute[10];              //同上
    int subattributetable[10];         //同上
    int sub_pro[10];                   //同上
    int subop[10];                     //同上
};


struct Tablenode
{
    char tablename[10];
};

struct Joinnode
{
    char newtable[10];
};

struct Filternode
{
    char attribute[10][100];
    char attrioftable[10][100];
    char op[10][100];
    int number;
};

struct Projectionnode
{
    char attribute[10][100];
    char tablename[10];
    int number;
};

struct Plannode
{ //查询计划树的一个节点
    char kind[10];
    union {
        Tablenode tablenode;
        Joinnode joinnode;
        Filternode filternode;
        Projectionnode projectionnode;
    };
    struct Plannode *left;
    struct Plannode *right;
    struct Plannode *parents;
};
extern Plannode *make_join_node(int start_table, int end_table, Plannode *cur);
extern Plannode *make_filter_node(int start_op, int end_op, int start_attri, int end_attri, Plannode *cur);
extern Plannode *make_project_node(int start_pro, int end_pro, Plannode *cur);
extern Plannode* plan_init(Selectnode *select);
/* hrc end */
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