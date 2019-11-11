#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"

//==================== global variable ====================
extern struct DataBase *DB; /* ȫ�ֹ��� */

// void file_Init(struct DataBase *db)
// {
// 	DB = db;
// }
/**************************************************
 *                  file                          *
 **************************************************/
//-1��ʾ�ļ�����ʧ��
int file_newFile(int type, long NeededPageNum){
	if(DB->dbMeta.currFileNum>=MAX_FILE_NUM||DB->dbMeta.blockFree<NeededPageNum){
		printf("���пռ䲻�㣬�ļ�����ʧ�ܣ�/n");
		exit(0);	
	}
	int id = DB->dbMeta.currFileNum;
	DB->dbMeta.currFileNum++;
	long NewPages = page_requestPage(NeededPageNum);
	if(NewPages>=0){
		int i,j;
		for(i=0,j =NewPages;i<NeededPageNum,j<(DB->dbMeta.blockNum);i++,j++){
			struct PageMeta pagemeta;
			pagemeta.recordNum = 0;
			pagemeta.pageNo = j;
			if(i==0){
				pagemeta.prePageNo=-1;//-1��ʾû��ǰҳ
				pagemeta.nextPageNo=j+1;
			}
			else{
				pagemeta.prePageNo=j-1;
				if(j==NewPages+NeededPageNum-1){
					pagemeta.nextPageNo=-1;//-1��ʾû�к�ҳ
					
				}
				else{
					pagemeta.nextPageNo = j+1;
				}
			}
			pagemeta.freeSpace = PAGE_SIZE - sizeof(pagemeta);
			rewind(DB->dbFile);
			fseek(DB->dbFile,DB->dbMeta.dataAddr+pagemeta.pageNo*PAGE_SIZE,SEEK_SET);
			fwrite(&pagemeta,sizeof(pagemeta),1,DB->dbFile);
		}
		for( i = 0;i<MAX_FILE_NUM;i++){
			//printf("DB->dbMeta.fileMeta[i].id %d: \n", DB->dbMeta.fileMeta[i].id);
			if(DB->dbMeta.fileMeta[i].id<0){
				break;
			}
		}
		DB->dbMeta.fileMeta[i].id=id;
		DB->dbMeta.fileMeta[i].fileType=type;
		DB->dbMeta.fileMeta[i].firstPageNo=NewPages;
		DB->dbMeta.fileMeta[i].pageNum=NeededPageNum;
		DB->dbMeta.fileMeta[i].segList[i].id=id;
		DB->dbMeta.fileMeta[i].segList[i].firstPageNo=NewPages;
		DB->dbMeta.fileMeta[i].segList[i].pageNum=NeededPageNum;
		DB->dbMeta.blockFree=DB->dbMeta.blockFree-NeededPageNum;
		file_print_freepace();
		
	}
	else{
		printf("δ���㹻�������洢�ռ䣬�ļ�����ʧ�ܣ�/n");
		return -1;//-1��ʾ����ʧ��
	}
	return id;
	
}

struct Record file_writeFile(int FileID, int length,char *str){
	Record record; // ���ظմ���ļ�¼������Ϣ

	int querypage=-1;
	int i;
	for( i=0;i<MAX_FILE_NUM;i++){                                               //��һ���ǲ����ļ��Ƿ����
		if(DB->dbMeta.fileMeta[i].segList[i].id==FileID){						//
			querypage=DB->dbMeta.fileMeta[i].segList[i].firstPageNo;			//
			break;																//
		}																		//
	}
	if(querypage==-1){
		printf("���ļ�id��Ӧ���ļ������ڣ�");
		exit(0);
	}

	// long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				
	// long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[i].firstPageNo;				
	long pagenum = DB->dbMeta.fileMeta[i].pageNum;
	int fileno = i;
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);									//��ȡ���ļ�����Ϣ
	rewind(DB->dbFile);					
	bool isfound = false;
	struct PageMeta pagehead;
	struct BufTag buftag = Buf_GenerateTag(CurpageNo);
	memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);						//��ȡ��һҳ�����ݲ������pagehead��
	OffsetInPage preoffset,curoffset;							//ҳ��ļ�¼�����Ľṹ�壬������file.h��
	long currecordpos,curoffsetpos;								//ǰһ����ָ��ǰ��¼������λ�ã��ڶ�����ָ��ǰ��¼��λ��
	for(int i=0;i<pagenum;i++){									//��ѭ����Ϊ�˱������е�ҳ�ҳ��ܴ�Ÿü�¼��ҳ
		// printf("page:%d,���пռ䣺%ld\n",i,pagehead.freeSpace);
		if(pagehead.freeSpace<=length+sizeofrecord){
			if(pagehead.nextPageNo==-1){
				break;
			}
			CurpageNo = pagehead.nextPageNo;
			buftag = Buf_GenerateTag(CurpageNo);
			memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
			continue;	
		}
		else{
			memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead,sizeofrecord);
			isfound = true;
			if(pagehead.recordNum==0){
				curoffset.recordID = 0;
				curoffset.offset = length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead;
				curoffsetpos =  PAGE_SIZE - length;
				
				
			}
			else{
				memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+(pagehead.recordNum-1)*sizeofrecord,sizeofrecord);
				curoffset.recordID = pagehead.recordNum;
				curoffset.offset = preoffset.offset+length;
				curoffset.isDeleted = false;
				currecordpos = sizeofpagehead + sizeofrecord*pagehead.recordNum; //currecordpos �ȼ��ڣ�ҳ���ѱ�ռ�ݵĿռ��С��ҳǰ���record������Ϣ������record���ݴ���ҳ�ס�
				curoffsetpos = PAGE_SIZE - preoffset.offset-length;
			}
			
		}
		pagehead.recordNum++;
		pagehead.freeSpace=pagehead.freeSpace-length-sizeofrecord;
		memcpy(Buf_ReadBuffer(buftag),&pagehead,sizeofpagehead);
		memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
		memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
		break;						//�ҵ����break
	}
	if(!isfound){					//��������û��ҳ��������һ��ҳ��
		long CurpageNo = page_requestPage(1);
		if(CurpageNo>=0){
			DB->dbMeta.blockFree=DB->dbMeta.blockFree-1;
			file_print_freepace();
			struct PageMeta pagemeta; //pagehead����δ����ǰ���һ��ҳ
			pagemeta.nextPageNo=-1;
			pagemeta.prePageNo=pagehead.pageNo;				
			pagemeta.pageNo=CurpageNo;
			pagehead.nextPageNo = CurpageNo;		//����ҳ��������ļ���				
			pagemeta.recordNum = 1;
			pagemeta.freeSpace = PAGE_SIZE - length - sizeofpagehead - sizeofrecord;
			curoffsetpos = PAGE_SIZE-length;
			currecordpos = sizeofpagehead;
			curoffset.recordID = 0;
			curoffset.offset = length;
			curoffset.isDeleted = false;
			buftag = Buf_GenerateTag(pagenum+1);
			memcpy(Buf_ReadBuffer(buftag),&pagemeta,sizeofpagehead);
			memcpy(Buf_ReadBuffer(buftag)+currecordpos,&curoffset,sizeofrecord);
			memcpy(Buf_ReadBuffer(buftag)+curoffsetpos,str,length);
			memcpy(Buf_ReadBuffer(buftag),&pagehead,sizeofpagehead);

			DB->dbMeta.fileMeta[fileno].segList[0].pageNum++;
		}
	}
	record.pageNo = CurpageNo;
	record.recordID = curoffset.recordID;
	return record;
}
void file_readFile(int FileID,char *str){
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){
			break;
		}
	}
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	OffsetInPage preoffset,curoffset;
	struct PageMeta pagehead;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);		//����ҳ�Ŵӻ�������ȡҳ������
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);//��ӡҳ�Ļ�����Ϣ
		printf("��%d���ļ��еĵ�%d��ҳ��\n",FileID,i+1);
		printf("ҳ�ţ�%ld\n",pagehead.pageNo);
		printf("ǰ��ҳ�ţ�%ld\n",pagehead.prePageNo);
		printf("���ҳ�ţ�%ld\n",pagehead.nextPageNo);
		printf("��¼������%d\n",pagehead.recordNum);
		printf("���пռ䣺%ld\n",pagehead.freeSpace);
		if(pagehead.recordNum>0){
			for(int j=0;j<pagehead.recordNum;j++){
				int readlength;
				if(j==0){																	//��ӡҳ��ÿ����¼
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead,sizeofrecord);
					readlength = curoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("��ҳ���е�%d��¼\n",j+1);
					printf("%s\n",str);
				}
				else{
					preoffset = curoffset;
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*j,sizeofrecord);
					readlength = curoffset.offset-preoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("��ҳ���е�%d��¼\n",j+1);
					printf("%s\n",str);
				}
			}
		}
		long nextPno = pagehead.nextPageNo;
		if(nextPno==-1){
			break;
		}
		else{
			CurpageNo = nextPno;
		}
	}
}

void file_deleteFile(int FileID){
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){			//�ҵ��ļ���Ӧ��ҳ
			break;
		}
	}
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;			//��ȡ��һҳ����Ϣ
	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	long pageAddr = DB->dbMeta.dataAddr +CurpageNo * PAGE_SIZE;
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	long nextPage = -1;
	struct PageMeta pagehead;
	for(long j=0;j<pagenum;j++){							//����ÿһҳ
		rewind(DB->dbFile);
		fseek(DB->dbFile,pageAddr,SEEK_SET);
		size_t sizeRead = fread(&pagehead,sizeofpagehead,1,DB->dbFile);			//��ȡ��һҳ������
		nextPage = pagehead.nextPageNo;
		page_recover_onepage(pagehead.pageNo);				//ɾ����һҳ
		if(nextPage>0){
			pageAddr = DB->dbMeta.dataAddr +nextPage * PAGE_SIZE;	//��ȡ�µ�һҳ�ĵ�ַ
		}
		else{
			break;
		}
	}
	DB->dbMeta.blockFree += pagenum;
	DB->dbMeta.currFileNum--;
	DB->dbMeta.fileMeta[0].segList[i].type = -1;
	DB->dbMeta.fileMeta[0].segList[i].id = -1;
	DB->dbMeta.fileMeta[0].segList[i].firstPageNo = -1;
	DB->dbMeta.fileMeta[0].segList[i].pageNum = -1;
	
}
void file_read_sd(long pageno,char *bufferpath){
	rewind(DB->dbFile);
	fseek(DB->dbFile,DB->dbMeta.dataAddr+pageno*PAGE_SIZE,SEEK_SET);
	size_t sizeRead = fread(bufferpath,PAGE_SIZE,1,DB->dbFile);
}
void file_write_sd(long pageno,char *bufferpath){
	rewind(DB->dbFile);
	fseek(DB->dbFile,DB->dbMeta.dataAddr+pageno*PAGE_SIZE,SEEK_SET);
	fwrite(bufferpath,PAGE_SIZE,1,DB->dbFile);
}
void file_print_freepace(){
	printf("already used %ld blocks, still have %ld blocks vacant.\n",DB->dbMeta.blockNum-DB->dbMeta.blockFree,DB->dbMeta.blockFree);
	
}

bool file_getrecord(long pageNo,int recordID,char *record){
	
	int sizeofpagehead = sizeof(struct PageMeta);
	int sizeofrecord = sizeof(struct OffsetInPage);
	OffsetInPage curoffset,preoffset;
	struct BufTag buftag = Buf_GenerateTag(pageNo);
	struct PageMeta pagehead;
	memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);
	if(recordID<0||recordID>pagehead.recordNum){
		return false;
	}
	if(recordID>0){//�ж��Ƿ��ǵ�һ����¼
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*recordID,sizeofrecord);
		memcpy(&preoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*(recordID-1),sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset-preoffset.offset);//��¼��λ��Ϊҳ����ʼλ�ü���pagesize-��¼����ҳβ�ľ��룬��¼����Ϊ�ü�¼����ҳβ��λ�ü�ȥ��һ����¼����ҳβ��λ��
		return true;
	}
	else{
		memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*recordID,sizeofrecord);
		memcpy(record,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,curoffset.offset);//��һ����¼�ĳ���Ϊ�ü�¼����ҳβ�ľ���
		return true;
	}
	return false;
}

bool file_getrecordAttribute(long pageNo,int recordID,char* tablename,char* Attributename,char* Attribute,char* posOffset){
	char *record;
	bool flag;

	if(file_getrecord(pageNo,recordID,record)){//���ظ�����¼
		int i=0;
		for(i=0;i<MAX_FILE_NUM;i++){
			if(DB->dataDict[i].fileID<0){
				return false;
			}
			else{
				if(strcmp(DB->dataDict[i].tableName,tablename)==0){//���ұ�
					int j=0;
					for(j=0;j<DB->dataDict[i].attrNum;j++){//�������ԣ������������ҵ������ڼ�¼�еľ���λ��
						if(strcmp(DB->dataDict[i].attr[j].name,Attributename)==0){
							if(getValueByAttrID(record, j, Attribute)>0){
								return true;
							}
							else{
								return false;
							}
							
						}
					}
				}
			}
		}
	}
	else{
		return false;
	}
}
int getValueByAttrID(char *str, int index, char *result){
	int length = strlen(str);
	int j = 0, k = 0;
	int start = 0;
	for (int i = 0; i <= length; i++) {
		if (str[i] == '|' || i == length){
			if (k == index){
				result[j] = '\0';
				return start;
			}
			else{
				start = i + 1;
				k++;
				memset(result, 0, length);
				j = 0;
			}
		}
		else
			result[j++] = str[i];
	}
	return -1;
}
void file_fseek(int fileID, long offset, int fromwhere)
{

}

/**************************************************
 *                  page                          *
 **************************************************/
int page_isEmpty(unsigned long bit_map,int position)
 {
	unsigned long result = 0x00000001;
	// bug 
	// ԭʼ��32-position
	result = result<<(SIZE_OF_LONG*8-position);
	result = result & bit_map;
	if (result == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void page_setbitmap(unsigned long *bit_map,int position,int value)
{
	if(value!=0&&value!=1){
		printf("value��ֵ�����Ϲ���!��ֻ����0��1��/n");
		return ;
	}
	if(value==page_isEmpty(*bit_map,position)){
		return;
	}
	unsigned long  result = 0x00000001;
	result = result<<(SIZE_OF_LONG*8-position);
	if(value==1){
		*bit_map = result|*bit_map;
	}
	else {
		result = ~result;
		*bit_map = *bit_map&result;
	}
	
}

long page_requestPage(long NeededPageNum)
{
	if(DB->dbMeta.blockFree<0){
		printf("û�п��е�ҳ������ʧ�ܣ�\n");
		return -1;
	}
	
	long i,j;
	long p_num,position;
	long count;
	long NewPages;
	for( i=0;i<DB->dbMeta.blockNum;i++){
		p_num = i/(8*sizeof(long));
		position = i- p_num*8*sizeof(long)+1;
		count = 0;
		if(page_isEmpty(*(DB->freeSpaceBitMap+p_num),position)==0){
			for(j=i;j<DB->dbMeta.blockNum;j++){
				if(page_isEmpty(*(DB->freeSpaceBitMap+p_num),position)==0){
					count++;
				}
				else{
					break;
				}
				if(count==NeededPageNum){
					break;
				}
			}
			
		}
		if(count==NeededPageNum){
			break;	
		}
		else{
			i = i+count;
			
		}
	}
	if(count==NeededPageNum){
		NewPages=i;
		for(j=0;j<NeededPageNum;j++){
			p_num = (i+j)/(8*sizeof(long));
			position =i+j- p_num*8*sizeof(long)+1;
			page_setbitmap(DB->freeSpaceBitMap+p_num,position,1);
		}
		DB->dbMeta.blockFree-=NeededPageNum;
		printf("1");
		return NewPages;
	}
	else{
		return -1;
	}
}

void page_recover_onepage(int PageNo)
{

	int p_num = PageNo/(8*sizeof(long));

	int position = PageNo - p_num*8*sizeof(long)+1;
	page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
}

void page_recover_allpages()
{
	for(int i= 0;i<DB->dbMeta.blockNum;i++){
		int p_num = i/(8*sizeof(long));
		int position = i- p_num*8*sizeof(long)+1;
		page_setbitmap(DB->freeSpaceBitMap+p_num,position,0);
	}
}


/**************************************************
 *                  segment                       *
 **************************************************/

