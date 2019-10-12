#include <assert.h>
#include <string>
using std::string;

#include "buffer.h"

// ==================== init func ====================
BufMgr::BufMgr(void)
{
    // 申请缓存区空间
    bufpool.data = (char *)MemAllocNoThrow(BUFFER_NUM * BLOCK_SIZE);
    bufpool.bufMeta = (BufMeta *)MemAllocNoThrow(BUFFER_NUM * sizeof(BufMeta));

    if (bufpool.data && bufpool.bufMeta)
    {
        // 没有申请到内存空间
        // TODO error log
        assert(bufpool.data && bufpool.bufMeta);
    }
    // 初始化缓冲区描述数据
    this->freeBlockHead = 0;
    int i;
    for (i = 0; i < BUFFER_NUM - 1; i++)
    {
        BufMeta *bp_p = &(this->bufpool.bufMeta[i]);
        bp_p->bufId = i;
        bp_p->visitTime = UTCNowTimestamp();
        bp_p->fNext = i + 1;
        bp_p->bufMode = BM_free;
    }
}

BufMgr::~BufMgr()
{
    MemFree(this->bufpool.bufMeta);
    MemFree(this->bufpool.data);
}

// ==================== public func ====================
char *BufMgr::buf_ReadBuffer(BufTag btag)
{
    // step1 - 检查btag的合法性
    // BufCheckTag(btag);

    long buf_id;
    // step2 - 查询tag和buf id的映射转换
    buf_id = this->buf_QuickLookup(btag);
    if (buf_id == -1)
    {
        // 该缓存页不存在，发出load动作
        buf_id = this->buf_LoadPage(btag);
    }

    assert(buf_id >= 0 && buf_id < BUFFER_NUM);

    return this->buf_GetBlock(buf_id);
}

// ==================== private func ====================
// 从外存中加载一个页进入缓存中
long BufMgr::buf_LoadPage(BufTag btag)
{
    // 检查Buftag的合法性
    // BufCheckTag(btag);

    // 查看是否有空闲的缓存块
    // 如果没有则调用淘汰算法

    // TODO 加锁控制

    if (this->freeBlockHead == BUF_FREE_LIST_EMPTY)
    {
        this->buf_Schedule();
    }

    // 此时一定有空闲块可以使用
    long newBufId = this->freeBlockHead;
    assert(newBufId >= 0 && newBufId < BUFFER_NUM);

    BufMeta *newbmeta = &(this->bufpool.bufMeta[newBufId]);
    this->freeBlockHead = newbmeta->fNext == BUF_FREE_END ? BUF_FREE_LIST_EMPTY : newbmeta->fNext;

    newbmeta->fNext = BUF_FREE_END;
    newbmeta->bufMode = BM_ioProgress;
    // TODO 释放锁

    // TODO: 调用OS接口，从disk调用目标page
    // char *page = load_disk_page(btag);
    // char *blockStart = this->buf_GetBlock(newBufId);
    // memcpy(blockStart, page, BLOCK_SIZE * sizeof(char) );
    newbmeta->bufMode = BM_isValid;
    return newBufId;
}

// 这一部分主要是buffer的调度策略，LRU、FIFO等调页淘汰算法的实现
// 暂时实现LRU算法
void BufMgr::buf_Schedule(void)
{
    // 调用淘汰算法
    long loserId = buf_StrategyLRU();
    this->Buf_Remove(loserId);
}

// LRU算法
long BufMgr::buf_StrategyLRU()
{
    long buf_id = -1;
    long minVisit = UTCNowTimestamp();
    int i;
    for (i = 0; i < BUFFER_NUM; i++)
    {
        BufMeta bmeta = bufpool.bufMeta[i];
        if (bmeta.bufMode != BM_isValid)
        {
            continue;
        }
        if (bmeta.visitTime < minVisit)
        {
            buf_id = i;
            minVisit = bmeta.visitTime;
        }
    }
    assert(buf_id >= 0 && buf_id < BUFFER_NUM);
    return buf_id;
}

// 删除指定id的缓存块，将该块返回free list中
bool BufMgr::Buf_Remove(long bufId)
{
    if (bufId < 0 || bufId > BUFFER_NUM)
    {
        // TODO: debug log
        return false;
    }

    BufMeta *bmeta = &(this->bufpool.bufMeta[bufId]);

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

    if (this->freeBlockHead == BUF_FREE_LIST_EMPTY)
    {
        // 如果当前空闲list为空
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

// 在所有状态为BM_isValid的缓存块中满足tag相等的缓存块
// 如果找到这样的缓存块则返回该块的下标
// 否则返回-1
// TODO : opt 目前实现顺序查找 太low了
long BufMgr::buf_QuickLookup(BufTag btag)
{
    int i;
    for (i = 0; i < BUFFER_NUM; i++)
    {
        BufMeta bmeta = bufpool.bufMeta[i];
        if (bmeta.bufMode != BM_isValid)
        {
            continue;
        }
        if (CMPBufTag(btag, bmeta.bTag) == true)
        {
            return i;
        }
    }
    return -1;
}

// 根据bufid返回缓存块的起始地址
char *BufMgr::buf_GetBlock(long bufId)
{
    assert(bufId >= 0 && bufId < BUFFER_NUM);
    char *p;
    p = this->bufpool.data + BLOCK_SIZE * bufId;
    return p;
}

// ==================== OS call ====================
// 这一部分主要和操作系统交互，包括读取块、内存的基本操作
// 如追加、清空、删除等，查看PG的bufmgr
// 以及一些内部的加锁（待定）

// ==================== utils ====================
long UTCNowTimestamp()
{
    return 0;
}

// BufTag 的比较函数,BufTag的比较结果就只有相等以及不相等
// 如果tag1等于tag2则返回true
// 否则false
bool CMPBufTag(const BufTag tag1, const BufTag tag2)
{
    return tag1.pageNo == tag2.pageNo;
}

// ==================== mem func ====================
void *MemAllocNoThrow(std::size_t alloc_size)
{
    void *newMem = NULL;
    newMem = std::malloc(alloc_size);
    return newMem;
}

void MemFree(void *start)
{
    if (start == NULL)
    {
        return;
    }
    free(start);
}