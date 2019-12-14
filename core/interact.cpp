#include "database.h"
#include "index.h"
#include "operator.h"
#include <stdio.h>
#include <string.h>

extern DataBase* DB;

void winMain()
{
	// windowApp();
	printf("not implement yet.TODO.\n");
}

void dosMain()
{
	while(1)
	{
		char strIn[512];
		printf(">> ");
		// gets(strIn); // deprecated
		fgets(strIn, 512, stdin);
		// scanf("%s",strIn); // stop by 'space'

		if(strcmp(strIn, "help\n")==0)
		{
			printf("usage:\n");
			printf("- %-15s, get help.\n","help");
			printf("- %-15s, delete database.\n","delete db");
			printf("- %-15s, create database.\n","create db");
			printf("- %-15s, show data dictionary.\n","data dict");
			printf("- %-15s, insert tuple of supplier.tbl.\n","insert");
			printf("- %-15s, select all from table supplier.\n","select all");
			printf("- %-15s, Memory to disk.\n","memtodisk");
			printf("- %-15s, exit DBMS.\n","exit");
		}
		else if(strcmp(strIn, "del db\n")==0 || strcmp(strIn, "delete db\n")==0)
		{
			printf("are you sure to \"drop database\"? yes/no: \n");
			fgets(strIn, 10, stdin);
			if(strcmp(strIn, "yes\n")==0){
				printf("close database and delete DB file. ...\n");
				closeDB();
				deleteDB();
			}
		}
		else if(strcmp(strIn, "data dict\n")==0)
		{
			printf("dataDictFid: %d\n", DB->dbMeta.dataDictFid);
			for (int i = 0; i < MAX_FILE_NUM; i++) {//一律重置数据字典，后续需要更改
				printf("dataDictFid[%d]: %d\n", i, DB->dataDict[i].fileID);
			}
		}
		else if(strcmp(strIn, "exit\n")==0)
		{			
			closeDB();
			exit(0);
		}
		else if(strcmp(strIn, "memtodisk\n")==0)
		{			
			memToDisk();
		}
		else if(strcmp(strIn, "reset db\n")==0)
		{			
			closeDB();
			deleteDB();
			initDB(DB, dbFile);
		}
		else if(strcmp(strIn, "create db\n")==0)
		{
              
			createDbFile(dbFile);
		}
		else if(strcmp(strIn, "insert\n")==0)
		{
			//创建表
			char tableFile[30] = "./data/table_list";
			int sup_dictID = createTable(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
			//读入数据，插入记录
			char tupleFile[30] = "./data/supplier.tbl";
			char buff[1000];
			FILE *fp = fopen(tupleFile, "rb");
			
			//================ index_test ================
			int count = 0;
			while(NULL != fgets(buff, 1000, fp))
			{
				strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
				//char *str = strtok(buff, "|");//使用|分割
				Log(INFO, "----- begin insert line No.%d -----", count);
				insertRecord(sup_dictID, buff);
				printf("----- end insert line No.%d -----\n", count++);
				//printf("%s\n", buff);
			}

			int nation_dictID = createTable2(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
			//读入数据，插入记录
			char tupleFile2[30] = "./data/nation.tbl";
			fp = fopen(tupleFile2, "rb");
			
			//================ index_test ================
			count = 0;
			while(NULL != fgets(buff, 1000, fp))
			{
				strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
				//char *str = strtok(buff, "|");//使用|分割
				Log(INFO, "----- begin insert line No.%d -----", count);
				insertRecord(nation_dictID, buff);
				printf("----- end insert line No.%d -----\n", count++);
				//printf("%s\n", buff);
			}
		}
		else if(strcmp(strIn, "select all\n")==0)
		{
			tableScanSelector(0, "S_SUPPKEY");
		}
		else if(strcmp(strIn, "drop database\n")==0)
		{
			printf("drop.\n");
		}
		else if(strcmp(strIn, "\n")==0)
		{
		}
		else
		{
			printf("unknown command. try help.\n");
		}
	}
	return;


	// memToDisk();
	
	// //创建表
	// char tableFile[30] = "../data/table_list";
	// int sup_dictID = createTable(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
	// //读入数据，插入记录
	// char tupleFile[30] = "../data/supplier.tbl";
	// char buff[1000];
	// FILE *fp = fopen(tupleFile, "rb");
	
	// //================ index_test ================
	// int count = 0;
	// while(NULL != fgets(buff, 1000, fp))
	// {
	// 	strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
	// 	//char *str = strtok(buff, "|");//使用|分割
	// 	Log(INFO, "----- begin insert line No.%d -----", count);
	// 	insertRecord(sup_dictID, buff);
	// 	printf("----- end insert line No.%d -----\n", count++);
	// 	//printf("%s\n", buff);
	// }

	// int nation_dictID = createTable2(tableFile);//后续需要改，要能根据parser解析结果自动创建表,目前只创建supplier,nation.tbl
	// //读入数据，插入记录
	// char tupleFile2[30] = "../data/nation.tbl";
	// fp = fopen(tupleFile2, "rb");
	
	// //================ index_test ================
	// count = 0;
	// while(NULL != fgets(buff, 1000, fp))
	// {
	// 	strtok(buff, "\n");//使用换行符分割，相当于去掉换行符
	// 	//char *str = strtok(buff, "|");//使用|分割
	// 	Log(INFO, "----- begin insert line No.%d -----", count);
	// 	insertRecord(nation_dictID, buff);
	// 	printf("----- end insert line No.%d -----\n", count++);
	// 	//printf("%s\n", buff);
	// }

	// // test pageNo & recordNum
	// int fileID = DB.dataDict[nation_dictID].fileID;
	// long CurpageNo = DB.dbMeta.fileMeta[fileID].firstPageNo;				
	// long pageNum = DB.dbMeta.fileMeta[fileID].pageNum;
	// struct BufTag buftag = Buf_GenerateTag(CurpageNo);
	// struct PageMeta pageMeta;
	// memcpy(&pageMeta,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
	// printf("after insert table2: ==--==--==--==--== pageNo:%d, recordNum: %d ==--==--==--==--== \n", CurpageNo, pageMeta.recordNum);


	// create_index("Supplier","S_SUPPKEY");
	// printf("------ creat index in %s for table %s -----\n", "Supplier","S_SUPPKEY");
	

	// Record *recordList;
	// recordList = searchRecord("Supplier","S_SUPPKEY","Not_exist_for_test");
	// recordList = searchRecord("Supplier","S_SUPPKEY","9840");
	// printf("------ search record by index in column '%s' for table '%s' with value '%s' -----\n","S_SUPPKEY","Supplier","9840");
	// while(recordList != NULL)
	// {
	// 	deleteRecord("Supplier", "S_SUPPKEY", recordList);
	// 	printf("------ delete index in %s for table %s -----\n","S_SUPPKEY","Supplier");
	// 	recordList++;
	// }
	// drop_index("Supplier","S_SUPPKEY");
	// printf("------ drop index in %s for table %s -----\n","S_SUPPKEY","Supplier");

	// //================ executor_test -- projection ================
	// printf("------------------ begin ouput project on S_SUPPKEY: ---------------------\n");
	// projection(0, "S_SUPPKEY");

	// //================ executor_test -- select ================
	// printf("------------------ begin ouput record == 2: ---------------------\n");
	// tableScanEqualSelector(0, "S_SUPPKEY", "2");
	// printf("------------------ begin ouput 8 >= record >= 4: ---------------------\n");
	// tableScanRangeSelector(0,"S_SUPPKEY","4","8");//根据给定的属性列上的属性值范围min-max进行选择，返回存储结果的临时表的下标
	// printf("------------------ begin ouput record <= 9: ---------------------\n");
	// tableScanMaxRangeSelector(0,"S_SUPPKEY","9");//只有max值
	// printf("------------------ begin ouput record >= 19: ---------------------\n");
	// tableScanMinRangeSelector(0,"S_SUPPKEY","19");//只有最小值
	// printf("------------------ begin ouput record != 2: ---------------------\n");
	// tableScanUnEqualSelector(0,"S_SUPPKEY","2");//非等值连接


	// //================ executor_test -- joint ================
	// printf("------------------ begin nestedLoopJoin: ---------------------\n");
	// nestedLoopJoin(sup_dictID, nation_dictID);
	// printf("------------------ begin HashJoin: ---------------------\n");
	// HashJoin(sup_dictID, nation_dictID);



	// printf("------------------ end executor_test ---------------------\n");

	// showDbInfo();

}