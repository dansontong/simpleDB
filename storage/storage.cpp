#include <stdlib.h>
#include "storage.h"

void StorageMgr::initial(struct Storage *storage, char *fileName){
	FILE *dbFile = fopen(fileName, "rb");
	if(dbFile == NULL){
		printf("DataBase isn't exist, creating new dataBase.\n");
		this->createDbFile(fileName);
		dbFile = fopen(fileName, "rb");
	}

	fread(&(storage->dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 为空闲空间的位示图bitMap分配空间
	storage->freeSpaceBitMap = (unsigned long *)malloc(storage->dbMeta.bitMapSize);
	//rewind(dbFile);
	// 在文件中定位到bitMap开始的位置，令文件指针指向这里
	fseek(dbFile, storage->dbMeta.bitMapAddr, SEEK_SET);
	fread(storage->freeSpaceBitMap, storage->dbMeta.bitMapSize, 1, dbFile);// 从文件中读取bitMap的内容
	fclose(dbFile);
}

void StorageMgr::createDbFile(char *fileName){
	struct Storage storage;
	storage.dbMeta.blockSize = BLOCK_SIZE;
	storage.dbMeta.blockNum = FILE_DATA_SIZE / BLOCK_SIZE; // 256*1024
	storage.dbMeta.blockFree = storage.dbMeta.blockNum;
	storage.dbMeta.bitMapSize = BIT_MAP_SIZE;
	storage.dbMeta.bitMapAddr = BIT_MAP_ADDR;
	storage.dbMeta.dataSize = FILE_DATA_SIZE;
	storage.dbMeta.dataAddr = FILE_DATA_ADDR;
	storage.dbMeta.currFileNum = 0;
	//storage.dbMeta.fileMeta[MAX_FILE_NUM] = new FileMeta[MAX_FILE_NUM];

	storage.dbMeta.fileMeta[0].id = 0;
	strcpy(storage.dbMeta.fileMeta[0].name, "0");
	storage.dbMeta.fileMeta[0].segNum = SEGMENT_NUM;
	storage.dbMeta.fileMeta[0].state = 1;
	memset(storage.dbMeta.fileMeta[0].segList, -1, sizeof(struct Segment) * SEGMENT_NUM);

	// 为空闲空间映射表分配空间，所有的初始化为-1，表示空闲
	storage.freeSpaceBitMap = (unsigned long *)malloc(storage.dbMeta.bitMapSize);
	memset(storage.freeSpaceBitMap, -1, storage.dbMeta.bitMapSize);

	FILE *dbFile = fopen(fileName, "wb");
	// 把初始化的相关参数写到数据库文件头部
	fwrite(&(storage.dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 把bitMap写到数据库文件中
	fseek(dbFile, storage.dbMeta.bitMapAddr, SEEK_SET);
	fwrite(storage.freeSpaceBitMap, storage.dbMeta.bitMapSize, 1, dbFile);

	fclose(dbFile);
}

void StorageMgr::showDbInfo(struct Storage *storage){

}

int memToDisk(struct Storage *storage){

	return 0;
}

StorageMgr::StorageMgr(){}

StorageMgr::~StorageMgr(){}
