#ifdef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"

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
class FileMgr{
public:
	FileMgr();
	~FileMgr();

	void writeFile(struct Storage *storage, int fileID, int length, char *str);
	void NewFile(struct Storage *DB,int type, long NeededPageNum);
	void readFile(struct Storage *DB,int FileID,char *str);
	void deleteFile(struct Storage *DB,int FileID);
};

#endif