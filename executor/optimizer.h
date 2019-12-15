#ifndef EXECUTOR_H_INCLUDE
#define EXECUTOR_H_INCLUDE

#include "config.h"
#include "database.h"

// ==================== Logic optimiser =====================
struct Selectnode {//所需的数据结构
	char tablename[10][100]; //from 后的关系名
	char attribute[10][100]; //where 后的属性名 where a.a1 = b.a1 and b1 = 1 就是 a.a1 b.a1 b1 1
	char attrioftable[10][100];//属性所属的表名 接上一行 a b b null(null表示这个是单纯的一个数值，不是列名)
	char op[10][100];//运算符：= =
	char projectionattribute[10][100];//select 关键词后的投影的属性名select *  就是*
	int num_tab;//tablename的总数量
	int num_attri;//where中属性的总数量
	int num_attri_tab;//和num_attri值相同
	int num_op;//运算符的总数量
	int num_pro;//投影的属性的总数量
	int subselect;//子查询的总数量  只写了一个子查询的情况
	int subtablename[10];//主查询与子查询 在表名的下标，subtablename[0]=0 subtablename[1]=2
	int subattribute[10];//同上
	int subattributetable[10];//同上
	int sub_pro[10];//同上
	int subop[10];//同上
}select1;
int count_tab=49;
struct Tablenode{//表节点
	char tablename[10];
};

struct Filternode{//过滤节点
	char attribute[10][100];//属性名
	char attrioftable[10][100];//属性所属的表
	char op[10][100];//运算符
	int number;
	bool is_subquery;
};

struct Joinnode{
	char newtable[10];
	bool is_equijoin;//区分卡式积与各种连接（等值连接）
	char attribute[10][100];//卡式积只有第一个newtable属性 后面几个属性都是等值连接等用到的
	char attrioftable[10][100];
	char op[10][100];
	int number;//op的个数
};

struct Projectionnode{
	char attribute[10][100];
	char tablename[10];
	int number;//属性的个数
	char newtable[10];
};

struct Plannode{//查询计划树的一个节点
	char kind[10];
	union{
		Tablenode tablenode;
		Joinnode joinnode;
		Filternode filternode;
		Projectionnode projectionnode;
	};
	struct Plannode *left;
	struct Plannode *right;
	struct Plannode *parents;
}headplan;

struct Result{
	Plannode* node[10];
	int number;
};


// ==================== physic optimiser ========================




#endif