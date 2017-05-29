/*
** $Id: mdolphin_transform.h,v 1.1 2008/04/20 12:24:16 zwh Exp $
**
** mdolphin_transform.h: transform browser UI.
**
** Copyright(C) 2007~2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Create date: 2008-04-18
*/

#ifndef mdolphin_transform_h
#define mdolphin_transform_h

enum ACTION_TYPE {
   SCALE_ACTION = 0,
   MOVE_ACTION,
};

typedef void (*NotifyTrans)(void* pCtxt);
typedef BOOL (*UpdateRect)(void* pAct, const PRECT prc);
typedef BOOL (*FillBKGnd)(void* pCtxt, const PRECT prc);

typedef struct _TransAction {
    PAGE_DATA* pd;
    ACTION_TYPE  type;

    int  index;
    int  frameCount;

    int  rcCount;
    PRECT prcUpdate;

    RECT rcSrc;
    RECT rcDst;

    NotifyTrans  BeginAction;
    NotifyTrans  BeforeExecAction;
    NotifyTrans  ExecAction;
    NotifyTrans  AfterExecAction;
    NotifyTrans  EndAction;

    UpdateRect   AddUpdateRect;
    FillBKGnd    FillBackGround;

    struct _TransAction* sibling;
    struct _TransAction* next;
}TransAction;

typedef struct _TransContext {
    HDC  hMemDC;
    HWND hContainer;

    TransAction* head;
    TransAction* current;

    NotifyTrans  BeginTrans;
    NotifyTrans  EndTrans;

    int  actionCount;

    PLOGFONT titleFont;
    PLOGFONT urlFont;

    BOOL bStoped;
	BOOL bAutoStop;
    BOOL bFinished;
}TransContext;

TransContext* CreateTransContext(HWND hContainer, BOOL autoStop);
BOOL SetTransContextAttr(TransContext* ctx, NotifyTrans beginTrans, NotifyTrans endTrans);
TransAction* SetTransContextAction(TransContext* ctx, PAGE_DATA* pd, const PRECT src, const PRECT dst);
BOOL AppendTransAction(TransContext* ctx, TransAction* pAction);
BOOL UnionTransAction(TransAction* dst, TransAction* src);
void DestroyTransContext(TransContext* ctx);

TransAction* CreateTransAction(PAGE_DATA* pd, const PRECT src, const PRECT dst);
BOOL SetTransActionAttr(TransAction* pAction, ACTION_TYPE type, NotifyTrans beginAction, NotifyTrans endAction, NotifyTrans beforeExecAction, NotifyTrans execAction, NotifyTrans afterExecAction, int frameCount);
void DestroyTransAction(TransAction* pAction);
void DestroyAllTransAction(TransContext* ctx);

void BeginTransform(TransContext* ctx);
void ProcessTransform(TransContext* ctx);
void EndTransform(TransContext* ctx);

void UpdateTransActionRect(TransContext* pCtx, HDC hdc);

void ScrollPageInView(HWND hWnd, int dir);
void ScrollPage(HWND hWnd, int dir);
void ScalePage(HWND hWnd, PRECT prcSrc, PRECT prcDst);
void ScrollToLastPageAndScale(HWND hWnd);

#endif //mdolphin_transform_h

