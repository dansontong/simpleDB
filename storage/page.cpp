#include "page.h"
#include "storage.h"


 int page_isEmpty(unsigned long bit_map,int position)
 {
	unsigned long result = 0x00000001;
	result = result<<(SIZE_OF_LONG-position);
	result = result & bit_map;
	if (result == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void page_setbitmap(unsigned long *bit_map,int position,int value)
{
	if(value!=0&&value!=1){
		printf("value的值不符合规则!，只能是0或1。/n");
		return ;
	}
	if(value==page_isEmpty(*bit_map,position)){
		return;
	}
	unsigned long  result = 0x00000001;
	result = result<<(SIZE_OF_LONG-position);
	if(value==1){
		*bit_map = result+*bit_map;
	}
	else {
		result = ~result;
		*bit_map = *bit_map&result;
	}
	
}


int page_requestPage(struct Storage *DB, long NeededPageNum)
{
	int flag = 0;

	for(int i=0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		
		if(page_isEmpty(*(DB->freeSpaceBitMap)+p_num,position)==0){
			int count = 0;
			for(int j=i;j<DB->dbMeta.blockNum;j++){
				p_num = j/(8*sizeof(long));
				position = j- p_num*8*sizeof(long)+1;
				if(page_isEmpty(*(DB->freeSpaceBitMap)+p_num,position)==0){
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
					page_setbitmap(DB->freeSpaceBitMap+p_num,position,1);
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

void page_recove_onepage(struct Storage *DB,int PageNo)
{

	int p_num = PageNo/(8*sizeof(long));

	int position = PageNo - p_num*8*sizeof(long)+1;
	page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
}

void page_recover_allpages(struct Storage *DB)
{
	for(int i= 0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
	}
}
