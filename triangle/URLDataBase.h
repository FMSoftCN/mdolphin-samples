/*
 ** $Id$
 **
 ** URLDataBase.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */
#ifndef URLDataBase_h
#define URLDataBase_h

#include "URLDepository.h"
#include "sqlite3.h"

class URLDataBase : public URLDepository {
public:
    bool init(const char* etcFile);
    void finish();
    bool isEmpty();
    size_t size();
    
    const URLItem* findAt(int index);
    bool append(const URLItem*);
    bool canAddDynamic() {return true;};

static void setURLItemSelected(const char* url, bool isVisited);
static URLItem URLItemSelected;
private:
    sqlite3* m_pdb; 
    bool createTableIfNeed(const char* tableName);

    // for init url list
    int m_size;
    int m_vectorCurPos;
    bool m_vectorListFinished;
    string m_fileName;
    vector<URLItem> m_URLItems;

};

#endif

