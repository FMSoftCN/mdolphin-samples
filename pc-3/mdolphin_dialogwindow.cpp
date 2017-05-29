
#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
extern HWND propsheet_hwnd;

static DLGTEMPLATE DlgInitProgress =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_TOPMOST,
    0, 0, 400, 400,
    "mDolphin",
    0, 0,
    1, NULL,
    0
};
static CTRLDATA CtrlInitProgress [] =
{ 
    {
        MDOLPHIN_CTRL,
        WS_VISIBLE | WS_CHILD| WS_BORDER,
        0, 0, 400, 400,
        IDC_MDOLPHIN,
        "",
        0
    },
};

static int InitDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
            return 1;
        case MSG_CSIZECHANGED:
            {
                int width = (int)wParam;
                int height = (int)lParam;
                MoveWindow (GetDlgItem (hDlg, IDC_MDOLPHIN), 0, 0, width, height, TRUE);
            }
            break;
        case MSG_DESTROY:
            DestroyAllControls(hDlg);
            return 0;
        case MSG_CLOSE:
            DestroyMainWindowIndirect(hDlg);
            return 0;
    }
    return DefaultWindowProc (hDlg, message, wParam, lParam);
}

HWND my_create_dialog_window(HWND hParent)
{
    DlgInitProgress.controls = CtrlInitProgress;
    HWND hDlg = CreateMainWindowIndirect(&DlgInitProgress, hParent, InitDialogBoxProc);
    if (hDlg)
        return GetDlgItem (hDlg, IDC_MDOLPHIN);
    else
        return 0;
}
