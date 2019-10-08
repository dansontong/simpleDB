#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

#include "config.h"

// buf初始化
// 参数待定


// ==================== data structure ====================
//缓冲块描述数据
struct BufMeta{
	long pageNo;   //页号
	long visitTime;//访问时间
	bool isChanged;//是否更改
};

// 缓冲池数据
struct BufPool{
	char *data[BUFFER_NUM][PAGE_SIZE];
	struct BufMeta bufMeta[BUFFER_NUM];
};

// ==================== manager function ====================
// 缓冲池管理
class BufMgr{
public:
	BufMgr();
	~BufMgr();

	void BufInit(void);

	void ReadBuffer(std::string tag);
};

// extern void BufInit(void);

// buf 对外提供buf的读取函数
// 参数待定
// 基本思路：根据tag去返回一个块的数据
// tag与数据相关，确保唯一性
// extern void ReadBuffer(std::string tag);

#endif