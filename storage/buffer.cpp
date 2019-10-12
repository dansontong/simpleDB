#include <assert.h>
#include <string>

#include "buffer.h"
#include "log.h"
using std::string;

// ==================== buffer global variable ====================
char *bufBlocks;    /* buffer data */
BufMeta *bufMetas;  /* 每一个缓存块对应的参数信息 */
long freeBlockHead; /* 空闲块组成的链表的起始节点 */

// ==================== public func ====================
void Buf_Init(void)
{
    // temp
    log_init();

    char *newbufblocks = NULL;
    BufMeta *newbufmeta = NULL;
    // 申请缓存区空间
    newbufblocks = (char *)MemAllocNoThrow(BUFFER_NUM * BLOCK_SIZE);
    newbufmeta = (BufMeta *)MemAllocNoThrow(BUFFER_NUM * sizeof(BufMeta));

    if (newbufblocks == NULL || newbufmeta == NULL)
    {
        // 没有申请到内存空间
        log_Error("malloc memory for buffer error.");
        assert(newbufmeta && newbufblocks);
    }
    // 初始化缓冲区描述数据
    freeBlockHead = 0;
    bufBlocks = newbufblocks;
    bufMetas = newbufmeta;

    int i;
    for (i = 0; i < BUFFER_NUM - 1; i++)
    {
        BufMeta *bp_p = &(bufMetas[i]);
        bp_p->bTag.pageNo = -1;
        bp_p->bufId = i;
        bp_p->visitTime = UTCNowTimestamp();
        bp_p->fNext = i + 1;
        bp_p->bufMode = BM_free;
    }
    log_Info("buffer ready for work.");
}

void Buf_Free()
{
    MemFree(bufMetas);
    MemFree(bufBlocks);
}

char *Buf_ReadBuffer(BufTag btag)
{
    // TODO 多进程控制
    return Buf_ReadBuffer_inner(btag);
}

char *Buf_AllocBlock(BufTag tag)
{
    long id = Buf_AllocBlock_inner(tag);
    return Buf_GetBlock(id);
}

void Buf_WriteBuffer(BufTag tag, char *data)
{
    // TODO 多进程控制
    Buf_WriteBuffer_inner(tag, data);
}

BufTag Buf_GenerateTag(long pageNo)
{
    BufTag btag;
    btag.pageNo = pageNo;
    return btag;
}
// ==================== private func ====================
void Buf_WriteBuffer_inner(BufTag tag, char *data)
{
    if (data == NULL)
    {
        log_Error("writing to buffer can't be NULL");
        return;
    }
    long bufid = Buf_QuickLookup(tag);

    if (bufid == -1)
    { // buffer不存在
        // TODO 我先想一想，如果写缓存块时块不存在怎么办

        return;
    }

    BufMeta *bmeta = &(bufMetas[bufid]);
    bmeta->bufMode = BM_isDirty;

    char *buf = Buf_GetBlock(bufid);
    memcpy(buf, data, BLOCK_SIZE);
}

char *Buf_ReadBuffer_inner(BufTag btag)
{
    // step1 - 检查btag的合法性
    // BufCheckTag(btag);

    long buf_id;
    // step2 - 查询tag和buf id的映射转换
    buf_id = Buf_QuickLookup(btag);
    cout << "[debug] quick lookup res: " << buf_id << endl;
    if (buf_id == -1)
    {
        // 该缓存页不存在，发出load动作
        cout << "[debug] prepare to load page" << endl;
        buf_id = Buf_LoadPage(btag);
    }

    assert(buf_id >= 0 && buf_id < BUFFER_NUM);

    // TODO 如果当前的缓存块被标记为脏数据，如何处理

    Buf_HitBlockById(buf_id);
    return Buf_GetBlock(buf_id);
}

long Buf_AllocBlock_inner(BufTag btag)
{
    // 检查Buftag的合法性
    // BufCheckTag(btag);

    // 查看是否有空闲的缓存块
    // 如果没有则调用淘汰算法

    // TODO 加锁控制

    if (freeBlockHead == BUF_FREE_LIST_EMPTY)
    {
        Buf_Schedule();
    }
    long newBufId = freeBlockHead;

    // 此时一定有空闲块可以使用
    assert(newBufId >= 0 && newBufId < BUFFER_NUM);

    char logs[255];
    sprintf(logs, "alloc new buffer block success. the new block id: %ld", newBufId);
    log_Info(logs);

    BufMeta *newbmeta = &(bufMetas[newBufId]);
    // 维护空闲链表
    freeBlockHead = newbmeta->fNext == BUF_FREE_END ? BUF_FREE_LIST_EMPTY : newbmeta->fNext;
    newbmeta->bufMode = BM_isValid;
    newbmeta->fNext = BUF_FREE_END;

    // buf tag 的拷贝
    newbmeta->bTag.pageNo = btag.pageNo;
    // TODO 释放锁

    return newBufId;
}

// 从外存中加载一个页进入缓存中
long Buf_LoadPage(BufTag btag)
{
    long newBufId = Buf_AllocBlock_inner(btag);
    BufMeta *newbmeta = &(bufMetas[newBufId]);

    newbmeta->bufMode = BM_ioProgress;
    // TODO: 调用OS接口，从disk调用目标page
    // char *page = load_disk_page(btag);
    // memcpy(blockStart, page, BLOCK_SIZE * sizeof(char) );
    char *blockStart = Buf_GetBlock(newBufId);
    assert(blockStart != NULL);

    // just for test
    TestLoad(blockStart);
    newbmeta->bufMode = BM_isValid;
    return newBufId;
}

// 这一部分主要是buffer的调度策略，LRU、FIFO等调页淘汰算法的实现
// 暂时实现LRU算法
void Buf_Schedule(void)
{
    // 调用淘汰算法
    long loserId = Buf_StrategyLRU();
    Buf_Remove(loserId);
}

// LRU算法
long Buf_StrategyLRU()
{
    long buf_id = -1;
    long minVisit = UTCNowTimestamp();
    int i;
    for (i = 0; i < BUFFER_NUM; i++)
    {
        BufMeta bmeta = bufMetas[i];
        // 只有处于valid状态的缓存块才会被删除
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
bool Buf_Remove(long bufId)
{
    if (bufId < 0 || bufId > BUFFER_NUM)
    {
        log_Error("buffer id needed removed is ivalid.");
        return false;
    }

    BufMeta *bmeta = &(bufMetas[bufId]);

    // 检查缓存块能否删除
    // BufMode不为空、不在io_progress等状态
    if (bmeta->bufMode == BM_free || bmeta->bufMode == BM_ioProgress)
    {
        log_Error("buffer needed remove mode is free or in io process.");
        return false;
    }

    // 当前块符合释放条件，将该块放回freelist里
    bmeta->bufMode = BM_free;
    bmeta->visitTime = 0;

    if (freeBlockHead == BUF_FREE_LIST_EMPTY)
    {
        // 如果当前空闲list为空
        freeBlockHead = bufId;
        bmeta->fNext == BUF_FREE_END;
    }
    else
    {
        bmeta->fNext = freeBlockHead;
        freeBlockHead = bufId;
    }

    char logs[255];
    sprintf(logs, "%ld buffer already removed.", bufId);
    log_Info(logs);

    return true;
}

// 在所有状态为BM_isValid的缓存块中满足tag相等的缓存块
// 如果找到这样的缓存块则返回该块的下标
// 否则返回-1
// TODO : opt 目前实现顺序查找 太low了
long Buf_QuickLookup(BufTag btag)
{
    int i;
    for (i = 0; i < BUFFER_NUM; i++)
    {
        BufMeta bmeta = bufMetas[i];
        // if (bmeta.bufMode != BM_isValid)
        // {
        //     continue;
        // }
        if (CMPBufTag(btag, bmeta.bTag) == true)
        {
            return i;
        }
    }
    return -1;
}

// 根据bufid返回缓存块的起始地址
char *Buf_GetBlock(long bufId)
{
    assert(bufId >= 0 && bufId < BUFFER_NUM);
    char *p;
    p = bufBlocks + BLOCK_SIZE * bufId;
    return p;
}

void Buf_HitBlockById(long bufid)
{
    BufMeta *bmeta;

    bmeta = &(bufMetas[bufid]);
    bmeta->visitTime = UTCNowTimestamp();
}

void Buf_HitBlockByTag(BufTag btag)
{
    long bufid = Buf_QuickLookup(btag);
    Buf_HitBlockById(bufid);
}

void Buf_PrintInfo()
{
    char logs[1024];
    int used = 0;
    int i;
    for (i = 0; i < BUFFER_NUM; i++)
    {
        if (bufMetas[i].bufMode != BM_free)
        {
            used++;
        }
    }
    sprintf(logs, "blocks: %p, metas: %p, used: %d, freehead: %ld", bufBlocks, bufMetas, used, freeBlockHead);

    log_Info(logs);
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

void TestLoad(char *dest)
{
    char data[] = {"this is a test for load page."};
    memcpy(dest, data, BLOCK_SIZE);
}
// ==================== mem func ====================
void *MemAllocNoThrow(std::size_t alloc_size)
{
    void *newMem = NULL;
    newMem = malloc(alloc_size);
    return newMem;
}

void MemFree(void *addr)
{
    if (addr == NULL)
    {
        return;
    }
    free(addr);
}