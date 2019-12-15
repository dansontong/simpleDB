#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Plannode *make_join_node(int start_table,int end_table,Plannode *cur){//生成table节点或join节点，若只有一张表就是一个table节点，多于一张表，就两辆结成一个join节点
	for(int j=0;j<end_table-start_table;j++){
		if(j!=0){//当有多个表时生成join节点
			//////printf("1");
			Plannode *table = (Plannode *)malloc(sizeof(Plannode));
			strcpy(table->kind,"table");
			strcpy(table->tablenode.tablename,select1.tablename[start_table+j]);
			Plannode *join =(Plannode *)malloc(sizeof(Plannode)); 
			strcpy(join->kind,"join");
			char str[2];
			str[0]=count_tab;
			strcpy(&str[1],"");
			count_tab++;
			char str_1[20] = "tmptable";
			strcat(str_1,str);
			////printf("%s\n",str_1);
			strcpy(join->joinnode.newtable,str_1);
			join->joinnode.is_equijoin=false;
			cur->parents = join;
			join->left =cur;
			join->right = table;
			table->parents = join;
			cur = join;
			}
			else{//单独的表就是table
				Plannode *table = (Plannode *)malloc(sizeof(Plannode));
				strcpy(table->kind,"table");
				strcpy(table->tablenode.tablename,select1.tablename[start_table+j]);
				cur->parents = table;
				table->left = cur;
				cur = table;
			}
}
	Plannode *result = cur;
	return result;
}
Plannode *make_filter_node(int start_op,int end_op,int start_attri,int end_attri,Plannode *cur){//where节点，选择过滤
	Plannode *filter = (Plannode *)malloc(sizeof(Plannode));
	filter->filternode.is_subquery=false;
	strcpy(filter->kind,"filter");
	for(int j=0;j<end_op-start_op;j++){
		strcpy(filter->filternode.op[j],select1.op[start_op+j]);
		////printf("%s\n",filter->filternode.op[j]);
	}
	for(int j=0;j<end_attri-start_attri;j++){
		strcpy(filter->filternode.attribute[j],select1.attribute[start_attri+j]);
		if(strcmp(filter->filternode.attribute[j],"subselect")==0){
			filter->filternode.is_subquery=true;
		}
		strcpy(filter->filternode.attrioftable[j],select1.attrioftable[start_attri+j]);
		////printf("%s\n",filter->filternode.attribute[j]);
	}
	cur->parents = filter;
	filter->left = cur;
	filter->filternode.number = end_op-start_op;
	cur = filter;
	return cur;
}
Plannode *make_project_node(int start_pro,int end_pro,Plannode* cur){//生成投影节点
	Plannode *project = (Plannode*)malloc(sizeof(Plannode));
	strcpy(project->kind,"project");
	for(int j=0;j<end_pro-start_pro;j++){
		////printf("%d",j);
		strcpy(project->projectionnode.attribute[j],select1.projectionattribute[start_pro+j]);
		////printf("%s\n",project->projectionnode.attribute[j]);
	}
	cur->parents = project;
	project->left = cur;
	project->projectionnode.number = end_pro-start_pro;
	Plannode *test=project->left->left;
	if(strcmp(test->kind,"join")==0){
		strcpy(project->projectionnode.tablename,test->joinnode.newtable);
	}
	else{
		strcpy(project->projectionnode.tablename,test->tablenode.tablename);
	}
	char str[2];
	str[0]=count_tab;
	strcpy(&str[1],"");
	count_tab++;
	char str_1[20] = "tmptable";
	strcat(str_1,str);
	strcpy(project->projectionnode.newtable,str_1);
	////printf("%s\n",project->projectionnode.tablename);
	return project;
}
void init(Selectnode select1){//根据select1生成初始查询计划
	int countnode=0;
	Plannode *cur = &headplan;
	
	int i = select1.subselect;
	int end_table = select1.num_tab;//找到每个查询中（主查询或子查询）各个属性在各数组的起始和最终位置的下标
	int start_table = select1.subtablename[i];
	int end_attri=select1.num_attri;
	int start_attri=select1.subattribute[i];
	int end_attrioftable=select1.num_attri_tab;
	int start_attrioftable=select1.subattributetable[i];
	int end_op = select1.num_op;
	int start_op = select1.subop[i];
	int start_pro = select1.sub_pro[i];
	int end_pro = select1.num_pro;
	while(i>=0){//按照子查询->主查询的顺序生成 初始查询计划：join（单独的表就是一个table||两表以上就是卡式积）->filter(where部分：对卡式积进行过滤->投影)
		cur = make_join_node(start_table,end_table,cur);//生成join节点
		cur = make_filter_node(start_op,end_op,start_attri,end_attri,cur);//生成filter节点对应where，实现对join后的表进行过滤
		cur = make_project_node(start_pro,end_pro,cur);//生成投影节点，将select后的属性值进行投影
		end_table = start_table;
		end_op = start_op;
		end_attri = start_attri;
		end_pro = start_pro;
		i--;
		start_table = select1.subtablename[i];
		start_op = select1.subop[i];
		start_attri = select1.subattribute[i];	
		start_pro = select1.sub_pro[i];
	}
	headplan.left = cur;		
		
		
	
}
Result ergodic(Plannode *head,char a[10]){//根据当前的节点对查询树进行遍历
	Plannode* stack[20];
	int top = -1;
	Plannode* cur=head;
	Result result;
	int counter=0;
	while((cur||top>=0)){
		while(cur){
			stack[++top]=cur;
			cur = cur->left;
		}
		if(top>=0){
			cur = stack[top--];
			if(strcmp(cur->kind,a)==0){
				result.node[counter++]=cur;
			}
			//printf("%s	",cur->kind);
			cur = cur->right;
		}
	}
	result.number = counter;
	return result;
	
}void aaa(Plannode *head){
	Plannode* stack[20];
	int top = -1;
	Plannode* cur=head;
	Result result;
	int counter=0;
	while((cur||top>=0)){
		while(cur){
			stack[++top]=cur;
			cur = cur->left;
		}
		if(top>=0){
			cur = stack[top--];
			if(strcmp(cur->kind,"join")==0){
				if(cur->joinnode.is_equijoin==true){
					printf("  equijoin  ");
				}
				else{
					printf("%s	",cur->kind);
				}
			}
			else{
				printf("%s	",cur->kind);
			}
			
			cur = cur->right;
		}
	}
	printf("\n");
}
void Eliminate_subquery(Plannode headplan,Selectnode *select){//消除子查询
	Plannode* cur = headplan.left;
	bool is_related = false;//判断是否是相关子查询
	int end_main = select->subtablename[select->subselect];
	int start_main = 0;
	int end_sub = select->num_attri_tab;
	int start_sub = select->subattributetable[select->subselect];
	int a[10][2];
	int count = 0;
	////printf("%d",end_sub);
	for(int j=0;j<end_sub-start_sub;j++){
		//printf("1\n");
		
		for(int m=0;m<end_main-start_main;m++){
			//printf("1");
			if(strcmp(select->attrioftable[start_sub+j],select->tablename[start_main+m])==0){
				a[count][0] = m+start_main;
				a[count][1] = j+start_sub;
				is_related = true;
				count++;
			//	printf("%d",count);
			}
		}
	}
	
	if(is_related==false){//非相关子查询
		while(1){
			if(strcmp(cur->kind,"filter")==0){
				for(int i=0;i<cur->filternode.number*2;i++){
					
					if(strcmp(cur->filternode.attribute[i],"subselect")==0){
						Plannode* tmp = cur->left;
						////printf("2\n");
						
						while(strcmp(tmp->kind,"join")==0){//在初始查询计划中，子查询直接接到主查询的最后一个table节点上，因为除了join节点外，其他节点相连都是在左孩子上
							tmp = tmp->left;
						}
						if(strcmp(cur->left->kind,"join")==0){
							strcpy(cur->filternode.attrioftable[i-1],cur->left->joinnode.newtable);
						}
						else{
							strcpy(cur->filternode.attrioftable[i-1],cur->left->tablenode.tablename);
						}
						printf("%s",cur->filternode.attrioftable[i-1]);
						Plannode* join = (Plannode*)malloc(sizeof(Plannode));//将子查询投影生成的表与主查询最高的join节点进行join（卡式积）操作
						strcpy(join->kind,"join");
						char str[2];
						str[0]=count_tab;
						strcpy(&str[1],"");
						count_tab++;
						char str_1[20] = "tmptable";
						strcat(str_1,str);
						strcpy(join->joinnode.newtable,str_1);
						join->parents = cur;
						join->joinnode.is_equijoin=false;
						join->left = cur->left;
						cur->left = join;
						join->right = tmp->left;
						tmp->left = NULL;
						strcpy(cur->filternode.attribute[i],join->right->projectionnode.attribute[0]);//这两条更新过滤条件中子查询对应部分
						strcpy(cur->filternode.attrioftable[i],join->right->projectionnode.newtable);//
						//	printf("%s->%s",cur->filternode.attribute[i],cur->filternode.attrioftable[i]);
						break;
					}
				}
				break;
			}
			cur = cur->left;
		}
	}
	else{//相关子查询  
		while(1){
			if(strcmp(cur->kind,"filter")==0){
				for(int i=0;i<cur->filternode.number*2;i++){
					if(strcmp(cur->filternode.attribute[i],"subselect")==0){
						if(strcmp(cur->left->kind,"join")==0){
							strcpy(cur->filternode.attrioftable[i-1],cur->left->joinnode.newtable);
						}
						else{
							strcpy(cur->filternode.attrioftable[i-1],cur->left->tablenode.tablename);
						}
						//printf("%s",cur->filternode.attrioftable[i-1]);
						Plannode* tmp_1= cur->left;
						//printf("%d",cur->filternode.number);
						//printf("2\n");
						while(strcmp(tmp_1->kind,"join")==0){//在初始查询计划中，子查询直接接到主查询的最后一个table节点上，因为除了join节点外，其他节点相连都是在左孩子上
							tmp_1 = tmp_1->left;
						}
						Plannode* tmp_2 = cur->left;
						while(strcmp(tmp_2->kind,"filter")!=0){//找到子查询中的filter节点，将其中与主查询相关的判断条件提前到join操作中作为连接的条件
							tmp_2 = tmp_2->left;
							//printf("%d",tmp_2->filternode.number);
						}
						int tmp[10] ={0};
						for(int m=0;m<count;m++){
							for(int j=0;j<tmp_2->filternode.number*2;j++){
								if(strcmp(tmp_2->filternode.attrioftable[j],select->attrioftable[a[m][1]])==0){
									tmp[m] = j;
									//printf("%s",select->attrioftable[a[m][1]]);
								}
						}
						}
						Plannode* join = (Plannode*)malloc(sizeof(Plannode));
						char str[2];
						str[0]=count_tab;
						strcpy(&str[1],"");
						count_tab++;
						char str_1[20] = "tmptable";
						strcat(str_1,str);
						strcpy(join->joinnode.newtable,str_1);//生成临时表的名称
						join->parents = cur;
						strcpy(join->kind,"join");//更改join节点的右孩子对应子查询
						join->left = cur->left;
						cur->left = join;
						join->right = tmp_1->left;
						tmp_1->left = NULL;
						strcpy(cur->filternode.attribute[i],join->right->projectionnode.attribute[0]);//这两条更新过滤条件中子查询对应部分
						strcpy(cur->filternode.attrioftable[i],join->right->projectionnode.newtable);//
						//printf("%s->%s",cur->filternode.attribute[i],cur->filternode.attrioftable[i]);
						join->joinnode.is_equijoin=true;
						join->joinnode.number = count;
						for(int j=0;j<count*2;j=j+2){
							strcpy(join->joinnode.attribute[j],tmp_2->filternode.attribute[tmp[j]-1]);
							strcpy(join->joinnode.attribute[j+1],tmp_2->filternode.attribute[tmp[j]]);
							strcpy(join->joinnode.attrioftable[j],tmp_2->filternode.attrioftable[tmp[j]-1]);
							strcpy(join->joinnode.attrioftable[j+1],tmp_2->filternode.attrioftable[tmp[j]]);
							//printf("%s->%s",join->joinnode.attrioftable[j],join->joinnode.attrioftable[j+1]);
							strcpy(join->joinnode.op[j/2],tmp_2->filternode.op[j/2]);
							//printf("%s",join->joinnode.op[j]);
						}
						if(tmp_2->filternode.number-count==0){//判断将相关过滤条件提取后是否还有其它的判断条件，没有就删除节点
							tmp_2->parents->left = tmp_2->left;
							tmp_2->left->parents =tmp_2->parents;
							//printf("OK");
						}	
						else{//将该节点中提前的属性删除
							int b[10] = {0};
							for(int j=0;j<count;j++){
								b[tmp[j]/2]=1;
								//printf("%d",tmp[j]);
							}
							Plannode* filter = (Plannode*)malloc(sizeof(Plannode));
							strcpy(filter->kind,"filter");
							int count1 = 0;
							for(int j=0;j<tmp_2->filternode.number*2;j=j+2){
								if(b[(j)/2]==1){
									continue;
								}
								else{
									strcpy(filter->filternode.attribute[count1],tmp_2->filternode.attribute[j]);
									strcpy(filter->filternode.attribute[count1+1],tmp_2->filternode.attribute[j+1]);
									strcpy(filter->filternode.attrioftable[count1],tmp_2->filternode.attrioftable[j]);
									strcpy(filter->filternode.attrioftable[count1+1],tmp_2->filternode.attrioftable[j+1]);
									strcpy(filter->filternode.op[count1/2],tmp_2->filternode.op[j/2]);
									//printf("%s %s %s %s",filter->filternode.attribute[count1],filter->filternode.attribute[count1+1],filter->filternode.attrioftable[count1],filter->filternode.attrioftable[count1+1]);
									count1 = count1+2;
								}
							}
							filter->filternode.number = count1/2;
							//printf("%d",filter->filternode.number);
							tmp_2->parents->left = filter;
							filter->parents = tmp_2->parents;
							filter->left = tmp_2->left;
							tmp_2->left->parents = filter;
							
						}
						break;
						
					}
				}
				
				break;
			}
			cur = cur->left;
		}
	}
	
}

void Down_filterandproject(Plannode headplan){
	Plannode* cur = headplan.left;
	char a[10];
	strcpy(a,"filter");
	Result result = ergodic(cur,a);
	
	for(int j=0;j<result.number;j++){
		cur = result.node[j];
		int b[10]={0};
		//printf("%d",cur->filternode.number);
		for(int i=0;i<cur->filternode.number*2;i=i+2){
				if(strcmp(cur->filternode.attrioftable[i],"NULL")!=0&&(strcmp(cur->filternode.attrioftable[i+1],"NULL")==0)){
					b[i/2]=1;
					
				}
		}		
		for(int i=0;i<cur->filternode.number*2;i=i+2){
			if(b[i/2]==1){
				//printf("1");
				strcpy(a,"table");
				Result result_1 =ergodic(cur,a);
				for(int m=0;m<result_1.number;m++){
					
					Plannode* tmp = result_1.node[m];
					
					if(strcmp(tmp->tablenode.tablename,cur->filternode.attrioftable[i])==0){
						
						if(tmp->parents!=cur){
							if(strcmp(tmp->parents->kind,"filter")!=0){//不存在filter节点
								Plannode* filter  = (Plannode*)malloc(sizeof(Plannode));
								int count1=0;

								strcpy(filter->filternode.attribute[count1],cur->filternode.attribute[i]);
								strcpy(filter->filternode.attribute[count1+1],cur->filternode.attribute[i+1]);
								strcpy(filter->filternode.attrioftable[count1],cur->filternode.attrioftable[i]);
								strcpy(filter->filternode.attrioftable[count1+1],cur->filternode.attrioftable[i+1]);
								strcpy(filter->filternode.op[count1/2],cur->filternode.op[i/2]);
								count1++;
								strcpy(filter->kind,"filter");
								filter->filternode.number=count1;
								tmp->parents->left =filter;
								filter->parents = tmp->parents;
								tmp->parents = filter;
								filter -> left = tmp;
								//printf("%s",filter->left->kind);
								//aaa(headplan.left);
								break;
							}
							else{								
								Plannode* filter = tmp->parents;
								int count2 = filter->filternode.number;
								int count1 = count2*2;
								strcpy(filter->filternode.attribute[count1],cur->filternode.attribute[i]);
								strcpy(filter->filternode.attribute[count1+1],cur->filternode.attribute[i+1]);
								strcpy(filter->filternode.attrioftable[count1],cur->filternode.attrioftable[i]);
								strcpy(filter->filternode.attrioftable[count1+1],cur->filternode.attrioftable[i+1]);
								strcpy(filter->filternode.op[count1/2],cur->filternode.op[i/2]);
								count2++;
								filter->filternode.number=count2;
								break;
							}
						}
						
					}
				}
			}
			else{
				strcpy(a,"join");
				Result result_1 =ergodic(cur,a);
				
				for(int m=0;m<result_1.number;m++){
					Plannode* tmp = result_1.node[m];
					//printf("%s x %s		",cur->filternode.attrioftable[i],cur->filternode.attrioftable[i+1]);
					char left[10];
					char right[10];
					if(strcmp(tmp->left->kind,"join")==0){
						strcpy(left,tmp->left->joinnode.newtable);
					}
					else if(strcmp(tmp->left->kind,"project")==0){
						strcpy(left,tmp->left->projectionnode.newtable);
					}
					else if(strcmp(tmp->left->kind,"table")==0){
						strcpy(left,tmp->left->tablenode.tablename);
					}
					if(strcmp(tmp->right->kind,"join")==0){
						strcpy(right,tmp->right->joinnode.newtable);
					}
					else if(strcmp(tmp->right->kind,"project")==0){
						strcpy(right,tmp->right->projectionnode.newtable);
					}
					else if(strcmp(tmp->right->kind,"table")==0){
						strcpy(right,tmp->right->tablenode.tablename);
					}
					
					if((strcmp(left,cur->filternode.attrioftable[i])==0&&strcmp(right,cur->filternode.attrioftable[i+1])==0)||(strcmp(left,cur->filternode.attrioftable[i+1])==0&&strcmp(right,cur->filternode.attrioftable[i])==0))
					{
						if(tmp->joinnode.is_equijoin==false){
							tmp->joinnode.is_equijoin=true;
							tmp->joinnode.number=1;
							int count1=0;
							strcpy(tmp->joinnode.attribute[count1],cur->filternode.attribute[i]);
							strcpy(tmp->joinnode.attribute[count1+1],cur->filternode.attribute[i+1]);
							strcpy(tmp->joinnode.attrioftable[count1],cur->filternode.attrioftable[i]);
							strcpy(tmp->joinnode.attrioftable[count1+1],cur->filternode.attrioftable[i+1]);
							strcpy(tmp->joinnode.op[count1/2],cur->filternode.op[i/2]);
						}
						else{
							int count1 = tmp->joinnode.number*2;
							strcpy(tmp->joinnode.attribute[count1],cur->filternode.attribute[i]);
							strcpy(tmp->joinnode.attribute[count1+1],cur->filternode.attribute[i+1]);
							strcpy(tmp->joinnode.attrioftable[count1],cur->filternode.attrioftable[i]);
							strcpy(tmp->joinnode.attrioftable[count1+1],cur->filternode.attrioftable[i+1]);
							strcpy(tmp->joinnode.op[count1/2],cur->filternode.op[i/2]);
							tmp->joinnode.number++;
						}
						break;
					}
				}
			}
		}
		//aaa(headplan.left);
		if(strcmp(cur->left->kind,"table")!=0){
			
			Plannode* tmp=cur->left;
			tmp->parents = cur->parents;
			cur->parents->left=tmp;
		}
		
		
	}
	

}
