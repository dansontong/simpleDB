#ifndef CORE_DATABASE_H
#define CORE_DATABASE_H

#include "config.h"
#include "file.h"
#include "buffer.h"
// #include "page.h"
// #include "segment.h"

// ==================== data structure ====================
// 数据库文件的描述信息
struct DbMeta{
	// long blockSize;//数据块大小，默认4k
	long blockNum; //总块数
	long blockFree;//空闲块数
	// long bitMapSize; //位示图大小
	// long bitMapAddr; //位示图的起始位置
	// long dataSize; //数据区大小,默认1G
	// long dataAddr; //数据区的起始位置
	int currFileNum; //当前的文件数，currFileNum < MAX_FILE_NUM
	// int dataDictFid; // 数据字典文件号

	struct FileMeta fileMeta[MAX_FILE_NUM];
};

// 取消这个结构体，因为只有indexFileID有用，已经加到Attrbute结构体中
// struct Index{ 
// 	int fileID;   //索引文件的文件号
// 	char tableName[MAX_NAME_LENGTH];//表名
// 	char attrName[MAX_NAME_LENGTH]; //属性名
// };

struct Attribute{
	char name[MAX_NAME_LENGTH]; //属性名
	int length; 
	DATA_TYPE type;//数据类型
	int offset; //偏移位置
	bool notNull;
	int indexFileID;
};

struct Table{
	int fileID;   //表文件的文件号
	char tableName[MAX_NAME_LENGTH];   //表名
	Attribute attr[MAX_ATTRIBUTE_NUM]; //属性列表
	int attrNum;  //属性个数
	int recordLength;//记录总长度
	int recordNum;  //属性个数
};

// 数据存储信息
struct DataBase{
	FILE *dbFile;
	DbMeta dbMeta;
	//BufPool bufPool;
	unsigned long *freeSpaceBitMap;
	Table dataDict[MAX_FILE_NUM];
};


// ==================== manager function ====================
// 存储管理

void initDB(struct DataBase *DB, char *fileName);
void closeDB();
void deleteDB();
void showDbInfo();
void createDbFile(char *fileName); 
void database_showSegList(int fileID); 


//定时把内存中缓冲区的所有数据写到磁盘
int memToDisk();
int saveDbHead();


//字典相关
int readDataDictionary();
int getDictIDbyTableName(char *tableName);


//交互相关interact
void dosMain();
void winMain();

#endif