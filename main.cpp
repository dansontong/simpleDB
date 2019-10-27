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
		recordInsert(sup_dictID, buff);
		Log(INFO, "insert line No.%d", count);
		printf("insert line No.%d\n", count++);
		//printf("%s\n", buff);
	}
		database_showDbInfo();

	// int fileID = file_createFile(&dbMeta, TABLE_FILE, 1);
	// printf("创建文件%d成功！\n", fileID);
	// //int fileID = 0;
	// int num = 2000;
	// for (int i = 0; i < num; i++) {
	// 	struct Student stu = { i, "abc", 30 + i, 5000 + i };
	// 	char str[1000];
	// 	sprintf(str, "%ld", stu.rid);
	// 	strcat(str, stu.name); 
	// 	char tmp[100];
	// 	sprintf(tmp, "%d", stu.age); 
	// 	sprintf(tmp, "%d", stu.weight); 
	// 	strcat(str, tmp);
	// 	if (i % 10 == 0)
	// 		printf("str: %s\n", str);
	// 	file_writeFile(&dbMeta, fileID, strlen(str), str);
	// }

	database_closeDB();
}
