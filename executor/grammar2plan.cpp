#include "executor.h"

// author theGloves
// 及其垃圾的转换!_!不想写了

/*--------------------------------------------------------------------
 * 全局变量
 *------------------------------------------------------------------*/
extern int count_tab;


/*--------------------------------------------------------------------
 * InitSelectNode
 * 
 * 初始化一个select节点
 *------------------------------------------------------------------*/
Selectnode *InitSelectNode(){
    Selectnode *node = (Selectnode *)malloc(sizeof(Selectnode));
    node->num_attri = 0;
    node->num_tab = 0;
    node->num_attri = 0 ;
    node->num_attri_tab = 0;
    node->num_op = 0;
    node->num_pro = 0;
    node->subselect = 1;

    return node;
}


/* hrc - 查询树 */
Plannode *make_join_node(int start_table, int end_table, Plannode *cur, Selectnode *select1)
{ //生成table节点或join节点，若只有一张表就是一个table节点，多于一张表，就两辆结成一个join节点
    for (int j = 0; j < end_table - start_table; j++)
    {
        if (j % 2 != 0)
        {
            ////printf("1");
            Plannode *table = (Plannode *)malloc(sizeof(Plannode));
            strcpy(table->kind, "table");
            strcpy(table->tablenode.tablename, select1->tablename[start_table + j]);
            Plannode *join = (Plannode *)malloc(sizeof(Plannode));
            strcpy(join->kind, "join");
            char str[2];
            str[0] = count_tab;
            strcpy(&str[1], "");
            count_tab++;
            char str_1[20] = "tmptable";
            strcat(str_1, str);
            //printf("%s\n",str_1);
            strcpy(join->joinnode.newtable, str_1);
            cur->parents = join;
            join->left = cur;
            join->right = table;
            table->parents = join;
            cur = join;
        }
        else
        {
            Plannode *table = (Plannode *)malloc(sizeof(Plannode));
            strcpy(table->kind, "table");
            strcpy(table->tablenode.tablename, select1->tablename[start_table + j]);
            cur->parents = table;
            table->left = cur;
            cur = table;
        }
    }
    Plannode *result = cur;
    return result;
}

Plannode *make_filter_node(int start_op, int end_op, int start_attri, int end_attri, Plannode *cur, Selectnode *select1)
{ //where节点，选择过滤
    bool flag = false;
    Plannode *filter = (Plannode *)malloc(sizeof(Plannode));
    strcpy(filter->kind, "filter");
    for (int j = 0; j < end_op - start_op; j++)
    {
        strcpy(filter->filternode.op[j], select1->op[start_op + j]);
        //printf("%s\n",filter->filternode.op[j]);
    }
    for (int j = 0; j < end_attri - start_attri; j++)
    {
        strcpy(filter->filternode.attribute[j], select1->attribute[start_attri + j]);
        if (strcmp(filter->filternode.attribute[j], "sub"))
            strcpy(filter->filternode.attrioftable[j], select1->attrioftable[start_attri + j]);
        //printf("%s\n",filter->filternode.attribute[j]);
    }
    cur->parents = filter;
    filter->left = cur;
    filter->filternode.number = end_op - start_op;
    cur = filter;
    return cur;
}

Plannode *make_project_node(int start_pro, int end_pro, Plannode *cur, Selectnode *select1)
{ //生成投影节点
    Plannode *project = (Plannode *)malloc(sizeof(Plannode));
    strcpy(project->kind, "project");
    for (int j = 0; j < end_pro - start_pro; j++)
    {
        //printf("%d",j);
        strcpy(project->projectionnode.attribute[j], select1->projectionattribute[start_pro + j]);
        //printf("%s\n",project->projectionnode.attribute[j]);
    }
    cur->parents = project;
    project->left = cur;
    project->projectionnode.number = end_pro - start_pro;
    Plannode *test = project->left->left;
    if (strcmp(test->kind, "join") == 0)
    {
        strcpy(project->projectionnode.tablename, test->joinnode.newtable);
    }
    else
    {
        strcpy(project->projectionnode.tablename, test->tablenode.tablename);
    }
    //printf("%s\n",project->projectionnode.tablename);
    return project;
}

Plannode* plan_init(Selectnode *select1)
{
    Plannode *headplan = (Plannode *)malloc(sizeof(Plannode));

    int countnode = 0;
    Plannode *cur = headplan;
    if (select1->subselect > 0)
    {
        int i = select1->subselect;
        int end_table = select1->num_tab;
        int start_table = select1->subtablename[i];
        int end_attri = select1->num_attri;
        int start_attri = select1->subattribute[i];
        int end_attrioftable = select1->num_attri_tab;
        int start_attrioftable = select1->subattributetable[i];
        int end_op = select1->num_op;
        int start_op = select1->subop[i];
        int start_pro = select1->sub_pro[i];
        int end_pro = select1->num_pro;
        while (i >= 0)
        {
            cur = make_join_node(start_table, end_table, cur, select1);
            cur = make_filter_node(start_op, end_op, start_attri, end_attri, cur, select1);
            cur = make_project_node(start_pro, end_pro, cur, select1);
            end_table = start_table;
            end_op = start_op;
            end_attri = start_attri;
            end_pro = start_pro;
            i--;
            start_table = select1->subtablename[i];
            start_op = select1->subop[i];
            start_attri = select1->subattribute[i];
            start_pro = select1->sub_pro[i];
        }
        headplan->left = cur;
    }
    return headplan;
}
/* hrc end */


/*--------------------------------------------------------------------
 * grammar2plan
 * 
 * 语法树变为查询优化想要的结构
 *------------------------------------------------------------------*/
Selectnode* select2plan(trivialtree * root){
    Selectnode* node = InitSelectNode();
    trivialtree *cur = root->Child(), *tmp;

    node->num_attri = 0;
    node->num_tab = 0;
    node->num_attri = 0 ;
    node->num_attri_tab = 0;
    node->num_op = 0;
    node->num_pro = 0;
    node->subselect = 1;
    
    node->subtablename[node->num_tab] = 0;
    node->subattribute[node->num_tab] = 0;
    node->subattributetable[node->num_tab] = 0;
    node->sub_pro[node->num_tab] = 0;
    node->subop[node->num_tab] = 0;

    // 遍历语法树，遍历树根的所有孩子节点，cur初始值为树根的第一个孩子
    while (cur){
        printf("current data: %s\n", cur->data().c_str());
        if (strcmp(cur->data().c_str(), "select") == 0){
            // 处理投影
            printf("处理select\n");
            tmp = cur->Child();
            printf("[debug] %s\n", tmp->data().c_str());
            if (strcmp(tmp->data().c_str(), "<SEL_LIST>") != 0){
                printf("语法树格式错误！没有<SEL_LIST>\n");
                goto END;
            }
            while(tmp){
                tmp = tmp->Child();
                strcpy(node->projectionattribute[node->num_pro++], tmp->data().c_str());
                tmp = tmp->Brother();
            }
            printf("投影处理完毕，一共有: %d\n", node->num_pro);
        }else if (strcmp(cur->data().c_str(), "from") == 0){
            // 处理from
            printf("处理from\n");
            tmp = cur->Child();
            printf("[debug] %s\n", tmp->data().c_str());
            if (strcmp(tmp->data().c_str(), "<VAL_LIST>") != 0){
                printf("语法树格式错误！没有<VAL_LIST>\n");
                goto END;
            }
            while(tmp){
                tmp = tmp->Child();
                strcpy(node->tablename[node->num_tab++], tmp->data().c_str());
                tmp = tmp->Brother();
            }
            printf("from处理完毕，一共有: %d\n", node->num_tab);
        }else if (strcmp(cur->data().c_str(), "<WHERE>") == 0){
            // 处理where字句
            printf("处理where\n");
            tmp = cur->Child();
            // 匹配WHER
            if (strcmp(tmp->data().c_str(), "where") != 0){
                printf("语法树格式错误！没有where\n");
                goto END;
            }
            // 切到<condition_list>起点
            tmp = tmp->Brother();

            while(tmp){
                // 切到<condition_list>
                if (strcmp(tmp->data().c_str(), "<CONDITION_LIST>") != 0){
                    printf("语法树格式错误！没有<CONDITION_LIST>\n");
                    goto END;
                }
                trivialtree *condition = tmp->Child(), *condition_tuple;
                if (strcmp(condition->data().c_str(), "<CONDITION>") != 0){
                    printf("语法树格式错误！没有<CONDITION>\n");
                    goto END;
                }
                condition_tuple = condition->Child();
                printf("处理三元组\n");
                // 处理condition三元组
                for (int i=0;i<3;i++){
                    printf("condition data: %s\n", condition_tuple->data().c_str());
                    if (i==1){
                        // 处理op
                        strcpy(node->op[node->num_op++], condition_tuple->data().c_str());
                    }else{
                        // TODO 低级处理，通过前缀区分属性&数值
                        if (condition_tuple->data().find("DATA:")==0){
                            strcpy(node->attribute[node->num_attri++], condition_tuple->data().c_str()+5);
                            strcpy(node->attrioftable[node->num_attri_tab++], "NULL");
                        }else if (condition_tuple->data().find("ATTR:")==0){
                            strcpy(node->attribute[node->num_attri++], condition_tuple->data().c_str()+5);
                            // TODO 查询数据字典，判断属性是否存在
                            char *strtmp = (char *)malloc(sizeof(char)* (1+strlen(condition_tuple->data().c_str())));
                            memcpy(strtmp, condition_tuple->data().c_str(), 1+strlen(condition_tuple->data().c_str()));
                            char * res = strtok(strtmp, ".");
                            printf("attr: %s; res: %s\n", strtmp, res);
                            if (res == NULL){
                                strcpy(node->attrioftable[node->num_attri_tab++], "NULL");
                            }else{
                                strcpy(node->attrioftable[node->num_attri_tab++], res+5);
                            }
                        }
                    }
                    condition_tuple = condition_tuple->Brother();
                }
                tmp = condition->Brother();
                if (tmp){
                    tmp = tmp->Brother();
                }
            }
            printf("where处理完毕，一共有: %d\t%d\t%d\n", node->num_op, node->num_attri_tab, node->num_attri) ;
        }else{
            printf("未知的语句: %s\n", cur->data().c_str());
        }
        END:
        cur  = cur->Brother();
    }
    return node;
}


/*--------------------------------------------------------------------
 * create2plan
 * 
 * 将crate语句转化为plan
 *------------------------------------------------------------------*/
PerformPlan *create2plan(trivialtree * root)
{
    PerformPlan *plan = (PerformPlan *) malloc(sizeof(PerformPlan));
    plan->sql_type = CREATE_SQL;
    plan->val_index = 0;
    printf("create 2 plan start.\n");

    // 切换到create
    trivialtree *cur = root->Child(), *tuple;
    if (cur->data().find("create")==root->data().npos){
        printf("create 语法树格式错误\n");
        return NULL;
    }
    cur = cur->Brother();

    // 切换到table
    if (cur->data().find("table")==root->data().npos){
        printf("table 语法树格式错误\n");
        return NULL;
    }

    cur = cur->Brother();
    plan->table_name = strdup(cur->data().c_str());

    // 切换到<TUPLE_LIST>
    cur = cur->Brother();
    while (cur){
        if (strcmp(cur->data().c_str(), "<TUPLE_LIST>") == 0){
            cur = cur->Child();
        }
        if (strcmp(cur->data().c_str(), "<TUPLE>") == 0){            
            tuple = cur->Child();
            // plan->val_list.push_back(strdup(tuple->data().c_str())) ;
            plan->val_list[plan->val_index] = strdup(tuple->data().c_str());
            tuple = tuple->Brother();
            // plan->val_type.push_back(strdup(tuple->data().c_str()));
            plan->val_type[plan->val_index++] = strdup(tuple->data().c_str());
        }
        cur = cur->Brother();
    }
    
    printf("create 2 plan finished.\n");
    return plan;
}

/*--------------------------------------------------------------------
 * InitSelectNode
 * 
 * 初始化一个select节点
 *------------------------------------------------------------------*/
PerformPlan *drop2plan(trivialtree * root)
{
    return NULL;
}

/*--------------------------------------------------------------------
 * InitSelectNode
 * 
 * 初始化一个select节点
 *------------------------------------------------------------------*/
PerformPlan *delete2plan(trivialtree * root)
{
    return NULL;
}

/*--------------------------------------------------------------------
 * InitSelectNode
 * 
 * 初始化一个select节点
 *------------------------------------------------------------------*/
PerformPlan *insert2plan(trivialtree * root)
{    
    PerformPlan *plan = (PerformPlan *) malloc(sizeof(PerformPlan));
    plan->sql_type = INSERT_SQL;
    plan->val_index = 0;
    printf("insert 2 plan start.\n");

    // 切换到insert into
    trivialtree *cur = root->Child();
    if (cur->data().find("insert into")==root->data().npos){
        printf("insert into 语法树格式错误\n");
        return NULL;
    }
    cur = cur->Brother();
    printf("%s\n", cur->data().c_str());
    plan->table_name = strdup(cur->data().c_str());
    
    // 切换到values
    cur = cur->Brother();

    // 切换到<DATA_LIST>
    cur = cur->Brother();
    while (cur){
        printf("current data: %s\n", cur->data().c_str());
        if (strcmp(cur->data().c_str(), "<DATA_LIST>") == 0){
            cur = cur->Child();
            // plan->val_list.push_back( strdup(cur->data().c_str()));
            plan->val_list[plan->val_index++] = const_cast<char*>(cur->data().c_str());

            cur = cur->Brother();
        }else{
            printf("语法树错误\n");
            free(plan);
            return NULL;
        }

    }
    
    printf("insert 2 plan finished.\n");
    return plan;
}
