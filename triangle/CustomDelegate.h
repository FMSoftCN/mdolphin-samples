/*
 ** $Id$
 **
 ** IMDWebCustomDelegate.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-07-13 
 */

#ifndef CustomDelegate_H
#define CustomDelegate_H

#include <mdolphin/IMDWebCustomDelegate.h>
#include "URLDepository.h"


//define the common struct of this delegate
#ifdef __cplusplus
class IMDWebView;

class CustomDelegate : public IMDWebCustomDelegate {
public:
    virtual const char* userArget(IMDWebView*, const char* url);
    virtual void beforeDrawViewport(IMDWebView*, HDC windowdDC, HDC memDC, const RECT* prcClient);
    virtual void afterDrawViewport(IMDWebView*, HDC windowDC, HDC memDC, const RECT *prcClient);
    virtual void  linkURL(const char *url);

public:
    CustomDelegate(URLDepository* depo){ m_depository = depo;};
    ~CustomDelegate(){};

private:
   URLDepository* m_depository; 

};

#endif

#endif
