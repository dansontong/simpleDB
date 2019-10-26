#include<stdio.h>

//���ڵ���
#define MAX 5
//��С�ڵ���
#define MIN 3
//�ڲ��ڵ�Ϊ 0
#define NODE 0
//Ҷ�ڵ�Ϊ 1
#define LEAF 1

#define LEFT 0
#define RIGHT 1


typedef struct
{
	int key;//��¼�ļ�ֵ
	int posPage;//ҳ��
	int posOffset;//ƫ����
}RecordOffset;//B+���ڵ��ϵļ�¼

typedef struct
{
	int type;//�ڵ������ ��ΪҶ�ڵ� ���� �Һ���
	int count;//record�ĸ���
	RecordOffsetOffset pair[MAX];
	int parent;//���ڵ����ڵ�λ��
}Node;//B+���Ľڵ�

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