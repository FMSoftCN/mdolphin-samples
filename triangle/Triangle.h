/*
 ** $Id$
 **
 ** Triangle.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-10 
 */
#ifndef Triangle_h
#define Triangle_h

#include "TriangleClient.h"

class IMDWebView;
class TriangleLogging;
class URLStorage;
class IMDWebUIDelegate;
class IMDWebCustomDelegate;
class IMDWebFrameLoadDelegate;

class Triangle {
public:
    bool init(RECT rc, int id, HWND hParent); //read cfg file
    IMDWebView* view() {return m_view; }; 
    bool start();
    void finish();
    URLStorage* storage() {return m_storage;}
    TriangleLogging* log() {return m_log;}
    void loadURL(const char* url);
    TriangleClient* client() { return m_client;}
    void setClient(TriangleClient *client) { m_client = client;}
    

#if 0
    virtual void setUIDelegate(IMDWebUIDelegate* d);
    virtual IMDWebUIDelegate*  uiDelegate();
    virtual void setCustomDelegate(IMDWebCustomDelegate* d);
    virtual IMDWebCustomDelegate* customDelegate();
    virtual void setFrameLoadDelegate(IMDWebFrameLoadDelegate* d);
    virtual IMDWebFrameLoadDelegate* frameLoadDelegate();
#endif
static Triangle* triangleInstance(); 
static const char* etcFile();
static BOOL timerFired(HWND, int, DWORD);
protected:
    Triangle();
    ~Triangle();

private:
static bool searchEtcFile(char *etcFile);
    IMDWebView* m_view;
    URLStorage* m_storage;
    TriangleLogging *m_log;
    TriangleClient* m_client;
    //command
};


#endif
