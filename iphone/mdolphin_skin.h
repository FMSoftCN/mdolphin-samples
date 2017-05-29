/*
** $Id: mdolphin_skin.h,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_skin.h: browser UI skin file.
**
** Copyright(C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date: 2008-04-18
*/

#ifndef mdolphin_skin_h
#define mdolphin_skin_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* vertical space */
#define CAPTION_VSPACE    0
#define TOP_PNL_VSPACE    0
#define MDOLPHIN_VSPACE   1
#define ALL_VSPACE        ((CAPTION_VSPACE) + (TOP_PNL_VSPACE) + (MDOLPHIN_VSPACE))

#if IS_PORTRAIT
#define BOTTOM_HEIGHT     39

#define FIXED_BTN         1

#if FIXED_BTN
#define FIXED_BTN_WIDTH   22
#endif

/*caption geometry */
#define CAPTION_LEFT        0
#define CAPTION_TOP         0
#define CAPTION_WIDTH       219
#define CAPTION_HEIGHT      0
// close button
#define CLOSE_BTN_WIDTH   21
#define CLOSE_BTN_HEIGHT  (19)
#define CLOSE_BTN_LEFT    ((WIN_WIDTH) - (CLOSE_BTN_WIDTH))
#define CLOSE_BTN_TOP     (CAPTION_TOP)
// title part
#define CAPTION_PART_WIDTH  ((WIN_WIDTH)-(CAPTION_WIDTH)-(CLOSE_BTN_WIDTH))
#define CAPTION_PART_HEIGHT (CAPTION_HEIGHT)
#define CAPTION_PART_LEFT   ((CAPTION_LEFT)+(CAPTION_WIDTH))
#define CAPTION_PART_TOP    (CAPTION_TOP) 

/* begin top panel geometry */
#define TOP_PNL_WIDTH     (WIN_WIDTH) 
#define TOP_PNL_HEIGHT    0 
#define TOP_PNL_LEFT      0
#define TOP_PNL_TOP       ((CAPTION_TOP)+(CAPTION_HEIGHT)+(CAPTION_VSPACE))

// home button geometry
#define HOME_BTN_WIDTH    20
#define HOME_BTN_HEIGHT   (TOP_PNL_HEIGHT)
#define HOME_BTN_LEFT     (TOP_PNL_LEFT)
#define HOME_BTN_TOP      (TOP_PNL_TOP)
// goto button geometry
#define GOTO_BTN_WIDTH    20 
#define GOTO_BTN_HEIGHT   (TOP_PNL_HEIGHT)
#define GOTO_BTN_LEFT     ((WIN_WIDTH)-(GOTO_BTN_WIDTH))
#define GOTO_BTN_TOP      (TOP_PNL_TOP)

// location geometry
#define LOCATION_WIDTH    ((TOP_PNL_WIDTH)-(HOME_BTN_WIDTH)-(GOTO_BTN_WIDTH))
#define LOCATION_HEIGHT   (TOP_PNL_HEIGHT)
#define LOCATION_LEFT     ((HOME_BTN_LEFT)+(HOME_BTN_WIDTH))
#define LOCATION_TOP      (TOP_PNL_TOP)
/* end top panel */

/* mdolphin ctrl geometry */
#define MDOLPHIN_WIDTH    (WIN_WIDTH)
#define MDOLPHIN_HEIGHT   \
    ((WIN_HEIGHT)-(CAPTION_HEIGHT)-(TOP_PNL_HEIGHT)-(BOTTOM_HEIGHT)-(ALL_VSPACE))
#define MDOLPHIN_LEFT     0
#define MDOLPHIN_TOP      ((TOP_PNL_TOP) + (TOP_PNL_HEIGHT) + (TOP_PNL_VSPACE))

/* bottom toolbar geometry */
#define TBAR_HEIGHT       (BOTTOM_HEIGHT)
#define TBAR_LEFT         10
#define TBAR_RIGHT_EXTRA  10
#define TBAR_WIDTH        ((WIN_WIDTH)-(TBAR_LEFT)-(TBAR_RIGHT_EXTRA))
#define TBAR_TOP          ((MDOLPHIN_TOP) + (MDOLPHIN_HEIGHT) + (MDOLPHIN_VSPACE))

// toolbar button geometry
#define NR_TBAR_BTN       10

#if FIXED_BTN
#define TBAR_BTN_WIDTH    FIXED_BTN_WIDTH
#define TBAR_BTN_HSPACE   (((TBAR_WIDTH)- (TBAR_BTN_WIDTH)*(NR_TBAR_BTN))/((NR_TBAR_BTN)-1))
#else
#define TBAR_BTN_HSPACE   1
#define TBAR_BTN_WIDTH  \
    (((WIN_WIDTH) - ((NR_TBAR_BTN)-1)*(TBAR_BTN_HSPACE))/(NR_TBAR_BTN))
#endif

#define TBAR_BTN_VSPACE   8
#define TBAR_BTN_HEIGHT   22
#define TBAR_BTN_LEFT(i)  ((TBAR_LEFT) + ((TBAR_BTN_WIDTH)+(TBAR_BTN_HSPACE))*(i))
#define TBAR_BTN_TOP      ((TBAR_TOP) + (TBAR_BTN_VSPACE))
#define TBAR_BTN_BOTTOM   ((TBAR_BTN_TOP)+ (TBAR_BTN_HEIGHT))
#define TBAR_BTN_RIGHT(i) (TBAR_BTN_LEFT(i)+TBAR_BTN_WIDTH)

/* page geometry */
#define PAGE_HSPACE       (((MDOLPHIN_WIDTH)*3/25))
#if 0
#define PAGE_LEFT         (((MDOLPHIN_WIDTH)/4))
#define PAGE_TOP          ((MDOLPHIN_TOP)+((MDOLPHIN_HEIGHT)/3))
#define PAGE_WIDTH        ((MDOLPHIN_WIDTH)/2)
#define PAGE_HEIGHT       ((MDOLPHIN_HEIGHT)/2)
#else
#define PAGE_WIDTH(i)   ((MDOLPHIN_WIDTH)*3/5)
#define PAGE_LEFT(i)   \
    ((MDOLPHIN_WIDTH)/5 + PAGE_WIDTH(i)*(i) + PAGE_HSPACE*(i))
#define PAGE_TOP(i)     ((MDOLPHIN_TOP)+((MDOLPHIN_HEIGHT)*7/25))
#define PAGE_HEIGHT(i)  ((MDOLPHIN_HEIGHT)*3/5)
#endif

/* close page button */
#define CLOSEPAGE_BTN_LEFT   (PAGE_LEFT(0)-7)
#define CLOSEPAGE_BTN_TOP    (PAGE_TOP(0)-7)
#define CLOSEPAGE_BTN_WIDTH  (15)
#define CLOSEPAGE_BTN_HEIGHT (15)

/* title text */
#define TITLE_TXT_LEFT     (PAGE_LEFT(0))
#define TITLE_TXT_TOP      (PAGE_TOP(0)/4)
#define TITLE_TXT_WIDTH    (PAGE_WIDTH(0))
#define TITLE_TXT_HEIGHT   (16)

/* url text */
#define URL_TXT_LEFT         (PAGE_LEFT(0))
#define URL_TXT_TOP          (TITLE_TXT_TOP + TITLE_TXT_HEIGHT + 4)
#define URL_TXT_WIDTH        (PAGE_WIDTH(0))
#define URL_TXT_HEIGHT       (12)

/* progress meter */
#define PROGRESS_METER_WIDTH  (PAGE_WIDTH(0)/3)
#define PROGRESS_METER_HEIGHT (6)
#define PROGRESS_METER_LEFT   (PAGE_LEFT(0) + PAGE_WIDTH(0)/3)
#define PROGRESS_METER_TOP    \
    (PAGE_TOP(0)+PAGE_HEIGHT(0)+(MDOLPHIN_TOP+MDOLPHIN_HEIGHT-PAGE_HEIGHT(0)-PAGE_TOP(0))*1/4)

#define PROGRESS_METER_ITEM_WIDTH(i)  (6)
#define PROGRESS_METER_ITEM_HEIGHT(i) (PROGRESS_METER_HEIGHT)

#else

#define BOTTOM_HEIGHT     40

#define FIXED_BTN         1

#if FIXED_BTN
#define FIXED_BTN_WIDTH   32
#endif

/*caption geometry */
#define CAPTION_LEFT        0
#define CAPTION_TOP         0
#define CAPTION_WIDTH       219
#define CAPTION_HEIGHT      0
// close button
#define CLOSE_BTN_WIDTH   21
#define CLOSE_BTN_HEIGHT  (19)
#define CLOSE_BTN_LEFT    ((WIN_WIDTH) - (CLOSE_BTN_WIDTH))
#define CLOSE_BTN_TOP     (CAPTION_TOP)
// title part
#define CAPTION_PART_WIDTH  ((WIN_WIDTH)-(CAPTION_WIDTH)-(CLOSE_BTN_WIDTH))
#define CAPTION_PART_HEIGHT (CAPTION_HEIGHT)
#define CAPTION_PART_LEFT   ((CAPTION_LEFT)+(CAPTION_WIDTH))
#define CAPTION_PART_TOP    (CAPTION_TOP) 

/* begin top panel geometry */
#define TOP_PNL_WIDTH     (WIN_WIDTH) 
#define TOP_PNL_HEIGHT    0 
#define TOP_PNL_LEFT      0
#define TOP_PNL_TOP       ((CAPTION_TOP)+(CAPTION_HEIGHT)+(CAPTION_VSPACE))

// home button geometry
#define HOME_BTN_WIDTH    20
#define HOME_BTN_HEIGHT   (TOP_PNL_HEIGHT)
#define HOME_BTN_LEFT     (TOP_PNL_LEFT)
#define HOME_BTN_TOP      (TOP_PNL_TOP)
// goto button geometry
#define GOTO_BTN_WIDTH    20 
#define GOTO_BTN_HEIGHT   (TOP_PNL_HEIGHT)
#define GOTO_BTN_LEFT     ((WIN_WIDTH)-(GOTO_BTN_WIDTH))
#define GOTO_BTN_TOP      (TOP_PNL_TOP)

// location geometry
#define LOCATION_WIDTH    ((TOP_PNL_WIDTH)-(HOME_BTN_WIDTH)-(GOTO_BTN_WIDTH))
#define LOCATION_HEIGHT   (TOP_PNL_HEIGHT)
#define LOCATION_LEFT     ((HOME_BTN_LEFT)+(HOME_BTN_WIDTH))
#define LOCATION_TOP      (TOP_PNL_TOP)
/* end top panel */

/* mdolphin ctrl geometry */
#define MDOLPHIN_WIDTH    (WIN_WIDTH-1)
#define MDOLPHIN_HEIGHT   \
    ((WIN_HEIGHT)-(CAPTION_HEIGHT)-(TOP_PNL_HEIGHT)-(BOTTOM_HEIGHT)-(ALL_VSPACE))
#define MDOLPHIN_LEFT     0
#define MDOLPHIN_TOP      ((TOP_PNL_TOP) + (TOP_PNL_HEIGHT) + (TOP_PNL_VSPACE))

/* bottom toolbar geometry */
#define TBAR_HEIGHT       (BOTTOM_HEIGHT)
#define TBAR_LEFT         20
#define TBAR_RIGHT_EXTRA  20
#define TBAR_WIDTH        ((WIN_WIDTH)-(TBAR_LEFT)-(TBAR_RIGHT_EXTRA))
#define TBAR_TOP          ((MDOLPHIN_TOP) + (MDOLPHIN_HEIGHT) + (MDOLPHIN_VSPACE))

// toolbar button geometry
#define NR_TBAR_BTN       10

#if FIXED_BTN
#define TBAR_BTN_WIDTH    FIXED_BTN_WIDTH
#define TBAR_BTN_HSPACE   (((TBAR_WIDTH)- (TBAR_BTN_WIDTH)*(NR_TBAR_BTN))/((NR_TBAR_BTN)-1))
#else
#define TBAR_BTN_HSPACE   1
#define TBAR_BTN_WIDTH  \
    (((WIN_WIDTH) - ((NR_TBAR_BTN)-1)*(TBAR_BTN_HSPACE))/(NR_TBAR_BTN))
#endif

#define TBAR_BTN_VSPACE   2
#define TBAR_BTN_HEIGHT   36
#define TBAR_BTN_LEFT(i)  ((TBAR_LEFT) + ((TBAR_BTN_WIDTH)+(TBAR_BTN_HSPACE))*(i))
#define TBAR_BTN_TOP      ((TBAR_TOP) + (TBAR_BTN_VSPACE))
#define TBAR_BTN_BOTTOM   ((TBAR_BTN_TOP)+ (TBAR_BTN_HEIGHT))
#define TBAR_BTN_RIGHT(i) (TBAR_BTN_LEFT(i)+TBAR_BTN_WIDTH)

/* page geometry */
#define PAGE_HSPACE       (((MDOLPHIN_WIDTH)*3/25))
#if 0
#define PAGE_LEFT         (((MDOLPHIN_WIDTH)/4))
#define PAGE_TOP          ((MDOLPHIN_TOP)+((MDOLPHIN_HEIGHT)/3))
#define PAGE_WIDTH        ((MDOLPHIN_WIDTH)/2)
#define PAGE_HEIGHT       ((MDOLPHIN_HEIGHT)/2)
#else
#define PAGE_WIDTH(i)   ((MDOLPHIN_WIDTH)*3/5)
#define PAGE_LEFT(i)   \
    ((MDOLPHIN_WIDTH)/5 + PAGE_WIDTH(i)*(i) + PAGE_HSPACE*(i))
#define PAGE_TOP(i)     ((MDOLPHIN_TOP)+((MDOLPHIN_HEIGHT)*7/25))
#define PAGE_HEIGHT(i)  ((MDOLPHIN_HEIGHT)*3/5)
#endif

/* close page button */
#define CLOSEPAGE_BTN_LEFT   (PAGE_LEFT(0)-7)
#define CLOSEPAGE_BTN_TOP    (PAGE_TOP(0)-7)
#define CLOSEPAGE_BTN_WIDTH  (15)
#define CLOSEPAGE_BTN_HEIGHT (15)

/* title text */
#define TITLE_TXT_LEFT     (PAGE_LEFT(0))
#define TITLE_TXT_TOP      (PAGE_TOP(0)/4)
#define TITLE_TXT_WIDTH    (PAGE_WIDTH(0))
#define TITLE_TXT_HEIGHT   (16)

/* url text */
#define URL_TXT_LEFT         (PAGE_LEFT(0))
#define URL_TXT_TOP          (TITLE_TXT_TOP + TITLE_TXT_HEIGHT + 4)
#define URL_TXT_WIDTH        (PAGE_WIDTH(0))
#define URL_TXT_HEIGHT       (12)

/* progress meter */
#define PROGRESS_METER_WIDTH  (PAGE_WIDTH(0)/3)
#define PROGRESS_METER_HEIGHT (6)
#define PROGRESS_METER_LEFT   (PAGE_LEFT(0) + PAGE_WIDTH(0)/3)
#define PROGRESS_METER_TOP    \
    (PAGE_TOP(0)+PAGE_HEIGHT(0)+(MDOLPHIN_TOP+MDOLPHIN_HEIGHT-PAGE_HEIGHT(0)-PAGE_TOP(0))*1/4)

#define PROGRESS_METER_ITEM_WIDTH(i)  (6)
#define PROGRESS_METER_ITEM_HEIGHT(i) (PROGRESS_METER_HEIGHT)

#endif /* !IS_PORTRAIT */

enum SKIN_ELEMENT{
    /* title */
    CAPTION_ELEMENT = 0,
    CAPTION_PART_ELEMENT,
    CLOSE_ELEMENT,
   
    /* top panel */
    HOME_ELEMENT,
    LOCATION_ELEMENT,
    GOTO_ELEMENT,
 
    /* browser ctrl */
    MDOLPHIN_ELEMENT,

    /* toolbar */
    BACK_ELEMENT,
    FORWARD_ELEMENT,
    REFRESH_ELENENT,
    STOP_ELEMENT,
    ADDR_ELEMENT,
    ZOOMIN_ELEMENT,
    ZOOMOUT_ELEMENT,
    PAGEVIEW_ELEMENT,
    NEWPAGE_ELEMENT,
    PANVIEW_ELEMENT,

    NR_ELEMENT,

    /* bottom */
    BOTTOM_PART_ELEMENT,

    /* close page */
    CLOSEPAGE_ELEMENT,

    /* misc */
    IME_ELEMENT,
    MENU_ELEMENT,
 
    BACKGROUND_ELEMENT,

    MAX_ELEMENT,
};

typedef struct _ELEMENT_DATA {
    char* name;
    PBITMAP pBitmapNormal;
    PBITMAP pBitmapHover;
    PBITMAP pBitmapDisable;
    RECT* prc;
    BOOL  bHover;
    BOOL  bEnabled;
    BOOL  bVisible;
}ELEMENT_DATA;

#define SET_RECT_GEOMETRY(prc, name) \
    (prc)->left = name##_LEFT; \
    (prc)->top = name##_TOP; \
    (prc)->right = name##_LEFT + name##_WIDTH; \
    (prc)->bottom = name##_TOP + name##_HEIGHT;

#define SET_PAGE_GEOMETRY(prc, name, i) \
    (prc)->left = name##_LEFT(i); \
    (prc)->top = name##_TOP(i); \
    (prc)->right = name##_LEFT(i) + name##_WIDTH(i); \
    (prc)->bottom = name##_TOP(i) + name##_HEIGHT(i);

#define SET_PROGRESS_METER_ITEM(prc, name, i) \
    (prc)->left = name##_LEFT(i); \
    (prc)->top = name##_TOP(i); \
    (prc)->right = name##_LEFT(i) + name##_WIDTH(i); \
    (prc)->bottom = name##_TOP(i) + name##_HEIGHT(i);

int GetElementGeometry(int id, RECT* prc);
BOOL GetPageGeometry(HWND hwnd, RECT* prc, int index);
PBITMAP GetElementResource(int id, int status);

void InitSkinElements();
void DestroySkinElements();
void HandlePaintMessage(HWND hwnd, HDC hdc);
void PaintAnimationBackground(HDC hdc);
void PaintMainWndBackground(HDC hdc);
void HandlePaintTitle(HDC hdc);
void HandlePaintBottom(HDC hdc);
void ModifyImageShowState(int x_pos,int y_pos);
int  LoadBitmapResource();
int GetElementByPostion(int x, int y);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // mdophin_skin_h

