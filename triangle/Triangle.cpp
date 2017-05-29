/*
 ** $Id$
 **
 ** Triangle.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-10 
 */

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

/* mDolphin header file.*/
#include <mdolphin/mdolphin.h>
#include <mdolphin/IMDWebView.h>
#include <mdolphin/IMDWebFrameLoadDelegate.h>
#include <mdolphin/IMDWebUIDelegate.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#include "Triangle.h"
#include "URLStorage.h"
#include "TriangleLogging.h"
#include "FrameLoadDelegate.h"
#include "CustomDelegate.h"
#include "UIDelegate.h"

#define TRIANGLECFG  "triangle.cfg"

static DWORD TIMERID = 0;

Triangle::Triangle() 
    :m_view(0)
    ,m_log(0)
{

}

Triangle::~Triangle() 
{
    delete m_log;
}

//How to get Triangle object in timerFired ?
//so using singletone mode 
Triangle* Triangle::triangleInstance()
{
    static Triangle triangleInstance;
    return &triangleInstance;
}

bool Triangle::init(RECT rc, int id, HWND hParent)
{
    m_view = CreateWebViewInstance(rc, id, hParent);

    //read cfg file 
    const char* etcFileName = Triangle::etcFile();
    if (!etcFileName) {
        fprintf(stderr, "Get triangle.cfg file failed\n");
        return false;
    }

    //input module
    m_storage = new URLStorage(); 
    m_storage->init(etcFileName);
    
    //output module
    m_log = TriangleLoggingFactory::createLoggingInstance(etcFileName);
    m_log->init(etcFileName);

    //set delegate
    FrameLoadDelegate *frameLoaderDeleaget = new FrameLoadDelegate(m_log);
    m_view->setFrameLoadDelegate(frameLoaderDeleaget);
    m_view->setUIDelegate(new UIDelegate(m_log));
    m_view->setCustomDelegate(new CustomDelegate(m_storage->depository()));
}


bool Triangle::start()
{
    unsigned int timeID = 1;
    while (IsTimerInstalled(HWND_NULL, timeID))
        timeID++;

    TIMERID = timeID;
    unsigned int intervalInMS = 30;
#if 1 
    if (GetIntValueFromEtcFile (Triangle::etcFile(), "global", "intervalTime", (int *)&intervalInMS) !=  ETC_OK) {
        fprintf(stderr, "Triangle: Get intervalTime failed !\n");
        intervalInMS = 30;  //default 30 senconds
    }
#endif

    intervalInMS *= 1000;

    HWND hWnd = NULL;
    if (m_view) 
        hWnd = m_view->viewWindow();
    else
        return false;

    //FIXME: run once firstly
    Triangle::timerFired(NULL, 0, 0);
    if (!SetTimerEx(hWnd, TIMERID, intervalInMS/10.0, Triangle::timerFired)) {
        fprintf(stderr, "Triangle: SetTimerEx failed !\n");
        return false;
    }

    return true;
}

void Triangle::loadURL(const char* url) 
{
    if(m_view)
        m_view->loadUrl(url);

    if (m_client)
        m_client->willLoadURL(url);
}

void Triangle::finish() 
{
    m_storage->finish();
    m_log->finish();
}

BOOL Triangle::timerFired(HWND hWnd, int, DWORD) 
{
    const char *url = NULL;

    //FIXME: how to get Triangle object?
    //so using sinletone mode
#if 0
    if (view)
        url = view->storage()->nextURL();
#endif
    Triangle* triangle = Triangle::triangleInstance();
    if (triangle)
        url =  triangle->storage()->nextURL();

    if (!url) {
        printf("test completed\n");  
        triangle->finish();
        return FALSE;
    }

    triangle->loadURL(url);

    triangle->log()->plainDebug("\n");
    triangle->log()->plainDebug("\n");
    triangle->log()->debug("****************************************************************\n");
    triangle->log()->debug("******  Start Open URL:%s  ******\n", url);
    triangle->log()->meminfo();
    return TRUE;
}

const char* Triangle::etcFile()
{
    static char *file = NULL;
    if (file == NULL) {
        file =  (char *)malloc(MAX_PATH);
        searchEtcFile(file);
    }

    return file;
}

bool Triangle::searchEtcFile (char *etcFile)
{
     char tmpEtcFile [MAX_PATH + 1];
     char buff [51];
     struct passwd *pwd;

     getcwd (tmpEtcFile, MAX_PATH);
     strcat (tmpEtcFile, "/");
     strcat (tmpEtcFile, TRIANGLECFG);
     if (GetValueFromEtcFile (tmpEtcFile, "global", "input", buff, 50) ==  ETC_OK) {
         strcpy (etcFile, tmpEtcFile);
         return true;
     }

     if ((pwd = getpwuid (geteuid ())) != NULL) {
         strcpy (tmpEtcFile, pwd->pw_dir);
         if (tmpEtcFile [strlen (tmpEtcFile) - 1] != '/')
             strcat (tmpEtcFile, "/");
         strcat (tmpEtcFile, ".");
         strcat (tmpEtcFile, TRIANGLECFG);
         if (GetValueFromEtcFile (tmpEtcFile, "global", "input", buff, 50) ==  ETC_OK) {
             strcpy (etcFile, tmpEtcFile);
             return true;
         }
     }

     strcpy (tmpEtcFile, "/etc/"TRIANGLECFG);
     if (GetValueFromEtcFile (tmpEtcFile, "global", "input", buff, 50) ==  ETC_OK) {
         strcpy (etcFile, tmpEtcFile);
         return true;
     }

     return false;
    
}

