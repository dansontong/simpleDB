#include <iostream>
#include "buffer.h"
#include "database.h"
#include "file.h"

using namespace std;

int main()
{
    printf("======================= buffer_test begin ============\n");
    //初始化数据库
    DataBase DB;
    char dbFile[30] = "../data/simpleDb";
    database_initDB(&DB, dbFile);
    // file_Init(&DB);
    Buf_Init();
    cout << "Buffer init success." << endl;

    cout << "--- test generate buftag ---" << endl;
    BufTag btag = Buf_GenerateTag(101);
    
    Buf_PrintInfo();
    cout << "-------- test alloc --------" << endl;
    Buf_AllocBlock(btag);
    Buf_PrintInfo();

    cout << "-------- test write --------" << endl;
    char data[] = {"this is a test for buffer."};
    cout << "data for write: " << data << endl;
    Buf_WriteBuffer(btag, data);
    Buf_PrintInfo();

    cout << "-------- test read ---------" << endl;
    // test read
    char *buf_p = Buf_ReadBuffer(btag);
    if (buf_p != NULL)
        cout << "res: " << buf_p << endl;

    cout << "------- test load page -----" << endl;
    btag.pageNo = 102;
    // test read
    buf_p = Buf_ReadBuffer(btag);
    Buf_PrintInfo();

    if (buf_p != NULL)
        cout << "res: " << buf_p << endl;
    printf("======================= buffer_test end ==============\n");
    return 0;
}