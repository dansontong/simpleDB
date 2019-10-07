#ifdef BUF

// buf初始化
// 参数待定

#include <string>

extern void BufInit(void);

// buf 对外提供buf的读取函数
// 参数待定
// 基本思路：根据tag去返回一个块的数据
// tag与数据相关，确保唯一性
extern void ReadBuffer(std::string tag);

#endif