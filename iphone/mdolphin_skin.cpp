/*
** $Id: mdolphin_skin.cpp,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_skin.cpp: browser UI skin implement file.
**
** Copyright(C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date: 2008-04-18
*/

#include "minigui/common.h"
#include "minigui/minigui.h"
#include "minigui/gdi.h"

#include "mdolphin_app.h"
#include "mdolphin_skin.h"
#include "mdolphin_app_res.h"

static void set_title_geometry();
static void set_top_panel_geometry();
static void set_tbar_geometry();

extern HWND hMainWnd;

RECT BackGroundRect = {0, 0, WIN_WIDTH, WIN_HEIGHT};

RECT TitleRect;
RECT TitlePartRect;
RECT CloseBtnRect;

RECT HomeBtnRect;
RECT LocationRect;
RECT GotoBtnRect;

RECT MdolphinRect;

RECT BackBtnRect;
RECT ForwardBtnRect;
RECT RefreshBtnRect;
RECT StopBtnRect;
RECT AddrBtnRect;
RECT ZoomInBtnRect;
RECT ZoomOutBtnRect;
RECT PageViewBtnRect;
RECT NewPageBtnRect;
RECT PanViewBtnRect;

RECT BottomSpace;

RECT ClosePageBtnRect;

RECT ImeBtnRect;
RECT MenuBtnRect;

static RECT* tbar_btn_rect[NR_TBAR_BTN] = {
    &BackBtnRect,
    &ForwardBtnRect,
    &RefreshBtnRect,
    &StopBtnRect,
    &AddrBtnRect,
    &ZoomInBtnRect,
    &ZoomOutBtnRect,
    &PageViewBtnRect,
    &NewPageBtnRect,
    &PanViewBtnRect,
};

static int last_hovered = -1;

BITMAP gif_res[MAX_BITMAP];
ELEMENT_DATA skin_elements[MAX_ELEMENT] = {
    /*name, pBitmapNormal, pPitmapHover,pBitmapDisable, rect, hover, enable, visible */
    {"title", &gif_res[TITLE_BITMAP], 0, 0,
        &TitleRect, FALSE, TRUE, TRUE},
    {"title_part", &gif_res[TITLE_PART_BITMAP], &gif_res[TITLE_PART_BITMAP], 0,
        &TitlePartRect, FALSE, TRUE, TRUE},
    {"close", &gif_res[CLOSE_BITMAP_NORMAL], &gif_res[CLOSE_BITMAP_HOVER], 0,
        &CloseBtnRect, FALSE, TRUE, TRUE},

    {"home", &gif_res[HOME_BITMAP_NORMAL],  &gif_res[HOME_BITMAP_HOVER], 0,
        &HomeBtnRect, FALSE, FALSE, FALSE},
    {"location", 0,  0, 0,
        &LocationRect, FALSE, FALSE, FALSE},
    {"goto", &gif_res[GOTO_BITMAP_NORMAL],    &gif_res[GOTO_BITMAP_HOVER], 0,
        &GotoBtnRect, FALSE, FALSE, FALSE},

    {"mdolphin", &gif_res[MDOLPHIN_BITMAP_NORMAL],  0, 0,
        &MdolphinRect, FALSE, FALSE, FALSE},

    {"back", &gif_res[BACK_BITMAP_NORMAL], 
		&gif_res[BACK_BITMAP_HOVER],
		&gif_res[BACK_BITMAP_DISABLE],
        &BackBtnRect, FALSE, FALSE, TRUE},
    {"forward", &gif_res[FORWARD_BITMAP_NORMAL],
	  	&gif_res[FORWARD_BITMAP_HOVER], 
		&gif_res[FORWARD_BITMAP_DISABLE],
        &ForwardBtnRect, FALSE, TRUE, TRUE},
    {"refresh", &gif_res[REFRESH_BITMAP_NORMAL],
	   	&gif_res[REFRESH_BITMAP_HOVER],
	   	&gif_res[REFRESH_BITMAP_DISABLE],
        &RefreshBtnRect, FALSE, TRUE, TRUE},
    {"stop", &gif_res[STOP_BITMAP_NORMAL],
	   	&gif_res[STOP_BITMAP_HOVER],
	   	&gif_res[STOP_BITMAP_DISABLE],
        &StopBtnRect, FALSE, TRUE, TRUE},
    {"addr", &gif_res[ADDR_BITMAP_NORMAL],
		&gif_res[ADDR_BITMAP_HOVER],
		&gif_res[ADDR_BITMAP_DISABLE],
        &AddrBtnRect, FALSE, TRUE, TRUE},
    {"zoomin", &gif_res[ZOOMIN_BITMAP_NORMAL], 
		&gif_res[ZOOMIN_BITMAP_HOVER],
		&gif_res[ZOOMIN_BITMAP_DISABLE],
        &ZoomInBtnRect, FALSE, TRUE, TRUE},
    {"zoomout", &gif_res[ZOOMOUT_BITMAP_NORMAL], 
		&gif_res[ZOOMOUT_BITMAP_HOVER],
		&gif_res[ZOOMOUT_BITMAP_DISABLE],
        &ZoomOutBtnRect, FALSE, TRUE, TRUE},
    {"pageview", &gif_res[PAGEVIEW_BITMAP_NORMAL],
		&gif_res[PAGEVIEW_BITMAP_HOVER], 
		&gif_res[PAGEVIEW_BITMAP_DISABLE], 
        &PageViewBtnRect, FALSE, TRUE, TRUE},
    {"newpage", &gif_res[NEWPAGE_BITMAP_NORMAL],
	   	&gif_res[NEWPAGE_BITMAP_HOVER],
	   	&gif_res[NEWPAGE_BITMAP_DISABLE],
        &NewPageBtnRect, FALSE, TRUE, TRUE},
    {"panview", &gif_res[PANVIEW_BITMAP_NORMAL],
	   	&gif_res[PANVIEW_BITMAP_HOVER],
	   	&gif_res[PANVIEW_BITMAP_DISABLE],
        &PanViewBtnRect, FALSE, TRUE, TRUE},

    {"", 0, 0, 0, FALSE, FALSE, FALSE},

    {"bottompart", &gif_res[BOTTOM_PART_BITMAP], 0, 0,
        &BottomSpace, FALSE, TRUE, TRUE},

    {"closepage", &gif_res[CLOSEPAGE_BITMAP_NORMAL], 0, 0,
        &ClosePageBtnRect, FALSE, TRUE, TRUE},
  
    {"ime", &gif_res[IME_BITMAP_NORMAL], 0, 0,
        &ImeBtnRect, FALSE, FALSE, FALSE},
    {"menu", &gif_res[MENU_BITMAP_NORMAL], &gif_res[MENU_BITMAP_HOVER], 0,
        &MenuBtnRect, FALSE, FALSE, FALSE},
    {"background", &gif_res[BACKGROUND_BITMAP], 0, 0,
        &BackGroundRect, FALSE, FALSE, FALSE},
};

void InitSkinElements()
{
    set_title_geometry();
    set_top_panel_geometry();
    SET_RECT_GEOMETRY(&MdolphinRect, MDOLPHIN);
    SET_RECT_GEOMETRY(&ClosePageBtnRect, CLOSEPAGE_BTN);
    set_tbar_geometry();

	if (!LoadBitmapResource())
		return;
#if 0
    RECT* prc = &MdolphinRect;
    fprintf(stderr, "md:(%d,%d)-(%d,%d)\n", prc->left, prc->top,
        prc->right, prc->bottom);
#endif    
}

void DestroySkinElements()
{
    size_t i;
    for (i=0; i<TABLESIZE(gif_res); i++)
        UnloadBitmap(&gif_res[i]);
}

BOOL GetElementGeometry(int id, RECT* rc)
{
    if (id < CAPTION_ELEMENT || id >= MAX_ELEMENT)
        return FALSE;

    *rc = *skin_elements[id].prc;
    return TRUE;
}

BOOL GetPageGeometry(HWND hwnd, RECT* prc, int index)
{
    if (!prc)
        return FALSE;

    SET_PAGE_GEOMETRY(prc, PAGE, index);
    return TRUE;
}

int GetElementByPostion(int x, int y)
{
    int i;
#if 0
    if (PtInRect(skin_elements[CLOSEPAGE_ELEMENT].prc, x, y))
        return CLOSEPAGE_ELEMENT;
#endif
    for (i=0; i<NR_ELEMENT; i++) {
        if (PtInRect(skin_elements[i].prc, x, y))
            return i;
    }
    return -1;
}

int LoadBitmapResource()
{
    size_t i;
    size_t num = 0;
    
    memset(gif_res, 0, sizeof(gif_res));

    for (i=0; i<TABLESIZE(gif_res); i++) {
#if LoadResFromFile
        if (LoadBitmap(HDC_SCREEN,&gif_res[i],bitmap_files[i]))
#else
        if (!LoadBitmapFromMem(HDC_SCREEN,&gif_res[i],bitmaps_buffer[i],bitmaps_size[i],EXT_NAME))
#endif
            num++;
    }
    return num;
}

PBITMAP GetElementResource(int id, int status)
{
    if (id<CAPTION_ELEMENT || id>MAX_ELEMENT)
        return NULL;

    switch (status) {
        case 0:
            return skin_elements[id].pBitmapNormal;
        case 1:
            return skin_elements[id].pBitmapHover;
        case 2:
            return skin_elements[id].pBitmapDisable;

    }
    return NULL;
}

void PaintAnimationBackground(HDC hdc)
{
    PBITMAP pbitmap = skin_elements[MDOLPHIN_ELEMENT].pBitmapNormal;
    RECT* prc = skin_elements[MDOLPHIN_ELEMENT].prc;
    if (pbitmap && prc)
        FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pbitmap);
}

void PaintMainWndBackground(HDC hdc)
{
    PBITMAP pbitmap = skin_elements[BACKGROUND_ELEMENT].pBitmapNormal;
    RECT* prc = skin_elements[BACKGROUND_ELEMENT].prc;
    if (pbitmap && prc)
        FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pbitmap);
}

void HandlePaintTitle(HDC hdc)
{
    PBITMAP pbitmap = skin_elements[CAPTION_ELEMENT].pBitmapNormal;
    RECT* prc = skin_elements[CAPTION_ELEMENT].prc;
    if (pbitmap && prc)
        FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pbitmap);

    pbitmap = skin_elements[CAPTION_PART_ELEMENT].pBitmapNormal;
    prc = skin_elements[CAPTION_PART_ELEMENT].prc;
    if (pbitmap && prc)
        FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pbitmap);

    pbitmap = skin_elements[CLOSE_ELEMENT].pBitmapNormal;
    prc = skin_elements[CLOSE_ELEMENT].prc;
    if (pbitmap && prc)
        FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), pbitmap);
}

void HandlePaintBottom(HDC hdc)
{
    int i;
    RECT rc;
    PBITMAP pbitmap = skin_elements[BOTTOM_PART_ELEMENT].pBitmapNormal;
    for (i=0; i<NR_TBAR_BTN-1; i++) {
        rc.left = tbar_btn_rect[i]->left;
        rc.top = tbar_btn_rect[i]->top;
        rc.right = tbar_btn_rect[i]->right + TBAR_BTN_HSPACE;
        rc.bottom = tbar_btn_rect[i]->bottom;
        FillBoxWithBitmap(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc), pbitmap);
    }
}

void HandlePaintMessage(HWND hwnd, HDC hdc)
{
    int i;
    RECT* prc = NULL;
    ELEMENT_DATA* pelement = NULL;

    for (i=BACK_ELEMENT; i<NR_ELEMENT; i++) {
        pelement = &skin_elements[i];
        if (!pelement->bVisible)
            continue;

		if (!pelement->bEnabled) {
			prc = pelement->prc;
			FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc),
					pelement->pBitmapDisable);
			continue;
		}
        
		prc = pelement->prc;
        if (pelement->bHover) {
            if (pelement->pBitmapHover)
                FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc),
                        pelement->pBitmapHover);
        } else {
            if (pelement->pBitmapNormal)
                FillBoxWithBitmap(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc),
                        pelement->pBitmapNormal);
        }
#if 0
        fprintf(stderr, "paint:(%d,%d)-(%d,%d)name=%s\n", prc->left, prc->top,
            RECTWP(prc), RECTHP(prc),pelement->name);
#endif
    }
}

void ModifyImageShowState(int x, int y)
{
    int i;
	BOOL haveHover = FALSE;
	for (i=0; i<NR_ELEMENT; i++) {
		if (!skin_elements[i].bEnabled)
			continue;

		if (PtInRect(skin_elements[i].prc, x, y)) {
			haveHover = TRUE;
			if (skin_elements[i].bHover)
				continue;
			skin_elements[i].bHover = TRUE;
			if (i != last_hovered) {
				last_hovered = i;
				InvalidateRect(hMainWnd, skin_elements[i].prc, TRUE);
			}
		} else {
			if (skin_elements[i].bHover) {
				skin_elements[i].bHover = FALSE;
				InvalidateRect(hMainWnd, skin_elements[i].prc, TRUE);
			}
		}
	}

	if (!haveHover)
		last_hovered = -1;
}

static void set_tbar_geometry()
{
    int i;
    RECT* prc;

    for (i=0; i<NR_TBAR_BTN; i++) {
        prc = tbar_btn_rect[i];
        prc->left = TBAR_BTN_LEFT(i);
        prc->top = TBAR_BTN_TOP;
        prc->right = TBAR_BTN_RIGHT(i);
        prc->bottom = TBAR_BTN_BOTTOM;
#if 0
        fprintf(stderr, "toolbar:(%d,%d)-(%d,%d)\n", prc->left, prc->top,
            RECTWP(prc), RECTHP(prc));
#endif        
    }
}

static void set_title_geometry()
{
    SET_RECT_GEOMETRY(&TitleRect, CAPTION);
    SET_RECT_GEOMETRY(&TitlePartRect, CAPTION_PART);
    SET_RECT_GEOMETRY(&CloseBtnRect, CLOSE_BTN);
#if 0
    RECT* prc = &TitleRect;
    fprintf(stderr, "title:(%d,%d)-(%d,%d)\n", prc->left, prc->top,
        RECTWP(prc), RECTHP(prc));
#endif    
}

static void set_top_panel_geometry()
{
    SET_RECT_GEOMETRY(&HomeBtnRect, HOME_BTN);
    SET_RECT_GEOMETRY(&LocationRect, LOCATION);
    SET_RECT_GEOMETRY(&GotoBtnRect, GOTO_BTN);
}
