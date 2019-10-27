#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "index.h"
#include<stdio.h>

//==================== file global variable ====================
extern struct DataBase *DB; /* 全局共享 */

// void file_Init(struct DataBase *db)
// {
// 	DB = db;
// }


void create_index(char *tableName,char *Attributename){
	FILE *index;
	int indexID,FileID,i,j;
	for(i=0;i<MAX_FILE_NUM;i++){												//查找表文件的文件号
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){
			FileID=DB->dataDict[i].fileID;
		}
	}
	for(j=0;attr<DB->dataDict[i].attrNum;j++){									//查找属性列
		if(strcmp(DB->dataDict[i].attr[j].name,Attributename)==0){
			if(DB->dataDict[i].attr[j].indexFile.fileID==0){
				indexID=DB->dataDict[i].fileID*100+j；							//新建索引文件
				index=fopen("../data/indexID","wb+");
				DB->dataDict[i].attr[j].indexFile.fileID=indexID;	
				DB->dataDict[i].attr[j].indexFile.tableName=tableName;
				DB->dataDict[i].attr[j].indexFile.attrName=Attributename;
			}
			else{
				indexID=DB->dataDict[i].attr[j].indexFile.fileID;				//获取索引文件
			}
		}
	}
	for(i=0;i<MAX_FILE_NUM;i++){												//查找表文件的起始页号
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID)						//
			break;																//
	}																			//
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				//
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	int sizeofpagehead = sizeof(struct PageMeta);								//读取文件信息
	int sizeofrecord = sizeof(struct OffsetInPage);								//
	struct TreeRecord indexRecord;
	struct PageMeta pagehead;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);						//根据页号从缓冲区调取页的内容
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);				//读取第一页的内容并存放在pagehead里
		if(pagehead.recordNum>0){
			for(j=0;j<pagehead.recordNum;j++){
				indexRecord.posPage=pagehead.pageNo;

				file_getrecordAttribute(DB,pagehead.pageNo,j,tableName,Attributename,indexRecord.key,indexRecord.posOffset);
        indexRecord.recordID = j;
				int value=insert(index, indexRecord);							//建立B+树索引
				if(value==-1){
					printf("error:Insertion failed!\n");
					break；
				}
			}
		}
		long nextPno = pagehead.nextPageNo;
		if(nextPno==-1)
			break;
		else
			CurpageNo = nextPno;
	}
}

void drop_index(char *tableName,char *Attributename){
	int value=find_indexfile(tableName,Attributename);
	if(value==-1){
		printf("error:the indexfile is not exist!\n");
	}
	else{

	}
}

int find_indexfile(char *tableName,char *Attributename){								//查找索引文件号
	for(int i=0;i<MAX_FILE_NUM;i++){
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){
			for(int j=0;attr<DB->dataDict[i].attrNum;j++){
				if(strcmp(DB->dataDict[i].attr[j].name,Attributename)==0){
					if(DB->dataDict[i].attr[j].indexFile.fileID==0){
						return -1;
					}
					else{
						return DB->dataDict[i].attr[j].indexFile.fileID;
					}
				}
			}
		}
	}
}

void update_index(char *tableName, char *Attributename, Record* oldRecord, Record* newRecord)
{
	delete_index(tableName, Attributename, oldRecord);
	insert_index(tableName, *Attributename, newRecord);
}