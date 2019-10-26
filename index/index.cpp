#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"

void creat_index(struct DataBase *DB,char *tableName,char *Attributename){
	int FileID;
	for(int i=0;i<MAX_FILE_NUM;i++){											//查找表文件的文件号
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){
			FileID=DB->dataDict[i].fileID;
		}
	}
	
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){												//查找起始页号
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){						//
			break;				
		}
	}

	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	int sizeofpagehead = sizeof(struct PageMeta);								//读取文件信息
	int sizeofrecord = sizeof(struct OffsetInPage);								//
	OffsetInPage preoffset,curoffset;
	struct PageMeta pagehead;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);						//根据页号从缓冲区调取页的内容
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);				//读取第一页的内容并存放在pagehead里

		if(pagehead.recordNum>0){
			for(int j=0;j<pagehead.recordNum;j++){
				int readlength;
				if(j==0){																	//打印页的每条记录
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead,sizeofrecord);
					readlength = curoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
					printf("%s\n",str);
				}
				else{
					preoffset = curoffset;
					memcpy(&curoffset,Buf_ReadBuffer(buftag)+sizeofpagehead+sizeofrecord*j,sizeofrecord);
					readlength = curoffset.offset-preoffset.offset;
					memcpy(str,Buf_ReadBuffer(buftag)+PAGE_SIZE-curoffset.offset,readlength);
					str[readlength] = '\0';
					printf("该页面中第%d记录\n",j+1);
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