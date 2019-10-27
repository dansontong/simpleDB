#include "config.h"
#include "file.h"
#include "buffer.h"
#include "database.h"


typedef struct
{
	int key;        //索引的列值
	int pos;
	int posPage;    //列值对应的页号
    int posOffset;  //列值的偏移量
}TreeRecord;      //索引的记录