#include "buffer.h"
#include <string>
using std::string;

// ==================== extern api ====================
BufMgr::BufMgr(void)
{
    this->freeBlockHead = 0;
    // 初始化缓冲区描述数据
    int i;
    for (i = 0; i < BUFFER_NUM - 1; i++)
    {
        BufPool *bp_p = &(this->bufpool.bufMeta[i]);
        bp_p->bufTd = i;
        bp_p->pageNo = -1;
        bp_p->visitTime = UTC();
        bp_p->fNext = i + 1;
        bp_p->bufMode = BM_free;
    }
}

long BufMgr::BufQuickLookup(BufTag btag)
{
    return -1;
}

char *BufMgr::BufGetBlock(long bufId)
{
    return this->bufpool.data[bufId];
}

char *BufMgr::ReadBuffer(BUfTag btag)
{
    // step1 - 检查btag的合法性
    // BufCheckTag(btag);

    long resId;
    // step2 - 查询tag和buf id的映射转换
    resId = this->BufQuickLookup(btag);
    if (resId == -1)
    {
        // 该缓存页不存在，发出load动作
        resId = this->BufLoadPage(btag);
    }

    assert(resId >= 0 && resId < BUFFER_NUM);

    return this->BufGetBlock(resId);
}

// ==================== internal func ====================
// 这一部分主要是buffer的调度策略，LRU、FIFO等调页淘汰算法的实现
void BufMgr::BufSchedule(void)
{
    // 调用淘汰算法
    // loserId = strategy();
    // this->BufRemove(loserId);
}

bool BufMgr::BufRemove(long bufId)
{
    if (bufId < 0 || bufId > BUFFER_NUM)
    {
        // TODO: debug log
        return false;
    }

    BUfMeta *bmeta = &(this->bufpool.bufMeta[index]);

    // 检查缓存块能否删除
    // BufMode不为空、不在io_progress等状态
    if (bmeta->bufMode == BM_free || bmeta->bufMode == BM_ioProgress)
    {
        // TODO: debug log
        return false;
    }

    // 当前块符合释放条件，将该块放回freelist里
    bmeta->bufMode = BM_free;
    bmeta->visitTime = 0;

    if (this->freeBlockHead == BUF_FREE_END)
    {
        this->freeBlockHead = bufId;
        bmeta->fNext == BUF_FREE_END;
    }
    else
    {
        bmeta->fNext = this->freeBlockHead;
        this->freeBlockHead = bufId;
    }

    // TODO : debug log
    return true;
}

long BufMgr::BufLoadPage(BufTag btag)
{
    // 检查Buftag的合法性
    // BufCheckTag(btag);

    // 查看是否有空闲的缓存块
    // 如果没有则调用淘汰算法

    // TODO 加锁控制

    if (this->freeBlockHead == BUF_FREE_END)
    {
        this->BufShedule();
    }

    // 此时一定有空闲块可以使用
    long newBufId = this->freeBlockHead;
    assert(resId >= 0 && resId < BUFFER_NUM);

    BufMeta *newbmeta = &(this->bufpool.bufMeta[newBufId]);
    this->freeBlockHead = newbmeta->fNext;

    newbmeta->fNext = BUF_FREE_END;
    newbmeta->bufMode = BM_ioProgress;
    // TODO 释放锁

    // TODO: 调用OS接口，从disk调用目标page
    // load_disk_page(btag);
    newbmeta->bufMode = BM_isValid;
    return newBufId;
}

// ==================== OS call ====================
// 这一部分主要和操作系统交互，包括读取块、内存的基本操作
// 如追加、清空、删除等，查看PG的bufmgr
// 以及一些内部的加锁（待定）

BufMgr::BufMgr(){}

BufMgr::~BufMgr(){}