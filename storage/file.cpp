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
int file_newFile(int type, long NeededPageNum){
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
			if(DB->dbMeta.fileMeta[0].segList[i].id<0){
				break;
			}
		}
		DB->dbMeta.fileMeta[0].segList[i].id=id;
		DB->dbMeta.fileMeta[0].segList[i].type=type;
		DB->dbMeta.fileMeta[0].segList[i].firstPageNo=NewPages;
		DB->dbMeta.fileMeta[0].segList[i].pageNum=NeededPageNum;
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
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){						//
			querypage=DB->dbMeta.fileMeta[0].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}

	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	int fileno = i;
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);									//读取该文件的信息
	rewind(DB->dbFile);					
	bool isfound = false;
	struct PageMeta pagehead;
	struct BufTag buftag = Buf_GenerateTag(CurpageNo);
	memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);						//读取第一页的内容并存放在pagehead里
	OffsetInPage preoffset,curoffset;							//页里的记录索引的结构体，定义在file.h里
	long currecordpos,curoffsetpos;								//前一个是指当前记录索引的位置，第二个是指当前记录的位置
	for(int i=0;i<pagenum;i++){									//该循环是为了遍历所有的页找出能存放该记录的页
		if(pagehead.freeSpace<=length+sizeofrecord){
			if(pagehead.nextPageNo==-1){
				break;
			}
			CurpageNo = pagehead.nextPageNo;
			buftag = Buf_GenerateTag(CurpageNo);
			memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
			continue;	
		}
		else{
			memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead,sizeofrecord);
			isfound = true;
			if(pagehead.recordNum==0){
				curoffset.recordID = 0;
				curoffset.offset = length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead;
				curoffsetpos =  PAGE_SIZE - length;
				
				
			}
			else{
				memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+(pagehead.recordNum-1)*sizeofrecord,sizeofrecord);
				curoffset.recordID = pagehead.recordNum;
				curoffset.offset = preoffset.offset+length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead + sizeofrecord*pagehead.recordNum; //currecordpos 等价于，页顶已被占据的空间大小。页前面放record描述信息，具体record数据存在页底。
				curoffsetpos = PAGE_SIZE - preoffset.offset-length;
			}
			
		}
		pagehead.recordNum++;
		pagehead.freeSpace=pagehead.freeSpace-length-sizeofrecord;
		memcpy(Buf_ReadBuffer(buftag),&pagehead,sizeofpagehead);
		memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
		memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
		break;						//找到后就break
	}
	if(!isfound){					//若遍历完没有页就新申请一个页。
		long CurpageNo = page_requestPage(1);
		if(CurpageNo>=0){
			DB->dbMeta.blockFree=DB->dbMeta.blockFree-1;
			file_print_freepace();
			struct PageMeta pagemeta; //pagehead就是未申请前最后一个页
			pagemeta.nextPageNo=-1;
			pagemeta.prePageNo=pagehead.pageNo;				
			pagemeta.pageNo=CurpageNo;
			pagehead.nextPageNo = CurpageNo;		//将这页加在这个文件中				
			pagemeta.recordNum = 1;
			pagemeta.freeSpace = PAGE_SIZE - length - sizeofpagehead - sizeofrecord;
			curoffsetpos = PAGE_SIZE-length;
			currecordpos = sizeofpagehead;
			curoffset.recordID = 0;
			curoffset.offset = length;
			curoffset.isDeleted = false;
			buftag = Buf_GenerateTag(pagenum);
			memcpy(Buf_ReadBuffer(buftag),&pagemeta,sizeofpagehead);
			memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
			memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
			memcpy(Buf_ReadBuffer(buftag),&pagehead,sizeofpagehead);

			DB->dbMeta.fileMeta[0].segList[fileno].pageNum++;
		}
	}
	record.pageNo = CurpageNo;
	record.recordID = curoffset.recordID;
	return record;
}
void file_readFile(int FileID,char *str){
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){
			break;
		}
	}
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	OffsetInPage preoffset,curoffset;
	struct PageMeta pagehead;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);		//根据页号从缓冲区调取页的内容
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);//打印页的基本信息
		printf("第%d号文件中的第%d个页面\n",FileID,i+1);
		printf("页号：%ld\n",pagehead.pageNo);
		printf("前继页号：%ld\n",pagehead.prePageNo);
		printf("后继页号：%ld\n",pagehead.nextPageNo);
		printf("记录个数；%d\n",pagehead.recordNum);
		printf("空闲空间：%ld\n",pagehead.freeSpace);
		if(pagehead.recordNum>0){
			for(int j=0;j<pagehead.recordNum;j++){
				int readlength;
				if(j==0){																	//打印页的每条记录
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead,sizeofrecord);
					readlength = curoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
					printf("%s\n",str);
				}
				else{
					preoffset = curoffset;
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*j,sizeofrecord);
					readlength = curoffset.offset-preoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
					printf("%s\n",str);
				}
			}
		}
		long nextPno = pagehead.nextPageNo;
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
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long nextPage = -1;
	struct PageMeta pagehead;
	for(long j=0;j<pagenum;j++){							//遍历每一页
		rewind(DB->dbFile);
		fseek(DB->dbFile,pageAddr,SEEK_SET);
		size_t sizeRead = fread(&pagehead,sizeofpagehead,1,DB->dbFile);			//读取这一页的内容
		nextPage = pagehead.nextPageNo;
		page_recover_onepage(pagehead.pageNo);				//删除这一页
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
	printf("已经用了%ld块，还空闲%ld块\n",DB->dbMeta.blockNum-DB->dbMeta.blockFree,DB->dbMeta.blockFree);
	
}

bool file_getrecord(long pageNo,int recordID,char *record){
	
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	OffsetInPage curoffset,preoffset;
	struct BufTag buftag = Buf_GenerateTag(pageNo);
	struct PageMeta pagehead;
	memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
	if(recordID<0||recordID>pagehead.recordNum){
		return false;
	}
	if(recordID>0){//判断是否是第一个记录
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*recordID,sizeofrecord);
		memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*recordID-1,sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset-preoffset.offset);//记录的位置为页的起始位置加上pagesize-记录距离页尾的距离，记录长度为该记录距离页尾的位置减去上一条记录距离页尾的位置
		return true;
	}
	else{
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*recordID,sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset);//第一条记录的长度为该记录距离页尾的距离
		return true;
	}
	return false;
}

bool file_getrecordAttribute(long pageNo,int recordID,char* tablename,char* Attributename,char* Attribute,int* posOffset){
	char *record;
	if(file_getrecord(pageNo,recordID,record)){//返回该条记录
		int i=0;
		for(i=0;i<MAX_FILE_NUM;i++){
			if(DB->dataDict[i].fileID<0){
				return false;
			}
			else{
				if(strcmp(DB->dataDict[i].tableName,tablename)==0){//查找表
					int j=0;
					for(j=0;j<DB->dataDict[i].attrNum;j++){//查找属性，根据属性名找到属性在记录中的具体位置
						if(strcmp(DB->dataDict[i].attr[j].name,Attributename)==0){
							if(j<DB->dataDict[i].attrNum-1){
								memcpy(Attribute,record+DB->dataDict[i].attr[j].offset,DB->dataDict[i].attr[j+1].offset-DB->dataDict[i].attr[j].offset);//一般情况：位置为record的起始地址加上属性的偏移量，长度为该下一条属性的偏移量减去该属性的偏移量
								posOffset=record+DB->dataDict[i].attr[j].offset;
								return true;
							}
							else{
								memcpy(Attribute,record+DB->dataDict[i].attr[j].offset,DB->dataDict[i].attrLength-DB->dataDict[i].attr[j].offset);//当该属性为最后一个属性时，长度为总属性长度减去该属性的偏移量
								posOffset=record+DB->dataDict[i].attr[j].offset;
								return true;
							}
							
						}
					}
				}
			}
		}
	}
	else{
		return false;
	}
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

int page_requestPage(long NeededPageNum)
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

