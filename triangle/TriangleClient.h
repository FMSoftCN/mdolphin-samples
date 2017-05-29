/*
 ** $Id$
 **
 ** TriangleClient.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-16 
 */

#ifndef TriangleClient_H
#define TriangleClient_H

class TriangleClient {
public:
    virtual void willLoadURL(const char* url) = 0;
};
#endif
