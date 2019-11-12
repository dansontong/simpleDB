#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"

//==================== global variable ====================
extern struct DataBase *DB; /* 全局共享 */

// void file_Init(struct DataBase *db)
// {
// 	DB = db;
// }
/**************************************************
 *                  file                          *
 **************************************************/
//-1表示文件创建失败
int file_newFile(FILE_TYPE fileType, long NeededPageNum){
	if(DB->dbMeta.currFileNum>=MAX_FILE_NUM||DB->dbMeta.blockFree<NeededPageNum){
		printf("空闲空间不足，文件创建失败！/n");
		exit(0);	
	}
	int id = DB->dbMeta.currFileNum;
	DB->dbMeta.currFileNum++;
	long NewPages = page_requestPage(NeededPageNum);
	if(NewPages>=0){
		int i,j;
		for(i=0,j =NewPages;i<NeededPageNum,j<(DB->dbMeta.blockNum);i++,j++){
			struct PageMeta pagemeta;
			pagemeta.recordNum = 0;
			pagemeta.pageNo = j;
			if(i==0){
				pagemeta.prePageNo=-1;//-1表示没有前页
				pagemeta.nextPageNo=j+1;
			}
			else{
				pagemeta.prePageNo=j-1;
				if(j==NewPages+NeededPageNum-1){
					pagemeta.nextPageNo=-1;//-1表示没有后页
					
				}
				else{
					pagemeta.nextPageNo = j+1;
				}
			}
			pagemeta.freeSpace = PAGE_SIZE - sizeof(pagemeta);
			rewind(DB->dbFile);
			fseek(DB->dbFile,DB->dbMeta.dataAddr+pagemeta.pageNo*PAGE_SIZE,SEEK_SET);
			fwrite(&pagemeta,sizeof(pagemeta),1,DB->dbFile);
		}
		for( i = 0;i<MAX_FILE_NUM;i++){
			//printf("DB->dbMeta.fileMeta[i].id %d: \n", DB->dbMeta.fileMeta[i].id);
			if(DB->dbMeta.fileMeta[i].id<0){
				break;
			}
		}
		DB->dbMeta.fileMeta[i].id=id;
		DB->dbMeta.fileMeta[i].fileType=fileType;
		DB->dbMeta.fileMeta[i].firstPageNo=NewPages;
		DB->dbMeta.fileMeta[i].pageNum=NeededPageNum;
		DB->dbMeta.fileMeta[i].segList[i].id=id;
		DB->dbMeta.fileMeta[i].segList[i].firstPageNo=NewPages;
		DB->dbMeta.fileMeta[i].segList[i].pageNum=NeededPageNum;
		DB->dbMeta.blockFree=DB->dbMeta.blockFree-NeededPageNum;
		file_print_freepace();
		
	}
	else{
		printf("未有足够的连续存储空间，文件创建失败！/n");
		return -1;//-1表示创建失败
	}
	return id;
	
}

struct Record file_writeFile(int FileID, int length,char *str){
	Record record; // 返回刚存入的记录描述信息

	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].segList[i].id==FileID){						//
			querypage=DB->dbMeta.fileMeta[i].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}

	// long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				
	// long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	int fileno = i;
	int sizeofpageMeta = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);									//读取该文件的信息
	rewind(DB->dbFile);					
	bool isfound = false;
	struct PageMeta pageMeta;
	struct BufTag buftag = Buf_GenerateTag(CurpageNo);
	memcpy(&pageMeta,Buf_ReadBuffer(buftag),sizeofpageMeta);						//读取第一页的内容并存放在pagehead里
	OffsetInPage preoffset,curoffset;							//页里的记录索引的结构体，定义在file.h里
	long currecordpos,curoffsetpos;								//前一个是指当前记录索引的位置，第二个是指当前记录的位置
	for(int i=0;i<pagenum;i++){									//该循环是为了遍历所有的页找出能存放该记录的页
		// printf("page:%d,空闲空间：%ld\n",i,pageMeta.freeSpace);
		if(pageMeta.freeSpace<=length+sizeofrecord){
			if(pageMeta.nextPageNo==-1){
				break;
			}
			CurpageNo = pageMeta.nextPageNo;
			buftag = Buf_GenerateTag(CurpageNo);
			memcpy(&pageMeta,Buf_ReadBuffer(buftag),sizeofpageMeta);
			continue;	
		}
		else{
			memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta,sizeofrecord);
			isfound = true;
			if(pageMeta.recordNum==0){
				curoffset.recordID = 0;
				curoffset.offset = length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpageMeta;
				curoffsetpos =  PAGE_SIZE - length;
			}
			else{
				memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+(pageMeta.recordNum-1)*sizeofrecord,sizeofrecord);
				curoffset.recordID = pageMeta.recordNum;
				curoffset.offset = preoffset.offset+length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpageMeta + sizeofrecord*pageMeta.recordNum;//currecordpos等价于,页顶已被占据的空间大小。页前面放record描述信息，具体record数据存在页底。
				curoffsetpos = PAGE_SIZE - preoffset.offset-length;
			}
			
		}
		pageMeta.recordNum++;
		pageMeta.freeSpace=pageMeta.freeSpace-length-sizeofrecord;
		memcpy(Buf_ReadBuffer(buftag),&pageMeta,sizeofpageMeta);
		memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
		memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
		break;						//找到后就break
	}
	if(!isfound){					//若遍历完没有页就新申请一个页。
		long CurpageNo = page_requestPage(1);
		if(CurpageNo>=0){
			DB->dbMeta.blockFree=DB->dbMeta.blockFree-1;
			// file_print_freepace();
			struct PageMeta pagemeta; //pagehead就是未申请前最后一个页
			pagemeta.nextPageNo=-1;
			pagemeta.prePageNo=pageMeta.pageNo;				
			pagemeta.pageNo=CurpageNo;
			pageMeta.nextPageNo = CurpageNo;		//将这页加在这个文件中		
			pagemeta.recordNum = 1;
			pagemeta.freeSpace = PAGE_SIZE - length - sizeofpageMeta - sizeofrecord;
			curoffsetpos = PAGE_SIZE-length;
			currecordpos = sizeofpageMeta;
			curoffset.recordID = 0;
			curoffset.offset = length;
			curoffset.isDeleted = false;
			buftag = Buf_GenerateTag(pagenum+1);
			memcpy(Buf_ReadBuffer(buftag),&pagemeta,sizeofpageMeta);
			memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
			memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
			memcpy(Buf_ReadBuffer(buftag),&pageMeta,sizeofpageMeta);

			DB->dbMeta.fileMeta[fileno].pageNum++;// done-TODO:mnb-1: unComment this line, and buffer.schedule suck in.
		}
	}
	record.pageNo = CurpageNo;
	record.recordID = curoffset.recordID;
	printf("----------------------- pagemeta.recordNum: %d -------------------\n", pageMeta.recordNum);
	return record;
}
void file_readFile(int FileID,char *str){
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){
			break;
		}
	}
	int sizeofpageMeta = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	OffsetInPage preoffset,curoffset;
	struct PageMeta pageMeta;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);		//根据页号从缓冲区调取页的内容
		memcpy(&pageMeta,Buf_ReadBuffer(buftag),sizeofpageMeta);//打印页的基本信息
		printf("第%d号文件中的第%d个页面\n",FileID,i+1);
		printf("页号：%ld\n",pageMeta.pageNo);
		printf("前继页号：%ld\n",pageMeta.prePageNo);
		printf("后继页号：%ld\n",pageMeta.nextPageNo);
		printf("记录个数:%d\n",pageMeta.recordNum);
		printf("空闲空间：%ld\n",pageMeta.freeSpace);
		if(pageMeta.recordNum>0){
			for(int j=0;j<pageMeta.recordNum;j++){
				int readlength;
				if(j==0){																	//打印页的每条记录
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta,sizeofrecord);
					readlength = curoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
					printf("%s\n",str);
				}
				else{
					preoffset = curoffset;
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+sizeofrecord*j,sizeofrecord);
					readlength = curoffset.offset-preoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
					printf("%s\n",str);
				}
			}
		}
		long nextPno = pageMeta.nextPageNo;
		if(nextPno==-1){
			break;
		}
		else{
			CurpageNo = nextPno;
		}
	}
}

void file_deleteFile(int FileID){
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){			//找到文件对应的页
			break;
		}
	}
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;			//读取第一页的信息
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	long pageAddr = DB->dbMeta.dataAddr +CurpageNo * PAGE_SIZE;
	int sizeofpageMeta = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long nextPage = -1;
	struct PageMeta pageMeta;
	for(long j=0;j<pagenum;j++){							//遍历每一页
		rewind(DB->dbFile);
		fseek(DB->dbFile,pageAddr,SEEK_SET);
		size_t sizeRead = fread(&pageMeta,sizeofpageMeta,1,DB->dbFile);			//读取这一页的内容
		nextPage = pageMeta.nextPageNo;
		page_recover_onepage(pageMeta.pageNo);				//删除这一页
		if(nextPage>0){
			pageAddr = DB->dbMeta.dataAddr +nextPage * PAGE_SIZE;	//获取新的一页的地址
		}
		else{
			break;
		}
	}
	DB->dbMeta.blockFree += pagenum;
	DB->dbMeta.currFileNum--;
	DB->dbMeta.fileMeta[0].segList[i].type = -1;
	DB->dbMeta.fileMeta[0].segList[i].id = -1;
	DB->dbMeta.fileMeta[0].segList[i].firstPageNo = -1;
	DB->dbMeta.fileMeta[0].segList[i].pageNum = -1;
	
}
void file_read_sd(long pageno,char *bufferpath){
	rewind(DB->dbFile);
	fseek(DB->dbFile,DB->dbMeta.dataAddr+pageno*PAGE_SIZE,SEEK_SET);
	size_t sizeRead = fread(bufferpath,PAGE_SIZE,1,DB->dbFile);
}
void file_write_sd(long pageno,char *bufferpath){
	rewind(DB->dbFile);
	fseek(DB->dbFile,DB->dbMeta.dataAddr+pageno*PAGE_SIZE,SEEK_SET);
	fwrite(bufferpath,PAGE_SIZE,1,DB->dbFile);
}
void file_print_freepace(){
	printf("already used %ld blocks, still have %ld blocks vacant.\n",DB->dbMeta.blockNum-DB->dbMeta.blockFree,DB->dbMeta.blockFree);
	
}

void file_fseek(int fileID, long offset, int fromwhere)
{

}

/**************************************************
 *                  page                          *
 **************************************************/
int page_isEmpty(unsigned long bit_map,int position)
 {
	unsigned long result = 0x00000001;
	// bug 
	// Ô­Ê¼£º32-position
	result = result<<(SIZE_OF_LONG*8-position);
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
	result = result<<(SIZE_OF_LONG*8-position);
	if(value==1){
		*bit_map = result|*bit_map;
	}
	else {
		result = ~result;
		*bit_map = *bit_map&result;
	}
	
}

long page_requestPage(long NeededPageNum)
{
	if(DB->dbMeta.blockFree<0){
		printf("没有空闲的页，分配失败！\n");
		return -1;
	}
	
	long i,j;
	long p_num,position;
	long count;
	long NewPages;
	for( i=0;i<DB->dbMeta.blockNum;i++){
		p_num = i/(8*sizeof(long));
		position = i- p_num*8*sizeof(long)+1;
		count = 0;
		if(page_isEmpty(*(DB->freeSpaceBitMap+p_num),position)==0){
			for(j=i;j<DB->dbMeta.blockNum;j++){
				if(page_isEmpty(*(DB->freeSpaceBitMap+p_num),position)==0){
					count++;
				}
				else{
					break;
				}
				if(count==NeededPageNum){
					break;
				}
			}
			
		}
		if(count==NeededPageNum){
			break;	
		}
		else{
			i = i+count;
			
		}
	}
	if(count==NeededPageNum){
		NewPages=i;
		for(j=0;j<NeededPageNum;j++){
			p_num = (i+j)/(8*sizeof(long));
			position =i+j- p_num*8*sizeof(long)+1;
			page_setbitmap(DB->freeSpaceBitMap+p_num,position,1);
		}
		DB->dbMeta.blockFree-=NeededPageNum;
		return NewPages;
	}
	else{
		return -1;
	}
}

void page_recover_onepage(int PageNo)
{

	int p_num = PageNo/(8*sizeof(long));

	int position = PageNo - p_num*8*sizeof(long)+1;
	page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
}

void page_recover_allpages()
{
	for(int i= 0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
	}
}


/**************************************************
 *                  segment                       *
 **************************************************/

