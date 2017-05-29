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
#include "Triangle.h"
#include "TriangleLogging.h"
#include "TriangleClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/resource.h>

#include "fifo.h"

#define IDC_MDOLPHIN 101

static const char* homeURL = NULL; // "http://www.minigui.com/";
static const char* corePattern = "core-%p-%t-%s";
//FIXME: linux kernel vesion > 2.6.X
static const char* coreKernelFile = "/proc/sys/kernel/core_pattern";


class TriangleClientImpl : public TriangleClient 
{
public:
    TriangleClientImpl();
public:
    virtual void willLoadURL(const char* url);

private:
    bool m_haveDaemon;
};

TriangleClientImpl::TriangleClientImpl()
{
  m_haveDaemon = existFIFO();
}

void TriangleClientImpl::willLoadURL(const char* url)
{
    if(m_haveDaemon)
        sendClearMessage();
}

void dumpInfo(int signo)
{
	char *sig ;
	if(signo == SIGSEGV)
		sig = "SIGEGV";
	if(signo == SIGTERM)
		sig = "SIGTERM";
	if(signo == SIGINT)
		sig = "SIGINT, may be ctrl+c";

    Triangle * triangle = Triangle::triangleInstance();
	triangle->log()->error("program error when run , error is [%s]!\n", sig);

    triangle->finish();
	exit(0);
}

void registSignal()
{
	signal(SIGSEGV, dumpInfo);
	signal(SIGTERM, dumpInfo);
	signal(SIGINT, dumpInfo);
}

int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_DESTROY:
        DestroyAllControls (hWnd);
        return 0;
    case MSG_CLOSE:
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
    }
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int initCoreDumpSetupInfo(const char* etcFile) 
{
    //coredump
    char buff[51];
    int ret;
    int coredumpEnable = 1; 
    if (GetIntValueFromEtcFile(etcFile, "coredump", "enable", &coredumpEnable) !=  ETC_OK) 
        coredumpEnable = 1;  //default enable coredump

    if (!coredumpEnable)
        return 0;

    //ulimit -c unlimited
    struct rlimit coredumpLimit;
    memset(&coredumpLimit, 0, sizeof(struct rlimit));
    coredumpLimit.rlim_cur = RLIM_INFINITY;
    coredumpLimit.rlim_max = RLIM_INFINITY;
    ret = setrlimit(RLIMIT_CORE, &coredumpLimit);
    if (ret) {
        perror("setrlimit");
    }

    bzero(buff, 51);
    if (GetValueFromEtcFile (etcFile, "coredump", "coredir", buff, 50) !=  ETC_OK) 
        bzero(buff, 51);

    char *corePath = NULL;
    
    const char *mkdir = "mkdir -p ";
    if (strlen(buff) > 0)  {
       //create dir 
       char *cmd = (char *)malloc(strlen(mkdir) + strlen(buff) + 1);
       strcpy(cmd, mkdir);
       strcat(cmd, buff);
       ret = system(cmd);
       if (ret == -1 || ret == 127) {
           perror(cmd);
           free(cmd);
           return -1;
       }
       free(cmd);
    }

    int len = strlen(corePattern) + strlen("/") +  strlen(buff) + 1;
    corePath = (char *)malloc(len);
    bzero(corePath, len);
    strcpy(corePath, buff);
    strcat(corePath, "/");
    strcat(corePath, corePattern);

    int fd = open(coreKernelFile, O_RDWR, 0);
    if (fd <= 0 ) {
        free(corePath);
        fprintf(stderr, "Can't Open File %s, please logging as root\n", coreKernelFile);
        perror(coreKernelFile);
        return -1;
    }
    write(fd, corePath, strlen(corePath));
    close(fd);
    free(corePath);

    return 0;

}

int MiniGUIMain (int args, const char* argv[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    
    if(argv[1])
        homeURL = argv[1];
    
	registSignal();
    RECT rc;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mDolphin" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_CAPTION | WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mDolphin";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(IDC_ARROW);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MDolphinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = RECTW(g_rcScr);
    CreateInfo.by = RECTH(g_rcScr);
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    HWND hMainWnd;
    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    unsigned int intervalMaxInMin = 1;
    const char* etcFile = Triangle::etcFile();
    if (GetIntValueFromEtcFile (etcFile, "global", "intervalMaxTime", (int *)&intervalMaxInMin) !=  ETC_OK) {
        fprintf(stderr, "Triangle: Get intervalTime failed !\n");
        intervalMaxInMin = 5;  //default  5 min
    }

    // set coredump info
    initCoreDumpSetupInfo(etcFile);

    sendSetIntervalMessage(intervalMaxInMin);

    GetClientRect(hMainWnd, &rc);

    //Triangle *triangle = new Triangle();
    Triangle * triangle = Triangle::triangleInstance();
    TriangleClient *client = new TriangleClientImpl;
    triangle->init(rc, IDC_MDOLPHIN, hMainWnd);
    triangle->setClient(client); 
    triangle->start();
    if (homeURL)
        triangle->loadURL(homeURL);

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    //triangle->finish();
    MainWindowThreadCleanup (hMainWnd);

    return 0;
}
