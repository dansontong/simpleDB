#include "file.h"

void writeFile(struct Storage *storage, int fileID, int length, char *str){

}

int createFile(struct Storage *dbHead, int type, long requestBlockNum){
	if(requestBlockNum > dbHead.blockFree){
		printf("error: no enough free blocks, quit.\n");
		return -1;
	}
	if(dbHead.currFileNum + 1 > MAX_FILE_NUM){
		printf("error: reached MAX_FILE_NUM already, quit.\n");
		return -1;
	}
	// traverse bitMap look for free blocks
	for(int i=0; i<dbHead.bitMapSize; i++){
		
	}
	return 0;
}