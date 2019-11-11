#include "config.h"
#include "buffer.h"
#include "database.h"

int createTmpTableAfterSort(Table t1,int n);
int createTmpTable2(Table table1,Table table2,int table1_pub_attr,int table2_pub_attr);
Record* getNextRecord(int emp, int k, char* emp_record);
void insertOneRecord(int tmp_table_ID, char*res);

int SortJoin(int table1loyee_dictID, int department_dictID);
int nestedLoopJoin(int employee_dictID, int department_dictID);
int HashJoin(int table1loyee_dictID, int department_dictID);
int SortJoin(int table1_dictID, int table2_dictID);
int HashJoin(int table1loyee_dictID, int department_dictID);
int nestedLoopJoinByThree(int table1_dictID, int table2_dictID, int table3_dictID)