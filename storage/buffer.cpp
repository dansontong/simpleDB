#include <./buffer.h>
#include <string>
using std::string;

// ==================== data structure ====================
typedef struct
{
    /* data */
    int *p;
} BufBlock;

typedef struct
{
    /* data */
    BufBlock block;
    int id;
} Buffer;

// ==================== extern api ====================
void BufInit(void)
{
    //完成内存的初始化，请求分配空间
    return;
}

void ReadBuffer(string tag)
{
    // step1 - 检查buffer的合法性
    // step2 - 查询tag和buf id的映射转换
    // step3 - 如果buf id不为空即为该数据块在buffer内，将此块返回
    // step4 - 如果step3失败去调用调度算法，完成后返回
}

// ==================== internal func ====================
// 这一部分主要是buffer的调度策略，LRU、FIFO等调页淘汰算法的实现
void BufSchedule(void)
{
    // step 1 - 查看buf当前的空间，如果有空间直接调新的快进来
    // step 2 -如果空间不够则调用淘汰算法空出空间来调页
}

// ==================== OS call ====================
// 这一部分主要和操作系统交互，包括读取块、内存的基本操作
// 如追加、清空、删除等，查看PG的bufmgr
// 以及一些内部的加锁（待定）
