#ifndef STORAGE_PAGE_H
#define STORAGE_PAGE_H

#include "config.h"

// ==================== data structure ====================
// 页描述信息
struct PageMeta{
	long pageNo;      // 页号
	long prePageNo;	  // 前继页号
	long nextPageNo;  // 后继页号
	int recordNum;    // 该页存储的记录个数
	long freeSpace;   // 该页的空闲空间大小
};
struct OffsetInPage{
	int recordID;
	int offset;         //该记录相对于块尾地址的偏移量
	bool isDeleted;
};
// ==================== manager function ====================
// 页管理

int page_isEmpty(unsigned long bit_map,int position);
void page_setbitmap(unsigned long *bit_map,int position,int value);

int page_requestPage(struct Storage *DB, long NeededPageNum);

void page_recove_onepage(struct Storage *DB,int PageNo);
void page_recover_allpages(struct Storage *DB);

#endif
