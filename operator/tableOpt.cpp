#include "tableOpt.h"


extern struct DataBase *DB; /* 全局共享 */


//创建表，并返回数据字典下标
int createTable(char *str)
{
	//解析字符串 CREATE TABLE NATION ( N_NATIONKEY INTEGER NOT NULL,N_NAMECHAR(25) NOT NULL,N_REGIONKEY INTEGER NOT NULL,N_COMMENTVARCHAR(152));
	// parse a given query
	// hsql::SQLParserResult result;
	// hsql::SQLParser::parse(query, &result);//后期再使用
	char tableName[MAX_NAME_LENGTH] = "Supplier";

	int fileID = file_newFile(TABLE_FILE, 1);
	//插入数据字典
	int dictID = -1;
	for(int i=0; i<MAX_FILE_NUM; i++){
		if(DB->dataDict[i].fileID < 0){
			dictID = i;
			break;
		}
	}
	DB->dataDict[dictID].fileID = fileID;
	strcpy(DB->dataDict[dictID].tableName, tableName);

	DB->dataDict[dictID].attrNum = 0;  //属性个数
	DB->dataDict[dictID].recordLength = 0;//记录总长度
	DB->dataDict[dictID].recordNum = 0;  //属性个数

	//插入属性
	insertAttr(&DB->dataDict[dictID],"S_SUPPKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"S_NAME",CHAR_TYPE,25,true);
	insertAttr(&DB->dataDict[dictID],"S_ADDRESS",VARCHAR_TYPE,40,true);
	insertAttr(&DB->dataDict[dictID],"NATIONKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"S_PHONE",CHAR_TYPE,15,true);
	insertAttr(&DB->dataDict[dictID],"S_ACCTBAL",FLOAT_TYPE,8,true);
	insertAttr(&DB->dataDict[dictID],"S_COMMENT",VARCHAR_TYPE,101,true);

	// write to file
	
	
	return dictID;
}

// delete table by name
int deleteTable(char *tableName)
{
	// delete from bitmap, mark page be free in bitMap
	int dictID = getDictIDbyTableName(tableName);
	int fileID = DB->dataDict[dictID].fileID;
	long pageNo = DB->dbMeta.fileMeta[fileID].firstPageNo;
	struct PageMeta ph;
	struct BufTag buftag = Buf_GenerateTag(pageNo);
	memcpy(&ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
	while(pageNo != -1)
	{
		page_recover_onepage(pageNo);
		pageNo = ph.nextPageNo;
		buftag = Buf_GenerateTag(pageNo);
		memcpy(&ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
	}
	
	// delete related index file	
	for(int i=0; i<DB->dataDict[dictID].attrNum; i++)
	{
		if( DB->dataDict[dictID].attr[i].indexFileID != -1 )
		{
			DB->dataDict[dictID].attr[i].indexFileID = -1;
			// TODO:  
		}
	}
	
	// delete from dictionary
	DB->dataDict[dictID].fileID = -1;
}

//创建表，并返回数据字典下标
int createTable2(char *str)
{
	//解析字符串 CREATE TABLE NATION ( N_NATIONKEY INTEGER NOT NULL,N_NAMECHAR(25) NOT NULL,N_REGIONKEY INTEGER NOT NULL,N_COMMENTVARCHAR(152));
	// parse a given query
	// hsql::SQLParserResult result;
	// hsql::SQLParser::parse(query, &result);//后期再使用
	char tableName[MAX_NAME_LENGTH] = "nation";

	int fileID = file_newFile(TABLE_FILE, 1);
	//插入数据字典
	int dictID = -1;
	for(int i=0; i<MAX_FILE_NUM; i++){
		if(DB->dataDict[i].fileID < 0){
			dictID = i;
			break;
		}
	}
	DB->dataDict[dictID].fileID = fileID;
	strcpy(DB->dataDict[dictID].tableName, tableName);

	//插入属性
	insertAttr(&DB->dataDict[dictID],"NATIONKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"N_NAME",CHAR_TYPE,25,true);
	insertAttr(&DB->dataDict[dictID],"N_REGIONKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"N_COMMENT",VARCHAR_TYPE,152,true);

	return dictID;
}

long getLogicID(int fileID, long pageNo,int recordNo){
	long count = 0;
	int i;
	int querypage=-1;
	for( i=0;i<MAX_FILE_NUM;i++){                                           //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].segList[i].id==fileID){					//
			querypage=DB->dbMeta.fileMeta[i].segList[i].firstPageNo;		//
			break;															//
		}																	//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}

	// long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				
	// long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	struct PageMeta pageMeta;
	for(i=0;i<pagenum;i++){
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pageMeta,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		if(pageMeta.nextPageNo=-1){
			count = count+recordNo;
			return count;
		}
		else{
			count = count+pageMeta.recordNum;
			CurpageNo = pageMeta.nextPageNo;
		}
	}
}

bool getRecordByLogicID(int fileID,long logicID,char* result){  // logicID is match to the space one-by-one.logicID只跟表的第几页的位置有关，记录删除时,该位置的logicID不变
	long count = 0;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].segList[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("该文件id对应的文件不存在！");
		exit(0);
	}
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	struct PageMeta pageMeta;
	int recordNo;

	for(i=0;i<pagenum;i++){
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);
		memcpy(&pageMeta,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
		recordNo = logicID % pageMeta.recordNum; // TODO: is this right ???
		if(pageMeta.nextPageNo=-1){
			if(count+pageMeta.recordNum<logicID){
				printf("logicID有错误！\n");
				return false;
			}
			else{
				getRecord(CurpageNo,recordNo,result);
				return true;
			}
		}
		else{
			if(count+pageMeta.recordNum>=logicID){
				getRecord(CurpageNo,recordNo,result);
				return true;
			}
			else{
				count+=pageMeta.recordNum;
				CurpageNo = pageMeta.nextPageNo;
			}
		}
	}
	return false;
}

int getValueByAttrID(char *recordStr, int index, char *result){
	int length = strlen(recordStr);
	int j = 0, k = 0;
	int start = 0;
	for (int i = 0; i <= length+1; i++) {
		if (recordStr[i] == '|' || i == length){
			if (k == index){
				result[j] = '\0';
				return start;
			}
			else{
				start = i + 1;
				k++;
				memset(result, 0, length);
				j = 0;
			}
		}
		else
			result[j++] = recordStr[i];
	}
	return -1;
}

bool getRecord(long pageNo,int recordID,char *record)
{	
	int sizeofpageMeta = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	OffsetInPage curoffset,preoffset;
	struct BufTag buftag = Buf_GenerateTag(pageNo);
	struct PageMeta pageMeta;
	memcpy(&pageMeta,Buf_ReadBuffer(buftag),sizeofpageMeta);
	if(recordID<0||recordID>pageMeta.recordNum){
		return false;
	}
	if(recordID>0){//判断是否是第一个记录
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+sizeofrecord*recordID,sizeofrecord);
		memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+sizeofrecord*(recordID-1),sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset-preoffset.offset);//记录的位置为页的起始位置加上pagesize-记录距离页尾的距离，记录长度为该记录距离页尾的位置减去上一条记录距离页尾的位置
		return true;
	}
	else{
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpageMeta+sizeofrecord*recordID,sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset);//第一条记录的长度为该记录距离页尾的距离
		return true;
	}
	return false;
}

bool getrecordAttribute(long pageNo,int recordID,char* tablename,char* Attributename,char* Attribute,char* posOffset){
	char *record;
	bool flag;

	if(getRecord(pageNo,recordID,record)){//返回该条记录
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
							if(getValueByAttrID(record, j, Attribute)>0){
								return true;
							}
							else{
								return false;
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
int getAttrIndexByName(int dictID, char *attrName)
{
	for(int i=0; i<DB->dataDict[dictID].attrNum; i++)
	{
		if(strcmp(DB->dataDict[dictID].attr[i].name, attrName) == 0)
		{
			return i;
		}
	}
	return -1; // failure
}

bool tupleInsert(int length, int FileID, char *str){

}

void insertAttr(Table *table,const char *name, DATA_TYPE type, int length, bool notNull)
{
	if(table->attrNum >= MAX_ATTRIBUTE_NUM){
		printf("reach MAX_ATTRIBUTE_NUM,error.\n");
	}
	//第一个属性的偏移为0
	if (table->attrNum == 0){
		strcpy(table->attr[table->attrNum].name, name);
		table->attr[table->attrNum].type = type;
		table->attr[table->attrNum].length = length;
		table->attr[table->attrNum].offset = 0;
	}
	else {
		int offset = table->attr[table->attrNum-1].length + table->attr[table->attrNum-1].offset;
		strcpy(table->attr[table->attrNum].name, name);
		table->attr[table->attrNum].type = type;
		table->attr[table->attrNum].length = length;
		table->attr[table->attrNum].notNull = notNull;
		table->attr[table->attrNum].offset = offset;
	}
	table->attrNum += 1;
	table->recordLength += length;
}

void insertRecord(int dictID, char *str)
{
	int fileID = DB->dataDict[dictID].fileID;
	int length = strlen(str);
	Record record = file_writeFile(fileID, length, str);
	
	char recordStr[1000]; //假设记录最长1000个字节
	// char *attribute;
	getRecord(record.pageNo,record.recordID, recordStr);
	int i;
	// char *offset;
	for(i=0; i<DB->dataDict[dictID].attrNum; i++)
	{
		if( DB->dataDict[dictID].attr[i].indexFileID != 0 )
		{		
			// offset = recordStr+DB->dataDict[dictID].attr[i].offset; //TODO：是否需要加上record?
			// if(i<DB->dataDict[dictID].attrNum-1)
			// {
			// 	memcpy(attribute,recordStr+DB->dataDict[dictID].attr[i].offset,DB->dataDict[dictID].attr[i+1].offset-DB->dataDict[dictID].attr[i].offset);//一般情况：位置为record的起始地址加上属性的偏移量，长度为该下一条属性的偏移量减去该属性的偏移量
			// }
			// else
			// {
			// 	memcpy(attribute,recordStr+DB->dataDict[dictID].attr[i].offset,DB->dataDict[dictID].recordLength-DB->dataDict[dictID].attr[i].offset);//当该属性为最后一个属性时，长度为总属性长度减去该属性的偏移量
			// }
			insert_index(DB->dataDict[dictID].tableName, DB->dataDict[dictID].attr[i].name, &record);
			printf("------- insert_index success -------\n");
		}
	}
}

// another version of insertRecord, no use now.
void insertOneRecord(int dictID,char *record){//dictID为DB->dataDict[]的下标
	int length = strlen(record);
	int fileID = DB->dataDict[dictID].fileID;
	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){									//这一块是查找文件是否存在
		if(DB->dbMeta.fileMeta[i].id==fileID){						//
			querypage=DB->dbMeta.fileMeta[i].firstPageNo;			//
			break;													//
		}															//
	}
	if(querypage<0){
		printf("该表对应的文件号不存在！\n");
		exit(0);
	}
	
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	int fileno = i;
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);				//读取该文件的信息
	//rewind(DB->dbFile);					
	bool isfound = false;
	struct PageMeta pagehead;
	struct BufTag buftag2 = Buf_GenerateTag(CurpageNo);
	memcpy(&pagehead,Buf_ReadBuffer(buftag2),sizeofpagehead);	//读取第一页的内容并存放在pagehead里
	OffsetInPage preoffset, curoffset;							//页里的记录索引的结构体，定义在file.h里
	long currecordpos,curoffsetpos;								//前一个是指当前记录索引的位置，第二个是指当前记录的位置
	for(int i=0;i<pagenum;i++){									//该循环是为了遍历所有的页找出能存放该记录的页
		// printf("page:%d,空闲空间：%ld\n",i,pagehead.freeSpace);
		if(pagehead.freeSpace<=length+sizeofrecord){
			if(pagehead.nextPageNo==-1){
				break;
			}
			CurpageNo = pagehead.nextPageNo;
			buftag2 = Buf_GenerateTag(CurpageNo);
			memcpy(&pagehead,Buf_ReadBuffer(buftag2),sizeofpagehead);
			continue;	
		}
		else{
			memcpy(&preoffset,Buf_ReadBuffer(buftag2)+sizeofpagehead,sizeofrecord);
			isfound = true;
			if(pagehead.recordNum==0){
				curoffset.recordID = 0;
				curoffset.offset = length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead;
				curoffsetpos =  PAGE_SIZE - length;
			}
			else{
				memcpy(&preoffset,Buf_ReadBuffer(buftag2)+sizeofpagehead+(pagehead.recordNum-1)*sizeofrecord,sizeofrecord);
				curoffset.recordID = pagehead.recordNum;
				curoffset.offset = preoffset.offset+length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead + sizeofrecord*pagehead.recordNum; //currecordpos 等价于，页顶已被占据的空间大小。页前面放record描述信息，具体record数据存在页底。
				curoffsetpos = PAGE_SIZE - preoffset.offset-length;
			}
			
		}
		pagehead.recordNum++;
		pagehead.freeSpace=pagehead.freeSpace-length-sizeofrecord;
		memcpy(Buf_ReadBuffer(buftag2),&pagehead,sizeofpagehead);
		memcpy(Buf_ReadBuffer(buftag2)+currecordpos,&curoffset,sizeofrecord);
		memcpy(Buf_ReadBuffer(buftag2)+curoffsetpos,record,length);
		break;						//找到后就break
	}
	if(!isfound){					//若遍历完没有页就新申请一个页。
		long CurpageNo = page_requestPage(1);
		if(CurpageNo>=0){
			DB->dbMeta.blockFree=DB->dbMeta.blockFree-1;
			// file_print_freepace();
			struct PageMeta pagemeta; //pagehead就是未申请前最后一个页
			pagemeta.nextPageNo=-1;
			pagemeta.prePageNo=pagehead.pageNo;				
			pagemeta.pageNo=CurpageNo;
			// pagehead.nextPageNo = CurpageNo;		//将这页加在这个文件中				
			pagemeta.recordNum = 1;
			pagemeta.freeSpace = PAGE_SIZE - length - sizeofpagehead - sizeofrecord;
			curoffsetpos = PAGE_SIZE-length;
			currecordpos = sizeofpagehead;
			curoffset.recordID = 0;
			curoffset.offset = length;
			curoffset.isDeleted = false;
			buftag2 = Buf_GenerateTag(pagenum);
			memcpy(Buf_ReadBuffer(buftag2),&pagemeta,sizeofpagehead);
			memcpy(Buf_ReadBuffer(buftag2)+currecordpos,&curoffset,sizeofrecord);
			memcpy(Buf_ReadBuffer(buftag2)+curoffsetpos,record,length);
			memcpy(Buf_ReadBuffer(buftag2),&pagehead,sizeofpagehead);

			DB->dbMeta.fileMeta[fileno].pageNum++;
		}
		else{
			printf("记录插入失败！\n");
		}
	}
}

void deleteRecord(char *tableName, char *attributeName, Record *record)
{
	delete_index(tableName, attributeName, record);
}

Record* searchRecord(char* tableName, char* attrName, char* attribute)
{
	Record* recordList;//尚未申请内存，careful！
	
	search_index(tableName, attrName, attribute, recordList);
	

	return recordList;
}

int create_tmptable(Table table_1){
	int fileID = file_newFile(TMP_TABLE_FILE,1);//文件类型的标识值，3表示为临时表，type为段的标识值。
	int dictID = -1;
	for(int i=0;i<MAX_FILE_NUM; i++){
		printf("%d\n", DB->dataDict[i].fileID);
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

int createTmpTable2(Table table1,Table table2,int table1_pub_attr,int table2_pub_attr)
{
	int fid = file_newFile(TMP_TABLE_FILE, 1);
	int dictID = -1;
	for (int i = 0; i < MAX_FILE_NUM; i++)
	{
		if (DB->dataDict[i].fileID < 0 && DB->dataDict[i].attrNum == 0)
		{
			//先把r1完全赋给 DB->dataDict[i]
			DB->dataDict[i] = table1;
			DB->dataDict[i].fileID = fid;
			strcat(DB->dataDict[i].tableName, "_");
			strcat(DB->dataDict[i].tableName, table2.tableName);
			strcat(DB->dataDict[i].tableName, "_tmp");
			DB->dataDict[i].attrNum = table1.attrNum + table2.attrNum - 1;
			int j = table1.attrNum;
			for (int k = 0; k < table2.attrNum; k++)
			{
				if (k != table2_pub_attr)
				{
					DB->dataDict[i].attr[j] = table2.attr[k];
					// DB->dataDict[i].isIndexed[j] = table2.isIndexed[k];
					// DB->dataDict[i].isOrdered[j] = table2.isOrdered[k];
					j++;
				}
			}
			DB->dataDict[i].recordNum = 0;
			DB->dataDict[i].recordLength = table1.recordLength + table2.recordLength - table1.attr[table1_pub_attr].length;

			dictID = i;
			break;
		}
	}
	if (dictID < 0)
	{
		printf("当前数据库中已存在过多的关系，无法创建新关系！\n");
		return  -1;
	}
	return dictID;
}

int createTmpTableAfterSort(Table table, int shareAttr){
	int fid = file_newFile(TMP_TABLE_FILE, 1);
	int dictID = -1;
	for (int i = 0; i < MAX_FILE_NUM; i++) {
		if (DB->dataDict[i].fileID < 0){
			DB->dataDict[i] = table;
			DB->dataDict[i].fileID = fid;
			strcat(DB->dataDict[i].tableName, "_tmp");
			DB->dataDict[i].recordNum = 0;
			dictID = i;
			break;
		}
	}
	if (dictID < 0) {
		printf("当前数据库中已存在过多的关系，无法创建新关系！\n");
		return  -1;
	}

	int table_fid = table.fileID;
	long pageNo = DB->dbMeta.fileMeta[table_fid].firstPageNo;
	long pageNum = DB->dbMeta.fileMeta[table_fid].pageNum;
    /**
    *multimap多重映照容器
	*   multimap多重映照容器 : 容器的数据结构采用红黑树进行管理
	*   multimap的所有元素都是pair : 第一元素为键值(key), 不能修改; 第二元素为实值(value), 可被修改
	*   multimap特性以及用法与map完全相同，唯一的差别在于:
    *允许重复键值的元素插入容器(使用了RB - Tree的insert_equal函数)
	*   因此:
	*键值key与元素value的映照关系是多对多的关系
	*      没有定义[]操作运算
	*      按照key进行了排序
	*/  

	struct BufTag buftag;
	multimap<int, long> mmap;
	for (int i = 0; i < pageNum; i++) {
		buftag = Buf_GenerateTag(pageNo);
		struct PageMeta pm;
		memcpy(&pm, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);

		for (int j = 0; j < pm.recordNum; j++) {
			char *record = (char*)malloc(table.recordLength);
            long logicID = getLogicID(table_fid, pageNo, j);
            getRecord(pageNo, j, record);
			if (logicID < 0){
				cout << logicID << endl;
			}
			char *val = (char*)malloc(table.recordLength);
			getValueByAttrID(record, shareAttr, val);
			//暂时先只考虑排序属性为int类型的
			int tmp = atoi(val);
			mmap.insert(pair<int, long>(tmp, logicID));
		}

		if (pm.nextPageNo < 0)
			break;
		else
			pageNo = pm.nextPageNo;
	}
	//    int count = 0;
	while (!mmap.empty()) {
		multimap<int, long>::iterator it = mmap.begin();
		char* des = (char*)malloc(table.recordLength);

		getRecordByLogicID(table_fid, it->second, des);
		insertRecord(dictID, des);
		//        if(count == -1 || isExisted(buffNo, mapNo, DB->dbMeta.fileMeta[queryFileIndex(DB, table.fileID)].filePageNum) == false){
		//            buffNo[count] =mapNo;
		//            count++;
		//            DB->buff.map[mapNo].isPin = true;
		//        }        
		mmap.erase(it);
	}
	return dictID;
}