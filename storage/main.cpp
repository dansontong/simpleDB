#include "storage.h"
#include "Test.h" 
#include "logging.h"


int main(int argc, char* argv[]) {
	//初始化数据库
	initLog();//初始化日志系统
	logInfo("logging.h test success!, this is a Info.");
	Storage dbMeta;
	char dbFile[30] = "/opt/mydb/db_meta";
	StorageMgr storageMgr;
	printf("DB initial begin.\n");
	storageMgr.initial(&dbMeta, dbFile);
	printf("DB initial done.\n");

	//展示数据库
	storageMgr.showDbInfo(&dbMeta);
	memToDisk(&dbMeta);
	//创建表


	//读入数据
	int fileID = storageMgr.fileMgr.createFile(&dbMeta, NORMAL_FILE, 1);
	printf("创建文件%d成功！\n", fileID);
	//int fileID = 0;
	int num = 2000;
	for (int i = 0; i < num; i++) {
		struct Student stu = { i, "abc", 30 + i, 5000 + i };
		char str[1000];
		sprintf(str, "%ld", stu.rid);
		strcat(str, stu.name); 
		char tmp[100];
		sprintf(tmp, "%d", stu.age); 
		sprintf(tmp, "%d", stu.weight); 
		strcat(str, tmp);
		if (i % 10 == 0)
			printf("str: %s\n", str);
		storageMgr.fileMgr.writeFile(&dbMeta, fileID, strlen(str), str);
	}

}
