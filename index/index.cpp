#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "index.h"
#include "tableOpt.h"
#include <stdio.h>
#include <stdlib.h>


//==================== file global variable ====================
extern struct DataBase *DB; /* 全局共享 */


//========================  function   =========================
void create_index(char *tableName,char *attributeName){
	FILE *index;
	char indexFilePath[50] = "./data/index/";
	char tmpStr[16] = {0};
	int indexID,FileID,i,j;
	for(i=0;i<MAX_FILE_NUM;i++){												//查找表文件的文件号
		// if(strncmp(DB->dataDict[i].tableName, tableName, strlen(DB->dataDict[i].tableName)-1)==0){
		if(strcmp(DB->dataDict[i].tableName, tableName)==0){
			FileID=DB->dataDict[i].fileID;
			break;
		}
	}
	if(i == MAX_FILE_NUM){
		printf("tableName:%s \n", tableName);
		printf("error: table not exist, create index failed! \n");
		return;
	}
	for(j=0;j<DB->dataDict[i].attrNum;j++){									//查找属性列
		// printf("attribute[%d] %s\n", j, DB->dataDict[i].attr[j].name);
		// printf("sizeof: %d\n", sizeof(attributeName));
		// printf("sizeof: %d\n", sizeof(DB->dataDict[i].attr[j].name));
		// printf("strlen: %d\n", strlen(DB->dataDict[i].attr[j].name));
		// 防止attributeName参数不带\0,长度不一样,导致对比不想等
		if(strcmp(DB->dataDict[i].attr[j].name, attributeName)==0){
			if(DB->dataDict[i].attr[j].indexFileID==0){
				indexID = DB->dataDict[i].fileID + 10000;					//新建索引文件
				snprintf(tmpStr, 16, "%d", i);
				strcat(indexFilePath, attributeName);
				strcat(indexFilePath, "_");
				strcat(indexFilePath, tmpStr);
				// printf("=========== indexFilePath: ========== %s\n", indexFilePath);
				index = fopen(indexFilePath, "wb+");
				DB->dataDict[i].attr[j].indexFileID=indexID;	
				// strcpy(DB->dataDict[i].attr[j].indexFile.tableName, tableName);
				// strcpy(DB->dataDict[i].attr[j].indexFile.attrName, attributeName);
			}
			else{
				indexID=DB->dataDict[i].attr[j].indexFileID;				//获取索引文件
			}
			break;
		}
	}
	if(j == DB->dataDict[i].attrNum){
		printf("error: attribute %s not  exist, create index failed! \n",attributeName);
		return;
	}
	for(i=0;i<MAX_FILE_NUM;i++){												//查找表文件的起始页号
		if(DB->dbMeta.fileMeta[0].id==FileID)						//
			break;																//
	}		
	char *tmpKey = (char *)malloc(MAX_NAME_LENGTH);																	//
	long CurpageNo = DB->dbMeta.fileMeta[0].firstPageNo;				//
	long pagenum = DB->dbMeta.fileMeta[0].pageNum;
	int sizeofpagehead = sizeof(struct PageMeta);								//读取文件信息
	int sizeofrecord = sizeof(struct OffsetInPage);								//
	TreeRecord indexRecord;
	struct PageMeta pagehead;
	struct OffsetInPage curoffset;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);						//根据页号从缓冲区调取页的内容
		memcpy(&pagehead, Buf_ReadBuffer(buftag),sizeofpagehead);				//读取第一页的内容并存放在pagehead里
		if(pagehead.recordNum>0){
			for(j=0;j<pagehead.recordNum;j++){
				memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*j,sizeofrecord);
				indexRecord.offset = curoffset.offset;							//记录偏移量

				indexRecord.posPage=pagehead.pageNo;							//记录所在页号

				getrecordAttribute(pagehead.pageNo,j,tableName,attributeName, tmpKey, indexRecord.posOffset);
				indexRecord.key = atoi(tmpKey);									//目前只支持key值类型为int的列进行建索引。
				printf("tmpKey:%s, key:%d\n", tmpKey, indexRecord.key);
        		indexRecord.recordID = j;										//记录ID
				int value=insert(index, indexRecord);							//建立B+树索引
				if(value==-1){
					printf("error:Insertion failed!\n");
					break;
				}
				else{
					printf("insert %d success.\n", j);
				}
			}
		}
		long nextPno = pagehead.nextPageNo;
		if(nextPno==-1)
			break;
		else
			CurpageNo = nextPno;
	}
	free(tmpKey);
	printf("---- create_index function finish! ----\n");
}

void drop_index(char *tableName,char *attributeName)//删除索引文件
{
	char indexFilePath[50] = "./data/index/";
	char tmpStr[16] = {0};
	for(int i=0;i<MAX_FILE_NUM;i++)
	{
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){
			for(int j=0;j<DB->dataDict[i].attrNum;j++){
				if(strcmp(DB->dataDict[i].attr[j].name,attributeName)==0){
					if(DB->dataDict[i].attr[j].indexFileID==0)
					{
						printf("error: the indexfile is not exist!\n");	
					}
					else
					{
						DB->dataDict[i].attr[j].indexFileID=0;

						snprintf(tmpStr, 16, "%d", i);
						strcat(indexFilePath, attributeName);
						strcat(indexFilePath, "_");
						strcat(indexFilePath, tmpStr);

						int value=remove(indexFilePath);
						if(value==EOF)
						{
							printf("error: delete failed!\n");
						}
						else
						{
							printf("---- drop_index success! ----\n");
						}
					}
				}
			}
		}
	}	
}

int find_indexfile(char *tableName,char *attributeName){					//查找索引文件号
	for(int i=0;i<MAX_FILE_NUM;i++){
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){

			for(int j=0;j<DB->dataDict[i].attrNum;j++){
				if(strcmp(DB->dataDict[i].attr[j].name,attributeName)==0){
					if(DB->dataDict[i].attr[j].indexFileID==0){
						return -1;
					}
					else{
						return DB->dataDict[i].attr[j].indexFileID;
					}
				}
			}
		}
	}
}

void insert_index(char *tableName, char *attributeName, Record* record){		//索引插入结点
	char indexFilePath[50] = "./data/index/";
	char tmpStr[16] = {0};

	int value = find_indexfile(tableName,attributeName);
	char *tmpKey;
	if(value==-1){
		printf("error:the indexfile is not exist!\n");
	}
	else{
		TreeRecord indexRecord;
		indexRecord.posPage=record->pageNo;
		indexRecord.recordID=record->recordID;
		getrecordAttribute(record->pageNo,record->recordID,tableName,attributeName,tmpKey,indexRecord.posOffset);
		indexRecord.key = atoi(tmpKey);
		indexRecord.pos = atoi(tmpKey) * 2;

		FILE *index;

		int dictID = getDictIDbyName(tableName);
		snprintf(tmpStr, 16, "%d", dictID);
		strcat(indexFilePath, attributeName);
		strcat(indexFilePath, "_");
		strcat(indexFilePath, tmpStr);

		index=fopen(indexFilePath, "rb+");
		int result=insert(index, indexRecord);							
		if(result==-1){
			printf("error:insert failed!\n");
		}
	}
}

void delete_index(char *tableName, char *attributeName, Record* record){		//索引删除结点
	char indexFilePath[50] = "./data/index/";
	char tmpStr[16] = {0};

	int value=find_indexfile(tableName,attributeName);
	char *tmpKey;
	if(value==-1){
		printf("error:the indexfile is not exist!\n");
	}
	else{
		TreeRecord indexRecord;
		getrecordAttribute(record->pageNo,record->recordID,tableName,attributeName,tmpKey,indexRecord.posOffset);
		indexRecord.key = atoi(tmpKey);
		FILE *index;

		int dictID = getDictIDbyName(tableName);
		snprintf(tmpStr, 16, "%d", dictID);
		strcat(indexFilePath, attributeName);
		strcat(indexFilePath, "_");
		strcat(indexFilePath, tmpStr);

		index=fopen(indexFilePath,"rb+");
		int result=del(index, indexRecord.key);							
		if(result==-1){
			printf("error: delete failed! \n");
		}else{
			printf("info: delete record success! \n");
		}
	}
}

void update_index(char *tableName, char *attributeName, Record* oldRecord, Record* newRecord)
{
	delete_index(tableName, attributeName, oldRecord);
	insert_index(tableName, attributeName, newRecord);
}

void search_index(char *tableName, char *attributeName, char* Attribute, Record* recordList)
{
	char indexFilePath[50] = "./data/index/";
	char tmpStr[16] = {0};

	int value=find_indexfile(tableName, attributeName);
	if(value==-1)
	{
		printf("error: index file is not exist!\n");
	}
	else
	{
		FILE *index;

		int dictID = getDictIDbyName(tableName);
		snprintf(tmpStr, 16, "%d", dictID);
		strcat(indexFilePath, attributeName);
		strcat(indexFilePath, "_");
		strcat(indexFilePath, tmpStr);

		index=fopen(indexFilePath, "rb");
		int key = atoi(Attribute);
		TreeRecord indexRecord = search(index, key);
		if(indexRecord.searchfaild == 1)
		{
			printf("error:search failed!\n");
		}
		else
		{
			int sizeofpageMeta = sizeof(struct PageMeta);
			int sizeofrecord = sizeof(struct OffsetInPage);
			OffsetInPage preoffset;
			int readlength,recordID;
			char* str;
			BufTag buftag = Buf_GenerateTag(indexRecord.posPage);	//根据页号从缓冲区调取页的内容
			if(indexRecord.recordID == 0)									//打印记录
			{																	
				readlength = indexRecord.offset;
				memcpy(&str,Buf_ReadBuffer(buftag)+PAGE_SIZE-indexRecord.offset,readlength);
				str[readlength] = '\0';
				printf("%s文件中属性%s为%s的记录：\n",tableName,attributeName,Attribute);
				printf("%s\n",str);
			}
			else
			{
				recordID = indexRecord.recordID-1;
				memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+sizeofrecord*recordID,sizeofrecord);
				readlength = indexRecord.offset-preoffset.offset;
				memcpy(&str,Buf_ReadBuffer(buftag)+PAGE_SIZE-indexRecord.offset,readlength);
				str[readlength] = '\0';
				printf("%s文件中属性%s为%s的记录：\n",tableName,attributeName,Attribute);
				printf("%s\n",str);
			}
		}
	}
}