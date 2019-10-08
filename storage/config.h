#ifndef STORAGE_CONFIG_H
#define STORAGE_CONFIG_H

#include <iostream>
#include <cstdio>
#include <time.h>
#include <vector> 
#include <fstream>
#include <string>
#include <string.h>
using namespace std; 

// configuration配置参数

// 磁盘中的数据库文件 配置参数
#define BLOCK_SIZE (4*1024)            // 磁盘中数据块的大小，也等于buffer中的PAGE_SIZE
#define FILE_DATA_SIZE (1024*1024*1024)// 文件数据区大小1G，单位字节
#define FILE_DATA_ADDR (BIT_MAP_ADDR + BIT_MAP_SIZE)         // 数据区起始位置
#define BIT_MAP_SIZE (FILE_DATA_SIZE / BLOCK_SIZE / 8) // 位示图大小
#define BIT_MAP_ADDR 1024               // 位示图起始位置   
//TODO: 需测试确保dbMeta不会超出1024，否则位示图数据将会与之重叠，会出现未知错误

// file 配置参数
#define MAX_FILE_NUM 30      //最大文件数量
#define MAP_FILE         0
#define NORMAL_FILE      1
#define INDEX_FILE_BTREE 2

// page 配置参数
#define PAGE_SIZE BLOCK_SIZE//页大小==文件块大小

// buffer 配置参数
#define BUFFER_NUM 2048     //缓冲区数量,每个4k，总大小8M

// segment
#define SEGMENT_NUM 5       //文件的段数

class Config{
public:
	Config();
	~Config();
};

#endif