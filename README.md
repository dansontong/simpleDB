# simpleDB
simple database using cpp.  

  

problem:  
1, 磁盘使用空间受bitMap大小限制，以后如何扩展数据库磁盘空间？  
2, 如何互斥访问buffer数据页？设置lock变量，然后轮询？应用回调思想设置等待队列？每个数据页都要一个队列，是不是不现实？



模块分工：  
1, 缓冲相关:马纳波, 包括buffer.h/cpp  
2, 段页式文件管理：韩瑞琛，包括file.h/cpp, page.h/cpp, segment.h/cpp  
3, 存储管理之其他：唐鑫桂，包括storage.h/cpp, config.h  
4, 日志系统：唐鑫桂, 包括log文件夹  
5, 查询编译器parser：唐鑫桂（来自github）,parser文件夹  
