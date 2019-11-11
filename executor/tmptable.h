#ifndef TMPTABLE_H_INCLUDE
#define TMPTABLE_H_INCLUDE

#include "config.h"
#include "buffer.h"


int create_tmptable(Table table_1);//根据表创建临时表

void insert_onerecord(int dictID,char *record)//往临时表里插入一条数据
void HashRelation( Table table1, int attrid, multimap<int, long> *m);//建立hash桶

#endif