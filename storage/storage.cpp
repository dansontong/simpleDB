#include <stdlib.h>
#include "storage.h"
// include the sql parser
// #include "SQLParser.h"
// contains printing utilities
// #include "util/sqlhelper.h"


void storage_initDB(struct Storage *DB, char *fileName)
{
	FILE *dbFile = fopen(fileName, "rb");
	if(dbFile == NULL)
	{
		printf("DataBase isn't exist, creating new dataBase.\n");
		storage_createDbFile(fileName);
		dbFile = fopen(fileName, "rb");
	}

	size_t sizeRead = fread(&(DB->dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 为空闲空间的位示图bitMap分配空间
	DB->freeSpaceBitMap = (unsigned long *)malloc(DB->dbMeta.bitMapSize);
	//rewind(dbFile);
	// 在文件中定位到bitMap开始的位置，令文件指针指向这里
	fseek(dbFile, DB->dbMeta.bitMapAddr, SEEK_SET);
	sizeRead = fread(DB->freeSpaceBitMap, DB->dbMeta.bitMapSize, 1, dbFile);// 从文件中读取bitMap的内容
	fclose(dbFile);
	DB->dbFile = fopen(fileName, "rb+");

	//加载 数据字典
	int fid = DB->dbMeta.dataDictFid;
	if(fid < 0)
	{
		//printf("dataDictionary file doesn't exist.\n");
		fid = file_newFile(DB, DATA_DICT_FILE, 1);
		DB->dbMeta.dataDictFid = fid;
	}
	int count = readDataDictionary(DB);
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
void storage_closeDB(struct Storage *DB)
{
	fclose(DB->dbFile);
	free(DB->freeSpaceBitMap);
}

int readDataDictionary(struct Storage *DB)
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

void storage_createDbFile(char *fileName)
{
	struct Storage DB;
	DB.dbMeta.blockSize = BLOCK_SIZE;
	DB.dbMeta.blockNum = FILE_DATA_SIZE / BLOCK_SIZE; // 256*1024
	DB.dbMeta.blockFree = DB.dbMeta.blockNum;
	DB.dbMeta.bitMapSize = BIT_MAP_SIZE;
	DB.dbMeta.bitMapAddr = BIT_MAP_ADDR;
	DB.dbMeta.dataSize = FILE_DATA_SIZE;
	DB.dbMeta.dataAddr = FILE_DATA_ADDR;
	DB.dbMeta.currFileNum = 0;
	DB.dbMeta.dataDictFid = -1;
	//DB.dbMeta.fileMeta[MAX_FILE_NUM] = new FileMeta[MAX_FILE_NUM];
	DB.dbMeta.fileMeta[0].id = 0;
	strcpy(DB.dbMeta.fileMeta[0].name, "0");
	DB.dbMeta.fileMeta[0].segNum = SEGMENT_NUM;
	DB.dbMeta.fileMeta[0].state = 1;
	memset(DB.dbMeta.fileMeta[0].segList, -1, sizeof(struct Segment) * SEGMENT_NUM);

	// 为空闲空间映射表分配空间，所有的初始化为-1，表示空闲
	DB.freeSpaceBitMap = (unsigned long *)malloc(DB.dbMeta.bitMapSize);
	memset(DB.freeSpaceBitMap, -1, DB.dbMeta.bitMapSize);

	FILE *dbFile = fopen(fileName, "wb");
	// 把初始化的相关参数写到数据库文件头部
	fwrite(&(DB.dbMeta), sizeof(struct DbMeta), 1, dbFile);
	// 把bitMap写到数据库文件中
	fseek(dbFile, DB.dbMeta.bitMapAddr, SEEK_SET);
	fwrite(DB.freeSpaceBitMap, DB.dbMeta.bitMapSize, 1, dbFile);

	free(DB.freeSpaceBitMap);
	fclose(dbFile);
	
	printf("create dataBase done.\n");
}

void storage_showDbInfo(struct Storage *DB){
	Buf_PrintInfo();
}

int storage_memToDisk(struct Storage *DB){

	return 0;
}

bool tupleInsert(struct Storage *DB, int length, int FileID, char *str){

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
int createTable(struct Storage *DB, char *str)
{
	//解析字符串 CREATE TABLE NATION ( N_NATIONKEY INTEGER NOT NULL,N_NAMECHAR(25) NOT NULL,N_REGIONKEY INTEGER NOT NULL,N_COMMENTVARCHAR(152));
	// parse a given query
	// hsql::SQLParserResult result;
	// hsql::SQLParser::parse(query, &result);//后期再使用
	char tableName[MAX_NAME_LENGTH] = "Supplier";

	int fid = file_newFile(DB, TABLE_FILE, 1);
	//插入数据字典
	int dictID = -1;
	for(int i=0; i<MAX_FILE_NUM; i++){
		if(DB->dataDict[i].fileID < 0){
			dictID = i;
			break;
		}
	}
	DB->dataDict[dictID].fileID = fid;
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

void recordInsert(struct Storage *DB, int dictID, char *str)
{
	int fid = DB->dataDict[dictID].fileID;
	int length = strlen(str);
	file_writeFile(DB, fid, length, str);
}