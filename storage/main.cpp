int main(int argc, char* argv[]) {
	//初始化数据库
	Storage dbMeta;
	char DbMetaFile[15] = "/opt/mydb/db_meta";
	StorageMgr storageMgr;
	storageMgr.initial(&dbMeta, fileName);

	//展示数据库
	storageMgr.showDbInfo(&dbMeta);
	//创建表


	//读入数据

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
		StorageManager.fileOpt.writeFile(&dbhead, fileID, strlen(str), str);
	}

}
int main(int argc, char *argv[])
{
	//printf("hello");
	Storage StorageManager;
	DbMetaHead dbhead; 
	StorageManager.initDB(&dbhead, fileName);
	StorageManager.showDbInfo(&dbhead);

	StorageManager.fileOpt.deleteFile(&dbhead, 0);
	StorageManager.pageOpt.recyAllPage(&dbhead);
	StorageManager.showDbInfo(&dbhead);

	int fileID = StorageManager.fileOpt.createFile(&dbhead, NORMAL_FILE, 1);
	printf("创建文件%d成功！\n", fileID); 
	//int mapfileID = StorageManager.fileOpt.createFile(&dbhead, MAP_FILE, 1);

	memToDisk(&dbhead);
	StorageManager.showDbInfo(&dbhead);

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
		StorageManager.fileOpt.writeFile(&dbhead, fileID, strlen(str), str);
	}
}