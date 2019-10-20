# simpleDB
simple database using cpp.  

  

problem:  
1, 磁盘使用空间受bitMap大小限制，以后如何扩展数据库磁盘空间？  
2, 如何互斥访问buffer数据页？设置lock变量，然后轮询？应用回调思想设置等待队列？每个数据页都要一个队列，是不是不现实？
3, 索引模块需提供的接口：（参数根据需要调整）
　create_index(table,collumn)//对列建索引  
　drop_index(table,collumn) //对列删除索引  
　insert_index(table,collumn,rowid)//对指定索引插入一项  
　delete_index(table,collumn,rowid)//对指定索引删除一项  
  
  
  
模块分工：  
1, 缓冲区管理:   mnb，包括buffer.h/cpp  
2, 段页式文件管理:hrc，包括file.h/cpp, page.h/cpp, segment.h/cpp  
3, 存储管理之其他:txg，包括storage.h/cpp, config.h  
4, 简易日志系统:  txg，包括log文件夹  
5, 查询编译器:    txg，包括parser文件夹（来自github）  
6, 索引:         lh ，
