#include "operator.h"


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
            if (strcmp(table1.attr[table1_pub_attr].name, table2.attr[table2_pub_attr].name) == 0)
            {
                isFound = true;
                break;
            }
        }
        if (isFound) break;//利用标志位跳出第二重循环
    }
    //标志位如果为空的话代表两个表没有公共属性
    if (isFound == false) {
        printf("两表没有公共属性\n");
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
        printf("创建临时表失败\n");
        return -1;
    }

    Table *tmp = &(DB->dataDict[tmp_table_ID]);
    //两表循环连接
    int outer = table2_pagenum / (BUFFER_NUM-1) + 1;
    struct BufTag buftag;
    int *buffID = (int*)malloc(table2_pagenum);

    // char *table1_record = (char*)malloc(table1.recordLength);
    // char *table1_value = (char*)malloc(table1.recordLength);
    // char *table2_record = (char*)malloc(table2.recordLength);
    // char *table2_value = (char*)malloc(table2.recordLength);
    char *table1_record = (char*)malloc(RECORD_MAX_SIZE);
    char *table1_value = (char*)malloc(RECORD_MAX_SIZE);
    char *table2_record = (char*)malloc(RECORD_MAX_SIZE);
    char *table2_value = (char*)malloc(RECORD_MAX_SIZE);
    // char *res = (char*)malloc(tmp->recordLength);
    char *res = (char*)malloc(RECORD_MAX_SIZE*2);

    int count = 0;
    for (int x = 0; x < outer; x++){
        for (int y = 0; y < BUFFER_NUM - 1 && y < table2_pagenum; y++) {
            struct PageMeta ph;
            buftag = Buf_GenerateTag(table2_pageno);
            memcpy(&ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
            // DB->buff.map[mapNo].isPin = true; //PIN住
            buffID[y] = table2_pageno;
            if (ph.nextPageNo < 0)
                break;
            else
                table2_pageno = ph.nextPageNo;
        }

        for (int z = 0; z < table1_pagenum; z++)
        {
            struct PageMeta table1_pm;
            buftag = Buf_GenerateTag(table1_pageno);
            memcpy(&table1_pm, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);//head
            for (int k = 0; k < table1_pm.recordNum; k++)
            {
                getRecord(table1_pageno, k, table1_record);//读取下一条记录
                int pe = getValueByAttrID(table1_record, table1_pub_attr, table1_value);
                if (pe < 0)
                {
                    exit(0);
                }
                for (int y = 0; y < BUFFER_NUM - 1 && y < table2_pagenum; y++)
                {
                    struct PageMeta table2_pm;
                    buftag = Buf_GenerateTag(buffID[y]);
                    memcpy(&table2_pm, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
                    for (int i = 0; i < table2_pm.recordNum; i++) {//curRecordNUm
                        getRecord(buffID[y], i, table2_record);//head
                        int pd = getValueByAttrID(table2_record, table2_pub_attr, table2_value);
                        if (pd < 0) exit(0);

                        if (strcmp(table1_value, table2_value) == 0){
                            memset(res, 0, RECORD_MAX_SIZE*2);
                            strcpy(res, table1_record);
                            strcat(res, "|");
                            strncat(res, table2_record, pd);
                            // printf("=========== table2_value: %s, table2_record: %s, pd: %d.\n", table2_value, table2_record, pd);
                            strcat(res, table2_record + pd + strlen(table2_value) + 1);
                            insertRecord(tmp_table_ID, res);//head
                            printf("===== Nest-Join-tmpTable on attr:%s, value:%s, tmpRecord:%.60s.\n", table1.attr[table1_pub_attr].name,table2_value, res);

                            if(++count>15) break;// temp: if-not, there're  too many records
                        }
                    }
                    
                }
                break;// temp: if-not, there're  too many records
            }
            if(z > 1) break;// temp: if-not, there're  too many records
            
            if (table1_pm.nextPageNo < 0)  break;
            else table1_pageno = table1_pm.nextPageNo;
        }
        
    }

    return tmp_table_ID;
}


int HashJoin(int table1_dictID, int department_dictID){
    Table table1 = DB->dataDict[table1_dictID];
    Table table2 = DB->dataDict[department_dictID];

    int table1_pub_attr = 0, table2_pub_attr = 0;
    bool isFound = false;
    for (table1_pub_attr = 0; table1_pub_attr < table1.attrNum; table1_pub_attr++) {
        for (table2_pub_attr = 0; table2_pub_attr < table2.attrNum; table2_pub_attr++) {
            if (strcmp(table1.attr[table1_pub_attr].name, table2.attr[table2_pub_attr].name) == 0){
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
    int tmp_table_dictID = createTmpTable2(table1, table2, table1_pub_attr, table2_pub_attr);
    if (tmp_table_dictID < 0){
        printf("创建临时表失败\n");
        return -1;
    }
    Table *tmp = &(DB->dataDict[tmp_table_dictID]);
    multimap<int, long> m_table1[BUCKET_NUM];
    multimap<int, long> m_table2[BUCKET_NUM];


    HashRelation(table1, table1_pub_attr, m_table1);//HashRelation
    HashRelation(table2, table2_pub_attr, m_table2);//HashRelation

    char* record_table1 = (char*)malloc(table1.recordLength);
    char* record_table2 = (char*)malloc(table2.recordLength);
    char *res = (char*)malloc(tmp->recordLength);
    printf("table1.recordLength: %d\n", table1.recordLength);
    printf("table2.recordLength: %d\n", table1.recordLength);
    printf("tmp.recordLength: %d\n", tmp->recordLength);

    int count = 0;
    for (int i = 0; i < BUCKET_NUM; i++)
    {
        map<int, long>::iterator it_table1, it_table2;
        for (it_table2 = m_table2[i].begin(); it_table2 != m_table2[i].end(); it_table2++)
        {
            for (it_table1 = m_table1[i].begin(); it_table1 != m_table1[i].end(); it_table1++)
            {
                if (it_table1->first == it_table2->first)
                {
                    getRecordByLogicID(table1.fileID, it_table1->second, record_table1);
                    getRecordByLogicID(table2.fileID, it_table2->second, record_table2);

                    char* val_table2 = new char[strlen(record_table2)];
                    int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

                    memset(res, 0, tmp->recordLength);
                    strcpy(res, record_table1);
                    strcat(res, "|");
                    strncat(res, record_table2, pd);
                    strcat(res, record_table2 + pd + strlen(val_table2) + 1);
                    insertRecord(tmp_table_dictID, res);
                    printf("======== hash Join on Attr:%s,=== tmp_record: %.60s.\n", table2.attr[table2_pub_attr].name, res);
                    delete(val_table2);

                    if(++count>15) break;// temp: if-not, there're  too many records
                }
                else if (it_table1->first > it_table2->first)
                {
                    break;
                }
                else
                {
                    continue;
                }
                
            }
        }
        break;// temp: if-not, there're  too many records
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
        for (int i = 0; i < ph_table2.recordNum; i++) {
            char *record_table2 = (char*)malloc(tmp_table2->recordLength);
            getRecord(table2_pageno, i, record_table2);
            char *val_table2 = (char*)malloc(strlen(record_table2));
            int pd = getValueByAttrID(record_table2, table2_pub_attr, val_table2);

            for (int x = 0; x < table1_pagenum; x++) {
                buftag = Buf_GenerateTag(table1_pageno);
                struct PageMeta ph_table1;
                memcpy(&ph_table1,  Buf_ReadBuffer(buftag), PAGEMETA_SIZE);//PAGEMETA_SIZE
                bool flag = false;
                for (int y = 0; y < ph_table1.recordNum; y++){
                    char *record_table1 = (char*)malloc(tmp_table1->recordLength);
                    getRecord(table1_pageno, y, record_table1);
                    char* val_table1 = (char*)malloc(strlen(record_table1));
                    getValueByAttrID(record_table1, table1_pub_attr, val_table1);
                    if (strcmp(val_table1, val_table2) == 0){
                        char *result = (char*)malloc(tmp->recordLength);
                        memset(result, 0, tmp->recordLength);
                        strcpy(result, record_table1);
                        strcat(result, "|");
                        strncat(result, record_table2, pd);
                        strcat(result, record_table2 + pd + strlen(val_table2) + 1);
                        insertRecord(tmp_table_dictID, result);
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
            for (int k = 0; k < table1_ph.recordNum; k++) {
                char *table1_record = (char*)malloc(table1.recordLength);
                getRecord(table1_pageNo, k, table1_record);
                char *table1_value = (char*)malloc(table1.recordLength);
                int pe = getValueByAttrID(table1_record, table1_pub_attr, table1_value);
                if (pe < 0)
                    exit(0);
                for (int y = 0; y < m && y < table2_pageNum; y++){
                    int table2_pageNo = buffID[y];
                    struct PageMeta table2_ph;
                    buftag = Buf_GenerateTag(table2_pageNo);
                    memcpy(&table2_ph, Buf_ReadBuffer(buftag), PAGEMETA_SIZE);
                    for (int i = 0; i < table2_ph.recordNum; i++) {
                        char *table2_record = (char*)malloc(table2.recordLength);
                        getRecord(table2_pageNo, i, table2_record);
                        char *table2_value = (char*)malloc(table2.recordLength);
                        int pd = getValueByAttrID(table2_record, table2_pub_attr, table2_value);
                        if (pd < 0) exit(0);
                        if (strcmp(table1_value, table2_value) == 0){
                            char *res = (char*)malloc(tmp->recordLength);
                            memset(res, 0, tmp->recordLength);
                            strcpy(res, table1_record);
                            strcat(res, "|");
                            strncat(res, table2_record, pd);
                            strcat(res, table2_record + pd + strlen(table2_value) + 1);
                            insertRecord(tmp_table_dictID, res);
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

void HashRelation( Table tbl, int attrID, multimap<int, long> *mmap) {
    int fileID = tbl.fileID;
    long pageNo = DB->dbMeta.fileMeta[fileID].firstPageNo;
    long pageNum = DB->dbMeta.fileMeta[fileID].pageNum;
    struct PageMeta pagehead;
    for (int i = 0; i < pageNum; i++) {
        struct BufTag buftag = Buf_GenerateTag(pageNo);
        memcpy(&pagehead,Buf_ReadBuffer(buftag),PAGEMETA_SIZE);
        for (int j = 0; j < pagehead.recordNum; j++) {
            char *record = (char*)malloc(tbl.recordLength);
            long logicID = getLogicID(fileID, pageNo, j);
            getRecord(pageNo, j, record);
            char *val = (char*)malloc(tbl.recordLength);
            getValueByAttrID(record, attrID, val);
            //暂时只考虑要进行hash的属性为int类型的情况
            int int_val = atoi(val);
            int bid = hashToBucket(int_val);
            mmap[bid].insert(pair<int, long>(int_val, logicID));
        }
        if (pagehead.nextPageNo < 0)
            break;
        else
            pageNo = pagehead.nextPageNo;
    }
}

int hashToBucket(int value)
{
    return value % BUCKET_NUM;
}