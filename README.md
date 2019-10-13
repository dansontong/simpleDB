# simpleDB
simple database using cpp.  

  

problem:  
1, 磁盘使用空间受bitMap大小限制，以后如何扩展数据库磁盘空间？  
2, 如何互斥访问buffer数据页？设置lock变量，然后轮询？应用回调思想设置等待队列？每个数据页都要一个队列，是不是不现实？
