# simpleDB
simple database using cpp.



problem:
1, 实际磁盘中，目前只支持一个数据库文件，怎么解决？如果多个，bitMap是否也需要动态调入调出？postgresql如何做的？
2, 如何互斥访问buffer数据页？设置lock变量，然后轮询？应用回调思想设置等待队列？每个数据页都要一个队列，是不是不现实？
