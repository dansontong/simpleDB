#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

#include "config.h"

#define BUF_FREE_END -1		   // free list内指针的空值
#define BUF_FREE_LIST_EMPTY -1 // free list的头指针为空的情况
// ==================== data structure ====================
enum BufMode
{
	BM_free = 0,
	BM_isValid,
	BM_isDirty,
	BM_ioProgress,
	BM_error,
	BM_Writing
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

// ==================== extern function ====================
// 缓冲池初始化
void Buf_Init();

// 缓冲池释放
void Buf_Free(void);

// buf 对外提供buf的读取函数
char *Buf_ReadBuffer(BufTag tag);

// buf 对外提供的写函数
void Buf_WriteBuffer(BufTag tag, char *data);

// 在空闲的缓存块中申请一块空白的缓存块，返回缓存块的起始地址
char *Buf_AllocBlock(BufTag tag);

BufTag Buf_GenerateTag(long pageNo);
// ==================== inner function ====================
void Buf_Schedule();					// 调用淘汰算法并且删除被淘汰的缓存块
long Buf_LoadPage(BufTag btag);			// 从外部文件中加载指定块
bool Buf_Remove(long bufId);			// 删除指定id的缓存块，bufId取值为[0, BUFFER_NUM)
long Buf_QuickLookup(BufTag btag);		// 根据buf tag 迅速查询是否有缓存,如果不存则返回-1
char *Buf_GetBlock(long bufId);			// 根据下标获取缓存内容
long Buf_StrategyLRU();					// 淘汰算法LRU
long Buf_AllocBlock_inner(BufTag btag); // 申请一个新的缓存块
void Buf_HitBlockById(long bufid);			// 更新bufmeta的调度信息
void Buf_HitBlockByTag(BufTag btag); 
char *Buf_ReadBuffer_inner(BufTag btag);
void Buf_WriteBuffer_inner(BufTag tag, char *data);
void Buf_ClearBufTag(BufTag* btag);
void Buf_PrintInfo();			// 打印buffer的信息

// ==================== mem function ====================
void *MemAllocNoThrow(std::size_t alloc_size);
void MemFree(void *start);

void bufToDisk();

// ==================== utils function ====================
long UTCNowTimestamp();
bool CMPBufTag(BufTag tag1, BufTag tag2);
void TestLoad(char *dest);
#endif