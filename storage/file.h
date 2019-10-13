#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"
#include "page.h"
#include "segment.h"
#include "buffer.h"

// ==================== data structure ====================
//文件描述信息
struct FileMeta{
	int id;       // 文件号
	char name[20];// 文件名,最长20个字符
	int state;    // 文件状态
	int segNum;   // 文件段数
	struct Segment segList[SEGMENT_NUM];// 段表
};


// ==================== manager function ====================
// 文件管理

void file_writeFile(struct Storage *storage, int fileID, int length, char *str);
int file_newFile(struct Storage *DB,int type, long NeededPageNum);
void file_readFile(struct Storage *DB,int FileID,char *str);
void file_deleteFile(struct Storage *DB,int FileID);
void file_read_sd(struct Storage *DB,long pageno,char *bufferpath);
void file_write_sd(struct Storage *DB,long pageno,char *bufferpath);
void file_print_freepace(struct Storage *DB);
#endif