#include "database.h"
#include "log.h"
#include "index.h"
#include "executor.h"


int main(int argc, char* argv[]) {
	printf("====================== file_test begin ======\n");

	//初始化数据库
	DataBase DB;
	char dbFile[30] = "../data/simpleDb";

	log_init("./DBrun.log");//初始化日志系统
	Buf_Init();
	database_initDB(&DB, dbFile);

	log_Debug("DB initial done.\n");

	//展示数据库
	database_showDbInfo();
	database_memToDisk();
	
	//创建表
	char tableFile[30] = "../data/table_list";
	int sup_dictID = createTable(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
	//读入数据，插入记录
	char tupleFile[30] = "../data/supplier.tbl";
	char buff[1000];
	FILE *fp = fopen(tupleFile, "rb");
	
	//================ index_test ================
	int count = 0;
	while(NULL != fgets(buff, 1000, fp))
	{
		strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
		//char *str = strtok(buff, "|");//使用|分割
		Log(INFO, "----- begin insert line No.%d -----", count);
		insertRecord(sup_dictID, buff);
		printf("----- end insert line No.%d -----\n", count++);
		//printf("%s\n", buff);
	}

	int nation_dictID = createTable2(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
	//读入数据，插入记录
	char tupleFile2[30] = "../data/nation.tbl";
	fp = fopen(tupleFile2, "rb");
	
	//================ index_test ================
	count = 0;
	while(NULL != fgets(buff, 1000, fp))
	{
		strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
		//char *str = strtok(buff, "|");//使用|分割
		Log(INFO, "----- begin insert line No.%d -----", count);
		insertRecord(nation_dictID, buff);
		printf("----- end insert line No.%d -----\n", count++);
		//printf("%s\n", buff);
	}


	database_showDbInfo();

	//database_closeDB();




	printf("======================= file_test end ========\n");
}
