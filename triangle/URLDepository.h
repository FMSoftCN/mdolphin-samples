/*
 ** $Id$
 **
 ** URLDepository.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */
#ifndef _URLDepository_h
#define  _URLDepository_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
using namespace std;

struct URLItem {
    URLItem():isVisited(false), url(NULL){}
    URLItem(bool visited, const char *str){
        isVisited = visited;
        if (str) {
            url = static_cast<char *>(malloc(strlen(str)+1));
            strcpy(url, str);
        }
    }

    URLItem(const URLItem &other) {
        isVisited = other.isVisited;
        url = static_cast<char *>(malloc(strlen(other.url)+1));
        strcpy(url, other.url);
    }
    ~URLItem(){free(url);}
    bool isVisited; 
    char* url;
};

class URLDepository {
public:
    virtual bool init(const char* etcFile) = 0;
    virtual void finish() = 0;
    virtual bool isEmpty() = 0;
    virtual size_t size() = 0;
    virtual bool canAddDynamic() {return false;};
    
    virtual const URLItem* findAt(int index) = 0;
    virtual bool append(const URLItem*) = 0;

private:
};


class URLMemDepository : public URLDepository {
public:
    URLMemDepository();

    bool init(const char* etcFile);
    void finish();
    bool isEmpty();
    size_t size();
    bool canAddDynamic() {return false;};
    
    const URLItem* findAt(int index);
    bool append(const URLItem*);
private:
    bool m_isAllReadMemory;
    int m_size;
    string m_fileName;

    // m_isAllReadMemory == true
    vector<URLItem> m_URLItems;

    // m_isAllReadMemory == false
    vector<int> m_FileAccessIndex;

};

#endif
