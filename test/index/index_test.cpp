#include "database.h"
#include "log.h"
#include "index.h"
#include "operator.h"
#include <stdio.h>
#include <string.h>

// 运行前，需将index.cpp中的 "./data/index/" 要改成 "../data/index/"
int main(int argc, char* argv[]) {
	printf("====================== index_test begin ======\n");

	//初始化数据库
	DataBase DB;
	char dbFile[30] = "../data/simpleDb";

	log_init("../data/DBrun.log");//初始化日志系统
	Buf_Init();
	initDB(&DB, dbFile);

	log_Debug("DB initial done.\n");

	//展示数据库
	showDbInfo();
	//创建表
	Attribute *attr_list = new Attribute[7];
	strcpy(attr_list[0].name, "S_SUPPKEY");
	attr_list[0].type = INT_TYPE;
	attr_list[0].length = 4;
	attr_list[0].notNull = true;
	strcpy(attr_list[1].name, "S_NAME");
	attr_list[1].type = CHAR_TYPE;
	attr_list[1].length = 25;
	attr_list[1].notNull = true;
	strcpy(attr_list[2].name, "S_ADDRESS");
	attr_list[2].type = VARCHAR_TYPE;
	attr_list[2].length = 40;
	attr_list[2].notNull = true;
	strcpy(attr_list[3].name, "NATIONKEY");
	attr_list[3].type = INT_TYPE;
	attr_list[3].length = 4;
	attr_list[3].notNull = true;
	strcpy(attr_list[4].name, "S_PHONE");
	attr_list[4].type = CHAR_TYPE;
	attr_list[4].length = 15;
	attr_list[4].notNull = true;
	strcpy(attr_list[5].name, "S_ACCTBAL");
	attr_list[5].type = FLOAT_TYPE;
	attr_list[5].length = 8;
	attr_list[5].notNull = true;
	strcpy(attr_list[6].name, "S_COMMENT");
	attr_list[6].type = VARCHAR_TYPE;
	attr_list[6].length = 101;
	attr_list[6].notNull = true;
	int sup_dictID = createTable("Supplier", attr_list, 7);

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

	strcpy(attr_list[0].name, "NATIONKEY");
	attr_list[0].type = INT_TYPE;
	attr_list[0].length = 4;
	attr_list[0].notNull = true;
	strcpy(attr_list[1].name, "N_NAME");
	attr_list[1].type = CHAR_TYPE;
	attr_list[1].length = 25;
	attr_list[1].notNull = true;
	strcpy(attr_list[2].name, "N_REGIONKEY");
	attr_list[2].type = INT_TYPE;
	attr_list[2].length = 4;
	attr_list[2].notNull = true;
	strcpy(attr_list[3].name, "N_COMMENT");
	attr_list[3].type = VARCHAR_TYPE;
	attr_list[3].length = 101;
	attr_list[3].notNull = true;
	int nation_dictID = createTable("nation", attr_list, 4);//
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



	showDbInfo();

	closeDB();




	printf("======================= index_test end ========\n");
}
