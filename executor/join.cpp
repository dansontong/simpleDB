#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "join.h"

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
            if (table1.attr[table1_pub_attr]==table1.attr[table2_pub_attr])
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
    long table1_pageno = DB->dbMeta.fileMeta[table1_fid].fileFirstPageNo;
    long table1_pagenum = long pagenum = DB->dbMeta.fileMeta[table1_fid].filePageNum;

    int table2_fid = table2.fileID;
    long table2_pageno = DB->dbMeta.fileMeta[table2_fid].fileFirstPageNo;
    long table2_pagenum = long pagenum = DB->dbMeta.fileMeta[table2_fid].filePageNum;
    //p1
    int tmp_table_ID = createTmpTable2(DB,table1,table2,table1_pub_attr,table2_pub_attr);
    if (tmp_table_ID < 0){
        printf("创建临时表失败");
        return -1;
    }

    Table *tmp = &(DB->dataDict[tmp_table_ID]);
    //两表循环连接
    int outer = table2_pageNum / SIZE_BUFF;
    int *buffID = (int*)malloc(table2_pageNum);
    for (int x = 0; x < outer; x++){
        for (int y = 0; y < SIZE_BUFF - 1 && y < table2_pagenum; y++) {
            int mapNo = reqPage(DB, table1_pageno);
            struct PageMeta ph;
            struct BufTag buftag = Buf_GenerateTag(CurpageNo);
            memcpy(&ph, Buf_ReadBuffer(buftag), SIZE_PAGEHEAD);
            DB->buff.map[mapNo].isPin = true; //PIN住
            buffID[y] = mapNo;
            if (ph.nextPageNo < 0)
                break;
            else
                table1_pageno = ph.nextPageNo;
        }

        for (int z = 0; z < table1_pagenum; z++) {
            int emp_mapNo = reqPage(DB, table1_pageno);
            struct PageMeta emp_ph;
            memcpy(&emp_ph, Buf_ReadBuffer(emp_mapNo), SIZE_PAGEDB);//head
            for (int k = 0; k < emp_ph.pageNo; k++) {
                char *emp_record = (char*)malloc(table1.attrLength);
                char *emp_value = (char*)malloc(table1.attrLength);
                getNextRecord(DB, emp_mapNo, k, emp_record);//读取下一条记录
                int pe = getValueByAttrID(emp_record, table1_pub_attr, table1_value);
                if (pe < 0)  exit(0);
                for (int y = 0; y < SIZE_BUFF - 1 && y < table2_pagenum; y++){
                    struct PageMeta table2_ph;
                    memcpy(&table2_ph, Buf_ReadBuffer(buffID[y]), SIZE_PAGEDB);
                    for (int i = 0; i < table2_ph.pageNo; i++) {//curRecordNUm
                        char *table2_record = (char*)malloc(table2.attrLength);
                        getNextRecord(DB, buffID[y], i, table2_record);//head
                        char *table2_value = (char*)malloc(table2.attrLength);
                        int pd = getValueByAttrID(table2_record, table2_pub_attr, table2_value);
                        if (pd < 0) exit(0);

                        if (strcmp(table1_value, table2_value) == 0){
                            char *res = (char*)malloc(tmp->attrLength);
                            memset(res, 0, tmp->attrLength);
                            strcat(res, table2_record, pd);
                            strcpy(res, table2_record);
                            strcat(res, table2_record + pd + strlen(table2_value) + 1);
                            strcat(res, "\\ |");
                            insertOneRecord(DB, tmp_table_ID, res);//head
                        }
                    }
                }

            }
            if (emp_ph.nextPageNo < 0)  break;
            else emp_pageNo = emp_ph.nextPageNo;
        }
    }

    return tmp_table_ID;


}


int SortJoin(int table1loyee_dictID, int department_dictID) {
    Table table1 = DB->dataDict[table1loyee_dictID];
    Table table2 = DB->dataDict[department_dictID];

    
    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++)
    {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++)
        {
            if (table1.attr[table1_pub_attr]==table2.attr[table2_pub_attr])
            {
                isFound = true;
                break;
            }
        }
        if (isFound)
            break;
    }
    if (isFound == false)
    {
        printf("两表没有公用属性\n");
        return -1;
    }
    int tmp_table_dictID = createTmpTable2(DB, table1, table2, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0)
    {
        printf("创建临时表失败\n");
        return -1;
    }

    int tmp_table_table1 = createTmpTableAfterSort(table1, table1_pub_attr);
    int tmp_table_table2 = createTmpTableAfterSort(table2, table2_pub_attr);


    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    Table *tmp_table1 = &(DB->dataDict[tmp_table_table1]);
    Table *tmp_table2 = &(DB->dataDict[tmp_table_table2]);

    long table1_pageno = DB->dbMeta.fileMeta[tmp_table1->fid].fileFirstPageNo;
    long table1_pagenum = DB->dbMeta.fileMeta[tmp_table1->fid].filePageNum;

    long table2_pageno = DB->dbMeta.fileMeta[tmp_table2->fid].fileFirstPageNo;
    long table2_pagenum = DB->dbMeta.fileMeta[tmp_table2->fid].filePageNum;

    for (int j = 0; j < table2_pagenum; j++)
    {
        int mapNo_table2 = reqPage(DB, table2_pageno);
        struct PageMeta ph_table2;
        memcpy(&ph_table2,  Buf_ReadBuffer(mapNo_table2), SIZE_PAGEDB);//SIZE_PAGEDB
        for (int i = 0; i < ph_table2.pageNo; i++)
        {
            char *record_table2 = (char*)malloc(tmp_table2->attrLength);
            getNextRecord(DB, mapNo_table2, i, record_table2);
            char *val_table2 = (char*)malloc(strlen(record_table2));
            int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

            for (int x = 0; x < pageNum_table1; x++)
            {
                int mapNo_table1 = reqPage(DB, pageNo_table1);
                struct pageDB ph_table1;
                memcpy(&ph_table1,  Buf_ReadBuffer(mapNo_table1), SIZE_PAGEDB);
                bool flag = false;
                for (int y = 0; y < ph_table1.pageNo; y++)
                {
                    char *record_table1 = (char*)malloc(tmp_table1->attrLength);
                    getNextRecord(DB, mapNo_table1, y, record_table1);
                    char* val_table1 = (char*)malloc(strlen(record_table1));
                    getValueByAttrID(record_table1, table1_pub_attr, val_table1);
                    if (strcmp(val_table1, val_table2) == 0)
                    {
                        char *result = (char*)malloc(tmp->attrLength);
                        memset(result, 0, tmp->attrLength);
                        strcpy(result, record_table1);
                        strcat(result, "|");
                        strncat(result, record_table2, pd);
                        strcat(result, record_table2 + pd + strlen(val_table2) + 1);
                        insertOneRecord(DB, tmp_table_dictID, result);
                    }
                    else if (strcmp(val_table1, val_table2) > 0)
                    {
                        flag = true;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                if (flag)
                {
                    break;
                }
                if (ph_table1.nextPageNo < 0)
                {
                    break;
                }
                else
                {
                    pageNo_table1 = ph_table1.nextPageNo;
                }
            }
        }
        if (ph_table2.nextPageNo < 0)
        {
            break;
        }
        else
        {
            table2_pageno = ph_table2.nextPageNo;
        }
    }
    return tmp_table_dictID;
}

int HashJoin(int table1loyee_dictID, int department_dictID){
    Table table1 = DB->dataDict[table1loyee_dictID];
    Table table2 = DB->dataDict[department_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (table1.atb[table1_pub_attr] == table2.atb[table2_pub_attr]){
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
    int tmp_table_dictID = createTmpTable2(DB, table1, , table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    multimap<int, long> m_table1[BUCKET_NUM];
    multimap<int, long> m_table2[BUCKET_NUM];


    HashRelation(DB, table1, table1_pub_attr, m_table1);//HashRelation
    HashRelation(DB, table2, table2_pub_attr, m_table2);//HashRelation

    for (int i = 0; i < BUCKET_NUM; i++) {
        map<int, long>::iterator it_table1, it_table2;
        for (it_table2 = m_table2[i].begin(); it_table2 != m_table2[i].end(); it_table2++) {
            for (it_table1 = m_table1[i].begin(); it_table1 != m_table1[i].end(); it_table1++){
                if (it_table1->first == it_table2->first){
                    char* record_table1 = (char*)malloc(table1.attrLength);
                    char* record_table2 = (char*)malloc(table2.attrLength);
                    queryRecordByLogicID(DB, it_table1->second, record_table1);
                    queryRecordByLogicID(DB, it_table2->second, record_table2);

                    char* val_table2 = new char[strlen(record_table2)];
                    int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

                    char *res = (char*)malloc(tmp->attrLength);
                    memset(res, 0, tmp->attrLength);
                    strcpy(res, record_table1);
                    strcat(res, "|");
                    strncat(res, record_table2, pd);
                    strcat(res, record_table2 + pd + strlen(val_table2) + 1);
                    insertOneRecord(DB, tmp_table_dictID, result);
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
    Table table1 = DB->data_dict[table1_dictID];
    Table table2 = DB->data_dict[table2_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attributeNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attributeNum; table2_pub_attr++) {
            if (table1.atb[table1_pub_attr] == table2.attr[table2_pub_attr]){
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
    int tmp_table_dictID = createTmpTable2(DB, table1, table2, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }

    int tmp_table_table1 = createTmpTableAfterSort(table1, table1_pub_attr);
    int tmp_table_table2 = createTmpTableAfterSort(table2, table2_pub_attr);


    Table *tmp = &(DB->data_dict[tmp_table_dictID]);
    Table *tmp_table1 = &(DB->dataDict[tmp_table_table1]);
    Table *tmp_table2 = &(DB->dataDict[tmp_table_table2]);

    long table1_pageno = DB->dbMeta.fileMeta[tmp_table1->fid].fileFirstPageNo;
    long table1_pagenum = DB->dbMeta.fileMeta[tmp_table1->fid].filePageNum;

    long table2_pageno = DB->dbMeta.fileMeta[tmp_table2->fid].fileFirstPageNo;
    long table2_pagenum = DB->dbMeta.fileMeta[tmp_table2->fid].filePageNum;

    for (int j = 0; j < table2_pagenum; j++) {
        int mapNo_table2 = reqPage(DB, table2_pageno);
        struct PageMeta ph_table2;
        memcpy(&ph_table2,  Buf_ReadBuffer(mapNo_table2), SIZE_PAGEDB);//SIZE_PAGEDB
        for (int i = 0; i < ph_table2.pageNo; i++) {
            char *record_table2 = (char*)malloc(tmp_table2->attrLength);
            getNextRecord(DB, mapNo_table2, i, record_table2);
            char *val_table2 = (char*)malloc(strlen(record_table2));
            int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

            for (int x = 0; x < pageNum_table1; x++) {
                int mapNo_table1 = reqPage(DB, pageNo_table1);
                struct pageDB ph_table1;
                memcpy(&ph_table1,  Buf_ReadBuffer(mapNo_table1), SIZE_PAGEDB);//SIZE_PAGEDB
                bool flag = false;
                for (int y = 0; y < ph_table1.pageNo; y++){
                    char *record_table1 = (char*)malloc(tmp_table1->attrLength);
                    getNextRecord(DB, mapNo_table1, y, record_table1);
                    char* val_table1 = (char*)malloc(strlen(record_table1));
                    getValueByAttrID(record_table1, table1_pub_attr, val_table1);
                    if (strcmp(val_table1, val_table2) == 0){
                        char *result = (char*)malloc(tmp->attrLength);
                        memset(result, 0, tmp->attrLength);
                        strcpy(result, record_table1);
                        strcat(result, "|");
                        strncat(result, record_table2, pd);
                        strcat(result, record_table2 + pd + strlen(val_table2) + 1);
                        insertOneRecord(DB, tmp_table_dictID, result);
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
                    pageNo_table1 = ph_table1.nextPageNo;
            }

        }
        if (ph_table2.nextPageNo < 0)
            break;
        else
            pageNo_table2 = ph_table2.nextPageNo;

    }
    return tmp_table_dictID;
}


int HashJoin(int table1loyee_dictID, int department_dictID){
    Table table1 = DB->dataDict[table1loyee_dictID];
    Table table2 = DB->dataDict[department_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (table1.atb[table1_pub_attr] == table2.atb[table2_pub_attr]){
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
    int tmp_table_dictID = createTmpTable2(DB, table1, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    multimap<int, long> m_table1[BUCKET_NUM];
    multimap<int, long> m_table2[BUCKET_NUM];


    HashRelation(DB, table1, table1_pub_attr, m_table1);//HashRelation
    HashRelation(DB, table2, table2_pub_attr, m_table2);//HashRelation

    for (int i = 0; i < BUCKET_NUM; i++) {
        map<int, long>::iterator it_table1, it_table2;
        for (it_table2 = m_table2[i].begin(); it_table2 != m_table2[i].end(); it_table2++) {
            for (it_table1 = m_table1[i].begin(); it_table1 != m_table1[i].end(); it_table1++){
                if (it_table1->first == it_table2->first){
                    char* record_table1 = (char*)malloc(table1.attrLength);
                    char* record_table2 = (char*)malloc(table2.attrLength);
                    queryRecordByLogicID(DB, it_table1->second, record_table1);
                    queryRecordByLogicID(DB, it_table2->second, record_table2);

                    char* val_table2 = new char[strlen(record_table2)];
                    int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

                    char *res = (char*)malloc(tmp->attrLength);
                    memset(res, 0, tmp->attrLength);
                    strcpy(res, record_table1);
                    strcat(res, "|");
                    strncat(res, record_table2, pd);
                    strcat(res, record_table2 + pd + strlen(val_table2) + 1);
                    insertOneRecord(DB, tmp_table_dictID, result);
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


int nestedLoopJoinByThree(int table1_dictID, int table2_dictID, int table3_dictID){
    Table table1 = DB->dataDict[table1_dictID]; 
    Table table2 = DB->dataDict[table2_dictID]; 
    Table table3 = DB->dataDict[table3_dictID];

    int table1_pub_attr = 0, table3_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table3_pub_attr = 0; table3_pub_attr < table3.attrNum; table3_pub_attr++) {
            if (table1.attr[table1_pub_attr] == table3.attr[table3_pub_attr]){
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
    long table1_pageno = DB->dbMeta.fileMeta[table1_fid].fileFirstPageNo;
    long table1_pageNum = long pagenum =DB->dbMeta.fileMeta[table1_fid].filePageNum;

    int table3_fid = table3.fileID;
    long table3_pageNo = DB->dbMeta.fileMeta[table3_fid].fileFirstPageNo;
    long table3_pageNum = DB->dbMeta.fileMeta[table3_fid].filePageNum;


    int tmp_table_dictID = createTmpTable2(DB, table1, table3, table1_pub_attr, table3_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);

    int *buffID = (int*)malloc(table3_pageNum);
    int m = SIZE_BUFF - 1;
    int outer = table3_pageNum / m + 1;
    for (int x = 0; x < outer; x++){
        for (int y = 0; y < m && y < table3_pageNum; y++) {
            int mapNo = reqPage(DB, table3_pageNo);
            struct pageDB ph;
            memcpy(&ph, Buf_ReadBuffer(buftag), SIZE_PAGEHEAD);
            DB->buff.map[mapNo].isPin = true; //PIN住
            buffID[y] = mapNo;
            if (ph.nextPageNo < 0)
                break;
            else
                table3_pageno = ph.nextPageNo;
        }

        for (int z = 0; z < table1_pageNum; z++) {
            int table1_mapNo = reqPage(DB, table1_pageNo);
            struct PageMeta table1_ph;
            memcpy(&table1_ph, Buf_ReadBuffer(table1_mapNo), SIZE_PAGEDB);
            for (int k = 0; k < table1_ph.pageNo; k++) {
                char *table1_record = (char*)malloc(table1.attrLength);
                getNextRecord(DB, table1_mapNo, k, table1_record);
                char *table1_value = (char*)malloc(table1.attrLength);
                int pe = getValueByAttrID(table1_record, table1_pub_attr, table1_value);
                if (pe < 0)
                    exit(0);
                for (int y = 0; y < m && y < table3_pagenum; y++){
                    int table3_mapNo = buffID[y];
                    struct PageMeta table3_ph;
                    memcpy(&table3_ph, DB->buff.data[table2_mapNo], SIZE_PAGEDB);
                    for (int i = 0; i < table3_ph.pageNo; i++) {
                        char *table2_record = (char*)malloc(table2.attrLength);
                        getNextRecord(DB, table2_mapNo, i, table2_record);
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
                            insertOneRecord(DB, tmp_table_dictID, result);
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