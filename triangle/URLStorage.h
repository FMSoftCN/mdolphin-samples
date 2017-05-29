/*
 ** $Id$
 **
 ** URLStorage.h: the URLStorage class
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */

#ifndef _URLStorage_h
#define _URLStorage_h

#include  "URLDepository.h"
#include  "URLAccessPolicy.h"
class URLDepository; 

class URLStorage {
public: 
    static URLDepository* createDepositoryInstance(const char* depoName);
    static URLAccessPolicy* createAccessPolicyInstance(URLAccessType accessType);
    static URLAccessPolicy* createAccessPolicyInstance(const char* type);

    URLStorage(char *depoName, URLAccessType accessType = SeqAccess);
    URLStorage(URLDepository*, URLAccessType accessType = SeqAccess);
    URLStorage();

    bool init(const char* etcFile);
    bool finish() {m_depository->finish();};

    const char *nextURL() {
        m_curPos = m_policy->nextIndex(*this);
        const URLItem *item = m_depository->findAt(m_curPos);
        if (item)
            return item->url;
        else 
            return NULL;
    };

    int size() {return m_depository->size();};
    int curPos(){ return m_curPos;};
    bool isEmpty();
    URLDepository* depository() {return m_depository;}
    URLAccessPolicy* policy() {return m_policy;}

private:
    URLDepository* m_depository;
    URLAccessPolicy* m_policy;

    int m_curPos;
};
#endif

