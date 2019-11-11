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

int create_tmptable(Table table_1){
	int fileID = file_newFile(0,1);//0文件类型的标识值，0表示为临时表，type为段的标识值。
	int dictID = -1;
	for(int i=0;i<MAX_FILE_NUM; i++){
		if(DB->dataDict[i].fileID<0){
			DB->dataDict[i]= table_1;
			DB->dataDict[i].fileID = fileID;
			dictID=i;
			break;
		}
	}
	if(dictID<0){
		printf("数据库中表的数量太多，创建临时表失败！\n");
		return -1;
	}
	return dictID;
}

void insert_onerecord(int dictID,char *record){//dictID为DB->dataDict[]的下标
	int length = strlen(record);
	int fileID = DB->dataDict[dictID].fileID;  
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage<0){
		printf("该表对应的文件号不存在！\n");
		exit(0);
	}
	
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
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
		// printf("page:%d,空闲空间：%ld\n",i,pagehead.freeSpace);
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
		memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,record,length);
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
			memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,record,length);
			memcpy(Buf_ReadBuffer(buftag),&pagehead,sizeofpagehead);

			DB->dbMeta.fileMeta[fileno].pageNum++;
		}
		else{
			printf("记录插入失败！\n");
		}
	}
}

void HashRelation(struct dbSysHead *head, Relation rl, int pub_attr, multimap<int, long> *m) {
	int rl_fid = rl.fileID;
	long pageNo = head->desc.fileDesc[rl_fid].fileFirstPageNo;
	long pageNum = head->desc.fileDesc[rl_fid].filePageNum;

	for (int i = 0; i < pageNum; i++) {
		int mapNo = reqPage(head, pageNo);
		struct pageHead ph;
		memcpy(&ph, head->buff.data[mapNo], SIZE_PAGEHEAD);
		for (int j = 0; j < ph.curRecordNum; j++) {
			char *record = (char*)malloc(rl.recordLength);
			long logicID = getNextRecord(head, mapNo, j, record);
			char *val = (char*)malloc(rl.recordLength);
			getValueByAttrID(record, pub_attr, val);
			//暂时只考虑要进行hash的属性为int类型的情况
			int int_val = atoi(val);
			int bid = hashToBucket(int_val);
			m[bid].insert(pair<int, long>(int_val, logicID));
		}
		if (ph.nextPageNo < 0)
			break;
		else
			pageNo = ph.nextPageNo;
	}
}