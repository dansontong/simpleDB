#include "database.h"
#include "log.h"


int main(int argc, char* argv[])
{
	//初始化数据库
	DataBase DB;
	char dbFile[30] = "../data/simpleDb";

	log_init("./DBrun.log");//初始化日志系统
	Buf_Init();
	database_initDB(&DB, dbFile);

	log_Debug("DB initial done.\n");

	//展示数据库
	database_showDbInfo();

	//进入交互,dos或window界面
	// dosInteract();

	if(DB.dbFile  != NULL)
	{
		database_closeDB();
	}
}
