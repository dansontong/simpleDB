#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"
#include "buffer.h"


int create_tmptable(Table table_1);//根据表创建临时表
void insert_onerecord(int dictID,char *record)//往临时表里插入一条数据
#endif