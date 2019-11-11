#ifndef EXECUTOR_H_INCLUDE
#define EXECUTOR_H_INCLUDE

#include "config.h"
#include "buffer.h"


// ==================== select.cpp =====================
int tableScanEqualSelector(int dictID,char* attri_name,char* value);//根据给定的属性列上的属性值进行选择，返回存储结果的临时表的下标
int tableScanRangeSelector(int dictID,char* attri_name,char* min,char* max);//根据给定的属性列上的属性值范围min-max进行选择，返回存储结果的临时表的下标
int tableScanMaxRangeSelector(int dictID,char* attri_name,char* max);//只有max值
int tableScanMinRangeSelector(int dictID,char* attri_name,char* min);//只有最小值
int tableScanUnEqualSelector(int dictID,char* attri_name,char* value);//非等值连接


// ==================== projection.cpp ==================
int projection(int dictID,char* attributename);//返回投影所在的临时表的下标



// ==================== join.cpp ========================
int createTmpTableAfterSort(Table t1,int n);
int createTmpTable2(Table table1,Table table2,int table1_pub_attr,int table2_pub_attr);
Record* getNextRecord(int emp, int k, char* emp_record);
void insertOneRecord(int tmp_table_ID, char*res);

int SortJoin(int table1loyee_dictID, int department_dictID);
int nestedLoopJoin(int employee_dictID, int department_dictID);
int HashJoin(int table1loyee_dictID, int department_dictID);
int SortJoin(int table1_dictID, int table2_dictID);
int HashJoin(int table1loyee_dictID, int department_dictID);
int nestedLoopJoinByThree(int table1_dictID, int table2_dictID, int table3_dictID);

#endif