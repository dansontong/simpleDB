#include "database.h"
#include "index.h"
#include "operator.h"
#include <stdio.h>
#include <string.h>
#include "executor.h"

// #include "mainwindow.h"
// #include <QApplication>

extern DataBase* DB;


void winMain(int argc, char* argv[])
{
	// windowApp();
    // QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    // return a.exec();
}

void print_help()
{
	printf("usage:\n");
	printf("- %-15s get help.\n","help");
	printf("- %-15s delete database.\n","del-db");
	printf("- %-15s create database.\n","create-db");
	printf("- %-15s show data dictionary.\n","data-dict");
	printf("- %-15s insert tuple of supplier.tbl.\n","insert-sup");
	printf("- %-15s select all from table supplier.\n","select-all");
	printf("- %-15s Memory to disk.\n","memtodisk");
	printf("- %-15s find record of key.\n","find key");
	printf("- %-15s create index on S_SUPPKEY of Supplier.\n","create-index");
	printf("- %-15s exit DBMS.\n","exit");
}

void dosMain()
{
	print_help();
	
	while(1)
	{
		char sql[512];
		char strIn[512];
		memset(strIn, 0, 512);
		printf("\033[32msql>> \033[0m");
		// gets(strIn); // deprecated
		fgets(strIn, 512, stdin);
		strcpy(sql, strIn);
		// scanf("%s",strIn); // stop by 'space'

		// split strIn to list
		char strlist[50][50]={""};/*指定分隔后子字符串存储的位置，这里定义二维字符串数组*/
		char seg[] = " ";
		char *substr;
		int varNum = 0;
		substr = strtok(strIn, "\n");
		substr = strtok(substr, seg);
		while(substr){
			strcpy(strlist[varNum], substr);
			// printf("varNum-%d:, %s, length:%d\n", varNum, substr,strlen(substr));
			varNum++;
		    substr = strtok(NULL, seg);
		}
		// printf("cmd: %s\n", strlist[0]);

		if(strcmp(strlist[0], "help")==0)
		{
			print_help();
		}
		else if(strcmp(strlist[0], "del-db")==0 || strcmp(strlist[0], "delete-db")==0)
		{
			printf("are you sure to \"drop database\"? yes/no: \n");
			fgets(strIn, 10, stdin);
			if(strcmp(strIn, "yes\n")==0){
				printf("close database and delete DB file. ...\n");
				closeDB();
				deleteDB();
			}
		}
		else if(strcmp(strlist[0], "data-dict")==0)
		{
			for (int i = 0; i < MAX_FILE_NUM; i++) {//数据字典
				printf("dataDict[%d].fileID: %d\n", i, DB->dataDict[i].fileID);
				printf("dataDict[%d].name: %s\n", i, DB->dataDict[i].tableName);
			}
		}
		else if(strcmp(strlist[0], "exit")==0)
		{			
			closeDB();
			exit(0);
		}
		else if(strcmp(strlist[0], "memtodisk")==0)
		{			
			memToDisk();
		}
		else if(strcmp(strlist[0], "reset-db")==0)
		{			
			closeDB();
			deleteDB();
			initDB(DB, DB_FILE);
		}
		else if(strcmp(strlist[0], "create-db")==0)
		{
            initDB(DB, DB_FILE);
		}
		else if(strcmp(strlist[0], "insert-sup")==0)
		{
			//创建表
			char tableFile[30] = "./data/table_list";
			Attribute attr_list[5];
			strcpy(attr_list[0].name, "S_SUPPKEY");
			attr_list[0].type = INT_TYPE;
			attr_list[0].length = 4;
			attr_list[0].notNull = true;
			strcpy(attr_list[1].name, "S_NAME");
			attr_list[1].type = CHAR_TYPE;
			attr_list[1].length = 25;
			attr_list[1].notNull = true;
			strcpy(attr_list[2].name, "S_ADDRESS");
			attr_list[2].type = VARCHAR_TYPE;
			attr_list[2].length = 40;
			attr_list[2].notNull = true;
			strcpy(attr_list[3].name, "NATIONKEY");
			attr_list[3].type = INT_TYPE;
			attr_list[3].length = 4;
			attr_list[3].notNull = true;
			strcpy(attr_list[4].name, "S_PHONE");
			attr_list[4].type = CHAR_TYPE;
			attr_list[4].length = 15;
			attr_list[4].notNull = true;
			strcpy(attr_list[5].name, "S_ACCTBAL");
			attr_list[5].type = FLOAT_TYPE;
			attr_list[5].length = 8;
			attr_list[5].notNull = true;
			strcpy(attr_list[6].name, "S_COMMENT");
			attr_list[6].type = VARCHAR_TYPE;
			attr_list[6].length = 101;
			attr_list[6].notNull = true;
			int sup_dictID = createTable("Supplier", attr_list, 7);//
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

			strcpy(attr_list[0].name, "NATIONKEY");
			attr_list[0].type = INT_TYPE;
			attr_list[0].length = 4;
			attr_list[0].notNull = true;
			strcpy(attr_list[1].name, "N_NAME");
			attr_list[1].type = CHAR_TYPE;
			attr_list[1].length = 25;
			attr_list[1].notNull = true;
			strcpy(attr_list[2].name, "N_REGIONKEY");
			attr_list[2].type = INT_TYPE;
			attr_list[2].length = 4;
			attr_list[2].notNull = true;
			strcpy(attr_list[3].name, "N_COMMENT");
			attr_list[3].type = VARCHAR_TYPE;
			attr_list[3].length = 101;
			attr_list[3].notNull = true;
			int nation_dictID = createTable("nation", attr_list, 4);//
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
		else if(strcmp(strlist[0], "create-index")==0)
		{
			create_index("Supplier","S_SUPPKEY");
		}
		else if(strcmp(strlist[0], "drop-index")==0)
		{
			drop_index("Supplier","S_SUPPKEY");
		}
		else if(strcmp(strlist[0], "find")==0)
		{
			Record *recordList;
			if(varNum < 2){
				// printf("no enough parameters, at least 2.\n");
				// recordList = searchRecord("Supplier","S_SUPPKEY","Not_exist_for_test");
				recordList = searchRecord("Supplier","S_SUPPKEY","9980");
			}
			else
			{
				recordList = searchRecord("Supplier","S_SUPPKEY",strlist[1]);
			}
		}
		else if(strcmp(strlist[0], "select-all")==0)
		{
			// tableScanSelector(0, "S_SUPPKEY");
			tableScanSelector(0);
		}
		else if(strcmp(strlist[0], "drop-db")==0)
		{
			printf("drop.\n");
		}
		else if(varNum==0)
		{
		}
		else
		{
			printf("sql:%s\n", sql);
			execute_sql(sql, strlen(sql));
			// printf("unknown command. try help.\n");
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
