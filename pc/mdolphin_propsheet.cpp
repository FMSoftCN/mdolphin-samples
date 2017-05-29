
#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
#include "mdolphin_menustate.h"

extern HWND status_hwnd;
extern HWND location_hwnd;
extern HWND propsheet_hwnd;
extern HWND hMainWnd;

extern HWND my_create_dialog_window(HWND hParent);
extern void showing_propsheet_update_search_for(HWND hWnd);
extern void set_callback_info(HWND hParent);
extern void set_preference_info(HWND mdolpin_current_hwnd);

typedef struct _PROP_DATA {
    char* status;
    char* location;
    char* title;
    BOOL load;
    unsigned int progress;
    unsigned int back;
    unsigned int forward;
    unsigned int encoding;
}PROP_DATA;

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
        MDOLPHIN_CTRL,
        WS_VISIBLE | WS_CHILD| WS_BORDER,
        0, 0, 370, 160,
        IDC_MDOLPHIN,
        "",
        0
    },
};

static void add_default_title(HWND hDlg, const char *location)
{
    PROP_DATA *data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
    if (data && !data->title) {
        data->title = strdup("mDolphin");

        int index = SendMessage(propsheet_hwnd, PSM_GETPAGEINDEX, hDlg, 0);
        SendMessage(propsheet_hwnd, PSM_SETTITLE, index, (LPARAM)data->location);
        HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
        if (active != hDlg)
            return;
        SetWindowText(GetParent(propsheet_hwnd), "mDolphin");
    }
}

static int PageProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    PROP_DATA *data = NULL;
    switch (message) {
        case MSG_INITPAGE:
            {
                RECT rc;
                GetWindowRect (GetParent(hDlg), &rc);
                MoveWindow (GetDlgItem (hDlg, IDC_MDOLPHIN), 0, 0, RECTW(rc), RECTH(rc)-25-8, TRUE);
                data = (PROP_DATA *)malloc(sizeof(PROP_DATA));
                if (data) {
                    memset(data, 0, sizeof(PROP_DATA));
                    SetWindowAdditionalData(hDlg, (DWORD)data);
                }
            }
            break;
        case MSG_SHOWPAGE:
            if (lParam == SW_SHOW) {
                data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                if (data) {
                    if (data->status)
                        SetWindowText (status_hwnd, data->status);
                    else
                        SetWindowText (status_hwnd, "");
                    if (data->location)
                        SetWindowText (location_hwnd, data->location);
                    else
                        SetWindowText (location_hwnd, "");
                    if (data->title)
                        SetWindowText(GetParent(propsheet_hwnd), data->title);
                    else
                        SetWindowText(GetParent(propsheet_hwnd), "");
                    change_loading_menu_status(data->load, data->progress);
                    change_history_menu_status(data->back, data->forward);
                    change_view_menu_status(GetDlgItem (hDlg, IDC_MDOLPHIN)); 
                    change_encoding_menu_status(data->encoding);
                    showing_propsheet_update_search_for(GetDlgItem (hDlg, IDC_MDOLPHIN));
                }
            }
            ShowWindow(GetDlgItem (hDlg, IDC_MDOLPHIN), lParam);
            return 1;
        case MSG_COMMAND:
            switch(wParam) {
                case USER_PMS_STATUS:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data && lParam) {
                        if (data->status)
                            free(data->status);
                        data->status = strdup((const char*)lParam);
                    }
                    break;
                case USER_PMS_LOAD:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data)
                        data->load = (BOOL)lParam;
                    break;
                case USER_PMS_LOCATION:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data && lParam) {
                        if (data->location)
                            free(data->location);
                        data->location = strdup((const char*)lParam);
                        if (data->location && !data->title) 
                            add_default_title(hDlg, data->location);
                    }
                    break;
                case USER_PMS_TITLE:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data && lParam) {
                        if (data->title)
                            free(data->title);
                        data->title = strdup((const char*)lParam);
                    }
                    break;
                case USER_PMS_PROGRESS:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data)
                        data->progress = (unsigned int)lParam;
                    break;
                case USER_PMS_BACK:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data)
                        data->back = (unsigned int)lParam;
                    break;
                case USER_PMS_FORWARD:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data)
                        data->forward = (unsigned int)lParam;
                    break;
                case USER_PMS_ENCODING:
                    data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
                    if (data)
                        data->encoding = (unsigned int)lParam;
                    break;
            }
            break;
        case MSG_SHEETCMD:
            switch(wParam) {
                case USER_PMS_RESIZE:
                    RECT rc;
                    GetWindowRect(GetParent(hDlg), &rc);
                    MoveWindow(GetDlgItem(hDlg, IDC_MDOLPHIN), 0, 0, RECTW(rc), RECTH(rc)-25-8, TRUE);
                    break;
                case USER_PMS_CLOSE_SEARCHFOR:
                    mdolphin_unmark_all_text_matches(GetDlgItem(hDlg, IDC_MDOLPHIN));
                    break;
            }
            return 0;

        case MSG_DESTROY:
            data = (PROP_DATA *)GetWindowAdditionalData(hDlg);
            if (data) {
                if (data->location)
                    free (data->location);
                if (data->title)
                    free (data->title);
                if (data->status)
                    free (data->status);
                free (data);
                SetWindowAdditionalData(hDlg, 0);
            }
            break;
    }
    /* FIXME: minigui capture the keys, for example: Up,Down,Left etc.
     *        but mdolphin must handle this keys. So I replace the DefaultPageProc
     *        with the DefaultControlProc.
     * return DefaultPageProc (hDlg, message, wParam, lParam);
     */
    return DefaultControlProc (hDlg, message, wParam, lParam);
}

HWND my_create_new_window(const char* url, DWORD styles, int x, int y, int width, int height)
{
    if (styles & CWS_MODAL) {
        return my_create_dialog_window(hMainWnd);
    } else {
        PageInfo.controls = CtrlSysInfo;
        PageInfo.caption = "";
        int index = SendMessage (propsheet_hwnd, PSM_ADDPAGE, (WPARAM)&PageInfo, (LPARAM)PageProc);
        HWND hPage = SendMessage(propsheet_hwnd, PSM_GETPAGE, index, 0);
        int count = SendMessage(propsheet_hwnd, PSM_GETPAGECOUNT, 0, 0);
        change_file_menu_status(count, TRUE);

        index = SendMessage(propsheet_hwnd, PSM_GETPAGEINDEX, hPage, 0);
        SendMessage(propsheet_hwnd, PSM_SETTITLE, index, (LPARAM)"Blank");

        return GetDlgItem (hPage, IDC_MDOLPHIN);
    }
}

void my_close_tab_window(HWND hWnd)
{
    if (hWnd)
        mdolphin_navigate(hWnd, NAV_STOP, NULL, FALSE);
    HWND hPage = GetParent(hWnd);
    if (!hPage)
        return;

    int index = SendMessage(propsheet_hwnd, PSM_GETPAGEINDEX, hPage, 0);
    if (index == PS_ERR) {
        DestroyMainWindowIndirect(hPage);
        return;
    }
    PROP_DATA *data = (PROP_DATA *)GetWindowAdditionalData(hPage);
    if (data) {
        if (data->status)
            free(data->status);
        if (data->location)
            free(data->location);
        if (data->title)
            free(data->title);
        free(data);
        SetWindowAdditionalData(hPage, (DWORD)0);
    }
    SendMessage(propsheet_hwnd, PSM_REMOVEPAGE, index, 0);

    int count = SendMessage(propsheet_hwnd, PSM_GETPAGECOUNT, 0, 0);
    change_file_menu_status(count, FALSE);
}

HWND get_current_mdolphin_hwnd(void)
{
    HWND hPage = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (hPage == HWND_INVALID)
        return 0;
    else
        return GetDlgItem (hPage, IDC_MDOLPHIN);
}

void view_page_in_source_mode(HWND hwnd)
{
    if (!mdolphin_in_view_source_mode(hwnd)) {
        int len = GetWindowTextLength (location_hwnd);
        if (len>0) {
            char* url = (char *)malloc(len+1);
            if (url) {
                memset(url, 0, len+1);
                GetWindowText(location_hwnd, url, len);
                url[len]='\0';
                HWND newHwnd = my_create_new_window(NULL, 0x3f, 0, 0, -1, -1);
                if (newHwnd) {
                    bool ret = mdolphin_set_in_view_source_mode(newHwnd, TRUE);
                    if (!ret)
                        fprintf(stderr, "Set in view source mode fail, please check the config option\n");
                    mdolphin_navigate(newHwnd, NAV_GOTO, url, FALSE);
                }
                free(url);
            }
        }
    }
}
