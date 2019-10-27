#include "config.h"
#include "file.h"
#include "buffer.h"
#include "database.h"
#include "b_plus_tree.h"


// typedef struct
// {
// 	char* key;        //索引的列值
// 	int pos;
// 	int posPage;    //列值对应的页号
//     char* posOffset;  //列值的地址
//     int recordID;   //记录ID

// }TreeRecord;      //索引的记录


void create_index(struct DataBase *DB,char *tableName,char *attributename);

void drop_index(struct DataBase *DB,char *tableName,char *attributename);

void insert_index(char *tableName, char *attributename, char* attribute, Record* record, char* offset);

void delete_index(char *tableName, char *attributename, Record* record);

void update_index(char *tableName, char *attributename, Record* oldRecord, Record* newRecord);

void search(char *tableName, char *attributeName, char* Attribute, Record* recordList);

