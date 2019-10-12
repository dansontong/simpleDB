#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

#include "config.h"

#define BUF_FREE_END -1	// free list内指针的空值
#define BUF_FREE_LIST_EMPTY -1 // free list的头指针为空的情况
// ==================== data structure ====================
enum BufMode
{
	BM_free = 0,
	BM_isValid,
	BM_isDirty,
	BM_ioProgress,
	BM_error
};

// 缓冲块的标签，数据的描述符
// 查询者自行组织buffer的tag，该tag作为查询的关键词
struct BufTag
{
	long pageNo; //页号
};

//缓冲块描述数据
struct BufMeta
{
	BufTag bTag;	 // buf的数据tag
	long bufId;		 // buf的下标，[0, BUFFER_NUM)
	long visitTime;  //访问时间
	long fNext;		 // 空闲块的下一个指针，分配新块时需要
	BufMode bufMode; // 是否为脏数据
};

// 缓冲池数据
struct BufPool
{
	char *data;		  // [BUFFER_NUM][BLOCK_SIZE]; // 不采用二维数组的形式，直接分配blocksize * buffer_num 的大小，自行管理
	BufMeta *bufMeta; // [BUFFER_NUM];
};

// ==================== manager function ====================
// 缓冲池管理

// void BufInit(void);

// buf 对外提供buf的读取函数
// 参数待定
// 基本思路：根据tag去返回一个块的数据
// tag与数据相关，确保唯一性
char *buf_ReadBuffer(BufTag tag);

// BufPool bufpool;
long freeBlockHead;

void buf_Schedule();
long buf_LoadPage(BufTag btag);
bool Buf_Remove(long bufId);		  // bufId取值为[0, BUFFER_NUM)
long buf_QuickLookup(BufTag btag); // 根据buf tag 迅速查询是否有缓存,如果不存则返回-1
char *buf_GetBlock(long bufId);	// 根据下标获取缓存内容
long buf_StrategyLRU();			// 淘汰算法LRU


#endif