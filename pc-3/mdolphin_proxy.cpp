
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

#include <mdolphin/mdolphin.h>

enum IDC_PROXY {
    IDC_PROXY_AUTO = 600,
    IDC_PROXY_MANUAL,
    IDC_HTTP_PROXY_HOST = 602,
    IDC_HTTP_PROXY_PORT,
    IDC_HTTP_PROXY_USERNAME,
    IDC_HTTP_PROXY_PASSWORD,
    IDC_SOCKS_PROXY_HOST,
    IDC_SOCKS_PROXY_PORT,
    IDC_SOCKS_PROXY_USERNAME,
    IDC_SOCKS_PROXY_PASSWORD,
    IDC_PROXY_SOCKS4,
    IDC_PROXY_SOCKS5
};

#define IDC_FTP_PROXY_HOST      608
#define IDC_FTP_PROXY_PORT      609

#define WIN_WIDTH          420
#define WIN_HEIGHT         320 

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_BUTTON,
        WS_TABSTOP |WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_GROUP,
        10, 20, 250, 25,
        IDC_PROXY_AUTO,
        "Direct connection to internet",
        0, WS_EX_TRANSPARENT

    },
    {
        CTRL_BUTTON,
        WS_TABSTOP |WS_VISIBLE | BS_AUTORADIOBUTTON,
        10, 55, 250, 25,
        IDC_PROXY_MANUAL,
        "Manual proxy configuration",
        0, WS_EX_TRANSPARENT

    },
    /* HTTP Proxy */
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        30, 92, 70, 25,
        0,
        "Http Proxy:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        100, 90, 180, 25,
        IDC_HTTP_PROXY_HOST,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        295, 92, 90, 20,
        0,
        "Port:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        325, 90, 50, 25,
        IDC_HTTP_PROXY_PORT,
        NULL,
        0
    },
    /* HTTP Proxy User Name */
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        30, 122, 70, 25,
        0,
        "User Name:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        100, 120, 100, 25,
        IDC_HTTP_PROXY_USERNAME,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        210, 122, 55, 20,
        0,
        "Password:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | WS_BORDER,
        270, 120, 100, 25,
        IDC_HTTP_PROXY_PASSWORD,
        NULL,
        0
    },
    /* SOCKS Proxy */ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        30, 152, 90, 25,
        0,
        "Socks Proxy:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        100, 150, 180, 25,
        IDC_SOCKS_PROXY_HOST,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        295, 152, 90, 20,
        0,
        "Port:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        325, 150, 50, 25,
        IDC_SOCKS_PROXY_PORT,
        NULL,
        0
    },

    /* Socks Proxy User Name*/ 
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        30, 182, 70, 25,
        0,
        "User Name:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        100, 180, 100, 25,
        IDC_SOCKS_PROXY_USERNAME,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
        210, 182, 90, 20,
        0,
        "Password:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | WS_BORDER,
        270, 180, 100, 25,
        IDC_SOCKS_PROXY_PASSWORD,
        NULL,
        0
    },

    /* Socks5 or Socks4 */ 
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 
        110, 210, 90, 25,
        IDC_PROXY_SOCKS5, 
        "Socks5",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON, 
        220, 210, 90, 25,
        IDC_PROXY_SOCKS4, 
        "Socks4",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        (WIN_WIDTH-90*2-50)/2, 245, 90, 20,
        IDOK, 
        "Ok",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        (WIN_WIDTH-90*2-50)/2+50+90, 245, 90, 20,
        IDCANCEL, 
        "Cancel",
        0
    }
};

static DLGTEMPLATE PopWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , WIN_WIDTH, WIN_HEIGHT,
     "Proxy Setting",
     0, 0,
     sizeof(CtrlInitProgress)/sizeof(CtrlInitProgress[0]),
     NULL, 0
};

static BOOL getProxyItem(HWND hDlg, int idc, char** item)
{
    int len;
    len = GetWindowTextLength(GetDlgItem(hDlg, idc));
    if (len > 0) {
        if (!(*item = (char*)malloc(len+1)))
            return FALSE;
        memset(*item, 0, len+1);
        GetWindowText(GetDlgItem(hDlg, idc), *item, len);
        (*item)[len] = '\0';
    }
    return TRUE;
}

static void setProxyItem(HWND hDlg, int idc, char* item)
{
    if (item)
        SetWindowText(GetDlgItem(hDlg, idc), item);
}

static CTRLDATA *findCtrl(int id)
{
    for (unsigned i=0; i<sizeof(CtrlInitProgress)/sizeof(CtrlInitProgress[0]); i++) {
        if (CtrlInitProgress[i].id == id)
            return &CtrlInitProgress[i];
    }
    return NULL;
}

static void initCtrlsStyle()
{
    if (mdolphin_get_proxy((PROXY_TYPE)0, NULL)) {
        findCtrl(IDC_PROXY_AUTO)->dwStyle &= ~BS_CHECKED;  
        findCtrl(IDC_PROXY_MANUAL)->dwStyle |= BS_CHECKED;  
        for (int i=IDC_HTTP_PROXY_HOST; i<=IDC_PROXY_SOCKS5; i++)
            findCtrl(i)->dwStyle &= ~WS_DISABLED;  
    }
    else {
        findCtrl(IDC_PROXY_AUTO)->dwStyle |= BS_CHECKED;  
        findCtrl(IDC_PROXY_MANUAL)->dwStyle &= ~BS_CHECKED;  
        for (int i=IDC_HTTP_PROXY_HOST; i<=IDC_PROXY_SOCKS5; i++)
            findCtrl(i)->dwStyle |= WS_DISABLED;  
    }
    
    PROXY_ITEM item;
    mdolphin_get_proxy(PROXY_SOCKS5, &item);
    if (item.host) {
        findCtrl(IDC_PROXY_SOCKS5)->dwStyle |= BS_CHECKED;
        findCtrl(IDC_PROXY_SOCKS4)->dwStyle &= ~BS_CHECKED;
    }
    else {
        findCtrl(IDC_PROXY_SOCKS5)->dwStyle &= ~BS_CHECKED;  
        findCtrl(IDC_PROXY_SOCKS4)->dwStyle |= BS_CHECKED;
    }
}


static void setProxyCtrlsEnable(HWND hDlg, BOOL enable)
{
    typedef BOOL (*SetStyleFunc)(HWND hWnd, DWORD dwStyle);
    SetStyleFunc SetStyle;
    
    if (enable) {
        IncludeWindowStyle(GetDlgItem(hDlg, IDC_PROXY_MANUAL), BS_CHECKED);
        ExcludeWindowStyle(GetDlgItem(hDlg, IDC_PROXY_AUTO), BS_CHECKED);
        SetStyle = ExcludeWindowStyle;
    }
    else {
        ExcludeWindowStyle(GetDlgItem(hDlg, IDC_PROXY_MANUAL), BS_CHECKED);
        IncludeWindowStyle(GetDlgItem(hDlg, IDC_PROXY_AUTO), BS_CHECKED);
        SetStyle = IncludeWindowStyle;
    }

    for (int i=IDC_HTTP_PROXY_HOST; i<=IDC_PROXY_SOCKS5; i++)
        SetStyle(GetDlgItem(hDlg, i), WS_DISABLED);
    UpdateWindow(hDlg, TRUE);
}

static int ProxyBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    PROXY_INFO proxys;
    
    switch (message) {
        case MSG_INITDIALOG:
        {
            for (int i=PROXY_HTTP; i<PROXY_MAX; i++) {
                PROXY_ITEM proxy;
                int idc = 0;
                memset(&proxy, 0x0, sizeof(proxy));
                mdolphin_get_proxy((PROXY_TYPE)i, &proxy);
                switch (i) {
                    case PROXY_HTTP:
                        idc = IDC_HTTP_PROXY_HOST;
                        break;
                    case PROXY_SOCKS4:
                    case PROXY_SOCKS5:
                        idc = IDC_SOCKS_PROXY_HOST; 
                        break;
                }
                setProxyItem(hDlg, idc, proxy.host);
                setProxyItem(hDlg, idc+1, proxy.port);
                setProxyItem(hDlg, idc+2, proxy.name);
                setProxyItem(hDlg, idc+3, proxy.pwd);
            }

            GetWindowRect((HWND)(lParam), &rc);
            MoveWindow(hDlg, RECTW(rc)/2-160, RECTH(rc)/2-120, WIN_WIDTH, WIN_HEIGHT, TRUE);
            return 1;
        }
        case MSG_COMMAND:
            switch (wParam) {
                case IDC_PROXY_MANUAL:
                    setProxyCtrlsEnable(hDlg, TRUE);
                    break;
                case IDC_PROXY_AUTO:
                    setProxyCtrlsEnable(hDlg, FALSE);
                    break;
                case IDOK: 
                {
                    BOOL enable = FALSE;
                    proxys.num = 2;
                    proxys.items = (PROXY_ITEM*)malloc(sizeof(PROXY_ITEM) * proxys.num);
                    memset(proxys.items, 0x0, sizeof(PROXY_ITEM) * proxys.num);
                    for (int i=0; i<proxys.num; i++) {
                        int idc = (i<<2) + IDC_HTTP_PROXY_HOST;
                        if (! getProxyItem(hDlg, idc, &proxys.items[i].host))
                            goto err;
                        if (! getProxyItem(hDlg, idc+1, &proxys.items[i].port))
                            goto err;
                        if (! getProxyItem(hDlg, idc+2, &proxys.items[i].name))
                            goto err;
                        if (! getProxyItem(hDlg, idc+3, &proxys.items[i].pwd))
                            goto err;

                        switch (idc) {
                        case IDC_HTTP_PROXY_HOST:
                            proxys.items[i].type = PROXY_HTTP;
                            break;
                        case IDC_SOCKS_PROXY_HOST:
                            proxys.items[i].type = PROXY_SOCKS5;
                            break;
                        default:
                            proxys.items[i].type = PROXY_MAX;
                            break;
                        }
                    }
                    
                    enable = (GetWindowStyle(GetDlgItem(hDlg, IDC_PROXY_AUTO)) & BS_CHECKED)?FALSE:TRUE;
                    mdolphin_set_proxy(&proxys, enable);
err:
                    for (int i=0; i<proxys.num; i++) {
                        if (proxys.items[i].host)
                            free(proxys.items[i].host);
                        if (proxys.items[i].port)
                            free(proxys.items[i].port);
                        if (proxys.items[i].name)
                            free(proxys.items[i].name);
                        if (proxys.items[i].pwd)
                            free(proxys.items[i].pwd);
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

void create_proxy_window(HWND hParent)
{
    PopWinData.controls = CtrlInitProgress;
    initCtrlsStyle();
    DialogBoxIndirectParam(&PopWinData, hParent, ProxyBoxProc, (DWORD)hParent);
}
