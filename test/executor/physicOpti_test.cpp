#include "optimize.h"

int main(){
	struct Selectnode select1;
	struct Plannode headplan;

	char sql[100] = "select * from a,b where a.a1 = b.a1 and num != (select1 num from b1,a1 where a=0);";
	
	select1.subselect = 0;
	strcpy(select1.tablename[0],"a");
	strcpy(select1.tablename[1],"b");
	strcpy(select1.tablename[2],"b1");
	strcpy(select1.tablename[3],"a1");
	select1.subtablename[0] = 0;
	select1.subtablename[1] = 2;
	strcpy(select1.attribute[0],"a.a1");
	strcpy(select1.attribute[1],"b.a1");
	strcpy(select1.attribute[2],"num");
	strcpy(select1.attribute[3],"subselect");
	strcpy(select1.attribute[4],"a");
	strcpy(select1.attribute[5],"0");
	strcpy(select1.attribute[6],"a");
	strcpy(select1.attribute[7],"0");
	select1.subattribute[0] = 0;
	select1.subattribute[1] = 4;
	strcpy(select1.attrioftable[0],"a");
	strcpy(select1.attrioftable[1],"b");
	strcpy(select1.attrioftable[2],"a");
	strcpy(select1.attrioftable[3],"NULL");
	strcpy(select1.attrioftable[4],"b1");
	strcpy(select1.attrioftable[5],"b");
	strcpy(select1.attrioftable[6],"b1");
	strcpy(select1.attrioftable[7],"NULL");
	select1.subattributetable[0]=0;
	select1.subattributetable[1]=4;
	strcpy(select1.projectionattribute[0],"*");
	strcpy(select1.projectionattribute[1],"num");
	select1.sub_pro[0] = 0;
	select1.sub_pro[1] = 1;
	strcpy(select1.op[0],"=");
	strcpy(select1.op[1],"!=");
	strcpy(select1.op[2],"=");
	strcpy(select1.op[3],"=");
	select1.subop[0]=0;
	select1.subop[1]=2;
	select1.num_tab=2;
	select1.num_attri=4;
	select1.num_attri_tab =4;
	select1.num_op = 2;
	select1.num_pro = 1;
	init(select1);
	Selectnode *select = &select1;
	////printf("%d",select->num_op);
	//////printf("%s",headplan.parents->parents->parents->right->kind);
	
	Plannode *cur = headplan.left;
	Plannode* last = headplan.parents;
	last->left=NULL;
	//Eliminate_subquery(headplan,select);
	//printf("%s->",cur->left->left->right->left->left->kind);
	//ergodic(cur);
	Down_filterandproject(headplan);
	bool flag=false;
	aaa(cur);
	/*while(1){
		if(cur==&headplan)break;
		else{
			printf("%s->",cur->kind);
			if(strcmp(cur->kind,"filter")==0&&flag==false){
				cur = cur->right;
				flag=true;
			}
			else{
				cur = cur->left;
			}
			
		}
	}*/
	//生成计划树：project-filter-join-table-project-filter-table   未优化的查询树都是先执行join再过滤再投影
}                  //              |
                        //        table