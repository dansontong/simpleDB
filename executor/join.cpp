#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "executor.h"
#include "tmptable.h"

extern struct DataBase *DB; /* 全局共享 */

int nestedLoopJoin(int employee_dictID, int department_dictID) {
    //找到两张表
    Table table1 = DB->dataDict[employee_dictID];
    Table table2 = DB->dataDict[department_dictID];

    //找到两个表需要做连接的公共下标
    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++)
    {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++)
        {
            if (table1.attr[table1_pub_attr].name==table1.attr[table2_pub_attr].name)
            {
                isFound = true;
                break;
            }
        }
        if (isFound) break;//利用标志位跳出第二重循环
    }
    //标志位如果为空的话代表两个表没有公共属性
    if (isFound == false) {
        printf("两表没有公共属性");
        return -1;
    }

    int table1_fid = table1.fileID;
    long table1_pageno = DB->dbMeta.fileMeta[table1_fid].firstPageNo;
    long table1_pagenum = DB->dbMeta.fileMeta[table1_fid].pageNum;

    int table2_fid = table2.fileID;
    long table2_pageno = DB->dbMeta.fileMeta[table2_fid].firstPageNo;
    long table2_pagenum = DB->dbMeta.fileMeta[table2_fid].pageNum;
    //p1
    int tmp_table_ID = createTmpTable2(table1,table2,table1_pub_attr,table2_pub_attr);
    if (tmp_table_ID < 0){
        printf("创建临时表失败");
        return -1;
    }

    Table *tmp = &(DB->dataDict[tmp_table_ID]);
    //两表循环连接
    int outer = table2_pagenum / BUFFER_NUM;
    struct BufTag buftag;
    int *buffID = (int*)malloc(table2_pagenum);
    for (int x = 0; x < outer; x++){
        for (int y = 0; y < BUFFER_NUM - 1 && y < table2_pagenum; y++) {
            struct PageMeta ph;
            buftag = Buf_GenerateTag(table1_pageno);
            memcpy(&ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
            // DB->buff.map[mapNo].isPin = true; //PIN住
            buffID[y] = table1_pageno;
            if (ph.nextPageNo < 0)
                break;
            else
                table1_pageno = ph.nextPageNo;
        }

        for (int z = 0; z < table1_pagenum; z++) {
            struct PageMeta emp_ph;
            buftag = Buf_GenerateTag(table1_pageno);
            memcpy(&emp_ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);//head
            for (int k = 0; k < emp_ph.pageNo; k++) {
                char *table1_record = (char*)malloc(table1.attrLength);
                char *table1_value = (char*)malloc(table1.attrLength);
                getNextRecord(table1_pageno, k, table1_record);//读取下一条记录
                int pe = getValueByAttrID(table1_record, table1_pub_attr, table1_value);
                if (pe < 0)  exit(0);
                for (int y = 0; y < BUFFER_NUM - 1 && y < table2_pagenum; y++){
                    struct PageMeta table2_ph;
                    buftag = Buf_GenerateTag(buffID[y]);
                    memcpy(&table2_ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
                    for (int i = 0; i < table2_ph.pageNo; i++) {//curRecordNUm
                        char *table2_record = (char*)malloc(table2.attrLength);
                        getNextRecord(buffID[y], i, table2_record);//head
                        char *table2_value = (char*)malloc(table2.attrLength);
                        int pd = getValueByAttrID(table2_record, table2_pub_attr, table2_value);
                        if (pd < 0) exit(0);

                        if (strcmp(table1_value, table2_value) == 0){
                            char *res = (char*)malloc(tmp->attrLength);
                            memset(res, 0, tmp->attrLength);
                            strncat(res, table2_record, pd);
                            strcpy(res, table2_record);
                            strcat(res, table2_record + pd + strlen(table2_value) + 1);
                            strcat(res, "\\ |");
                            insertOneRecord(tmp_table_ID, res);//head
                        }
                    }
                }

            }
            if (emp_ph.nextPageNo < 0)  break;
            else table1_pageno = emp_ph.nextPageNo;
        }
    }

    return tmp_table_ID;


}


int HashJoin(int table1loyee_dictID, int department_dictID){
    Table table1 = DB->dataDict[table1loyee_dictID];
    Table table2 = DB->dataDict[department_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (table1.attr[table1_pub_attr].name == table2.attr[table2_pub_attr].name){
                isFound = true;
                break;
            }
        }
        if (isFound)
            break;
    }
    if (isFound == false) {
        printf("两表没有公共特性\n");
        return -1;
    }
    int tmp_table_dictID = createTmpTable2(table1, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    multimap<int, long> m_table1[BUCKET_NUM];
    multimap<int, long> m_table2[BUCKET_NUM];


    HashRelation(table1, table1_pub_attr, m_table1);//HashRelation
    HashRelation(table2, table2_pub_attr, m_table2);//HashRelation

    for (int i = 0; i < BUCKET_NUM; i++) {
        map<int, long>::iterator it_table1, it_table2;
        for (it_table2 = m_table2[i].begin(); it_table2 != m_table2[i].end(); it_table2++) {
            for (it_table1 = m_table1[i].begin(); it_table1 != m_table1[i].end(); it_table1++){
                if (it_table1->first == it_table2->first){
                    char* record_table1 = (char*)malloc(table1.attrLength);
                    char* record_table2 = (char*)malloc(table2.attrLength);
                    queryRecordByLogicID(it_table1->second, record_table1);
                    queryRecordByLogicID(it_table2->second, record_table2);

                    char* val_table2 = new char[strlen(record_table2)];
                    int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

                    char *res = (char*)malloc(tmp->attrLength);
                    memset(res, 0, tmp->attrLength);
                    strcpy(res, record_table1);
                    strcat(res, "|");
                    strncat(res, record_table2, pd);
                    strcat(res, record_table2 + pd + strlen(val_table2) + 1);
                    insertOneRecord(tmp_table_dictID, res);
                }
                else if (it_table1->first > it_table2->first) {
                    break;
                }
                else {
                    continue;
                }
            }
        }
    }


    return tmp_table_dictID;
}


int SortJoin(int table1_dictID, int table2_dictID) {
    Table table1 = DB->dataDict[table1_dictID];
    Table table2 = DB->dataDict[table2_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (table1.attr[table1_pub_attr].name == table2.attr[table2_pub_attr].name){
                isFound = true;
                break;
            }
        }
        if (isFound)
            break;
    }
    if (isFound == false) {
        printf("两表没有公用属性\n");
        return -1;
    }
    int tmp_table_dictID = createTmpTable2(table1, table2, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }

    int tmp_table_table1 = createTmpTableAfterSort(table1, table1_pub_attr);
    int tmp_table_table2 = createTmpTableAfterSort(table2, table2_pub_attr);


    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    Table *tmp_table1 = &(DB->dataDict[tmp_table_table1]);
    Table *tmp_table2 = &(DB->dataDict[tmp_table_table2]);

    long table1_pageno = DB->dbMeta.fileMeta[tmp_table1->fileID].firstPageNo;
    long table1_pagenum = DB->dbMeta.fileMeta[tmp_table1->fileID].pageNum;

    long table2_pageno = DB->dbMeta.fileMeta[tmp_table2->fileID].firstPageNo;
    long table2_pagenum = DB->dbMeta.fileMeta[tmp_table2->fileID].pageNum;

    struct BufTag buftag;
    for (int j = 0; j < table2_pagenum; j++) {
        buftag = Buf_GenerateTag(table1_pageno);
        struct PageMeta ph_table2;
        memcpy(&ph_table2,  Buf_ReadBuffer(buftag), PAGEMETA_SIZE);//PAGEMETA_SIZE
        for (int i = 0; i < ph_table2.pageNo; i++) {
            char *record_table2 = (char*)malloc(tmp_table2->attrLength);
            getNextRecord(table2_pageno, i, record_table2);
            char *val_table2 = (char*)malloc(strlen(record_table2));
            int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

            for (int x = 0; x < table1_pagenum; x++) {
                buftag = Buf_GenerateTag(table1_pageno);
                struct PageMeta ph_table1;
                memcpy(&ph_table1,  Buf_ReadBuffer(buftag), PAGEMETA_SIZE);//PAGEMETA_SIZE
                bool flag = false;
                for (int y = 0; y < ph_table1.pageNo; y++){
                    char *record_table1 = (char*)malloc(tmp_table1->attrLength);
                    getNextRecord(table1_pageno, y, record_table1);
                    char* val_table1 = (char*)malloc(strlen(record_table1));
                    getValueByAttrID(record_table1, table1_pub_attr, val_table1);
                    if (strcmp(val_table1, val_table2) == 0){
                        char *result = (char*)malloc(tmp->attrLength);
                        memset(result, 0, tmp->attrLength);
                        strcpy(result, record_table1);
                        strcat(result, "|");
                        strncat(result, record_table2, pd);
                        strcat(result, record_table2 + pd + strlen(val_table2) + 1);
                        insertOneRecord(tmp_table_dictID, result);
                    }
                    else if (strcmp(val_table1, val_table2) > 0){
                        flag = true;
                        break;
                    }
                    else {
                        continue;
                    }
                }
                if (flag)
                    break;
                if (ph_table1.nextPageNo < 0)
                    break;
                else
                    table1_pageno = ph_table1.nextPageNo;
            }

        }
        if (ph_table2.nextPageNo < 0)
            break;
        else
            table2_pageno = ph_table2.nextPageNo;

    }
    return tmp_table_dictID;
}


int nestedLoopJoinByThree(int table1_dictID, int table2_dictID, int table3_dictID){
    Table table1 = DB->dataDict[table1_dictID]; 
    Table table2 = DB->dataDict[table2_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (table1.attr[table1_pub_attr].name == table2.attr[table2_pub_attr].name){
                isFound = true;
                break;
            }
        }
        if (isFound)
            break;
    }
    if (isFound == false) {
        printf("两表没有公共属性/n");
        return -1;
    }
    int table1_fid = table1.fileID;
    long table1_pageNo = DB->dbMeta.fileMeta[table1_fid].firstPageNo;
    long table1_pageNum = DB->dbMeta.fileMeta[table1_fid].pageNum;

    int table2_fid = table2.fileID;
    long table2_pageNo = DB->dbMeta.fileMeta[table2_fid].firstPageNo;
    long table2_pageNum = DB->dbMeta.fileMeta[table2_fid].pageNum;

    int tmp_table_dictID = createTmpTable2(table1, table2, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);

    int *buffID = (int*)malloc(table2_pageNum);
    int m = BUFFER_NUM - 1;
    int outer = table2_pageNum / m + 1;
    struct BufTag buftag;
    for (int x = 0; x < outer; x++){
        for (int y = 0; y < m && y < table2_pageNum; y++) {
            struct PageMeta ph;
            buftag = Buf_GenerateTag(table2_pageNo);
            memcpy(&ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
            // DB->buff.map[mapNo].isPin = true; //PIN住
            // buf_pin(mapNo); // TODO: add this func==> ma-na-bo
            buffID[y] = table2_pageNo;
            if (ph.nextPageNo < 0)
                break;
            else
                table2_pageNo = ph.nextPageNo;
        }

        for (int z = 0; z < table1_pageNum; z++) {
            struct PageMeta table1_ph;
            buftag = Buf_GenerateTag(table1_pageNo);
            memcpy(&table1_ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
            for (int k = 0; k < table1_ph.pageNo; k++) {
                char *table1_record = (char*)malloc(table1.attrLength);
                getNextRecord(table1_pageNo, k, table1_record);
                char *table1_value = (char*)malloc(table1.attrLength);
                int pe = getValueByAttrID(table1_record, table1_pub_attr, table1_value);
                if (pe < 0)
                    exit(0);
                for (int y = 0; y < m && y < table2_pageNum; y++){
                    int table2_mapNo = buffID[y];
                    struct PageMeta table2_ph;
                    buftag = Buf_GenerateTag(table2_mapNo);
                    memcpy(&table2_ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
                    for (int i = 0; i < table2_ph.pageNo; i++) {
                        char *table2_record = (char*)malloc(table2.attrLength);
                        getNextRecord(table2_mapNo, i, table2_record);
                        char *table2_value = (char*)malloc(table2.attrLength);
                        int pd = getValueByAttrID(table2_record, table2_pub_attr, table2_value);
                        if (pd < 0) exit(0);
                        if (strcmp(table1_value, table2_value) == 0){
                            char *res = (char*)malloc(tmp->attrLength);
                            memset(res, 0, tmp->attrLength);
                            strcpy(res, table1_record);
                            strcat(res, "|");
                            strncat(res, table2_record, pd);
                            strcat(res, table2_record + pd + strlen(table2_value) + 1);
                            insertOneRecord(tmp_table_dictID, res);
                        }
                    }
                }

            }
            if (table1_ph.nextPageNo < 0)
                break;
            else
                table1_pageNo = table1_ph.nextPageNo;
        }
    }

    return tmp_table_dictID;
}