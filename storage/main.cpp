#include "storage.h"
#include "Test.h" 
#include "log.h"


int main(int argc, char* argv[]) {
	//初始化数据库
	log_init();//初始化日志系统
	log_Info("logging test success!, this is a Info.");
	Storage dbMeta;
	char dbFile[30] = "./simpleDb";

	storage_initDB(&dbMeta, dbFile);

	log_Debug("DB initial done.\n");

	//展示数据库
	storage_showDbInfo(&dbMeta);
	storage_memToDisk(&dbMeta);
	
	//创建表
	char tableFile[30] = "./table_list";


	//读入数据，插入记录
	char tupleFile[30] = "./supplier.tbl";
	FILE *fp = fopen(tupleFile, "rb");
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

}
