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

int projection(int dictID,char* attributename){//attributename为投影所需的所有的属性名 格式为attri1|attri2|...
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i]id==fileID){						//
			querypage=DB->dbMeta.fileMeta[0].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}
	long CurpageNo = DB->dbMeta.fileMeta[querypage].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[querypage].pageNum;
	char* one_attribute = (char * )malloc(MAX_NAME_LENGTH);
	memset(one_attribute,0,MAX_NAME_LENGTH);
	int* attributeindex = (int*)malloc(DB->dataDict[dictID].attrNum);
	memset(attributeindex,-1,DB->dataDict[dictID].attrNum);
	int charnum=0,attributenum=0;
	for(int j=0;j<=strlen(attributename);j++){
		if(attributename[j]=='|'||j==strlen(attributename)){
			for(int m=0;m<DB->dataDict[dictID].attrNum;m++){
				if(strcmp(one_attribute,DB->dataDict[dicID].attr[m])==0){
					attributeindex[attributenum]=m;
				}
			}
			memset(one_attribute,0,MAX_NAME_LENGTH);
			charnum=0;
		}
		else{
			one_attribute[charnum]=attributename[j];
			charnum++;
		}
	}
	int pageno = creat_tmptable(DB->dataDict[dictID]);
	if(pageno<0){
		printf("临时表创建失败");
	}
	for(i=0;i<pagenum;i++){
		struct PageMeta pagehead;
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
		int num=0;
		for(int j = 0;j<pagehead.recordNum;j++){
			
			char* record = (char*)malloc(100);//暂定一条记录的最长长度为100
			file_getrecord(CurpageNo,i,record);
			char* new_record = (char*)malloc(100);
			memset(new_record,0,100);
			char* attribute = (char*)malloc(100);
			memset(attribute,0,100);
			for(int m=0;m<attributenum;m++){
				int flag = getValueByAttrID(record,attributeindex[m],attribute);
				if(flag<0){
					printf("error:获取属性值失败\n");
					return -1;
				}
				if(m==0){
					strcpy(new_record,attribute);
					strcat(new_record,"|");
				}
				else{
					strcat(new_record,attribute);
					if(m<attribute-1){
						strcat(new_record,"|");
					}
				}
				insert_onerecord(dictID,new_record);
			}
			if(pagehead.nextPageNo<0){
				break;
			}
			else{
				CurpageNo=pagehead.nextPageNo;
			}
		}
	}
	return pageno;
}