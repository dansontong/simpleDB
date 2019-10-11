#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"
#include "file.h"
#include "buffer.h"
#include "page.h"
#include "segment.h"

// ==================== data structure ====================
// 数据库文件的描述信息
struct DbMeta{
	long blockSize;//数据块大小，默认4k
	long blockNum; //总块数
	long blockFree;//空闲块数
	long bitMapSize; //位示图大小
	long bitMapAddr; //位示图的起始位置
	long dataSize; //数据区大小,默认1G
	long dataAddr; //数据区的起始位置
	int currFileNum; //当前的文件数，currFileNum < MAX_FILE_NUM

	struct FileMeta fileMeta[MAX_FILE_NUM];
};

// 数据存储信息
struct Storage{
	FILE *dataPath;
	DbMeta dbMeta;
	BufPool bufPool;
	unsigned long *freeSpaceBitMap;
};

// ==================== manager function ====================
// 存储管理

void createDbFile(char *fileName); 
void initDB(struct Storage *storage, char *fileName);
void showDbInfo(struct Storage *storage);
void showSegList(struct Storage *storage, int fileID); 


//定时把内存中缓冲区的所有数据写到磁盘
int memToDisk(struct Storage *storage);

#endif