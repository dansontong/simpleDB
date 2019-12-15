
#include <stdlib.h>

#include <string.h>

#include "trivialtree.h"
#include "executor.h"
#include "log.h"
#include "database.h"
#include "operator.h"

int main() {
    log_init("./testlog.txt");
    printf("\n======================= executor_test begin =======================\n");
    
    //初始化数据库
	DataBase DB;
	char dbFile[30] = "../data/simpleDb";

	log_init("./DBrun.log");//初始化日志系统
	Buf_Init();
	initDB(&DB, dbFile);

	log_Debug("DB initial done.\n");

    char sql[] = "create table user (id int,name char, age int);\n";
    // char sql[] = "insert into user values (32, \"yusf\" );\n";
    printf("sql: %s\n", sql);
    execute_sql(sql, strlen(sql));

    char sql1[] = "insert into user values (32, \"test\", 23 );\n";
    printf("sql: %s\n", sql1);
    execute_sql(sql1, strlen(sql1));
    
    printf("\n======================= executor_test end =======================\n");
}
