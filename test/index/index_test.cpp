#include "log.h"

int main(int argc, char* argv[]) {
	printf("====================== index_test begin ======\n");
	//初始化数据库
	DataBase DB;
	char dbFile[30] = "./data/simpleDb";

	log_init();//初始化日志系统
	database_initDB(&DB, dbFile);
	Buf_Init(&DB);
	






	printf("======================= index_test end ========\n");
}
