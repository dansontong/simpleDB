#include <stdlib.h>
#include "database.h"
// include the sql parser
// #include "SQLParser.h"
// contains printing utilities
// #include "util/sqlhelper.h"

//==================== global variable ====================
struct DataBase *DB = NULL; /* 模块内共享 */

//====================    function     ====================
void initDB(struct DataBase *db, char *fileName)
{
	DB = db;
	DB->dbFile = fopen(fileName, "rb");
	if(DB->dbFile == NULL)
	{
		printf("DataBase isn't exist, creating new dataBase.\n");
		createDbFile(fileName);
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
		printf("dataDictionary file doesn't exist.\n");
		fid = file_newFile(DATA_DICT_FILE, 1);
		DB->dbMeta.dataDictFid = fid;
		for (int i = 0; i < MAX_FILE_NUM; i++) {//一律重置数据字典，后续需要更改
			memset(&DB->dataDict[i], 0, sizeof(Table));
			DB->dataDict[i].fileID = -1;
		}
	}
	int count = readDataDictionary();
	// for (int i = count; i < MAX_FILE_NUM; i++) {//一律重置数据字典，后续需要更改
	// 	memset(&DB->dataDict[i], 0, sizeof(Table));
	// 	DB->dataDict[i].fileID = -1;
	// }
	
	printf("initDB done.\n");
	//建立表
	//插入数据
}

// 关闭数据库
void closeDB()
{
	fclose(DB->dbFile);
	DB->dbFile = NULL;
	free(DB->freeSpaceBitMap);
	memToDisk();
}

// 删除数据库
void deleteDB()
{
	if(remove(dbFile)==0)
	{
		cout<<"delete database success."<<endl;
	}
	else
	{
		cout<<"delete database failed."<<endl;
	}
}

int readDataDictionary()
{
	int fid = DB->dbMeta.dataDictFid;
	printf("DB->dbMeta.dataDictFid:%d\n", fid);
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

void createDbFile(char *fileName)
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
	memset(db.dbMeta.fileMeta, -1, sizeof(struct FileMeta) * MAX_FILE_NUM);
	//db.dbMeta.fileMeta[MAX_FILE_NUM] = new FileMeta[MAX_FILE_NUM];
	// db.dbMeta.fileMeta[0].id = 0;
	// strcpy(db.dbMeta.fileMeta[0].name, "0");
	// db.dbMeta.fileMeta[0].segNum = SEGMENT_NUM;
	// db.dbMeta.fileMeta[0].state = 1;
	// memset(db.dbMeta.fileMeta[0].segList, -1, sizeof(struct Segment) * SEGMENT_NUM);

	// 为空闲空间映射表分配空间，所有的初始化为0，表示空闲
	db.freeSpaceBitMap = (unsigned long *)malloc(db.dbMeta.bitMapSize);
	memset(db.freeSpaceBitMap, 0, db.dbMeta.bitMapSize);

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

void showDbInfo(){
	Buf_PrintInfo();
}

int memToDisk(){
	saveDbHead();
	bufToDisk();
	return 0;
}

int saveDbHead(){
	
	return 0;
}




