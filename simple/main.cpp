/*
 ** $Id: main.c 258 2010-08-06 05:27:43Z xwyan $
 **
 ** mdhello.c:  first test example.
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd.
 **
 ** All rights reserved by Feynman Software
 **
 ** Current maintainer: lijiangwei
 **
 ** Create date: 2010-06-07
 */

/* MiniGUI header file.*/
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

/* mDolphin header file.*/
#include "mdolphin/mdolphin.h"

#define IDC_MDOLPHIN 101

static const char * home_url = "http://test.szddzn.com/";

void set_callback_info(HWND hWnd);

HWND mdolphin_hwnd;

int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE: {
        RECT rc;
        GetClientRect(hWnd, &rc);
        /* Create a mDolphin control class instance. */
        mdolphin_hwnd = CreateWindow (MDOLPHIN_CTRL,
                                           "",
                                           WS_VISIBLE | WS_CHILD,
                                           IDC_MDOLPHIN,
                                           0, 0, RECTW(rc), RECTH(rc), hWnd, 0);
        set_callback_info(mdolphin_hwnd);
		SETUP_INFO setup_info;
		mdolphin_fetch_setup(mdolphin_hwnd, &setup_info);

		mdolphin_import_setup(mdolphin_hwnd, &setup_info);
        setup_info.enable_javascript = true;
        
            /* Open a specified URL. */
        SetFocus(mdolphin_hwnd);
        mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
    }
    break;
#if 1
    case MSG_KEYUP: 
    {
        //printf("MSG_KEYUP: wParam = %d\n", wParam); 
        if (wParam == SCANCODE_ESCAPE) {
            mdolphin_navigate(mdolphin_hwnd, NAV_BACKWARD, NULL, FALSE);
        }
        else if (wParam == SCANCODE_HOME) {
            mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
            //mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, NULL, FALSE);
        }
    }
    break;
#endif
    case MSG_CSIZECHANGED:
    {
        RECT newRect;
        SetRect(&newRect, 0, 0, wParam, lParam);
        SendMessage(GetDlgItem(hWnd, IDC_MDOLPHIN), 
                MSG_SIZECHANGED, (DWORD)&newRect, (DWORD)&newRect);
        return 0;
    }

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

/*
extern "C" {
extern char* kjs_dtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve);
}

void test_dtoa(double d) 
{
    int decimalPoint;
    int sign;
    char *result = kjs_dtoa(d, 0, 0, &decimalPoint, &sign, NULL);
    size_t length = strlen(result);

    printf("result=%s, sign=%d, length=%d\n", result, sign, length);
}
*/

int MiniGUIMain (int args, const char* argv[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;

    if (argv[1])
        home_url = argv[1];
/*
        test_dtoa(1.0);
        test_dtoa(-1.0);
        test_dtoa(100);
        test_dtoa(65536);
        test_dtoa(100000);
        */
    mdolphin_register_minigui_ctrlclass();
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mDolphin" , 0 , 0);
#endif

    CreateInfo.dwStyle = WS_VISIBLE ;
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

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    mdolphin_unregister_minigui_ctrlclass();
    MainWindowThreadCleanup (hMainWnd);

    return 0;
}
