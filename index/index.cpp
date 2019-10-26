#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "index.h"

void creat_index(struct DataBase *DB,char *tableName,char *Attributename){
	int FileID;
	for(int i=0;i<MAX_FILE_NUM;i++){											//查找表文件的文件号
		if(strcmp(DB->dataDict[i].tableName,tableName)==0){
			FileID=DB->dataDict[i].fileID;
		}
	}
	
	int i;
	for(i=0;i<MAX_FILE_NUM;i++){						
		if(DB->dbMeta.fileMeta[0].segList[i].id==FileID){	
			break;				
		}
	}

	long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				//查找起始页号
	long pagenum = DB->dbMeta.fileMeta[0].segList[i].pageNum;
	int sizeofpagehead = sizeof(struct PageMeta);								//读取文件信息
	int sizeofrecord = sizeof(struct OffsetInPage);								//
	struct RecordOffset indexRecord;
	struct PageMeta pagehead;
	for(i=0;i<pagenum;i++){					
		struct BufTag buftag = Buf_GenerateTag(CurpageNo);						//根据页号从缓冲区调取页的内容
		memcpy(&pagehead,Buf_ReadBuffer(buftag),sizeofpagehead);				//读取第一页的内容并存放在pagehead里

		if(pagehead.recordNum>0){
			for(int j=0;j<pagehead.recordNum;j++){
				indexRecord.posPage=pagehead.pageNo;
				file_getrecordAttribute(DB,pagehead.pageNo,j,tableName,Attributename,indexRecord.key,indexRecord.posOffset);
				//获得属性indexRecord参数，调用建树接口


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