
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_AUTH_USER  901
#define IDC_AUTH_PASS  902

#define DLG_AUTH_WIDTH 450
#define DLG_AUTH_HEIGHT 300

extern HWND propsheet_hwnd;

static char * user=NULL;
static char * pass=NULL;
static int isOk = 0;
static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_THINFRAME | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , DLG_AUTH_WIDTH, DLG_AUTH_HEIGHT,
     "Prompt",
     0, 0,
     7,
     NULL, 0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_LEFT,
        10, 22, 430, 110,
        0,
        "Enter username and password for Auth",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        50, 120, 100, 23,
        0,
        "User Name:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        160, 120, 200, 23,
        IDC_AUTH_USER,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        50, 160, 100, 23,
        0,
        "Password:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | WS_BORDER,
        160, 160, 200, 23,
        IDC_AUTH_PASS,
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        50, 200, 150, 30,
        IDOK, 
        "Ok",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        227, 200, 150, 30,
        IDCANCEL, 
        "Cancel",
        0
    }
};

//static int AuthBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)		// gengyue
static LRESULT AuthBoxProc (HWND hDlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    int len = 0;
    switch (message) {
        case MSG_INITDIALOG: 
            GetWindowRect ((HWND)(lParam), &rc);
            MoveWindow (hDlg , (rc.right-rc.left-DLG_AUTH_WIDTH)/2, (rc.bottom-rc.top-DLG_AUTH_HEIGHT)/2, DLG_AUTH_WIDTH, DLG_AUTH_HEIGHT , TRUE);
            return 1;
        case MSG_SETFOCUS:
            SetFocus(GetDlgItem (hDlg, IDC_AUTH_USER));
            break;
        case MSG_COMMAND:
            switch (wParam) {
                case IDOK: 
                    {
                        isOk = 1;
                        len = GetWindowTextLength (GetDlgItem (hDlg, IDC_AUTH_USER));
                        if (len > 0) 
                        {
                            user = (char*) malloc( len + 1);
                            if (user) {
                                memset(user, 0, len+1);
                                GetWindowText (GetDlgItem (hDlg, IDC_AUTH_USER), user, len);
                            }
                        }
                        len = GetWindowTextLength (GetDlgItem (hDlg, IDC_AUTH_PASS));
                        if (len > 0) 
                        {
                            pass = (char*) malloc( len + 1);
                            if (pass) {
                                memset(pass, 0, len+1);
                                GetWindowText (GetDlgItem (hDlg, IDC_AUTH_PASS), pass, len);
                            }
                        }
                    }    
                case IDCANCEL:
                    EndDialog (hDlg, wParam);
                    break;
            }
            break;

        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            break;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static int create_auth_window(HWND hParent, const char* title)
{
    PopWinData.controls = CtrlInitProgress;
    if (title && strlen(title)) {
        char buf[256] = "Enter username and password for \n";
        strncat(buf, title, strlen(title));
        buf[255]='\0';
        CtrlInitProgress[0].caption = buf;
    }
    return DialogBoxIndirectParam (&PopWinData, hParent, AuthBoxProc, (DWORD)hParent);
}

BOOL my_provide_auth_callbackv3 (const char* title, char *username, size_t userNameLen, 
        char *password, size_t passwordLen)
{
    BOOL result = FALSE;
    create_auth_window(GetParent(propsheet_hwnd), title);
    if (isOk) {
        if (user && pass) {
            strcpy(username, user);
            strcpy(password, pass);
        }
        isOk = 0;
        result = TRUE;
    }
    if (user) {
        free(user);
        user = NULL;
    }
    if (pass) {
        free(pass);
        pass = NULL;
    }
    return result;
}
BOOL my_provide_auth_callback (const char* title, char *username, char *password)
{
    BOOL result = FALSE;
    create_auth_window(GetParent(propsheet_hwnd), title);
    if (isOk) {
        if (user && pass) {
            strcpy(username, user);
            strcpy(password, pass);
        }
        isOk = 0;
        result = TRUE;
    }
    if (user) {
        free(user);
        user = NULL;
    }
    if (pass) {
        free(pass);
        pass = NULL;
    }
    return result;
}
