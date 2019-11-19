//
// Created by theGloves on 2019/11/18.
//

#include "trivialtree.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

trivialtree::trivialtree()
{

    this->brothers = NULL;
    this->children = NULL;

    this->childrear = NULL;
    this->parent = NULL;
}

trivialtree::trivialtree(string d)
{
    this->datas = d;
    this->brothers = NULL;
    this->children = NULL;

    this->childrear = NULL;
    this->parent = NULL;
}

trivialtree::~trivialtree() {}

string trivialtree::data()
{
    return this->datas;
}

void trivialtree::setParent(trivialtree *p)
{
    this->parent = p;
}

trivialtree *trivialtree::getParent()
{
    return this->parent;
}

void trivialtree::insertChild(trivialtree *child)
{
    child->setParent(this);
    if (this->childrear == NULL)
    {
        // 第一个儿子当然就是大儿子
        this->children = child;
    }
    else
    {
        // 小儿子有弟弟了
        this->childrear->insertBrother(child);
    }
    this->childrear = child;
}

void trivialtree::insertBrother(trivialtree *bro)
{
    if (this->brothers == NULL)
    {
        this->brothers = bro;
    }
    else
    {
        this->brothers->insertBrother(bro);
    }
}

void trivialtree::print(int dept)
{
    string prefix;
    for (int i = 0; i < dept; i++)
    {
        prefix.append("\t");
    }
    cout << prefix << datas << "\n";
    if (children)
    {
        cout << prefix << "child:" << endl;
        this->children->print(dept + 1);
    }
    if (brothers != NULL)
    {
        this->brothers->print(dept);
    }
}