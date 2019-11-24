### 文件说明
trivialtree.* 简单实现的一个多叉树，树结构：孩子兄弟结构表示
sqllexical.lex  词法解析文件
sqlparser.y     语法规则
sqlparser.h/.c  最终文件

### 测试说明
1. 依赖编译  
```
make depends
make sqlparser
```
2. 直接make编译生成可执行文件sqlparser  
    测试样例可看testcase.sql
3. 运行测试  
可以执行下面的demo
```
./sqlparser testcase.sql
```  
程序会在每一条sql语句后输出语法树

### TODO
1. ~~支持select *~~
2. 支持多种数据类型，目前仅支持int
3. 支持复杂的condition，优化语法规则（表达式？）
4. ~~出错信息提示（出错位置 => 何种错误）~~
5. ~~语法复用有些问题，没法处理\<sel_list\>、\<col_list\>、\<from_list\>等问题~~
6. 逻辑运算符优先级
7. ~~支持语句换行~~
8. ~~action优化（非重点）~~
9. ~~整理输入输出~~