#include <stdlib.h>
#include "storage.h"

void storage_initDB(struct Storage *DB, char *fileName)
{
	FILE *dbFile = fopen(fileName, "rb");
	if(dbFile == NULL)
	{
		printf("DataBase isn't exist, creating new dataBase.\n");
		storage_createDbFile(fileName);
		dbFile = fopen(fileName, "rb");
	}

	fread(&(DB->dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 为空闲空间的位示图bitMap分配空间
	DB->freeSpaceBitMap = (unsigned long *)malloc(DB->dbMeta.bitMapSize);
	//rewind(dbFile);
	// 在文件中定位到bitMap开始的位置，令文件指针指向这里
	fseek(dbFile, DB->dbMeta.bitMapAddr, SEEK_SET);
	fread(DB->freeSpaceBitMap, DB->dbMeta.bitMapSize, 1, dbFile);// 从文件中读取bitMap的内容
	fclose(dbFile);

	//加载 数据字典
	int fid = DB.DbMeta.dataDictFid;
	if(fid < 0)
	{
		printf("dataDictionary file doesn't exist.\n");
		fid = file_newFile(DB, DATA_DICT_FILE, 1);
		DB.DbMeta.dataDictFid = fid;
	}
	
}

void storage_createDbFile(char *fileName)
{
	struct Storage DB;
	DB.dbMeta.blockSize = BLOCK_SIZE;
	DB.dbMeta.blockNum = FILE_DATA_SIZE / BLOCK_SIZE; // 256*1024
	DB.dbMeta.blockFree = DB.dbMeta.blockNum;
	DB.dbMeta.bitMapSize = BIT_MAP_SIZE;
	DB.dbMeta.bitMapAddr = BIT_MAP_ADDR;
	DB.dbMeta.dataSize = FILE_DATA_SIZE;
	DB.dbMeta.dataAddr = FILE_DATA_ADDR;
	DB.dbMeta.currFileNum = 0;
	DB.dbMeta.dataDictFid = -1;
	//DB.dbMeta.fileMeta[MAX_FILE_NUM] = new FileMeta[MAX_FILE_NUM];
	DB.dbMeta.fileMeta[0].id = 0;
	strcpy(DB.dbMeta.fileMeta[0].name, "0");
	DB.dbMeta.fileMeta[0].segNum = SEGMENT_NUM;
	DB.dbMeta.fileMeta[0].state = 1;
	memset(DB.dbMeta.fileMeta[0].segList, -1, sizeof(struct Segment) * SEGMENT_NUM);

	printf("dbMeta set done.\n");

	// 为空闲空间映射表分配空间，所有的初始化为-1，表示空闲
	DB.freeSpaceBitMap = (unsigned long *)malloc(DB.dbMeta.bitMapSize);
	memset(DB.freeSpaceBitMap, -1, DB.dbMeta.bitMapSize);

	FILE *dbFile = fopen(fileName, "wb");
	// 把初始化的相关参数写到数据库文件头部
	fwrite(&(DB.dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 把bitMap写到数据库文件中
	fseek(dbFile, DB.dbMeta.bitMapAddr, SEEK_SET);
	fwrite(DB.freeSpaceBitMap, DB.dbMeta.bitMapSize, 1, dbFile);

	fclose(dbFile);
}

void storage_showDbInfo(struct Storage *DB){

}

int storage_memToDisk(struct Storage *DB){

	return 0;
}

bool tupleInsert(struct Storage *DB,int length, int FileID, char *str){

}
