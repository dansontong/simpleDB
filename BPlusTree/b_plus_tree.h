#include<stdio.h>

//最大节点数
#define MAX 5
//最小节点数
#define MIN 3
//内部节点为 0
#define NODE 0
//叶节点为 1
#define LEAF 1

#define LEFT 0
#define RIGHT 1


typedef struct
{
	int key;//记录的键值
	int posPage;//页号
	int posOffset;//偏移量
}RecordOffset;//B+树节点上的记录

typedef struct
{
	int type;//节点的种类 分为叶节点 左孩子 右孩子
	int count;//record的个数
	RecordOffsetOffset pair[MAX];
	int parent;//父节点所在的位置
}Node;//B+树的节点

void getRoot(FILE *index, Node &node);
int search(FILE *index, int key);
void searchNode(FILE *index, Node &node, int key);
int searchRecordOffset(Node &node, int key);
int insert(FILE *index, RecordOffset record);
int insertRecordOffset(FILE *index, Node &node, RecordOffset record);
void splitNode(FILE *index, Node &nodea, RecordOffset record, int pos);
void enlargeKey(FILE *index, Node &node);
int del(FILE *index, int key);
int delRecordOffset(FILE *index, Node &node, int key);
void transRecordOffset(FILE *index, Node &left, Node &right, int dir, int pos);
void mergeNode(FILE *index, Node &left, Node &right);
void ensmallKey(FILE *index, Node &nod);
void changeParent(FILE *index, Node &node, int child, int parent);
void showTree(FILE *index,Node &node);