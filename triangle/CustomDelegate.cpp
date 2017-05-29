/*
 ** $Id$
 **
 ** IMDWebCustomDelegate.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-07-13 
 */


#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mdolphin/mdolphin.h>

#include <mdolphin/IMDWebView.h>
#include <mdolphin/IMDWebFrameLoadDelegate.h>
#include "CustomDelegate.h"

//define the common struct of this delegate
#ifdef __cplusplus


const char* CustomDelegate::userArget(IMDWebView*, const char* url)
{
    return NULL;
}

void CustomDelegate::beforeDrawViewport(IMDWebView* view, HDC windowDC,
                                 HDC memDC, const RECT* prcClient)
{
    return;
}

void CustomDelegate::afterDrawViewport(IMDWebView* view, HDC windowDC, HDC memDC, const RECT *prcClient)
{
}

void  CustomDelegate::linkURL(const char *url) 
{
    if(!url)
        return;

    if(m_depository->canAddDynamic())
        m_depository->append(new URLItem(0, url));
}
#endif

