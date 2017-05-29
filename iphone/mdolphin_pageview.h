/*
** $Id: mdolphin_pageview.h,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_pageview.h: manage browser windows.
**
** Copyright (C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date:2008-04-15
*/

#ifndef mdolphin_pageview_h
#define mdolphin_pageview_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_PAGES  4

typedef struct _PAGE_DATA {
    char* status;
    char* title;
    char* url;
    HWND  hwnd;
    PBITMAP pBitmap;

	int mode; /* 1:GloablView 2:PageView; 3:NormalView */

    int back;
    int forward;
    int progress;
    int encoding;

    BOOL load;
    BOOL wasUserGesture; /* URL is guestrued by user */
    struct _PAGE_DATA* next;
    struct _PAGE_DATA* prev;
}PAGE_DATA;

typedef struct _PAGE_INFO {
    PAGE_DATA* active;
    PAGE_DATA* head;
    int page_count;
    int status;
}PAGE_INFO;


enum {
    MOVETO_FIRST = 0,
    MOVETO_PREV,
    MOVETO_CURRENT,
    MOVETO_NEXT,
    MOVETO_LAST,
};

enum {
    WNM_ADDPAGE = MSG_USER+1,
    WNM_REMOVEPAGE,
    WNM_GETPAGE,
    WNM_GETPAGECOUNT,
    WNM_GETPAGEINFO,
    WNM_SETACTIVEPAGE,
    WNM_GETACTIVEPAGE,
};

#define MDOLPHIN_PAGEVIEW   ("md_pageview")

BOOL RegisterPageViewControl(void);
void UnregisterPageViewControl(void);

HWND CreatePageView(HWND hParent, DWORD style, const RECT* rc);
HWND CreateMdolphinWindow(const char * url, DWORD style);

void SetActiveWindowForPageView(HWND hBrowser);
HWND GetActiveWindowInPageView();

BOOL GetBitmapFromWindow(HWND hWnd);
HWND my_create_mdolphin_window(const char* url, DWORD style, int x, int y, int w, int h);
void my_close_mdolphin_window(HWND hwnd);
void my_capture_window_image(HWND hwnd, HDC hdc, const RECT* prc);

PAGE_DATA* FindPage(HWND hWnd, int step);
void MovePage(HWND hwnd, int dir);
PAGE_DATA* GetPageData(HWND hwnd);
void MovePageInView(HWND hWnd, int dir);
int GetPageCount();
int GetPageIndex(PAGE_DATA* pd);

void SetActivePage(HWND hwnd);
PAGE_DATA* GetActivePage();

void DrawClosePageBtn(HDC hdc, int x, int y);

int GetViewMode(HWND hWnd);
void SetViewMode(HWND hWnd, int mode);
int SwitchViewMode(HWND hWnd, int from, int to);

PAGE_DATA* get_page_data(HWND hwnd);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

