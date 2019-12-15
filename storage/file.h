#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include "config.h"
#include "buffer.h"

/*       ��ҳʽ�ļ�����ģ��-ͷ�ļ�
 * ��ģ���ڲ��ɷ�Ϊsetment,page,file������ģ�顣
 * ��ģ��ֱ��ڲ�ͬ��cpp�ļ���ʵ�ֶ�Ӧ�ĺ�����
 *
**/

// ==================== data structure ====================
// ҳ������Ϣ
struct PageMeta{
	long pageNo;      // ҳ��
	long prePageNo;	  // ǰ��ҳ��
	long nextPageNo;  // ���ҳ��
	long freeSpace;   // ��ҳ�Ŀ��пռ��С
	int recordNum;    // ��ҳ�洢�ļ�¼����
};
struct OffsetInPage{
	int recordID;
	int offset;         //�ü�¼����ڿ�β��ַ��ƫ����
	bool isDeleted;
};

//��������Ϣ
struct Segment{
	int state;  // ��״̬ 
	int id;     // �κ�
	int type;   // -----�����ڱ�ǹ�����δʹ��
	long firstPageNo; // �öε���ʼҳ��
	long pageNum;     // �öε�ҳ��
};

//�ļ�������Ϣ
struct FileMeta{
	int id;       // �ļ���
	long pageNum; // �ļ�ҳ��	
	long firstPageNo;// �ļ�����
	char name[20];// �ļ���,�20���ַ�
	int state;    // �ļ�״̬
	int segNum;   // �ļ�����
	int fileType;
	struct Segment segList[SEGMENT_NUM];// �α�
};

struct Record{
	long pageNo;   //ҳ��
	int recordID;  //ҳ�ڵڼ�����¼
	char* posOffset;//��ַ
};

// ==================== manager function ====================
// void file_Init(struct DataBase *DB);

// �ļ�����-ʵ����file����
struct Record file_writeFile(int fileID, int length, char *str);
int file_newFile(FILE_TYPE fileType, long NeededPageNum);
void file_readFile(int FileID,char *str);
void file_deleteFile(int FileID);
void file_print_freepace();

void file_read_sd(long pageNo, char *bufferpath); //�ṩ��bufferģ�飬��ȡ����
void file_write_sd(long pageNo, char *bufferpath);//�ṩ��bufferģ�飬д�����

// �����ṩ��ͨ�ļ�����
void file_fseek(int fileID, long offset, int fromwhere);//�ļ��� ƫ����(��������ʾ����) ƫ��λ��(����-B+��-����)


// ҳ����-ʵ����page����
void writePage(long pageNo);//����buffer,дָ����ҳ
void readPage(long pageNo); //����buffer,��ָ����ҳ
int page_isEmpty(unsigned long bit_map,int position);
void page_setbitmap(unsigned long *bit_map,int position,int value);

long page_requestPage(long NeededPageNum);

void page_recover_onepage(int PageNo);
void page_recover_allpages();


// �ι���-ʵ����segment����




#endif