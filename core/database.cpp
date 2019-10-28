#include <stdlib.h>
#include "database.h"
#include "index.h"
// include the sql parser
// #include "SQLParser.h"
// contains printing utilities
// #include "util/sqlhelper.h"

//==================== global variable ====================
struct DataBase *DB = NULL; /* 模块内共享 */

//====================    function     ====================
void database_initDB(struct DataBase *db, char *fileName)
{
	DB = db;
	DB->dbFile = fopen(fileName, "rb");
	if(DB->dbFile == NULL)
	{
		printf("DataBase isn't exist, creating new dataBase.\n");
		database_createDbFile(fileName);
		DB->dbFile = fopen(fileName, "rb");
	}
	
	size_t sizeRead = fread(&(DB->dbMeta), sizeof(struct DbMeta), 1, DB->dbFile);
	// 为空闲空间的位示图bitMap分配空间
	DB->freeSpaceBitMap = (unsigned long *)malloc(DB->dbMeta.bitMapSize);
	//rewind(DB->dbFile);
	// 在文件中定位到bitMap开始的位置，令文件指针指向这里
	fseek(DB->dbFile, DB->dbMeta.bitMapAddr, SEEK_SET);
	sizeRead = fread(DB->freeSpaceBitMap, DB->dbMeta.bitMapSize, 1, DB->dbFile);// 从文件中读取bitMap的内容
	fclose(DB->dbFile);
	DB->dbFile = fopen(fileName, "rb+");

	//加载 数据字典
	int fid = DB->dbMeta.dataDictFid;
	if(fid < 0)
	{
		//printf("dataDictionary file doesn't exist.\n");
		fid = file_newFile(DATA_DICT_FILE, 1);
		DB->dbMeta.dataDictFid = fid;
	}
	int count = readDataDictionary();
	//一律重置数据字典，后续需要更改
	for (int i = count; i < MAX_FILE_NUM; i++) {
		memset(&DB->dataDict[i], 0, sizeof(Table));
		DB->dataDict[i].fileID = -1;
	}
	printf("initDB done.\n");
	//建立表
	//插入数据
}

// 关闭数据库
void database_closeDB()
{
	fclose(DB->dbFile);
	free(DB->freeSpaceBitMap);
}

int readDataDictionary()
{
	int fid = DB->dbMeta.dataDictFid;
	if (fid < 0) {
		printf("Data dictionary file does not exist.");
		return 0;
	}
	long pageNo = DB->dbMeta.fileMeta[fid].firstPageNo;
	long pageNum = DB->dbMeta.fileMeta[fid].pageNum;
	int count = 0;
	for (long i = 0; i < pageNum; i++) {
		struct PageMeta pageHead;
		BufTag buftag =  Buf_GenerateTag(pageNo);
		int sizeofpagehead = sizeof(struct PageMeta);
		memcpy(&pageHead, Buf_ReadBuffer(buftag), sizeofpagehead);
		for (int j = 0; j < pageHead.recordNum; j++) {
			memcpy(&DB->dataDict[count], Buf_ReadBuffer(buftag) + sizeofpagehead + j * sizeof(Table), sizeof(Table));
			count++;
		}
		if (pageHead.nextPageNo < 0)
			break;
		else
			pageNo = pageHead.nextPageNo;
	}
	printf("read Data dictionary done.\n");
	return count;
}

void database_createDbFile(char *fileName)
{
	DataBase db;
	db.dbMeta.blockSize = BLOCK_SIZE;
	db.dbMeta.blockNum = FILE_DATA_SIZE / BLOCK_SIZE; // 256*1024
	db.dbMeta.blockFree = db.dbMeta.blockNum;
	db.dbMeta.bitMapSize = BIT_MAP_SIZE;
	db.dbMeta.bitMapAddr = BIT_MAP_ADDR;
	db.dbMeta.dataSize = FILE_DATA_SIZE;
	db.dbMeta.dataAddr = FILE_DATA_ADDR;
	db.dbMeta.currFileNum = 0;
	db.dbMeta.dataDictFid = -1;
	//db.dbMeta.fileMeta[MAX_FILE_NUM] = new FileMeta[MAX_FILE_NUM];
	db.dbMeta.fileMeta[0].id = 0;
	strcpy(db.dbMeta.fileMeta[0].name, "0");
	db.dbMeta.fileMeta[0].segNum = SEGMENT_NUM;
	db.dbMeta.fileMeta[0].state = 1;
	memset(db.dbMeta.fileMeta[0].segList, -1, sizeof(struct Segment) * SEGMENT_NUM);

	// 为空闲空间映射表分配空间，所有的初始化为-1，表示空闲
	db.freeSpaceBitMap = (unsigned long *)malloc(db.dbMeta.bitMapSize);
	memset(db.freeSpaceBitMap, -1, db.dbMeta.bitMapSize);

	db.dbFile = fopen(fileName, "wb");
	// 把初始化的相关参数写到数据库文件头部
	fwrite(&(db.dbMeta), sizeof(struct DbMeta), 1, db.dbFile);
	// 把bitMap写到数据库文件中
	fseek(db.dbFile, db.dbMeta.bitMapAddr, SEEK_SET);
	fwrite(db.freeSpaceBitMap, db.dbMeta.bitMapSize, 1, db.dbFile);

	// close db
	free(db.freeSpaceBitMap);
	fclose(db.dbFile);
	
	printf("create dataBase done.\n");
}

void database_showDbInfo(){
	Buf_PrintInfo();
}

int database_memToDisk(){

	return 0;
}

bool tupleInsert(int length, int FileID, char *str){

}

void insertAttr(Table *table,const char *name, DATA_TYPE type, int length,bool notNull)
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
	table->attrLength += length;
}

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

	//插入属性
	insertAttr(&DB->dataDict[dictID],"S_SUPPKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"S_NAME",CHAR_TYPE,25,true);
	insertAttr(&DB->dataDict[dictID],"S_ADDRESS",VARCHAR_TYPE,40,true);
	insertAttr(&DB->dataDict[dictID],"S_NATIONKEY",INT_TYPE,4,true);
	insertAttr(&DB->dataDict[dictID],"S_PHONE",CHAR_TYPE,15,true);
	insertAttr(&DB->dataDict[dictID],"S_ACCTBAL",FLOAT_TYPE,8,true);
	insertAttr(&DB->dataDict[dictID],"S_COMMENT",VARCHAR_TYPE,101,true);

	return dictID;
}

void insertRecord(int dictID, char *str)
{
	int fileID = DB->dataDict[dictID].fileID;
	int length = strlen(str);
	Record record = file_writeFile(fileID, length, str);
	
	char recordStr[1000]; //假设记录最长1000个字节
	// char *attribute;
	file_getrecord(record.pageNo,record.recordID, recordStr);
	int i;
	// char *offset;
	for(i=0; i<DB->dataDict[dictID].attrNum; i++)
	{
		if( DB->dataDict[dictID].attr[i].indexFile.fileID != 0 )
		{		
			// offset = recordStr+DB->dataDict[dictID].attr[i].offset; //TODO：是否需要加上record?
			// if(i<DB->dataDict[dictID].attrNum-1)
			// {
			// 	memcpy(attribute,recordStr+DB->dataDict[dictID].attr[i].offset,DB->dataDict[dictID].attr[i+1].offset-DB->dataDict[dictID].attr[i].offset);//一般情况：位置为record的起始地址加上属性的偏移量，长度为该下一条属性的偏移量减去该属性的偏移量
			// }
			// else
			// {
			// 	memcpy(attribute,recordStr+DB->dataDict[dictID].attr[i].offset,DB->dataDict[dictID].attrLength-DB->dataDict[dictID].attr[i].offset);//当该属性为最后一个属性时，长度为总属性长度减去该属性的偏移量
			// }
			insert_index(DB->dataDict[dictID].tableName, DB->dataDict[dictID].attr[i].name, &record);
			printf("------- insert_index success -------\n");
		}
	}
}

void deleteRecord(int dictID, Record *record)
{

}

Record searchRecord(char* tableName,char* attrName, char* attribute)
{
	Record record;
	search_index(tableName, char *attributeName, char* Attribute, Record* recordList)
	

	return record;
}



