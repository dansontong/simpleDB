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
	int dataDictFid; // 数据字典文件号

	struct FileMeta fileMeta[MAX_FILE_NUM];
};

// 数据存储信息
struct Storage{
	FILE *dataPath;
	DbMeta dbMeta;
	BufPool bufPool;
	unsigned long *freeSpaceBitMap;
};

struct Attribute{
	char name[MAX_NAME_LENGTH]; //属性名
	int length; 
	DATA_TYPE type;//数据类型
};

struct Table{
	int fileID;   //表文件的文件号
	char tableName[MAX_NAME_LENGTH];   //表名
	Attribute attr[MAX_ATTRIBUTE_NUM]; //属性列表
	int attrNum;  //属性个数
	int recordNum;//记录个数
};

// ==================== manager function ====================
// 存储管理

void storage_createDbFile(char *fileName); 
void storage_initDB(struct Storage *storage, char *fileName);
void storage_showDbInfo(struct Storage *storage);
void storage_showSegList(struct Storage *storage, int fileID); 


//定时把内存中缓冲区的所有数据写到磁盘
int storage_memToDisk(struct Storage *storage);


// 记录相关
bool recordInsert(char *str);


// 表相关
bool creatTable(char *str);

#endif