# simpleDB
simple database using cpp.  

  

**problem**:  
1, 磁盘使用空间受bitMap大小限制，以后如何扩展数据库磁盘空间？  

2, 如何互斥访问buffer数据页？设置lock变量，然后轮询？应用回调思想设置等待队列？每个数据页都要一个队列，是不是不现实？  
2.2, 是否已经实现pin固定住块的功能？  
3, 索引模块需提供的接口：（参数根据需要调整）  
　create_index(table,collumn)//对列建索引  
　drop_index(table,collumn) //对列删除索引  
　insert_index(table,collumn,rowid)//对指定索引插入一项  
　delete_index(table,collumn,rowid)//对指定索引删除一项  
fseek(FILE *stream, long offset, int fromwhere);//文件流 偏移量(用正负表示方向) 偏移位置  
4, DB在database.h 和 file.cpp中作为全局变量，后续需要多个数据库，需要改成c++类实现   
5, b+树模块测试如果之前文件存在会有问题，待解决。  
6，目前索引模块测试index_test.cpp中查找9840的记录，查找失败，待解决。  
~~7, buffer模块，201行的TODO,运行曾出现过215行Buf_StrategyLRU函数的assert(buf_id>=0)失败 done~~  
8, 查到的元组从磁盘解析出来。  
  
  
**模块分工**：  
1, 缓冲区管理:   mnb，包括strorage/buffer.h/cpp  
2, 段页式文件管理:hrc，包括strorage/file.h-cpp, page.h-cpp, segment.h-cpp  
3, 数据库核心:    txg，包括core/database.h-cpp, config.h  
4, 简易日志系统:  txg，包括log文件夹  
5, 查询编译器:    mnb，包括sqlparser文件夹
6, 索引-B+树:     lh ，包括/index/BPlusTree/b_plus_tree.h-cpp  
7，索引-接口：     ly，包括/index/index.h-cpp   
----- 第五次 -----   
8, 可视化窗口： ly,window文件夹  
9, 物理优化：  lh,executor文件夹  
10,逻辑优化：  hrc,executor文件夹  
11,查询执行：  mnb,executor文件夹.对接parser结果，调用物理优化和逻辑优化，执行 查询计划  
12,完善功能： txg  
  
  
TODO:mnb  
~~1, projection.cpp和select.cpp里面，同样用pageNo=1构造buftag后，为什么projection.cpp取到的pageMeta里pageNo=9979,select.cpp取到pageNo=1才是对的。为什么会有同样buftag返回不同的地址？？？  done，部分load忘记更新buftag~~  
~~2, file.cpp168行放开注释后，buffer会在start schedule输出后卡住，原因跑的太快了，大家时间戳都一样……~~  
3, 其他地方需要增加dirty标记，以便buffer调出时写回磁盘。  
  
TODO:index-module: lh & ly  
1, 索引需要存键值和对应的位置，目前只存了键值，相当于索引没有任何用处  

  
  https://github.com/dansontong/simpleDB/invitations 邀请链接



**总结**： （2019.01.12，tang-xg@qq.com,最后提交前的总结)   
1, 段页式文件管理实现理解有误，应该是一个文件对应一个段  
2, 数据库元数据中，数据字典应组织为普通表;其他数据库的基本信息DataBase这个结构体，不能直接将结构体写入文件，否则可能因为虚函数、对齐等原因导致数据出错。  
3, 索引模块该对了，但还没能使用sql语句直接测试。  
4, 可视化界面make执行还有问题，使用的qt，应先qmake -project，然后qmake **.pro,再make。目前在make前手动修改Makefile，企图整合系统其他所有模块，但尚未调通。  
5, test文件下要使用index_test.cpp主要看该文件中的说明。  
6, 目前工程根目录下，直接make, 然后./database_d即可运行。  
