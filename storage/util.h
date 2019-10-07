#ifndef STORAGE_UTIL_H
#define STORAGE_UTIL_H

// configuration配置参数
#define PAGE_SIZE (4 * 1024) //文件块大小==页大小

// buffer 配置参数
#define BUFSIZE (8*1024)  // buf的总大小，单位字节
#define BLOCKSIZE (4*1024) // buf内每一个块的大小，与数据块大小PAGE_SIZE保持一致

#endif