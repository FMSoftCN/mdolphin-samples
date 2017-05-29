/*
 ** $Id$
 **
 ** URLAccessPolicy.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */

#include "URLAccessPolicy.h"
#include "URLStorage.h"

int SeqURLAccessPolicy::nextIndex(URLStorage & storage) 
{
    int pos = storage.curPos() + 1;
    return ((pos >  storage.size()) ? -1 : pos); 
}

int CycURLAccessPolicy::nextIndex(URLStorage & storage) 
{
    return (storage.curPos() + 1)% storage.size();
}

int RandomURLAccessPolicy::nextIndex(URLStorage &storage) 
{
    int ret;
    size_t size = storage.size();
    ret = size * (rand() / (RAND_MAX + 1.0));
    return ret;
}

