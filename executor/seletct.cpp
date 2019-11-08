#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"

//==================== global variable ====================
extern struct DataBase *DB; /* 全局共享 */

int tableScanEqualSelector(int dictID,char* attri_name,char* value){
	Table table1 = DB->dataDict[dictID].fileID;
	int fileID = tablel.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[0].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageno = DB->dbMeta.fileMeta[fileID].fileFirstPageNo;
	long pagenum = head->desc.fileMeta[fileID].filePageNum;
	int tmptable = create_tmptable(table1);
	if(tmptable<0){
		printf("创建临时表失败！\n");
		return -1;  //返回-1表示投影失败
	}
	int index=-1;
	for(int j=0;j<DB->dataDict[dictID].attrNum;j++){//在table1表中查找属性名：attri_name
		if(strcmp(attri_name,DB->dataDict[dictID].attr[j].name)==0){
			index=j;
		}
	}
	if(attr_index<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	for(i=0;i<pagenum;i++){
		struct PageMeta pagehead;
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
		for(int j=0;j<pagehead.recordNum;j++){
			char *record = (char*)malloc(100);//暂定该表的记录长度为100
			file_getrecord(pageno,j,record);
			char *value_table = (char*)malloc(100);
			memset(value_table, 0, 100);
			int flag = getValueByAttrID(record,index,value_table);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			if (strcmp(value_table, value) == 0){
				insert_onerecord(dictID,value_table);
			}
		}
		if(pagehead.nextPageNo<0){
			break;
		}
		else{
			pageno=pagehead.nextPageNo;
		}
	}
	return tmptable;
}
int tableScanRangeSelector(int dictID,char* attri_name,char* min,char* max){
	Table table1 = DB->dataDict[dictID].fileID;
	int fileID = tablel.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[0].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageno = DB->dbMeta.fileMeta[fileID].fileFirstPageNo;
	long pagenum = head->desc.fileMeta[fileID].filePageNum;
	int tmptable = create_tmptable(table1);
	if(tmptable<0){
		printf("创建临时表失败！\n");
		return -1;  //返回-1表示投影失败
	}
	int index=-1;
	for(int j=0;j<DB->dataDict[dictID].attrNum;j++){//在table1表中查找属性名：attri_name
		if(strcmp(attri_name,DB->dataDict[dictID].attr[j].name)==0){
			index=j;
		}
	}
	if(attr_index<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	for(i=0;i<pagenum;i++){
		struct PageMeta pagehead;
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
		for(int j=0;j<pagehead.recordNum;j++){
			char *record = (char*)malloc(100);//暂定该表的记录长度为100
			file_getrecord(pageno,j,record);
			char *value_table = (char*)malloc(100);
			memset(value_table, 0, 100);
			int flag = getValueByAttrID(record,index,value_table);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			//判断进行范围选择的属性的类型
			if (table1.attr[dictID].type == INT_TYPE){
				int min_ = atoi(min);
				int max_ = atoi(max);
				int val_ = atoi(value_table);
				if (val_ >= min_ && val_ <= max_)
					insert_onerecord(dictID,value_table);
			}
			//char类型
			else if (rl.atb[attr_index].getType() == CHAR_TYPE) {
				if (strcmp(min, val_table) <= 0 && strcmp(max, val_table) >= 0){
					insert_onerecord(dictID,value_table);
				}
			}
		}
		if(pagehead.nextPageNo<0){
			break;
		}
		else{
			pageno=pagehead.nextPageNo;
		}
	}
	return tmptable;
}