
#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

//#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
//#include "mdolphin_propsheet.h"

//extern HWND propsheet_hwnd;
//extern HWND status_hwnd;
extern HWND mdolphin_hwnd;

/* There is only one find window at the same time.*/
static BOOL canCreateFindWindow = TRUE;

/*just for find again function*/
static char *find_string = NULL;
static BOOL caseFlag;
static BOOL wrapFlag;
static BOOL highlight;
static BOOL isForward;

enum IDC_FIND {
    IDC_FIND_TEXT = 670,
    IDC_FIND_NEXT,
    IDC_FIND_CASE,
    IDC_FIND_WRAP,
    IDC_FIND_UP,
    IDC_FIND_DOWN,
    IDC_FIND_ALL
};


static DLGTEMPLATE DlgInitProgress =
{
    WS_CAPTION | WS_BORDER | WS_VISIBLE,
    WS_EX_TOPMOST,
    0, 0, FIND_WIDTH, FIND_HEIGHT,
    "Find in This Page",
    0, 0,
    9, NULL,
    0
};

static CTRLDATA CtrlInitProgress [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        FIND_WIDTH*5/320, FIND_HEIGHT*5/120, 
		FIND_WIDTH*185/320, FIND_HEIGHT*65/120,
        0,
        "Find",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_TABSTOP | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        FIND_WIDTH*20/320, FIND_HEIGHT*20/120, 
		FIND_WIDTH*150/320, FIND_HEIGHT*20/120,
        IDC_FIND_TEXT,
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        FIND_WIDTH*200/320, FIND_HEIGHT*15/120, 
		FIND_WIDTH*100/320, FIND_HEIGHT*20/120,
        IDC_FIND_NEXT, 
        "Find Next",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX,
        FIND_WIDTH*20/320, FIND_HEIGHT*45/120, 
		FIND_WIDTH*120/320, FIND_HEIGHT*20/120,
        IDC_FIND_CASE,
        "Case Sensitive",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
        FIND_WIDTH*140/320, FIND_HEIGHT*45/120, 
		FIND_WIDTH*50/320, FIND_HEIGHT*20/120,
        IDC_FIND_WRAP,
        "Wrap",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        FIND_WIDTH*200/320, FIND_HEIGHT*45/120, 
		FIND_WIDTH*100/320, FIND_HEIGHT*40/120,
        0,
        "Direction",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
        FIND_WIDTH*210/320, FIND_HEIGHT*60/120, 
		FIND_WIDTH*40/320, FIND_HEIGHT*20/120,
        IDC_FIND_UP, 
        "Up",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED,
        FIND_WIDTH*250/320, FIND_HEIGHT*60/120, 
		FIND_WIDTH*60/320, FIND_HEIGHT*20/120,
        IDC_FIND_DOWN, 
        "Down",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX,
        FIND_WIDTH*20/320, FIND_HEIGHT*70/120, 
		FIND_WIDTH*150/320, FIND_HEIGHT*20/120,
        IDC_FIND_ALL, 
        "Mark All Matches",
        0
    },
};
static void init_parameter_for_find(HWND hDlg)
{
    if (find_string) {
        free(find_string); 
        find_string= NULL;
    }
    if (SendMessage(GetDlgItem (hDlg, IDC_FIND_CASE), BM_GETCHECK, 0, 0) == BST_CHECKED ) 
        caseFlag = TRUE;
    else
        caseFlag = FALSE;
    if (SendMessage(GetDlgItem (hDlg, IDC_FIND_WRAP), BM_GETCHECK, 0, 0) == BST_CHECKED ) 
        wrapFlag = TRUE;
    else
        wrapFlag = FALSE;
    if (SendMessage(GetDlgItem (hDlg, IDC_FIND_ALL), BM_GETCHECK, 0, 0) == BST_CHECKED ) 
        highlight = TRUE;
    else
        highlight = FALSE;
    if (SendMessage(GetDlgItem (hDlg, IDC_FIND_DOWN), BM_GETCHECK, 0, 0) == BST_CHECKED ) 
        isForward = TRUE;
    else
        isForward = FALSE;
}

static void search_for(HWND hWnd)
{
    mdolphin_search_for(hWnd, find_string, isForward, caseFlag, wrapFlag);
}

static void clear_mark(HWND hWnd)
{
    mdolphin_unmark_all_text_matches(hWnd);
}

static void my_mark_all_matches_for_text(HWND hWnd)
{
    mdolphin_mark_all_matches_for_text(hWnd, find_string, caseFlag, highlight, 0);
}

static void case_sensitive_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            caseFlag = TRUE;
        else
            caseFlag = FALSE;
        
        clear_mark(mdolphin_hwnd);
        my_mark_all_matches_for_text(mdolphin_hwnd);
    }
}

static void wrap_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            wrapFlag = TRUE;
        else
            wrapFlag = FALSE;
    }
}

static void forward_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (id == IDC_FIND_UP)
            isForward = FALSE;
        else if (id == IDC_FIND_DOWN)
            isForward = TRUE;
    }
}

static void get_search_string(HWND hWnd)
{
    int len = 0;
    len = GetWindowTextLength (hWnd);
    if (!len)
        return ;
    if (find_string)
        free(find_string);
    find_string = (char*) malloc( len + 1);
    if (!find_string)
        return ;
    memset(find_string, 0, len+1);
    GetWindowText (hWnd, find_string, len);
    find_string[len]='\0';
}

static void text_string_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == EN_CHANGE) {
        get_search_string(hWnd);
        search_for(mdolphin_hwnd);
        clear_mark(mdolphin_hwnd);
        my_mark_all_matches_for_text(mdolphin_hwnd);
    }
}

static void mark_all_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED ) 
            highlight = TRUE;
        else
            highlight = FALSE;
        clear_mark(mdolphin_hwnd);
        my_mark_all_matches_for_text(mdolphin_hwnd);
    }
}

static int InitDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            RECT rc;
            GetWindowRect ((HWND)(lParam), &rc);
           // MoveWindow (hDlg , (rc.right-rc.left)/2-160 , (rc.bottom-rc.top)/2-60 , 320 , 120 , TRUE);
            init_parameter_for_find(hDlg);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_TEXT), text_string_notif_proc);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_CASE), case_sensitive_notif_proc);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_WRAP), wrap_notif_proc);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_ALL), mark_all_notif_proc);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_UP), forward_notif_proc);
            SetNotificationCallback (GetDlgItem (hDlg, IDC_FIND_DOWN), forward_notif_proc);
            return 1;
        case MSG_COMMAND:
            switch (wParam) {
                case IDC_FIND_NEXT:
                            search_for(mdolphin_hwnd);
                    break;
            }
            break;
        case MSG_DESTROY:
            DestroyAllControls(hDlg);
            return 0;
        case MSG_CLOSE:
            {
                canCreateFindWindow = TRUE;
//                SetWindowText (status_hwnd, "");
  //              SendMessage(propsheet_hwnd, PSM_SHEETCMD, USER_PMS_CLOSE_SEARCHFOR, 0);
                DestroyMainWindowIndirect(hDlg);
            }
            return 0;
    }
    return DefaultWindowProc (hDlg, message, wParam, lParam);
}

void create_find_window(HWND hParent)
{
    if (canCreateFindWindow) {
        canCreateFindWindow = FALSE;
        DlgInitProgress.controls = CtrlInitProgress;
        CreateMainWindowIndirectParam(&DlgInitProgress, hParent, InitDialogBoxProc, (DWORD)hParent);
    }
}

void find_again_with_last_result(void)
{
	mdolphin_search_for(mdolphin_hwnd, find_string, isForward, caseFlag, wrapFlag);
#if 0
    HWND mdolphin_chwnd = get_current_mdolphin_hwnd();
    if (mdolphin_chwnd) {
        if (!mdolphin_search_for(mdolphin_chwnd, find_string, isForward, caseFlag, wrapFlag))
            SetWindowText (status_hwnd, "Phrase not found.");
        else
            SetWindowText (status_hwnd, "");
    }
#endif
}

void showing_propsheet_update_search_for(HWND hWnd)
{
    if (canCreateFindWindow)
        return;
    if (hWnd) {
        search_for(hWnd);
        clear_mark(hWnd);
        my_mark_all_matches_for_text(hWnd);
    }
}
