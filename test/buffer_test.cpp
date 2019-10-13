#include <iostream>
#include "buffer.h"

using namespace std;

int main()
{
    Buf_Init();
    cout << "Buffer init success." << endl;

    cout << "======= test generate buftag =======" << endl;
    BufTag btag = Buf_GenerateTag(101);
    
    Buf_PrintInfo();
    cout << "======= test alloc =======" << endl;
    Buf_AllocBlock(btag);
    Buf_PrintInfo();

    cout << "======= test write =======" << endl;
    char data[] = {"this is a test for buffer."};
    cout << "data for write: " << data << endl;
    Buf_WriteBuffer(btag, data);
    Buf_PrintInfo();

    cout << "======= test read =======" << endl;
    // test read
    char *buf_p = Buf_ReadBuffer(btag);
    if (buf_p != NULL)
        cout << "res: " << buf_p << endl;

    cout << "======= test load page =======" << endl;
    btag.pageNo = 102;
    // test read
    buf_p = Buf_ReadBuffer(btag);
    Buf_PrintInfo();

    if (buf_p != NULL)
        cout << "res: " << buf_p << endl;
    return 0;
}