#include "config.h"
#include "file.h"
#include "buffer.h"
#include "database.h"



typedef struct
{
	int key;        //索引的列值
	int posPage;    //列值对应的页号
    int posOffset;  //列值的偏移量
    int recordID;   //记录ID
}RecordOffset;      //索引的记录


void create_index(struct DataBase *DB,char *tableName,char *Attributename);

void drop_index(struct DataBase *DB,char *tableName,char *Attributename);

void insert_index(char *tableName, char *Attributename, char* Attribute, Record* record);

void delete_index(char *tableName, char *Attributename, Record* record);

void update_index(char *tableName, char *Attributename, Record* oldRecord, Record* newRecord);