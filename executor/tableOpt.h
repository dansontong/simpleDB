#ifndef TABLEOPT_H_INCLUDE
#define TABLEOPT_H_INCLUDE

#include "config.h"
#include "buffer.h"
#include "file.h"
#include "database.h"
#include "index.h"

// record 
bool getRecordByLogicID(int fileID,long logicID,char* result);//�����߼��Ż��record����result
long getLogicID(int fileID, long pageNo,int recordNo);//���ظ�����¼��logicID
// void queryRecordByLogicID(long logicID, char* record);
int getValueByAttrID(char *str, int index, char *result);//���ݼ�¼�������ڱ�ͷ���±귵�ؾ��������ֵ

bool getRecord(long pageNo,int recordID,char *record);//record �洢���صļ�¼
bool getrecordAttribute(long pageNo,int recordID,char* Attributename,char*Attribute,char* tablename,char* posOffset);//Attribute�洢���ص�����ֵ
//bool getRecord(int fileID, int recordID, char *record);//fileID->table, recordID->record
//bool getRecordAttr(int fileID, int recordID, char AttrName char *recordAttr);//fileID->table, recordID->record



// ��¼���  --from database.h
void insertRecord(int dictID, char *record);
void insertOneRecord(int dictID,char *record);//���������һ������ ----from tmpTable.h
void deleteRecord(char *tableName, char *attributeName, Record *record);
Record* searchRecord(char* tableName,char* attrName, char* attribute);

// �����  --from database.h
int createTable(char *tableName);
int createTable2(char *tableName);
void insertAttr(Table *table, const char *name, DATA_TYPE type, int length,bool notNull);
int getDictIDbyName(char *tableName);
int getAttrIndexByName(int dictID, char *attrName);



//======================= tmp_table ================================
int create_tmptable(Table table_1);//���ݱ�����ʱ��
int createTmpTable2(Table table1,Table table2,int table1_pub_attr,int table2_pub_attr);
int createTmpTableAfterSort(Table table,int shareAttr);

//======================== tmp_table ===============================


#endif