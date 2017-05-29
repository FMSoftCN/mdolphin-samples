/*
 ** $Id$
 **
 ** URLAccessPolicy.h:  
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */

#ifndef URLAccessPolicy_h
#define URLAccessPolicy_h

//#include "URLStorage.h" 
class URLStorage;
enum URLAccessType { SeqAccess, CycAccess, RandomAccess}; 

class URLAccessPolicy {
public:
    URLAccessPolicy() {};
    ~URLAccessPolicy() {};
public:
    virtual int nextIndex(URLStorage &) = 0;
};

class SeqURLAccessPolicy : public URLAccessPolicy {
public:
    SeqURLAccessPolicy() {};
    int nextIndex(URLStorage &);
};

class CycURLAccessPolicy : public URLAccessPolicy {
public:
    CycURLAccessPolicy() {};
    int nextIndex(URLStorage &);
};


class RandomURLAccessPolicy : public URLAccessPolicy {
public:
    RandomURLAccessPolicy() {};
    int nextIndex(URLStorage &);
};




#endif
