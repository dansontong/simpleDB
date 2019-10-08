#ifndef STORAGE_SEGMENT_H
#define STORAGE_SEGMENT_H

#include "config.h"

// ==================== data structure ====================
struct Segment{
	int state;  // 段状态 
	int id;     // 段号
	int type;   // -----可用于标记共享，尚未使用
	long firstPageNo; // 该段的起始页号
	long pageNum;     // 该段的页数
};

// ==================== manager function ====================
class SegmentMgr
{
public:
	SegmentMgr();
	~SegmentMgr();
	
};

#endif