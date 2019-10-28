#include "database.h"
#include "log.h"


int main(int argc, char* argv[])
{
	//初始化数据库
	DataBase DB;
	char dbFile[30] = "./data/simpleDb";

	log_init();//初始化日志系统
	database_initDB(&DB, dbFile);
	Buf_Init();

	log_Debug("DB initial done.\n");

	//展示数据库
	database_showDbInfo();
	database_memToDisk();
	
	//创建表
	char tableFile[30] = "./data/table_list";
	int sup_dictID = createTable(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier表

	//读入数据，插入记录
	char tupleFile[30] = "./data/supplier.tbl";
	char buff[1000];
	FILE *fp = fopen(tupleFile, "rb");
	
	int count = 0;
	while(NULL != fgets(buff, 1000, fp))
	{
		strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
		//char *str = strtok(buff, "|");//使用|分割
		insertRecord(sup_dictID, buff);
		Log(INFO, "insert line No.%d", count);
		printf("insert line No.%d\n", count++);
		//printf("%s\n", buff);
	}
	create_index("Supplier","S_SUPPKEY");
	Record record = searchRecord("Supplier","S_SUPPKEY","Not_exist_for_test");
	Record record = searchRecord("Supplier","S_SUPPKEY","9840");
	if(record != NULL)
	{
		deleteRecord(sup_dictID, record);
	}
	drop_index("Supplier","S_SUPPKEY");



	database_showDbInfo();

	database_closeDB();
}
