#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"
#include "buffer.h"


int tableScanEqualSelector(int dictID,char* attri_name,char* value);//根据给定的属性列上的属性值进行选择，返回存储结果的临时表的下标
int tableScanRangeSelector(int dictID,char* attri_name,char* min,char* max);//根据给定的属性列上的属性值范围min-max进行选择，返回存储结果的临时表的下标