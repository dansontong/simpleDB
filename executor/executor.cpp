#include <string.h>
#include <iostream>
#include <vector>

#include "executor.h"
#include "sqlparser.h"
#include "tableOpt.h"
#include "config.h"
#include "log.h"
#include "operator.h"

extern DataBase *DB;


/*--------------------------------------------------------------------
 * create_table
 * 
 * 封装tableOpt的createtable接口
 *------------------------------------------------------------------*/
void createTable(PerformPlan *plan){
    char table_name[100];
    strcpy(table_name, plan->table_name);

    Attribute *attr_list;

    if (plan->val_index>0){
        attr_list = malloc(sizeof(Attribute)*plan->val_index);
        memset(attr_list, 0, plan->val_index);
        for (int i=0; i<plan->val_index ; i++ )
        {
            DATA_TYPE data_type;
            int length;
            if ( strcmp(plan->val_type[i], "int") ==0 ){
                data_type = INT_TYPE;
                length = 4;
            }else if  ( strcmp(plan->val_type[i], "char") ==0 ){
                data_type = VARCHAR_TYPE;
                length = 255;
            }
            char *name = strdup(plan->val_list[i]);

            memcpy(attr_list[i].name, name, strlen(name));
            attr_list[i].name[strlen(name)] = '\0';
	        attr_list[i].length = length;
	        attr_list[i].type = data_type;//数据类型
        }
    
        int did = createTable(table_name, attr_list, plan->val_index);
        printf("create table %s res: %d\n", table_name, getDictIDbyName(table_name));
    }
}

/*--------------------------------------------------------------------
 * insertRecord
 * 
 * 封装tableOpt的insert接口,组织记录格式
 *------------------------------------------------------------------*/
void insertRecord(PerformPlan *plan)
{
    char *table_name = plan->table_name;
    int did = getDictIDbyName(table_name);
    if (did == 1){
        printf("table not exist.\n");
        return ;
    }

    char *record = (char *)malloc(sizeof(char) * 1000);
//10000|Supplier#000010000|aTGLEusCiL4F PDBdv665XBJhPyCOB0i|19|29-578-432-2146|8968.42|ly regular foxes boost slyly. quickly special waters boost carefully ironi
    for (int i=0; i<plan->val_index;i++){
        strcat(record, plan->val_list[i]);
        if (i<plan->val_index - 1)
            strcat(record, "|");
    }
    printf("record: %s\n", record);

    insertRecord(did, record);
    free(record);
}

void selectRecord(Selectnode *plan)
{
    char *table_name = plan->tablename[0];
    char *attrName, *attr;
    char *op;
    int tmp_table_id = -1;
    int did = getDictIDbyName(table_name);

    if (did == 1){
        printf("table not exist.\n");
        return ;
    }

    // 先做选择
    if (plan->num_attri>0){
        attrName = plan->attribute[0];
        attr = plan->attribute[1];
        printf("做选择操作（等值） %s = %s\n", attrName, attr);
        tmp_table_id = tableScanEqualSelector(did, attrName, attr);
    }
    else{
        tmp_table_id = did;
    }


    if (plan->projectionattribute[0][0] == '*'){
        tmp_table_id = tableScanSelector(tmp_table_id);  
        return;
    }
    // 做投影操作
    if (tmp_table_id >= 0){
        int i=0;
        printf("|");
        for (i=0;i<plan->num_pro;i++){
            printf("%s|", plan->projectionattribute[i]);
        }
        printf("\n");
        tmp_table_id = projection(tmp_table_id, plan->projectionattribute[0]);
    }else{
        tableScanSelector(tmp_table_id,plan->projectionattribute[0]);//select*
    }
    tableScanSelector(tmp_table_id);
}

void executePlan(PerformPlan *plan)
{
    switch (plan->sql_type){
        case CREATE_SQL:
            printf("prepare to perform create sql\n");
            createTable(plan);
            break;
        case INSERT_SQL:
            printf("prepare to perform create sql\n");
            insertRecord(plan);
            break;
        default:
            printf("unhandle type.\n");
            break;
    }
    return;
}
/*--------------------------------------------------------------------
 * checkPlan
 * 
 * 内部接口，语义检查
 *------------------------------------------------------------------*/
bool checkPlan(PerformPlan *plan)
{
    int did = -1;
    switch (plan->sql_type){
        case CREATE_SQL:
            did = getDictIDbyName(plan->table_name);
            if (did != -1){
                printf("table: %s already exist.\n", plan->table_name);
                return false;
            }
            break;
        case INSERT_SQL:
            did = getDictIDbyName(plan->table_name);
            if (did == -1){
                printf("table: %s not exist.\n", plan->table_name);
                return false;
            }
            break;
        default:
            printf("unhandle type.\n");
            break;
    }
    return true;
}

/*--------------------------------------------------------------------
 * execute_sql
 * 
 * 对外接口，执行sql语句，将结果打印
 *------------------------------------------------------------------*/
void execute_sql(char *sql, int strlen){
    // 生成语法树
    trivialtree *root = sql_parser_single(sql, strlen);
    if (!root){
        log_Error("generate grammar tree error. exit\n");
        return;
    }
    root->print(0);
    
    PerformPlan *plan = NULL;
    Selectnode *f = NULL;
    Plannode *logicplan = NULL;
    if (root->data().find("<SELECT>")!=root->data().npos){
        // 生成查询计划&优化
        f = select2plan(root);
        logicplan = malloc(sizeof(Plannode));
        // lp_init(*f, logicplan);
    }else if (root->data().find("<CREATE>") != root->data().npos) {
        plan = create2plan(root);
    }else if (root->data().find("<INSERT>") != root->data().npos) {
        // BUG 参数有时不能保存，case "insert into user values (32, \"yusf\");"
        plan = insert2plan(root);
    }
    if (f!= NULL){
        // 执行select
        selectRecord(f);
    }else if (plan!=NULL) {
        bool res = checkPlan(plan);
        if (res == false){
            return;    
        }
        executePlan(plan);
    }
    return;
}

