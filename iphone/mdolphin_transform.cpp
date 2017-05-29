/*
** $Id: mdolphin_transform.cpp,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_transform.cpp: transform browser window.
**
** Copyright(C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date: 2008-04-18
*/

#include "mdolphin_app.h"
#include "mdolphin_skin.h"
#include "mdolphin_transform.h"
#include "mdolphin_animation.h"

#if USE_FTIMER
#include "ftimer/ft_timer.h"
#endif

extern HWND hAnimationWnd;
extern TransContext* g_pCtxt;

static void begin_transform(TransContext* pCtxt);
static void end_transform(TransContext* pCtxt);

static void render_frame(TransContext* pCtxt, TransAction* pAct);

static BOOL add_update_rect(TransAction* pAct, const PRECT prc);
static BOOL fill_background(TransContext* pAct, const PRECT prc);
static BOOL calc_update_rect(TransAction* pAct, PRECT prc);
static BOOL calc_scale_action_rect(TransAction* pAct, PRECT prc);
static BOOL calc_move_action_rect(TransAction* pAct, PRECT prc);

static HDC  create_memdc(HWND hWnd);
static void reset_memdc(HDC hdc, BOOL bFillBitmap);
static void destroy_memdc(HDC hdc);

static void begin_action(TransContext* pCtxt, TransAction* action);
static void end_action(TransContext* pCtxt, TransAction* action);

static void begin_scroll_action(void* pctxt);
static void scroll_page(HWND hWnd, int dir, const PRECT prcSrc, const PRECT prcDst);
static void draw_scroll_frame1(void* pctxt);
static void draw_scroll_frame2(void* pctxt);

static void begin_scale_action(void* pctxt);
static void begin_scroll_and_scale_action(void* pctxt);
static void draw_scale_frame(void* pctxt);
static void end_scale_action(void *pctxt);
static void end_scroll_and_scale_action(void *pctxt);

static void draw_page_border(TransContext* pCtxt, int l, int t, int r, int b, int pw, int c);
static void draw_adjacent_frame(TransContext* pCtxt, PAGE_DATA* page);
static void draw_progress_meter(TransContext* pCtxt, int pos);
static void draw_close_page_button(TransContext* pCtxt);
static void draw_page_title(TransContext* pCtxt, PAGE_DATA* page);

static TransAction* create_master_trans_action(PAGE_DATA* page, BOOL forward);
static void create_slave_trans_action(TransAction* master, BOOL forward);

#if USE_FTIMER
static void mySetTimer(HWND hWnd, int id, int speed);
static void myKillTimer(HWND hWnd, int id);
static int timer_cb(void* arg);
#endif

#define ANIMATION_FRAMES   9 
#define ANIMATION_INTERNAL 6
#define ANIMATION_TIMER    0xff00

#define TITLE_TEXT_COLOR   COLOR_lightwhite  
#define TITLE_FONT_SIZE    TITLE_TXT_HEIGHT
#define URL_TEXT_COLOR     COLOR_lightwhite 
#define URL_FONT_SIZE      URL_TXT_HEIGHT 

#define BORDER_INNER_COLOR RGB2Pixel(pCtxt->hMemDC,0xc1, 0x87, 0x1c)
#define BORDER_OUTER_COLOR RGB2Pixel(pCtxt->hMemDC, 0x6b, 0x7a, 0x36)

#define METER_BORDER_WIDTH   1
#define METER_BORDER_COLOR   RGB2Pixel(pCtxt->hMemDC,0x1d, 0x12, 0x7e)
#define METER_BACKGND_COLOR  RGB2Pixel(pCtxt->hMemDC,0x2, 0x74, 0x72)
#define METER_FOREGND_COLOR  COLOR_lightwhite

//#define PAGE_BACKGND_COLOR   COLOR_lightwhite
#define PAGE_BACKGND_COLOR  RGB2Pixel(pCtxt->hMemDC, 0x03, 0x7d, 0x8a)
#define IS_LAST_FRAME(pAct) (pAct->index == pAct->frameCount-1) 
#define IS_ZOOMIN(pAct) ((RECTW(pAct->rcDst) < RECTW(pAct->rcSrc)) && (RECTH(pAct->rcDst) < RECTH(pAct->rcSrc))) 

static ftimer_t g_timer = 0;

void ScalePage(HWND hWnd, PRECT prcSrc, PRECT prcDst)
{
    TransAction* pAct;
    PAGE_DATA* pd = GetPageData(hWnd);

    g_pCtxt = CreateTransContext(hAnimationWnd, TRUE);

    pAct = CreateTransAction(pd, prcSrc, prcDst);
    SetTransActionAttr(pAct, SCALE_ACTION, begin_scale_action, end_scale_action,
            0, draw_scale_frame, 0, ANIMATION_FRAMES);
    AppendTransAction(g_pCtxt, pAct);

    SetActivePage(hWnd);

    BeginTransform(g_pCtxt);
}

void ScrollPage(HWND hWnd, int dir)
{
    g_pCtxt = CreateTransContext(hAnimationWnd, TRUE);
    SetTransContextAttr(g_pCtxt, begin_scroll_action, 0);
    switch (dir) {
        case MOVETO_NEXT:
        case MOVETO_PREV: {
            RECT rcDst, rcSrc;
            GetPageGeometry(hWnd, &rcDst, (dir==MOVETO_NEXT)?-1: 1);
            GetPageGeometry(hWnd, &rcSrc, 0);
            scroll_page(hWnd, dir, &rcSrc, &rcDst);
            break;
        }
        case MOVETO_LAST:
            //scroll_to_last_page(hWnd);
            break;
        case MOVETO_FIRST:
        default:
            DestroyTransContext(g_pCtxt);
            return;
    }

    BeginTransform(g_pCtxt);
}

void ScrollPageInView(HWND hWnd, int dir)
{
    PAGE_DATA* pd;
    RECT rcSrc, rcDst;

    pd = get_page_data(hWnd);
    if (!pd)
        return;

    switch (dir) {
        case MOVETO_NEXT:
            GetPageGeometry(hWnd, &rcSrc, -1);
            break;
        case MOVETO_PREV:
            GetPageGeometry(hWnd, &rcSrc, 1);
            break;
        default:
            return;
    }
    GetPageGeometry(hWnd, &rcDst, 0);

    g_pCtxt = CreateTransContext(hAnimationWnd, TRUE);
    SetTransContextAttr(g_pCtxt, begin_scroll_action, 0);

    TransAction* pAct = CreateTransAction(pd, &rcSrc, &rcDst);
    SetTransActionAttr(pAct, MOVE_ACTION, 0, 0,
			0, draw_scroll_frame2, 0, ANIMATION_FRAMES);
    AppendTransAction(g_pCtxt, pAct);

    SetActivePage(hWnd);
    SetViewMode(hWnd, PAGE_VIEW);

    BeginTransform(g_pCtxt);
}

void ScrollToLastPageAndScale(HWND hWnd)
{
    RECT rcSrc, rcDst;
    TransAction* action;
    PAGE_DATA* page = GetPageData(hWnd);
    PAGE_DATA* next = page, *last = page;

    g_pCtxt = CreateTransContext(hAnimationWnd, TRUE);
    SetTransContextAttr(g_pCtxt, begin_scroll_action, 0);
    while (next && next->next) {
        action = create_master_trans_action(next, FALSE);
        AppendTransAction(g_pCtxt, action);

        create_slave_trans_action(action, FALSE);

        last = next->next;
        next = next->next;
    }

    GetPageGeometry(hWnd, &rcSrc, 0);
	GetElementGeometry(MDOLPHIN_ELEMENT, &rcDst);

    action = CreateTransAction(last, &rcSrc, &rcDst);
    SetTransActionAttr(action, SCALE_ACTION, begin_scroll_and_scale_action,
            end_scroll_and_scale_action,
            0, draw_scale_frame, 0, ANIMATION_FRAMES);
    AppendTransAction(g_pCtxt, action);

    SetActivePage(last->hwnd);

    SetViewMode(last->hwnd, NORMAL_VIEW);

    BeginTransform(g_pCtxt);
}

static void begin_scroll_action(void* ctx)
{
    TransContext* pCtxt = (TransContext*)ctx;
    if (!IsWindowVisible(pCtxt->hContainer))
        ShowWindow(pCtxt->hContainer, SW_SHOW);
}

static void draw_scroll_frame1(void* ctx)
{
    int pos;
    PAGE_DATA* page;
    TransContext* pCtxt = (TransContext*)ctx;
    TransAction* pAct = pCtxt->current;

    if (!pAct)
        return ;

    pos = GetPageIndex(pAct->pd);
    if (IS_LAST_FRAME(pAct)) {
        page = pAct->pd;

        if (pAct->rcSrc.left < pAct->rcDst.left)
            page = page->prev;
        else
            page = page->next;
        
        if (GetPageCount() > 1)
            draw_close_page_button(pCtxt);
        draw_page_title(pCtxt, page);
        pos = GetPageIndex(page);
    }
    
    draw_progress_meter(pCtxt, pos);
}

static void draw_scroll_frame2(void* ctx)
{
    int pos;
    PAGE_DATA* page;
    TransContext* pCtxt = (TransContext*)ctx;
    TransAction* pAct = pCtxt->current;

    if (!pAct)
        return ;

    page = pAct->pd;
    pos = GetPageIndex(pAct->pd);
    if (IS_LAST_FRAME(pAct)) {
        if (GetPageCount() > 1)
            draw_close_page_button(pCtxt);
        draw_page_title(pCtxt, page);
        pos = GetPageIndex(page);
    }
    draw_adjacent_frame(pCtxt, page);
    draw_progress_meter(pCtxt, pos);
}

static void draw_page_border(TransContext* pCtxt, int l, int t, int r, int b, int pw, int color)
{
    int old_color = SetPenColor(pCtxt->hMemDC, color);
    Rectangle(pCtxt->hMemDC, l, t, r, b);
    SetPenColor(pCtxt->hMemDC, old_color);
}

static void draw_adjacent_frame(TransContext* pCtxt, PAGE_DATA* page)
{
    RECT rc;
    TransAction* pAct = pCtxt->current;

    if (page->next && page->next->pBitmap) {
        GetPageGeometry(page->hwnd, &rc, 1);
        FillBoxWithBitmap(pCtxt->hMemDC, rc.left+1, rc.top+1,
                    RECTW(rc)-2, RECTH(rc)-2, page->next->pBitmap);
        draw_page_border(pCtxt, rc.left, rc.top,
                rc.right-1, rc.bottom-1, 1, BORDER_INNER_COLOR);
        pAct->AddUpdateRect(pAct, &rc);
    }

    if (page->prev && page->prev->pBitmap) {
        GetPageGeometry(page->hwnd, &rc, -1);
        FillBoxWithBitmap(pCtxt->hMemDC, rc.left+1, rc.top+1,
                    RECTW(rc)-2, RECTH(rc)-2, page->prev->pBitmap);
        draw_page_border(pCtxt, rc.left, rc.top,
                rc.right-1, rc.bottom-1, 1, BORDER_INNER_COLOR);
        pAct->AddUpdateRect(pAct, &rc);
    }
}

static void draw_scale_frame(void* pctxt)
{
    TransContext* pCtxt = (TransContext*)pctxt;
    TransAction* pAct = pCtxt->current;

    if (!pAct)
        return;

    if (IS_LAST_FRAME(pAct) && !pAct->next && IS_ZOOMIN(pAct)) {
        int pos = GetPageIndex(pAct->pd);
        if (GetPageCount() > 1)
            draw_close_page_button(pCtxt);
        draw_page_title(pCtxt, pAct->pd);
        draw_adjacent_frame(pCtxt, pAct->pd);
        draw_progress_meter(pCtxt, pos);
    }
}

static void begin_scroll_and_scale_action(void* pctxt)
{
    begin_scale_action(pctxt);
}

static void begin_scale_action(void* ctx)
{
    TransContext* pCtxt =  (TransContext*)ctx;
    TransAction* pAct = pCtxt->current;

	if (!IsWindowVisible(pCtxt->hContainer))
		ShowWindow(pCtxt->hContainer, SW_SHOW);

    if (IsWindowVisible(pAct->pd->hwnd))
		ShowWindow(pAct->pd->hwnd, SW_HIDE);
}

static void end_scroll_and_scale_action(void *ctx)
{
    TransContext* pCtxt =  (TransContext*)ctx;
    TransAction* pAct = pCtxt->current;

    SetViewMode(pAct->pd->hwnd, NORMAL_VIEW);
    end_scale_action(ctx);
}

static void end_scale_action(void *ctx)
{
    TransContext* pCtxt =  (TransContext*)ctx;
    TransAction* pAct = pCtxt->current;

    int mode = GetViewMode(pAct->pd->hwnd);
    switch (mode) {
        case NORMAL_VIEW:
            if (!IsWindowVisible(pAct->pd->hwnd))
                ShowWindow(pAct->pd->hwnd, SW_SHOW);
            if (IsWindowVisible(pCtxt->hContainer))
                ShowWindow(pCtxt->hContainer, SW_HIDE);
            break;
        case PAGE_VIEW:
            if (!IsWindowVisible(pCtxt->hContainer))
                ShowWindow(pCtxt->hContainer, SW_SHOW);
            if (IsWindowVisible(pAct->pd->hwnd))
                ShowWindow(pAct->pd->hwnd, SW_HIDE);
            break;
        default:
            return;
    }
}

static TransAction* create_master_trans_action(PAGE_DATA* page, BOOL forward)
{
    TransAction* pAct;
    RECT rcSrc, rcDst;
    int dir = forward? 1: -1;

    GetPageGeometry(page->hwnd, &rcSrc, 0);
    GetPageGeometry(page->hwnd, &rcDst, dir);

    pAct = CreateTransAction(page, &rcSrc, &rcDst);
    SetTransActionAttr(pAct, MOVE_ACTION, 0, 0,
			0, draw_scroll_frame1, 0, ANIMATION_FRAMES);

    SetViewMode(page->hwnd, PAGE_VIEW);
    return pAct;
}

static void create_slave_trans_action(TransAction* master, BOOL forward)
{
    RECT rcSrc, rcDst;
    TransAction* slave;
    PAGE_DATA* page;
    int dir = forward? 1: -1;

    if (master->pd->prev) {
        page = master->pd->prev;
    }
    else {
        page = master->pd->next;
        dir = 1;
    }

    while (page && (dir < 3)) {
        GetPageGeometry(page->hwnd, &rcSrc, dir);
        GetPageGeometry(page->hwnd, &rcDst, dir-1);

        slave = CreateTransAction(page, &rcSrc, &rcDst);
        SetTransActionAttr(slave, MOVE_ACTION, 0, 0,
				0, draw_scroll_frame1, 0, ANIMATION_FRAMES);
        SetViewMode(page->hwnd, PAGE_VIEW);

        UnionTransAction(master, slave);

        page = page->next;
        dir++;

        if (page == master->pd) {
            page = page->next;
            dir++;
            continue;
        }
    }
}

static void scroll_page(HWND hWnd, int dir, const PRECT prcSrc, const PRECT prcDst)
{
    int  i, num = 0, forward = 1;
    HWND hCurrent = hWnd;
    RECT rcSrc[MAX_PAGES+1];
    RECT rcDst[MAX_PAGES+1];
    PAGE_DATA* pd[MAX_PAGES+1];
    TransAction* pAct[MAX_PAGES+1];

    memset(pd, 0x0, sizeof(pd));

    forward = (dir==MOVETO_PREV || dir == MOVETO_FIRST)? -1:1;

    rcDst[0] = *prcDst;
    rcSrc[0] = *prcSrc;
    pd[0] = GetPageData(hCurrent);
    while (pd[num]) {
        pd[++num] = FindPage(hCurrent, dir);
        GetPageGeometry(hCurrent, &rcSrc[num], num*forward);
        rcDst[num] = rcSrc[num-1];
        if (pd[num])
            hCurrent = pd[num]->hwnd;
    }

    if (!(pd[0] && pd[1]))
        return;

    for (i=0; i<num; i++) {
        pAct[i] = CreateTransAction(pd[i], &rcSrc[i], &rcDst[i]); 
        SetTransActionAttr(pAct[i], MOVE_ACTION, 0, 0,
				0, draw_scroll_frame1, 0, ANIMATION_FRAMES);
        SetViewMode(pd[i]->hwnd, PAGE_VIEW);
    }

    AppendTransAction(g_pCtxt, pAct[0]);

    if (pd[1]) {
        UnionTransAction(pAct[0], pAct[1]);
        SetActivePage(pd[1]->hwnd);
    } else
        SetActivePage(pd[0]->hwnd);

    if (pd[2])
        UnionTransAction(pAct[0], pAct[2]);
}

static void draw_progress_meter(TransContext* pCtxt, int pos)
{
    RECT rc;
    int i, brushColor, hSpace, pageCount; 
    TransAction* pAct = pCtxt->current;

    brushColor = SetBrushColor(pCtxt->hMemDC, METER_BACKGND_COLOR);

    pageCount = GetPageCount();
    
    rc.top = PROGRESS_METER_TOP;
    rc.bottom = rc.top + PROGRESS_METER_HEIGHT;
    for (i=0; i<pageCount; i++) {
        int w = PROGRESS_METER_ITEM_WIDTH(i);
        int h = PROGRESS_METER_ITEM_HEIGHT(i);

        hSpace = (PROGRESS_METER_WIDTH - w*pageCount + (pageCount+1)/2)/(pageCount+1);
        rc.left = PROGRESS_METER_LEFT + i*(w + hSpace) + hSpace;
        rc.right = rc.left + w;

        FillBox(pCtxt->hMemDC, rc.left, rc.top, w, h);

        //Rectangle(pCtxt->hMemDC, rc.left, rc.top, rc.right, rc.bottom);
        if (pos == i) {
            SetBrushColor(pCtxt->hMemDC, METER_FOREGND_COLOR);
            FillBox(pCtxt->hMemDC, rc.left, rc.top,
                    RECTW(rc), RECTH(rc));
        }
        
        SetBrushColor(pCtxt->hMemDC, METER_BACKGND_COLOR);

        pAct->AddUpdateRect(pAct, &rc);
    }

    SetBrushColor(pCtxt->hMemDC, brushColor);
}

static void draw_close_page_button(TransContext *pCtxt)
{
    RECT rc;
    TransAction* pAct = pCtxt->current;

    PBITMAP pBitmap = GetElementResource(CLOSEPAGE_ELEMENT, 0);
    if (pBitmap) {
        GetElementGeometry(CLOSEPAGE_ELEMENT, &rc);
        FillBoxWithBitmap(pCtxt->hMemDC, rc.left, rc.top, RECTW(rc), RECTH(rc), pBitmap);
        pAct->AddUpdateRect(pAct, &rc);
    }
}

static int TextOverflow(HDC hdc, char* text, int w, int* num)
{
    int ret = 0;
    
    if (!text)
        return 0;

    SIZE size;
    int len = strlen(text);
   
    GetTextExtent(hdc, text, len, &size);
    if (size.cx > w) {
        while (len) {
            len--;
            GetTextExtent(hdc, text, len, &size);
            if (size.cx < w)
                break;
        }
        ret = 1;
    }

    if (num)
        *num = len;
    return ret;
}

static void draw_page_title(TransContext* pCtxt, PAGE_DATA* page)
{
    RECT rc;
    int oldColor;
    LOGFONT oldFont;
    TransAction* pAct = pCtxt->current;
    if (!page)
        return;

    GetLogFontInfo(pCtxt->hMemDC, &oldFont);
    if (page->url && strcmp(page->url, "about:blank")) {
        SET_RECT_GEOMETRY(&rc, URL_TXT);
        rc.bottom = rc.top + pCtxt->urlFont->size; 

        oldColor = SetTextColor(pCtxt->hMemDC, URL_TEXT_COLOR);
        SelectFont(pCtxt->hMemDC, pCtxt->urlFont);

        DrawText(pCtxt->hMemDC, page->url, -1, &rc, DT_NOCLIP | DT_LEFT| DT_SINGLELINE);
        pAct->AddUpdateRect(pAct, &rc);
        SetTextColor(pCtxt->hMemDC, oldColor);
    }

    if (page->title) {
        SET_RECT_GEOMETRY(&rc, TITLE_TXT);
        rc.bottom = rc.top + pCtxt->titleFont->size + 4; 

        oldColor = SetTextColor(pCtxt->hMemDC, TITLE_TEXT_COLOR);
        SelectFont(pCtxt->hMemDC, pCtxt->titleFont);

        int num = 0;
        char* text = page->title;
        if (TextOverflow(pCtxt->hMemDC, text, PAGE_WIDTH(0), &num))
            text = strndup(text, num);

        DrawText(pCtxt->hMemDC, text, -1, &rc,
                DT_SINGLELINE| DT_CENTER| DT_TOP);
        
        if (text != page->title)
            free(text);

        pAct->AddUpdateRect(pAct, &rc);
        SetTextColor(pCtxt->hMemDC, oldColor);
    }
    SelectFont(pCtxt->hMemDC, &oldFont);
}

TransContext* CreateTransContext(HWND hContainer, BOOL autoStop)
{
    TransContext* pCtxt = (TransContext*)calloc(sizeof(TransContext), 1);
	if (!pCtxt)
		return NULL;

	pCtxt->hContainer = hContainer;
    pCtxt->bAutoStop = autoStop;
	pCtxt->bStoped = FALSE;
	pCtxt->bFinished = FALSE;
    pCtxt->hMemDC = create_memdc(hContainer);
    SetAnimationCtxt(pCtxt->hContainer, pCtxt);

    pCtxt->titleFont = CreateLogFont(NULL, "fmkai", "UTF-8",
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, 
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, TITLE_FONT_SIZE, 0);
    pCtxt->urlFont = CreateLogFont(NULL, "fmsong", "UTF-8",
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, 
            FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, URL_FONT_SIZE, 0);
    return pCtxt;
}

BOOL SetTransContextAttr(TransContext* pCtxt, NotifyTrans beginTrans, NotifyTrans endTrans)
{
    if (!pCtxt) 
        return FALSE;

    pCtxt->BeginTrans = beginTrans;
    pCtxt->EndTrans = endTrans;
    return TRUE;
}

TransAction* SetTransContextAction(TransContext* pCtxt, PAGE_DATA* pd, const PRECT src, const PRECT dst)
{
    TransAction* oldAction;

    if (!pCtxt || pCtxt->head)
        return NULL;

    oldAction = pCtxt->head;

    pCtxt->head = CreateTransAction(pd, src, dst);

    pCtxt->actionCount = 1;

    return oldAction;
}

BOOL AppendTransAction(TransContext* pCtxt, TransAction* pAction)
{
    TransAction* action;

    if (!pCtxt || !pAction)
        return FALSE;

    action = pCtxt->head;

    while (action && action->next)
        action = action->next;

    if (action)
        action->next = pAction;
    else
        pCtxt->head = pAction;

    pCtxt->actionCount++;

    return TRUE;
}

BOOL UnionTransAction(TransAction* dst, TransAction* src)
{
    TransAction* pDst = dst;

    if (!src || !pDst)
        return FALSE;

    while (pDst && pDst->sibling)
        pDst = pDst->sibling;

    pDst->sibling = src;

    return TRUE;
}

TransAction* CreateTransAction(PAGE_DATA* pd, const PRECT src, const PRECT dst)
{
    TransAction* pAct= (TransAction*)calloc(sizeof(TransAction), 1);
    if (pAct) {
        pAct->pd = pd;
        pAct->rcSrc = *src;
        pAct->rcDst = *dst;
        pAct->AddUpdateRect = (UpdateRect)add_update_rect;
        pAct->FillBackGround = (FillBKGnd)fill_background;
    }
    return pAct;
}

BOOL SetTransActionAttr(TransAction* pAction, ACTION_TYPE type, NotifyTrans beginAction, NotifyTrans endAction, NotifyTrans beforeExecAction, NotifyTrans execAction, NotifyTrans afterExecAction, int frameCount)
{
    if (!pAction)
        return FALSE;
    
    pAction->type = type;
    pAction->BeginAction = beginAction;
    pAction->BeforeExecAction = beforeExecAction;
    pAction->ExecAction = execAction;
    pAction->AfterExecAction = afterExecAction;
    pAction->EndAction = endAction;
    pAction->frameCount = frameCount;
    return TRUE;
}

void DestroyTransAction(TransAction* pAction)
{
    if (pAction)
        free(pAction);
}

static void destroy_trans_action(TransAction* pAct)
{
    TransAction* pSibling = NULL;

    if (!pAct)
        return;

    while (pAct) {
        pSibling = pAct->sibling;
        if (pAct->prcUpdate)
            free(pAct->prcUpdate);
        free(pAct);
        pAct = pSibling;
    }
}

void DestroyAllTransAction(TransContext* pCtxt)
{
    TransAction* pAct, *pNext;
    if (!pCtxt)
        return;

    pAct = pCtxt->head;
    while (pAct) {
        pNext = pAct->next;
        destroy_trans_action(pAct);
        pAct = pNext;
    }
    pCtxt->head = NULL;
}

void DestroyTransContext(TransContext* pCtxt)
{
	if (!pCtxt)
        return;
    
    destroy_memdc(pCtxt->hMemDC);
    pCtxt->hMemDC = 0;

    if (pCtxt->bAutoStop)
        DestroyAllTransAction(pCtxt);

    DestroyLogFont(pCtxt->titleFont);
    DestroyLogFont(pCtxt->urlFont);

    SetAnimationCtxt(pCtxt->hContainer, 0);
    free(pCtxt);
    pCtxt = 0;
}

void BeginTransform(TransContext* pCtxt)
{
    begin_transform(pCtxt);
}

static int inc_frame_index(TransAction* pAct)
{
    return pAct->index++;
}

void ProcessTransform(TransContext* pCtxt)
{
    TransAction* sibling;
    TransAction* current = (pCtxt)?pCtxt->current: 0;

    if (!current)
        return;

    if (pCtxt->current->index == 0)
        begin_action(pCtxt, pCtxt->current);

    reset_memdc(pCtxt->hMemDC, TRUE);

    if (pCtxt->current->BeforeExecAction)
        pCtxt->current->BeforeExecAction(pCtxt);

    sibling = current->sibling;
    while (sibling) {
        render_frame(pCtxt, sibling);
        inc_frame_index(sibling);
        sibling = sibling->sibling;
    }
    render_frame(pCtxt, current);
    inc_frame_index(current);

#if 0
    InvalidateRect(pCtxt->hContainer, 0, FALSE);
#else

    HDC hdc = GetClientDC(pCtxt->hContainer);			
    if (pCtxt) {
        UpdateTransActionRect(pCtxt, hdc);
        if (!pCtxt->current) {
            if (pCtxt->bAutoStop) {
                EndTransform(pCtxt);
                DestroyTransContext(pCtxt);
            }
        }
    }

    ReleaseDC(hdc);
#endif
}

void UpdateTransActionRect(TransContext* pCtxt, HDC hdc)
{
    TransAction* pAct = pCtxt->current;
    
    if (!pCtxt || !pAct)
        return;
#if 0
    while (pAct) {
        for (i=0; i<pAct->rcCount; i++) {
            BitBlt(pCtxt->hMemDC, pAct->prcUpdate[i].left, pAct->prcUpdate[i].top,
                    RECTW(pAct->prcUpdate[i]), RECTH(pAct->prcUpdate[i]),
                    hdc, pAct->prcUpdate[i].left, pAct->prcUpdate[i].top, 0);
        }
        pAct->rcCount = 0;
        pAct = pAct->sibling;
    }
#else
    RECT rc;
    GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
    BitBlt(pCtxt->hMemDC, rc.left, rc.top, RECTW(rc), RECTH(rc),
			hdc, rc.left, rc.top, 0);
#endif
    pAct = pCtxt->current;
    if (pAct->index >= pAct->frameCount) {
        end_action(pCtxt, pAct);

        pCtxt->current = pAct->next;
        if (pCtxt->current) {
            if (pCtxt->current->BeginAction)
                pCtxt->current->BeginAction(pCtxt);
        }
    }
    
	if (0) {
        RECT rc;
        BITMAP bitmap;
        char buf[128];
        static int g_i = 0;

        InitBitmap(pCtxt->hMemDC, RECTW(rc), RECTH(rc), 0, 0, &bitmap);
        GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
        GetBitmapFromDC(pCtxt->hMemDC, 0, 0, RECTW(rc), RECTH(rc), &bitmap);
        sprintf(buf, "%d.bmp", g_i);
        SaveBitmapToFile(pCtxt->hMemDC, &bitmap, buf);
        g_i++;
    }
}

void EndTransform(TransContext* pCtxt)
{
    end_transform(pCtxt);
}

static BOOL fill_background(TransContext* pCtxt, const PRECT prc)
{
    if (pCtxt && prc) {
        SetBrushColor(pCtxt->hMemDC, PAGE_BACKGND_COLOR);
        FillBox(pCtxt->hMemDC, prc->left, prc->top, RECTWP(prc), RECTHP(prc));
    }
    return TRUE;
}

static BOOL add_update_rect(TransAction* pAct, const PRECT prc)
{
    pAct->rcCount++;
    pAct->prcUpdate = (PRECT)realloc(pAct->prcUpdate, pAct->rcCount*sizeof(RECT));
    pAct->prcUpdate[pAct->rcCount-1] = *prc;

    return TRUE;
}

static BOOL calc_move_action_rect(TransAction* pAct, PRECT prc)
{
    int reserved = pAct->frameCount - pAct->index-1;
	int dx = (pAct->rcSrc.left - pAct->rcDst.left)/pAct->frameCount;
	int dy = (pAct->rcSrc.top - pAct->rcDst.top)/pAct->frameCount;

    prc->left = pAct->rcDst.left + dx*reserved;
    prc->top = pAct->rcDst.top + dy*reserved;
    prc->right = RECTW(pAct->rcDst) + prc->left;
    prc->bottom = RECTH(pAct->rcDst) + prc->top;

    return TRUE;
}

static BOOL calc_scale_action_rect(TransAction* pAct, PRECT prc)
{
    int reserved = pAct->frameCount - pAct->index-1;
    int dx = (RECTW(pAct->rcSrc)-RECTW(pAct->rcDst))/pAct->frameCount;
    int dy = (RECTH(pAct->rcSrc)-RECTH(pAct->rcDst))/pAct->frameCount;

    prc->left = pAct->rcDst.left - dx*reserved/2;
    prc->top = pAct->rcDst.top - dy*reserved/2;
    prc->right = RECTW(pAct->rcDst) + dx*reserved + prc->left;
    prc->bottom = RECTH(pAct->rcDst) + dy*reserved+ prc->top;

    return TRUE;
}

static BOOL calc_update_rect(TransAction* pAct, PRECT prc)
{
    switch (pAct->type) {
        case SCALE_ACTION:
            return calc_scale_action_rect(pAct, prc);
        case MOVE_ACTION:
            return calc_move_action_rect(pAct, prc);
        default:
            break;
    }
    return FALSE ;
}

static void fill_update_rect(TransContext* pCtxt, TransAction* pAct, const PRECT prc)
{
    if (pAct->pd->pBitmap) {
        int oldColor = SetBrushColor(pCtxt->hMemDC, COLOR_lightwhite);
        FillBox(pCtxt->hMemDC, prc->left+1, 
                prc->top+1, RECTWP(prc)-2, RECTHP(prc)-2);
        SetBrushColor(pCtxt->hMemDC, oldColor);
        
        FillBoxWithBitmap(pCtxt->hMemDC, prc->left+1, 
            prc->top+1, RECTWP(prc)-2, RECTHP(prc)-2, pAct->pd->pBitmap);
    }
    draw_page_border(pCtxt, prc->left, prc->top,
            prc->right-1, prc->bottom-1, 1, BORDER_INNER_COLOR);
}

static void render_frame(TransContext* pCtxt, TransAction* pAct)
{
    RECT rc;
    BOOL ok = calc_update_rect(pAct, &rc);
    if (ok) {
        pCtxt->current->AddUpdateRect(pAct, &rc);
        fill_update_rect(pCtxt, pAct, &rc);
    }

    if (pAct->ExecAction)
        pAct->ExecAction(pCtxt);
}

static void begin_action(TransContext* pCtxt, TransAction* pAct)
{
    if (pAct && pAct->BeginAction)
        pAct->BeginAction(pCtxt);
}

static void end_action(TransContext* pCtxt, TransAction* pAct)
{
    if (pAct && pAct->EndAction)
        pAct->EndAction(pCtxt);
}

static void begin_transform(TransContext* pCtxt)
{
	if (!pCtxt || !pCtxt->head)
		return;

    pCtxt->bFinished = FALSE;
    pCtxt->current = pCtxt->head;

    if (pCtxt->BeginTrans)
        pCtxt->BeginTrans(pCtxt);
#if USE_FTIMER 
    myKillTimer(pCtxt->hContainer, ANIMATION_TIMER);
    mySetTimer(pCtxt->hContainer, ANIMATION_TIMER, ANIMATION_INTERNAL-2);
#else
    KillTimer(pCtxt->hContainer, ANIMATION_TIMER);
	SetTimer(pCtxt->hContainer, ANIMATION_TIMER, ANIMATION_INTERNAL);
#endif
}

static void end_transform(TransContext* pCtxt)
{
	if (!pCtxt)
		return;

    if (pCtxt->EndTrans)
        pCtxt->EndTrans(pCtxt);

    pCtxt->bFinished = TRUE;
#if USE_FTIMER    
    myKillTimer(pCtxt->hContainer, ANIMATION_TIMER);
#else
	KillTimer(pCtxt->hContainer, ANIMATION_TIMER);
#endif
}

static void reset_memdc(HDC hdc, BOOL bFillBitmap)
{
    RECT rc;
    GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
    if (bFillBitmap) {
        PaintAnimationBackground(hdc);
    }else {
        int oldColor = SetBrushColor(hdc, COLOR_lightwhite);
        FillBox(hdc, 0, 0, RECTW(rc), RECTH(rc)); 
        SetBrushColor(hdc, oldColor);
    }
}

static HDC create_memdc(HWND hWnd)
{
    RECT rc;
    HDC hdc, hMemDC;

    GetElementGeometry(MDOLPHIN_ELEMENT, &rc);

    hdc = GetClientDC(hWnd);
    hMemDC = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
    SetBkMode(hMemDC, BM_TRANSPARENT);
    //SetBkColor(hMemDC, COLOR_lightwhite);
    ReleaseDC(hdc);

    if (HDC_INVALID != hMemDC)
        reset_memdc(hMemDC, FALSE);

    return hMemDC;
}

static void destroy_memdc(HDC hdc)
{
    if (HDC_INVALID != hdc)
        DeleteCompatibleDC(hdc);
}

#if USE_FTIMER
static void mySetTimer(HWND hWnd, int id, int speed)
{
    if (g_timer)
        ft_free_timer(g_timer);

    g_timer = ft_create_timer(timer_cb, 0);
    if (g_timer)
        ft_start_timer(g_timer, speed*10, (void*)hWnd);
}

static void myKillTimer(HWND hWnd, int id)
{
    (void)ft_stop_timer(g_timer, (void (*)(void*))0, 0); 
    ft_free_timer(g_timer); 
}

static int timer_cb(void* arg)
{
    HWND hWnd = (HWND)arg;
    PostMessage(hWnd, MSG_TIMER, 0, 0);
}
#endif

#if 0
static BOOL get_bound_rect(TransAction* pAct, PRECT prc)
{
    int i;

    if (!pAct || !prc)
        return FALSE;

    *prc = pAct->prcUpdate[0];
    for (i=0; i<pAct->rcCount; i++)
        GetBoundRect(prc, &pAct->prcUpdate[i], prc);

    return TRUE;
}

static BOOL get_string_width(HDC hdc, int max, const char* text, int* w, int* len)
{
    SIZE size;

    if (!w || !len)
        return FALSE;

    GetTextExtent(hdc, text, strlen(text), &size);

    if (size.cx > max)
       return FALSE; 
}

static void scroll_to_last_page(HWND hWnd)
{
    TransAction* action = NULL;
    PAGE_DATA* page = GetPageData(hWnd);
    PAGE_DATA* next = page, *last = page;

    while (next) {
        action = create_master_trans_action(next, FALSE);
        AppendTransAction(g_pCtxt, action);

        create_slave_trans_action(action, FALSE);

        last = next;
        next = next->next;
    }
    SetActivePage(last->hwnd);
}
#endif
