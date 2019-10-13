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

struct Attribute{
	char name[MAX_NAME_LENGTH]; //属性名
	int length; 
	DATA_TYPE type;//数据类型
	int offset; //偏移位置
	bool notNull;
};

struct Table{
	int fileID;   //表文件的文件号
	char tableName[MAX_NAME_LENGTH];   //表名
	Attribute attr[MAX_ATTRIBUTE_NUM]; //属性列表
	int attrNum;  //属性个数
	int attrLength;//属性总长度
};

// 数据存储信息
struct Storage{
	FILE *dbFile;
	DbMeta dbMeta;
	//BufPool bufPool;
	unsigned long *freeSpaceBitMap;
	Table dataDict[MAX_FILE_NUM];
};


// ==================== manager function ====================
// 存储管理

void storage_createDbFile(char *fileName); 
void storage_initDB(struct Storage *DB, char *fileName);
void storage_closeDB(struct Storage *DB);
void storage_showDbInfo(struct Storage *DB);
void storage_showSegList(struct Storage *DB, int fileID); 


//定时把内存中缓冲区的所有数据写到磁盘
int storage_memToDisk(struct Storage *DB);


// 记录相关
void recordInsert(struct Storage *DB, int dictID, char *str);


// 表相关
int createTable(struct Storage *DB, char *str);
void insertAttr(Table *table,const char *name, DATA_TYPE type, int length,bool notNull);
int getDictIDbyName(char *tableName);

//字典相关
int readDataDictionary(struct Storage *DB);

#endif