#include "file.h"
#include "storage.h"

void FileMgr::writeFile(struct Storage *storage, int fileID, int length, char *str){

}

int FileMgr::createFile(struct Storage *dbHead, int type, long requestBlockNum){
	if(requestBlockNum > dbHead->dbMeta.blockFree){
		printf("error: no enough free blocks, quit.\n");
		return -1;
	}
	if(dbHead->dbMeta.currFileNum + 1 > MAX_FILE_NUM){
		printf("error: reached MAX_FILE_NUM already, quit.\n");
		return -1;
	}
	// traverse bitMap look for free blocks
	for(int i=0; i<dbHead->dbMeta.bitMapSize; i++){

	}
	return 0;
}

FileMgr::FileMgr(){}

FileMgr::~FileMgr(){}