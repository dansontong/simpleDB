#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"
#include "buffer.h"

/*       段页式文件管理模块-头文件
 * 该模块内部可分为setment,page,file三个子模块。
 * 子模块分别在不同的cpp文件中实现对应的函数。
 *
**/

// ==================== data structure ====================
// 页描述信息
struct PageMeta{
	long pageNo;      // 页号
	long prePageNo;	  // 前继页号
	long nextPageNo;  // 后继页号
	long freeSpace;   // 该页的空闲空间大小
	int recordNum;    // 该页存储的记录个数
};
struct OffsetInPage{
	int recordID;
	int offset;         //该记录相对于块尾地址的偏移量
	bool isDeleted;
};

//段描述信息
struct Segment{
	int state;  // 段状态 
	int id;     // 段号
	int type;   // -----可用于标记共享，尚未使用
	long firstPageNo; // 该段的起始页号
	long pageNum;     // 该段的页数
};

//文件描述信息
struct FileMeta{
	int id;       // 文件号
	long pageNum; // 文件页数	
	long firstPageNo;// 文件段数
	char name[20];// 文件名,最长20个字符
	int state;    // 文件状态
	int segNum;   // 文件段数
	int fileType;
	struct Segment segList[SEGMENT_NUM];// 段表
};

struct Record{
	long pageNo;   //页号
	int recordID;  //页内第几个记录
	char* posOffset;//地址
};

// ==================== manager function ====================
// void file_Init(struct DataBase *DB);

// 文件管理-实现在file部分
struct Record file_writeFile(int fileID, int length, char *str);
int file_newFile(FILE_TYPE fileType, long NeededPageNum);
void file_readFile(int FileID,char *str);
void file_deleteFile(int FileID);
void file_print_freepace();

void file_read_sd(long pageNo, char *bufferpath); //提供给buffer模块，读取磁盘
void file_write_sd(long pageNo, char *bufferpath);//提供给buffer模块，写入磁盘

// 对外提供普通文件操作
void file_fseek(int fileID, long offset, int fromwhere);//文件流 偏移量(用正负表示方向) 偏移位置(刘欢-B+树-需求)


// 页管理-实现在page部分
void writePage(long pageNo);//利用buffer,写指定的页
void readPage(long pageNo); //利用buffer,读指定的页
int page_isEmpty(unsigned long bit_map,int position);
void page_setbitmap(unsigned long *bit_map,int position,int value);

long page_requestPage(long NeededPageNum);

void page_recover_onepage(int PageNo);
void page_recover_allpages();


// 段管理-实现在segment部分




#endif