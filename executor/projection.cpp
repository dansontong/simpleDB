#include "executor.h"

//==================== global variable ====================
extern struct DataBase *DB; /* 全局共享 */

// void file_Init(struct DataBase *db)
// {
// 	DB = db;
// }

int projection(int dictID, char* attrName){//attributename为投影所需的所有的属性名 格式为attri1|attri2|...
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                   //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;													//
		}															//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}
	// int sizeofpagehead = sizeof(struct PageMeta);
	// int sizeofrecord = sizeof(struct OffsetInPage);	
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;				
	long pageNum = DB->dbMeta.fileMeta[i].pageNum;

	char* one_attribute = (char* )malloc(MAX_NAME_LENGTH);
	memset(one_attribute,0,MAX_NAME_LENGTH);
	int* attrIndex = (int* )malloc(sizeof(int)*DB->dataDict[dictID].attrNum);
	memset(attrIndex, -1, DB->dataDict[dictID].attrNum);
	int charnum=0, attrNum=0;
	for(int j=0; j<=strlen(attrName)+1; j++){
		if(attrName[j]=='|' || j==strlen(attrName)){
			attrIndex[attrNum++] = getAttrIndexByName(dictID, one_attribute);
			// printf("j:%d ++++++++++++++++++++ attrName: %s, attrIndex[0]: %d\n", j, one_attribute, attrIndex[0]);
			// for(int m=0;m<DB->dataDict[dictID].attrNum;m++){
			// 	if(strcmp(one_attribute,DB->dataDict[dictID].attr[m].name)==0){
			// 		attrIndex[attrNum]=m;
			// 	}
			// }
			memset(one_attribute,0,MAX_NAME_LENGTH);
			charnum=0;
		}
		else{
			one_attribute[charnum]=attrName[j];
			charnum++;
		}
	}
	int tmpDictID = create_tmptable(DB->dataDict[dictID]);
	if(tmpDictID<0){
		printf("临时表创建失败");
	}
	char* record = (char*)malloc(RECORD_MAX_SIZE);//一条记录的最长长度为RECORD_MAX_SIZE
	char* new_record = (char*)malloc(RECORD_MAX_SIZE);
	char* attribute = (char*)malloc(RECORD_MAX_SIZE);
	struct BufTag buftag;
	struct PageMeta pageMeta;
	for(i=0; i<pageNum; i++)
	{
		buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pageMeta, Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		int num=0;
		// printf("========== pageMeta.recordNum: %d, pageNo: %d, CurpageNo:%d. \n", pageMeta.recordNum, pageMeta.pageNo, CurpageNo);
		for(int j = 0; j<pageMeta.recordNum; j++)
		{
			getRecord(CurpageNo, j, record);
			memset(new_record, 0, RECORD_MAX_SIZE);
			memset(attribute, 0, RECORD_MAX_SIZE);
			for(int m=0; m<attrNum; m++)
			{
				int flag = getValueByAttrID(record, attrIndex[m], attribute);
				if(flag<0)
				{
					printf("error:获取属性值失败\n");
					return -1;
				}
				if(m==0)
				{
					strcpy(new_record,attribute);
					strcat(new_record,"|");
				}
				else
				{
					strcat(new_record,attribute);
					if(m<attrNum-1)
					{
						strcat(new_record,"|");
					}
				}
				
			}
			insertRecord(tmpDictID, new_record);
			printf("===== project on attribute:%s, new_record:%.60s.\n", attrName, new_record);
		}
		if(pageMeta.nextPageNo<0)
		{
			break;
		}
		else
		{
			CurpageNo=pageMeta.nextPageNo;
		}
		break; // temp: if-not, there're  too many records
	}
	return tmpDictID;
}
