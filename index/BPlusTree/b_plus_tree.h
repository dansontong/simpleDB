#include<stdio.h>

//���ڵ���
#define MAX 10
//��С�ڵ���
#define MIN 5
//�ڲ��ڵ�Ϊ 0
#define NODE 0
//Ҷ�ڵ�Ϊ 1
#define LEAF 1

#define LEFT 0
#define RIGHT 1

//extern struct DataBase *DB; 

typedef struct
{
	int key;//��¼�ļ�ֵ
	int pos;//�ڵ������¼�����λ��
	int posPage;//ҳ��
	int posOffset;//ƫ����
}TreeRecord;//B+���ڵ��ϵļ�¼

typedef struct Node
{
	int type;//�ڵ������ ��ΪҶ�ڵ� ���� �Һ���
	int count;//record�ĸ���
	TreeRecord pair[MAX];
	int parent;//���ڵ����ڵ�λ��
	struct Node *NextLeaf;
}Node;//B+���Ľڵ�

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