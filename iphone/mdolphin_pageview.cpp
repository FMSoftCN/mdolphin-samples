/*
** $Id: mdolphin_pageview.cpp,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_pageview.cpp: manager browser windows. 
**
** Copyright (C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date:2008-04-15
*/
#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
#include "mdolphin_pageview.h"
#include "mdolphin_transform.h"
#include "mdolphin_skin.h"

extern void draw_histroy_status(size_t bcount, size_t fcount);
extern void draw_loading_status(BOOL load, BOOL refresh);
extern void draw_addr_btn_status(BOOL enable);
extern void draw_zoom_btn_status(BOOL enable);

static HWND add_mdolphin_to_pageview(HWND hwnd, const char* url);
static void remove_mdolphin_from_pageview(HWND hwnd);

static PAGE_DATA* create_page(HWND hwnd, const char* url);
static void destroy_page(PAGE_DATA* pd);
static PAGE_DATA* find_page(HWND hwnd, int step);
static PAGE_DATA* get_page(PAGE_INFO* pi, HWND hwnd);
static int get_page_count();
static int get_page_index(PAGE_DATA* pd);

static PAGE_DATA* get_active_page();
static void set_active_page(HWND hwnd);

static void set_view_mode(HWND hwnd, int mode);
static int get_view_mode(HWND hwnd);
static int switch_view_mode(HWND hWnd, int from, int to);

extern HWND hMainWnd;
extern HWND hPageView;

HWND CreatePageView(HWND hParent, DWORD style, const RECT* prc)
{
    HWND hwnd = CreateWindowEx(MDOLPHIN_PAGEVIEW, "", style, 0, IDC_PAGEVIEW,
        prc->left, prc->top, RECTWP(prc), RECTHP(prc), hParent, 0);
    if (hwnd == HWND_INVALID)
        return HWND_NULL;

	return hwnd;
}

HWND CreateMdolphinWindow(const char* url, DWORD style)
{
    HWND hWnd = my_create_mdolphin_window(url, style, 0, 0, -1, -1);
    ShowWindow(hWnd, SW_SHOW);
    return hWnd;
}

HWND my_create_mdolphin_window(const char* url, DWORD style, int x, int y, int w, int h)
{
    if (GetPageCount() >= MAX_PAGES)
        return HWND_NULL;
 
    RECT rc;
    GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
    w= RECTW(rc);
    h= RECTH(rc);
fprintf(stderr, "(%d,%d)-(%d,%d)\n", rc.left, rc.top, w, h);
    HWND hwnd = CreateWindow(MDOLPHIN_CTRL,
            "", WS_CHILD, IDC_MDOLPHIN,
            x, y, w, h, hMainWnd, 0);
    if (hwnd == HWND_INVALID)
        return HWND_NULL;

    add_mdolphin_to_pageview(hwnd, url);

    SetActiveWindowForPageView(hwnd);

    mdolphin_set_rendering_mode(hwnd, MD_SCREENVIEW_MODE, WIN_WIDTH);

    mdolphin_set_scale_factor(hwnd, 0.4, 0.4);

    mdolphin_navigate(hwnd, NAV_GOTO, url, FALSE);

    set_view_mode(hwnd, NORMAL_VIEW);

    PAGE_DATA* page = FindPage(hwnd, MOVETO_PREV);
    if (page)
        ScrollToLastPageAndScale(page->hwnd);

    draw_histroy_status(0, 0);
    draw_addr_btn_status(TRUE);
    draw_zoom_btn_status(TRUE);
    return hwnd;
}

void my_close_mdolphin_window(HWND hwnd)
{
    remove_mdolphin_from_pageview(hwnd);

    if (!get_page_count())
        (void)CreateMdolphinWindow("", WS_CHILD| WS_VISIBLE);
}

int GetPageIndex(PAGE_DATA* pd)
{
    return get_page_index(pd);
}

int GetPageCount()
{
    return get_page_count();
}

PAGE_DATA* GetPageData(HWND hwnd)
{
	return get_page_data(hwnd);
}

PAGE_DATA* FindPage(HWND hWnd, int step)
{
    return find_page(hWnd, step);
}

void SetActiveWindowForPageView(HWND hwnd)
{
    HWND hActive = GetActiveWindowInPageView();
	if (IsWindowVisible(hActive)) {
		RECT rc;
		GetPageGeometry(hActive, &rc, 0);
		ShowWindow(hActive, SW_HIDE);
	}
    set_active_page(hwnd);
}

HWND GetActiveWindowInPageView()
{
    PAGE_DATA* pd = get_active_page();
    if (pd)
        return pd->hwnd;

    return HWND_NULL;
}

int GetViewMode(HWND hWnd)
{
    return get_view_mode(hWnd);
}

void SetViewMode(HWND hWnd, int mode)
{
    set_view_mode(hWnd, mode);
}

int SwitchViewMode(HWND hWnd, int from, int to)
{
    return switch_view_mode(hWnd, from, to);
}

PAGE_DATA* GetActivePage()
{
    return get_active_page();
}

void SetActivePage(HWND hwnd)
{
    set_active_page(hwnd);
}

static void remove_page(PAGE_INFO* pi, PAGE_DATA* pd)
{
    PAGE_DATA* page = pi->head, *prev = NULL;
    while (page) {
        if (page == pd) {
            if (prev) {
                prev->next = page->next;
                if (page->next)
                    page->next->prev = prev;
            }
            else {
                pi->head = page->next;
                if (page->next)
                    page->next->prev = NULL;
            }

            pi->page_count--;
            break;
        }
        prev = page;
        page = page->next;
    }
}

static int add_page(PAGE_INFO* pi, PAGE_DATA* pd)
{
    PAGE_DATA* page = pi->head;
    while (page && page->next)
        page = page->next; 

    pd->prev = NULL;
    pd->next = NULL;
    if (page) {
        page->next = pd;
        pd->prev = page;
    }
    else
        pi->head = pd;

    return (pi->page_count++);
}

static PAGE_DATA* get_page(PAGE_INFO* pi, HWND hwnd)
{
    PAGE_DATA* page = pi->head;
    while (page) {
        if (page->hwnd == hwnd)
            return page;
        page = page->next;
    }

    return NULL;
}

static PAGE_DATA* create_page(HWND hwnd, const char* url)
{
    PAGE_DATA* pd = (PAGE_DATA*)calloc(sizeof(PAGE_DATA), 1);
    if (pd) {
        pd->hwnd = hwnd;
        if (url)
            pd->url = strdup(url);
		pd->mode = NORMAL_VIEW;
    }
    return pd;
}

static PAGE_INFO* get_page_info()
{
     return (PAGE_INFO*)SendMessage(hPageView, WNM_GETPAGEINFO, 0, 0);
}

static PAGE_DATA* find_page(HWND hwnd, int step)
{
    PAGE_DATA* pd = NULL;
    PAGE_INFO* pi = get_page_info();
    if (!pi)
        return NULL;

    switch (step) {
        case MOVETO_FIRST:
            pd = pi->head;
            break;
        case MOVETO_PREV:
            pd = get_page(pi, hwnd);
            pd = pd->prev;
            break;
        case MOVETO_CURRENT:
            pd = get_page(pi, hwnd);
            break;
        case MOVETO_NEXT:
            pd = get_page(pi, hwnd);
            pd = pd->next;
            break;
        case MOVETO_LAST:
            pd = pi->head;
            while (pd->next)
                pd = pd->next;
            break;
        default:       
            return NULL;
    };
    return pd;
}

PAGE_DATA* get_page_data(HWND hwnd)
{
    return (PAGE_DATA*)SendMessage(hPageView, WNM_GETPAGE, (DWORD)hwnd, 0);
}

static void destroy_page(PAGE_DATA* pd)
{
    if (pd->pBitmap) {
        UnloadBitmap(pd->pBitmap);
        free(pd->pBitmap);
    }
    if (pd->url)
        free(pd->url);
    if (pd->title)
        free(pd->title);

    free(pd);
}

static int get_page_index(PAGE_DATA* pd)
{
    int count = 0;
    PAGE_INFO* pi = get_page_info();
    PAGE_DATA* page = pi->head;

    while (page && page != pd) {
        page = page->next;
        count++;
    }

    return count;
}

static int get_page_count()
{
    return (int)SendMessage(hPageView, WNM_GETPAGECOUNT, 0, 0);
}

static HWND add_mdolphin_to_pageview(HWND hwnd, const char* url)
{
    PAGE_DATA* pd = create_page(hwnd, url);
    if (pd)
        SendMessage(hPageView, WNM_ADDPAGE, (WPARAM)pd, 0); 
}

static void remove_mdolphin_from_pageview(HWND hwnd)
{
    PAGE_DATA* pd = get_page_data(hwnd);
    SendMessage(hPageView, WNM_REMOVEPAGE, (WPARAM)pd, 0); 
    destroy_page(pd);  
}

static int get_view_mode(HWND hwnd)
{
    RENDERING_MODE rMode;
    mdolphin_get_rendering_mode(hwnd, &rMode);
	PAGE_DATA* pd = GetPageData(hwnd);
	if (pd)
		return pd->mode;

	return -1;
}

static void set_view_mode(HWND hwnd, int mode)
{
	PAGE_DATA* pd = GetPageData(hwnd);
	if (pd)
		pd->mode = mode;
}

static int switch_view_mode(HWND hWnd, int from, int to)
{
	RECT rc1, rc2;
	PRECT prcSrc, prcDst;
	HWND hBrowser = GetActiveWindowInPageView();
    PAGE_DATA* pd = get_page_data(hBrowser);

	if (from == to)
		return to;

    GetPageGeometry(hWnd, &rc1, 0);
	GetElementGeometry(MDOLPHIN_ELEMENT, &rc2);
	switch (to) {
		case NORMAL_VIEW: {
			prcSrc = &rc1;
			prcDst = &rc2;
			break;
		}
		case PAGE_VIEW: {
			prcSrc = &rc2;
			prcDst = &rc1;
			break;
		}
		default:
			return from;
	}

    if (to == PAGE_VIEW) {
        draw_histroy_status(0, 0);
        draw_loading_status(false, false);
        draw_addr_btn_status(FALSE);
		draw_zoom_btn_status(FALSE);
    } else {
        draw_addr_btn_status(TRUE);
		draw_zoom_btn_status(TRUE);
        draw_histroy_status(pd->back, pd->forward);
        draw_loading_status(pd->load, !pd->load);
    }

    ScalePage(hBrowser, prcSrc, prcDst);

    return to;
}

static PAGE_DATA* get_active_page()
{
    return (PAGE_DATA*)SendMessage(hPageView, WNM_GETACTIVEPAGE, 0, 0);
}

static void set_active_page(HWND hwnd)
{
    PAGE_DATA* pd = get_page_data(hwnd);
    SendMessage(hPageView, WNM_SETACTIVEPAGE, (WPARAM)pd, 0); 
}

static int pageViewProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PAGE_INFO* pi = (PAGE_INFO*)GetWindowAdditionalData2(hwnd);
    switch (message) {
        case MSG_CREATE:
            pi = (PAGE_INFO*)calloc(sizeof(PAGE_INFO), 1);
            SetWindowAdditionalData2(hwnd, (DWORD)pi);
            break;
        case MSG_PAINT:
            return 0;
        case MSG_MOUSEMOVE:
            break;
        case MSG_DESTROY: {
            PAGE_DATA* page, *next;
            if (!pi)
                break;

            page = pi->head;
            while (page) {
                next = page->next;
                destroy_page(page);
                page = next;
            }
            free(pi);
            break;
        }
        case WNM_GETPAGEINFO:
            return (int)pi;
        case WNM_GETPAGECOUNT:
            return (pi)? pi->page_count: 0;
        case WNM_ADDPAGE:
            return add_page(pi, (PAGE_DATA*)wParam); 
        case WNM_REMOVEPAGE:
            remove_page(pi, (PAGE_DATA*)wParam);
            return 0;
        case WNM_GETPAGE:
            return (int)get_page(pi, (HWND)wParam);

        case WNM_SETACTIVEPAGE: {
            PAGE_DATA* pd = (PAGE_DATA*)wParam;
            if (pi && pd && get_page(pi, pd->hwnd))
                pi->active = pd;
            break;
        }
        case WNM_GETACTIVEPAGE:
            return (pi)?((int)(pi->active)):0;
    }
	return DefaultControlProc(hwnd, message, wParam, lParam);
}

void my_capture_window_image(HWND hwnd, HDC hdc, const RECT* prc)
{
    PAGE_DATA* pd = get_page_data(hwnd);
    if (pd && prc) {
        RECT rc;
        GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
        if (RECTW(rc) != RECTWP(prc) || RECTH(rc) != RECTHP(prc))
            return;

        if (!pd->pBitmap) {
            pd->pBitmap = (PBITMAP)calloc(sizeof(BITMAP), 1);
            if (!pd->pBitmap)
                return;
            InitBitmap(hdc, RECTWP(prc), RECTHP(prc), 0, 0, pd->pBitmap);
        } 
        GetBitmapFromDC(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pd->pBitmap);
    }
}

BOOL RegisterPageViewControl(void)
{
    WNDCLASS pageViewClass;
    
    pageViewClass.spClassName = MDOLPHIN_PAGEVIEW;
    pageViewClass.dwStyle = WS_NONE;
    pageViewClass.dwExStyle = WS_EX_NONE;
    pageViewClass.hCursor = GetSystemCursor(IDC_ARROW);
    pageViewClass.iBkColor = COLOR_red;
    pageViewClass.WinProc = pageViewProc;
	return RegisterWindowClass(&pageViewClass);
}

void UnregisterPageViewControl(void)
{
    UnregisterWindowClass(MDOLPHIN_PAGEVIEW);
}

#if 0
static DLGTEMPLATE PageInfo =
{
    WS_THINFRAME | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 0, 0,
    "",
    0, 0,
    1, NULL,
    0
};

static CTRLDATA CtrlSysInfo [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | WS_CHILD| WS_BORDER,
        0, 0, 370, 160,
        IDC_TITLE,
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | WS_CHILD| WS_BORDER,
        0, 0, 370, 160,
        IDC_LOCATION,
        "",
        0
    },
};
#endif
