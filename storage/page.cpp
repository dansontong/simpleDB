#include "page.h"
#include "storage.h"


 int bool_empty(unsigned long bit_map,int position){
	unsigned long result = 0x00000001;
	result = result<<(32-position);
	result = result & bit_map;
	if (result == 0) {
		return 0;
	}
	else {
		return 1;
	}
}


int setbitmap(unsigned long *bit_map,int position，int value){
	if(value!=0&&value!=1){
		printf("value的值不符合规则!/n");
		return ;
	}
	if(value==bool_empty(bit_map,position)){
		return;
	}
	unsigned long  result = 0x00000001;
	result = result<<(32-position);
	if(value==1){
		*bit_map = result+*bit_map;
	}
	else {
		result = ~result;
		*bit_map = *bitmap&result;
	}
	
}


int RequestPage(struct Storage *DB, long NeededPageNum){
	for(int i=0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		int flag = 0;
		if(bool_empty(*(DB->freeSpaceBitMap)+p_num,position)==0){
			int count = 0;
			for(int j=i;j<DB->dbMeta.blockNum;j++){
				p_num = j/(8*sizeof(long));
				position = j- p_num*8*sizeof(long)+1;
				if(bool_empty(*(DB->freeSpaceBitMap)+p_num,position)==0){
					count++;
				}
				else{
					break;
				}
			}
			if(count==NeededPageNum){
				int NewPages = i;
				for(int j=0;j<NeededPageNum;j++){
					p_num = (i+j)/(8*sizeof(long));
					position =i+j- p_num*8*sizeof(long)+1;
					setbitmap(*(*(DB->freeSpaceBitMap)+p_num),position,1);
				}
				flag = 1;
				return NewPages;
			}
			else{
				i = i+count;
			}
			
		}
	}
	if(flag ==0){
		return -1;
	}
}

void recove_onepage(struct Storage *DB,int PageNo){
	int p_num = PageNo/(8*sizeof(long);
	int position = PageNo - p_num*8*sizeof(long)+1;
	setbitmap(*(DB->freeSpaceBitMap)+p_num,position,0);
}

void recover_allpages(struct Storage *DB){
	for(int i= 0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		setbitmap(*(DB->freeSpaceBitMap)+p_num,position,0);
	}
}
