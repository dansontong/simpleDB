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

// ==================== manager function ====================
// 页管理

int bool_empty(unsigned long bit_map,int position);
void setbitmap(unsigned long *bit_map,int position,int value);
void RequestPage(struct Storage *DB, long NeededPageNum);
void recove_onepage(struct Storage *DB,int PageNo);
void recover_allpages(struct Storage *DB);

#endif
