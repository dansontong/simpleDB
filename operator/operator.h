#ifndef OPERATOR_H_INCLUDE
#define OPERATOR_H_INCLUDE

#include "config.h"
#include "database.h"
#include "file.h"
#include "buffer.h"
#include "log.h"
#include "tableOpt.h"	


// ==================== select.cpp =====================
int tableScanEqualSelector(int dictID,char* attri_name,char* value);//根据给定的属性列上的属性值进行选择，返回存储结果的临时表的下标
int tableScanRangeSelector(int dictID,char* attri_name,char* min,char* max);//根据给定的属性列上的属性值范围min-max进行选择，返回存储结果的临时表的下标
int tableScanMaxRangeSelector(int dictID,char* attri_name,char* max);//只有max值
int tableScanMinRangeSelector(int dictID,char* attri_name,char* min);//只有最小值
int tableScanUnEqualSelector(int dictID,char* attri_name,char* value);//非等值连接
int tableScanSelector(int dictID,char* attri_name);//select*
int tableScanSelector(int dictID);//select*


// ==================== projection.cpp ==================
int projection(int dictID,char* attributename);//返回投影所在的临时表的下标



// ==================== join.cpp ========================
int nestedLoopJoin(int employee_dictID, int department_dictID);
int HashJoin(int employee_dictID, int department_dictID);
int SortJoin(int table1_dictID, int table2_dictID);
int nestedLoopJoinByThree(int table1_dictID, int table2_dictID, int table3_dictID);

void HashRelation( Table table1, int attrid, multimap<int, long> *mmap);//建立hash桶
int hashToBucket(int value);

#endif