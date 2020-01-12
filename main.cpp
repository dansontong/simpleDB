#include "database.h"
#include "log.h"
#include <string.h>


int main(int argc, char* argv[])
{
	//初始化数据库
	DataBase DB;
	// char dbFile[30] = "./data/simpleDb";//see config.h

	// log_init("./DBrun.log");//初始化日志系统,contain by Buf_init()
	Buf_Init();
	initDB(&DB, DB_FILE);

	log_Debug("DB initial done.\n");

	//展示数据库
	showDbInfo();

	//进入交互,dos或window界面	
	if(argc > 1 && strcmp(argv[1], "win") == 0)
	{
		winMain(argc, argv);
	}else
	{
		dosMain();
	}
	

	if(DB.dbFile  != NULL)
	{
		closeDB();
	}
}
