#include "executor.h"

//==================== global variable ====================
extern struct DataBase *DB; /* 全局共享 */

int tableScanEqualSelector(int dictID,char* attri_name,char* value){
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	long querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageNo = DB->dbMeta.fileMeta[i].firstPageNo;
	long pageNum = DB->dbMeta.fileMeta[i].pageNum;
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
	int attrIndex = getAttrIndexByName(dictID, attri_name);
	if(attrIndex<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	char *record = (char*)malloc(RECORD_MAX_SIZE);
	char *attrValue = (char*)malloc(RECORD_MAX_SIZE);
	struct PageMeta pageMeta;
	struct BufTag buftag;
	for(i=0;i<pageNum;i++){
		buftag = Buf_GenerateTag(pageNo);
		memcpy(&pageMeta,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		printf("========== pageMeta.recordNum: %d, pageNo: %d, CurpageNo:%d. \n", pageMeta.recordNum, pageMeta.pageNo, pageNo);
		for(int j=0;j<pageMeta.recordNum;j++){
			getRecord(pageNo,j,record);
			memset(attrValue, 0, RECORD_MAX_SIZE);
			int flag = getValueByAttrID(record,index,attrValue);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			if (strcmp(attrValue, value) == 0){
				insertOneRecord(tmptable,record);
				printf(" tmpTable ===== attrValue: %s\n", record);
			}
		}
		if(pageMeta.nextPageNo<0){
			break;
		}
		else{
			pageNo=pageMeta.nextPageNo;
		}
	}
	// free(record);
	// free(attrValue);
	return tmptable;
}
int tableScanRangeSelector(int dictID,char* attri_name,char* min,char* max){
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageNo = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
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
	int attrIndex = getAttrIndexByName(dictID, attri_name);
	if(attrIndex<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	char *record = (char*)malloc(RECORD_MAX_SIZE);
	char *attrValue = (char*)malloc(RECORD_MAX_SIZE);
	for(i=0;i<pagenum;i++){
		struct PageMeta pageMeta;
		struct BufTag buftag = Buf_GenerateTag(pageNo);
		memcpy(&pageMeta,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		printf("========== pageMeta.recordNum: %d, pageNo: %d, CurpageNo:%d. \n", pageMeta.recordNum, pageMeta.pageNo, pageNo);
		for(int j=0;j<pageMeta.recordNum;j++){
			getRecord(pageNo,j,record);
			memset(attrValue, 0, RECORD_MAX_SIZE);
			int flag = getValueByAttrID(record,index,attrValue);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			//判断进行范围选择的属性的类型
			if (table1.attr[dictID].type == INT_TYPE){
				int min_ = atoi(min);
				int max_ = atoi(max);
				int val_ = atoi(attrValue);
				if (val_ >= min_ && val_ <= max_)
				{
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);
				}
			}
			//char类型
			else if (table1.attr[attrIndex].type == CHAR_TYPE) {
				if (strcmp(min, attrValue) <= 0 && strcmp(max, attrValue) >= 0){
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);
				}
			}
		}
		if(pageMeta.nextPageNo<0){
			break;
		}
		else{
			pageNo=pageMeta.nextPageNo;
		}
	}
	// free(record);
	// free(attrValue);
	return tmptable;
}
int tableScanUnEqualSelector(int dictID,char* attri_name,char* value){//非等值连接
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageno = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
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
	int attrIndex = getAttrIndexByName(dictID, attri_name);
	if(attrIndex<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	char *record = (char*)malloc(RECORD_MAX_SIZE);
	char *attrValue = (char*)malloc(RECORD_MAX_SIZE);
	for(i=0;i<pagenum;i++){
		struct PageMeta pagehead;
		struct BufTag buftag = Buf_GenerateTag(pageno);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		for(int j=0;j<pagehead.recordNum;j++){
			getRecord(pageno,j,record);
			memset(attrValue, 0, RECORD_MAX_SIZE);
			int flag = getValueByAttrID(record,index,attrValue);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			if (strcmp(attrValue, value) != 0){
				insertOneRecord(tmptable,record);
				printf(" tmpTable ===== attrValue: %s\n", record);
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
int tableScanMinRangeSelector(int dictID,char* attri_name,char* min){//只有最小值
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageno = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
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
	int attrIndex = getAttrIndexByName(dictID, attri_name);
	if(attrIndex<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attri_name);
	}
	char *record = (char*)malloc(RECORD_MAX_SIZE);//暂定该表的记录长度为RECORD_MAX_SIZE
	char *attrValue = (char*)malloc(RECORD_MAX_SIZE);
	struct PageMeta pagehead;
	struct BufTag buftag;
	for(i=0;i<pagenum;i++){
		buftag = Buf_GenerateTag(pageno);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		for(int j=0;j<pagehead.recordNum;j++){
			getRecord(pageno,j,record);
			memset(attrValue, 0, RECORD_MAX_SIZE);
			int flag = getValueByAttrID(record,index,attrValue);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			//判断进行范围选择的属性的类型
			if (table1.attr[dictID].type == INT_TYPE){
				int min_ = atoi(min);
				//int max_ = atoi(max);
				int val_ = atoi(attrValue);
				if (val_ >= min_ )
				{
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);					
				}
			}
			//char类型
			else if (table1.attr[attrIndex].type == CHAR_TYPE)
			{
				if (strcmp(min, attrValue) <= 0 )
				{
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);
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
	// free(record);
	// free(attrValue);
	return tmptable;
}
int tableScanMaxRangeSelector(int dictID,char* attrName,char* max){//只有max值
	Table table1 = DB->dataDict[dictID];
	int fileID = table1.fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该表应的文件不存在！");
		exit(0);
	}
	long pageno = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	int tmptable = create_tmptable(table1);
	if(tmptable<0){
		printf("创建临时表失败！\n");
		return -1;  //返回-1表示投影失败
	}
	int index=-1;
	for(int j=0;j<DB->dataDict[dictID].attrNum;j++){//在table1表中查找属性名：attrName
		if(strcmp(attrName,DB->dataDict[dictID].attr[j].name)==0){
			index=j;
		}
	}
	int attrIndex = getAttrIndexByName(dictID, attrName);
	if(attrIndex<0){
		printf("在表%s中不含有属性%s\n",table1.tableName,attrName);
	}
	char *record = (char*)malloc(RECORD_MAX_SIZE);//暂定该表的记录长度为RECORD_MAX_SIZE
	char *attrValue = (char*)malloc(RECORD_MAX_SIZE);
	struct PageMeta pagehead;
	struct BufTag buftag;
	for(i=0;i<pagenum;i++){
		buftag = Buf_GenerateTag(pageno);
		memcpy(&pagehead,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		for(int j=0;j<pagehead.recordNum;j++){
			getRecord(pageno,j,record);
			memset(attrValue, 0, RECORD_MAX_SIZE);
			int flag = getValueByAttrID(record,index,attrValue);
			if(flag<0){
				printf("获取表中属性值失败！\n");
				return -1;
			}
			//判断进行范围选择的属性的类型
			if (table1.attr[dictID].type == INT_TYPE){
			
				int max_ = atoi(max);
				int val_ = atoi(attrValue);
				if (val_ <= max_)
				{
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);
				}
			}
			//char类型
			else if(table1.attr[attrIndex].type == CHAR_TYPE) {
				if( strcmp(max, attrValue) >= 0){
					insertOneRecord(tmptable,record);
					printf(" tmpTable ===== attrValue: %s\n", record);
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
	// free(record);
	// free(attrValue);
	return tmptable;
}