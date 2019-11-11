#ifndef TABLEOPT_H_INCLUDE
#define TABLEOPT_H_INCLUDE

#include "config.h"
#include "buffer.h"
#include "file.h"
#include "database.h"
#include "index.h"

// record 
bool getRecordByLogicID(int fileID,long logicID,char* result);//根据逻辑号获得record存入result
long getLogicID(int fileID, long pageNo,int recordNo);//返回该条记录的logicID
// void queryRecordByLogicID(long logicID, char* record);
int getValueByAttrID(char *str, int index, char *result);//根据记录和属性在表头的下标返回具体的属性值

bool getRecord(long pageNo,int recordID,char *record);//record 存储返回的记录
bool getrecordAttribute(long pageNo,int recordID,char* Attributename,char*Attribute,char* tablename,char* posOffset);//Attribute存储返回的属性值
//bool getRecord(int fileID, int recordID, char *record);//fileID->table, recordID->record
//bool getRecordAttr(int fileID, int recordID, char AttrName char *recordAttr);//fileID->table, recordID->record



// 记录相关  --from database.h
void insertRecord(int dictID, char *record);
void insertOneRecord(int dictID,char *record);//往表里插入一条数据 ----from tmpTable.h
void deleteRecord(char *tableName, char *attributeName, Record *record);
Record* searchRecord(char* tableName,char* attrName, char* attribute);

// 表相关  --from database.h
int createTable(char *tableName);
int createTable2(char *tableName);
void insertAttr(Table *table, const char *name, DATA_TYPE type, int length,bool notNull);
int getDictIDbyName(char *tableName);
int getAttrIndexByName(int dictID, char *attrName);



//======================= tmp_table ================================
int create_tmptable(Table table_1);//根据表创建临时表
int createTmpTable2(Table table1,Table table2,int table1_pub_attr,int table2_pub_attr);
int createTmpTableAfterSort(Table table,int shareAttr);

//======================== tmp_table ===============================


#endif