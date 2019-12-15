#ifndef B_PLUS_TREE_H_INCLUDE
#define B_PLUS_TREE_H_INCLUDE

#include <stdio.h>

//最大节点数
#define MAX 10
//最小节点数
#define MIN 5
//内部节点为 0
#define NODE 0
//叶节点为 1
#define LEAF 1

#define LEFT 0
#define RIGHT 1

//extern struct DataBase *DB; 

typedef struct
{
	int key;//记录的键值
	int pos;//节点里面记录的相对位置
	int posPage;//页号
	// char* posOffset;//偏移量
	int recordID;   //记录ID
}TreeRecord;//B+树节点上的记录

typedef struct Node
{
	int type;//节点的种类 分为叶节点 左孩子 右孩子
	int count;//record的个数
	TreeRecord pair[MAX];
	int parent;//父节点所在的位置
	struct Node *NextLeaf;
}Node;//B+树的节点

void getRoot(FILE *index, Node &node);
int search(FILE *index, int key);
void searchNode(FILE *index, Node &node, int key);
int searchTreeRecord(Node &node, int key);
int insert(FILE *index, TreeRecord record);
int insertTreeRecord(FILE *index, Node &node, TreeRecord record);
void splitNode(FILE *index, Node &nodea, TreeRecord record, int pos);
void enlargeKey(FILE *index, Node &node);
int del(FILE *index, int key);
int delTreeRecord(FILE *index, Node &node, int key);
void transTreeRecord(FILE *index, Node &left, Node &right, int dir, int pos);
void mergeNode(FILE *index, Node &left, Node &right);
void ensmallKey(FILE *index, Node &nod);
void changeParent(FILE *index, Node &node, int child, int parent);
void showTree(FILE *index,Node &node);

#endif