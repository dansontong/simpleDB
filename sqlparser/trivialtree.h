//
// Created by theGloves on 2019/11/18.
//

#ifndef TREE_TRIVIALTREE_H
#define TREE_TRIVIALTREE_H
#include<iostream>

class trivialtree {
private:
    std::string datas;
    trivialtree *childrear, *parent; // childrear记录小儿子
    trivialtree *children, *brothers; // children记录大儿子， brother记录自己的下一个弟弟

public:
    trivialtree();

    ~trivialtree();

    trivialtree(std::string d);

    std::string data();
    void setdata(char *data);
    void setParent(trivialtree *p);
    trivialtree* getParent();
    trivialtree* getChild();            // 返回小儿子
    void insertChild(trivialtree *child);

    void insertBrother(trivialtree *bro);

    void print(int dept);
};

#endif //TREE_TRIVIALTREE_H
