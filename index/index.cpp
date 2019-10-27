#include "file.h"
#include "database.h"
#include "buffer.h"
#include "log.h"
#include "index.h"

void creat_index(struct DataBase *DB,char *tableName,char *Attributename){

	int fid,FileID,tb,attr;
	for(tb=0;tb<MAX_FILE_NUM;tb++){														//查找表文件的文件号
		if(strcmp(DB->dataDict[tb].tableName,tableName)==0){
			FileID=DB->dataDict[tb].fileID;
		}
	}
	for(attr=0;attr<DB->dataDict[tb].attrNum;attr++){									//查找属性列
		if(strcmp(DB->dataDict[tb].attr[attr].name,Attributename)==0){
			if(DB->dataDict[tb].attr[attr].indexIsExist==0){							//属性列没有索引文件存在
				fid=file_newFile(DB,INDEX_FILE,1)；										//新建索引文件
				DB->dataDict[tb].attr[attr].indexIsExist=1；
				int i;
				for(i=0;i<MAX_FILE_NUM;i++){												//查找表文件的起始页号
					if(DB->dbMeta.fileMeta[0].segList[i].id==FileID)						//
						break;																//
				}																			//
				long CurpageNo = DB->dbMeta.fileMeta[0].segList[i].firstPageNo;				//
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

//							insert(FILE *index, TreeRecord record);							//建立B+树索引

						}
					}
					long nextPno = pagehead.nextPageNo;
					if(nextPno==-1)
						break;
					else
						CurpageNo = nextPno;
				}
			}
			else{
				printf("该属性列的索引文件已存在！");
			}
		}	
	}	
}