#include "database.h"
#include "log.h"
#include "index.h"


int main(int argc, char* argv[]) {
	printf("====================== index_test begin ======\n");

	//初始化数据库
	DataBase DB;
	char dbFile[30] = "../data/simpleDb";

	log_init("../data/DBrun.log");//初始化日志系统
	Buf_Init();
	database_initDB(&DB, dbFile);

	log_Debug("DB initial done.\n");

	//展示数据库
	database_showDbInfo();
	database_memToDisk();
	
	//创建表
	char tableFile[30] = "../data/table_list";
	int sup_dictID = createTable(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier表

	//读入数据，插入记录
	char tupleFile[30] = "../data/supplier.tbl";
	char buff[1000];
	FILE *fp = fopen(tupleFile, "rb");
	
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
	create_index("Supplier","S_SUPPKEY");
	printf("------ creat index in %s for table %s -----\n", "Supplier","S_SUPPKEY");
	Record *recordList;
	recordList = searchRecord("Supplier","S_SUPPKEY","Not_exist_for_test");
	recordList = searchRecord("Supplier","S_SUPPKEY","9840");
	printf("------ search record by index in column '%s' for table '%s' with value '%s' -----\n","S_SUPPKEY","Supplier","9840");
	while(recordList != NULL)
	{
		deleteRecord("Supplier", "S_SUPPKEY", recordList);
		printf("------ delete index in %s for table %s -----\n","S_SUPPKEY","Supplier");
		recordList++;
	}
	drop_index("Supplier","S_SUPPKEY");
	printf("------ drop index in %s for table %s -----\n","S_SUPPKEY","Supplier");



	database_showDbInfo();

	database_closeDB();




	printf("======================= index_test end ========\n");
}