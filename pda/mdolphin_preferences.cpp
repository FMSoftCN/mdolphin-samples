#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mdolphin_app.h"

extern HWND mdolphin_hwnd;

#if ENABLE_PLUGIN	
//extern void create_plugins_manage_window(HWND hParent);
#endif 

#define IDC_SET_DEF_LANGUAGE            610 /*0*/
#define IDC_SET_DEF_ENCODING            611 /*3*/
#define IDC_SET_STD_FONT                612 /*0*/
#define IDC_SET_SERIF_FONT              613 /*0*/
#define IDC_SET_FIXED_FONT              614 /*0*/
#define IDC_SET_SANS_FONT               615 /*0*/
#define IDC_SET_FONT_SIZE               616 /*8*/
#define IDC_SET_FIXED_FONT_SIZE         617 /*5*/
#define IDC_SET_AUTOLOAD_IMAGES         618 /*TRUE*/
#define IDC_SET_ENABLE_JS               619 /*TRUE*/
#define IDC_SET_ENABLE_COOKIES          620 /*TRUE*/

#if ENABLE_PLUGIN	
#define IDC_SET_ENABLE_PLUGIN           621 /*TRUE*/
#define IDC_SET_PLUGIN                  622
#endif

#define IDC_SET_BLOCK_POPUPWINDOW       623 /*TRUE*/


static SETUP_INFO setup_info;

static const char* default_language[]=
{
    "en",
    "zh"
};

static void default_language_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<(int)TABLESIZE(default_language); i++) {
        SendDlgItemMessage(hDlg, IDC_SET_DEF_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)default_language[i]);
        if (!strcmp(default_language[i], setup_info.default_language)) 
            SendDlgItemMessage(hDlg, IDC_SET_DEF_LANGUAGE, CB_SETCURSEL, i, 0);
    }
}

// static void language_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)	// gengyue
static void language_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        memset(setup_info.default_language, 0, sizeof(setup_info.default_language));
        strcpy(setup_info.default_language, default_language[num]);
    }
}

static const char* default_encoding[]=
{
  "ISO8859-1",
  "ISO8859-2",
  "ISO8859-3",
  "ISO8859-4",
  "ISO8859-5",
  "ISO8859-6",
  "ISO8859-7",
  "ISO8859-8",
  "ISO8859-9",
  "ISO8859-10",
  "ISO8859-11",
  "ISO8859-12",
  "ISO8859-13",
  "ISO8859-14",
  "ISO8859-15",
  "ISO8859-16",
  "EUC-CN",
  "GB1988-0",      
  "GB2312-0",      
  "GBK",
  "GB18030-0",     
  "BIG5",
  "KSC5636-0",     
  "KSC5601-0",     
  "EUC-JP",
  "JISX0201-0",    
  "JISX0208-0",    
  "SHIFT-JIS",
  "JISX0201-1",    
  "JISX0208-1",    
  "ISO-10646-1",   
  "UTF-8",
};

static void default_encoding_current_position(HWND hDlg)
{
    for (int i=0; i < (int)TABLESIZE(default_encoding); i++) {
        SendDlgItemMessage(hDlg, IDC_SET_DEF_ENCODING, CB_ADDSTRING, 0, (LPARAM)default_encoding[i]);
        if (i == (int)setup_info.default_encoding) 
            SendDlgItemMessage(hDlg, IDC_SET_DEF_ENCODING, CB_SETCURSEL, i, 0);
    }
}

//static void encoding_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)		// gengyue
static void encoding_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        setup_info.default_encoding = (ENCODING_NAME)num;
    }
}

static const char* std_font[]=
{
    "Times New Roman",
    "Arial",
};

static void std_font_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<(int)TABLESIZE(std_font); i++) {
        SendDlgItemMessage(hDlg, IDC_SET_STD_FONT, CB_ADDSTRING, 0, (LPARAM)std_font[i]);
        if (!strcmp(std_font[i], setup_info.std_font)) 
            SendDlgItemMessage(hDlg, IDC_SET_STD_FONT, CB_SETCURSEL, i, 0);
    }
}

//static void std_font_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)		// gengyue
static void std_font_notif_proc(HWND hDlg, int long id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        memset(setup_info.std_font, 0, sizeof(setup_info.std_font));
        strcpy(setup_info.std_font, std_font[num]);
    }
}

static const char* serif_font[]=
{
    "Times New Roman",
    "Arial",
};

static void serif_font_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<(int)TABLESIZE(serif_font); i++) {
        SendDlgItemMessage(hDlg, IDC_SET_SERIF_FONT, CB_ADDSTRING, 0, (LPARAM)serif_font[i]);
        if (!strcmp(serif_font[i], setup_info.serif_font)) 
            SendDlgItemMessage(hDlg, IDC_SET_SERIF_FONT, CB_SETCURSEL, i, 0);
    }
}

//static void serif_font_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)		// gengyue
static void serif_font_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        memset(setup_info.serif_font, 0, sizeof(setup_info.serif_font));
        strcpy(setup_info.serif_font, serif_font[num]);
    }
}

static const char* fixed_font[]=
{
    "Courier New",
};

static void fixed_font_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<1; i++) {
        SendDlgItemMessage(hDlg, IDC_SET_FIXED_FONT, CB_ADDSTRING, 0, (LPARAM)fixed_font[i]);
        if (!strcmp(fixed_font[i], setup_info.fixed_font)) 
            SendDlgItemMessage(hDlg, IDC_SET_FIXED_FONT, CB_SETCURSEL, i, 0);
    }
}

//static void fixed_font_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)		// gengyue
static void fixed_font_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        memset(setup_info.fixed_font, 0, sizeof(setup_info.fixed_font));
        strcpy(setup_info.fixed_font, fixed_font[num]);
    }
}

static const char* sans_serif_font[]=
{
    "Arial",
};

static void sans_serif_font_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<1; i++) {
        SendDlgItemMessage(hDlg, IDC_SET_SANS_FONT, CB_ADDSTRING, 0, (LPARAM)sans_serif_font[i]);
        if (!strcmp(sans_serif_font[i], setup_info.sans_serif_font)) 
            SendDlgItemMessage(hDlg, IDC_SET_SANS_FONT, CB_SETCURSEL, i, 0);
    }
}

//static void sans_font_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)	// gengyue
static void sans_font_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        memset(setup_info.sans_serif_font, 0, sizeof(setup_info.sans_serif_font));
        strcpy(setup_info.sans_serif_font, sans_serif_font[num]);
    }
}

static const char* medium_fontsize[]=
{
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
};

static void medium_fontsize_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<13; i++) {
        SendDlgItemMessage(hDlg, IDC_SET_FONT_SIZE, CB_ADDSTRING, 0, (LPARAM)medium_fontsize[i]);
        if (atoi(medium_fontsize[i]) == setup_info.medium_fontsize) 
            SendDlgItemMessage(hDlg, IDC_SET_FONT_SIZE, CB_SETCURSEL, i, 0);
    }
}

//static void font_size_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)	// gengyue
static void font_size_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        setup_info.medium_fontsize = atoi(medium_fontsize[num]);
    }
}

static const char* medium_fixed_fontsize[]=
{
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
};

static void medium_fixed_fontsize_current_position(HWND hDlg)
{
    int i;
    for (i=0; i<13; i++) {
        SendDlgItemMessage(hDlg, IDC_SET_FIXED_FONT_SIZE, CB_ADDSTRING, 0, (LPARAM)medium_fixed_fontsize[i]);
        if (atoi(medium_fixed_fontsize[i]) == setup_info.medium_fixed_fontsize) 
            SendDlgItemMessage(hDlg, IDC_SET_FIXED_FONT_SIZE, CB_SETCURSEL, i, 0);
    }
}

//static void fixed_size_notif_proc(HWND hDlg, int id, int nc, DWORD add_data)	// gengyue
static void fixed_size_notif_proc(HWND hDlg, long int id, int nc, DWORD add_data)
{
    int num;
    if (nc == CBN_SELCHANGE) {
        num = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
        setup_info.medium_fixed_fontsize = atoi(medium_fixed_fontsize[num]);
    }
}

//static void images_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)	// gengyue
static void images_notif_proc(HWND hwnd, long int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            setup_info.autoload_images = TRUE;
        else
            setup_info.autoload_images = FALSE;
    }
}

//static void js_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)	// gengyue
static void js_notif_proc(HWND hwnd, long int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            setup_info.enable_javascript = TRUE;
        else
            setup_info.enable_javascript = FALSE;
    }
}

//static void cookies_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)	// gengyue
static void cookies_notif_proc(HWND hwnd, long int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            setup_info.enable_cookies = TRUE;
        else
            setup_info.enable_cookies = FALSE;
    }
}

//static void popupwindow_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)	// gengyue
static void popupwindow_notif_proc(HWND hwnd, long int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED )
            setup_info.block_popupwindow = TRUE;
        else
            setup_info.block_popupwindow = FALSE;
    }
}

#if ENABLE_PLUGIN	
static void plugin_notif_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        if (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED ) {
            setup_info.enable_plugin = TRUE;
            EnableWindow(GetDlgItem(GetParent(hwnd), IDC_SET_PLUGIN), TRUE);
        } else {
            setup_info.enable_plugin = FALSE;
            EnableWindow(GetDlgItem(GetParent(hwnd), IDC_SET_PLUGIN), FALSE);
        }
    }
}
#endif

static void set_notify_proc(HWND hDlg)
{
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_DEF_LANGUAGE), language_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_DEF_ENCODING), encoding_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_STD_FONT), std_font_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_SERIF_FONT), serif_font_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_FIXED_FONT), fixed_font_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_SANS_FONT), sans_font_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_FONT_SIZE), font_size_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_FIXED_FONT_SIZE), fixed_size_notif_proc);

    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_AUTOLOAD_IMAGES), images_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_ENABLE_JS), js_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_ENABLE_COOKIES), cookies_notif_proc);
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_BLOCK_POPUPWINDOW), popupwindow_notif_proc);

#if ENABLE_PLUGIN	
    SetNotificationCallback (GetDlgItem (hDlg, IDC_SET_ENABLE_PLUGIN), plugin_notif_proc);
#endif
}

static void init_setup_info(HWND hDlg)
{ 
    mdolphin_fetch_setup(mdolphin_hwnd, &setup_info);
    default_language_current_position(hDlg);
    default_encoding_current_position(hDlg);
    std_font_current_position(hDlg);
    serif_font_current_position(hDlg);
    fixed_font_current_position(hDlg);
    sans_serif_font_current_position(hDlg);
    medium_fontsize_current_position(hDlg);
    medium_fixed_fontsize_current_position(hDlg);

    if (setup_info.autoload_images) 
        SendMessage (GetDlgItem (hDlg, IDC_SET_AUTOLOAD_IMAGES), BM_SETCHECK, BST_CHECKED, 0);
    else 
        SendMessage (GetDlgItem (hDlg, IDC_SET_AUTOLOAD_IMAGES), BM_SETCHECK, BST_UNCHECKED, 0);
    
    if (setup_info.enable_javascript) 
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_JS), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_JS), BM_SETCHECK, BST_UNCHECKED, 0);

    if (setup_info.enable_cookies)
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_COOKIES), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_COOKIES), BM_SETCHECK, BST_UNCHECKED, 0);

    if (setup_info.block_popupwindow)
        SendMessage (GetDlgItem (hDlg, IDC_SET_BLOCK_POPUPWINDOW), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage (GetDlgItem (hDlg, IDC_SET_BLOCK_POPUPWINDOW), BM_SETCHECK, BST_UNCHECKED, 0);

#if ENABLE_PLUGIN	
    if (setup_info.enable_plugin) {
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_PLUGIN), BM_SETCHECK, BST_CHECKED, 0);
        EnableWindow(GetDlgItem (hDlg, IDC_SET_PLUGIN), TRUE);
    } else {
        SendMessage (GetDlgItem (hDlg, IDC_SET_ENABLE_PLUGIN), BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(GetDlgItem (hDlg, IDC_SET_PLUGIN), FALSE);
    }
#endif

    set_notify_proc(hDlg);
}

static DLGTEMPLATE SettingsWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , PRE_WIDTH, PRE_HEIGHT,
     "Preferences",
     0, 0,
#if ENABLE_PLUGIN	
     27,
#else
     25,
#endif
     NULL, 0
};

static CTRLDATA CtrlInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
		5*PRE_WIDTH/336,  5*PRE_HEIGHT/390,
		PRE_WIDTH - 10,  40*PRE_HEIGHT/390,
        0,
        "Language",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
		100*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Default Language",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		120*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_DEF_LANGUAGE,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
		5*PRE_WIDTH/336,  45*PRE_HEIGHT/390,
		PRE_WIDTH - 10,  40*PRE_HEIGHT/390,
        0,
        "Encoding",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  60*PRE_HEIGHT/390,
		100*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Default Encoding",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		120*PRE_WIDTH/336,  60*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_DEF_ENCODING,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
		5*PRE_WIDTH/336,  90*PRE_HEIGHT/390,
		320*PRE_WIDTH/336,  165*PRE_HEIGHT/390,
        0,
        "Font",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  110*PRE_HEIGHT/390,
		90*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Standard Font",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		110*PRE_WIDTH/336,  110*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_STD_FONT,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  140*PRE_HEIGHT/390,
		90*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Serif Font",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		110*PRE_WIDTH/336,  140*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_SERIF_FONT,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  170*PRE_HEIGHT/390,
		90*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Fixed Font",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		110*PRE_WIDTH/336,  170*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_FIXED_FONT,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  200*PRE_HEIGHT/390,
		90*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Sans Font",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		110*PRE_WIDTH/336,  200*PRE_HEIGHT/390,
		200*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_SANS_FONT,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_SIMPLE,
		10*PRE_WIDTH/336,  230*PRE_HEIGHT/390,
		60*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Font Size",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		80*PRE_WIDTH/336,  230*PRE_HEIGHT/390,
		60*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_FONT_SIZE,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_TABSTOP | WS_VISIBLE | SS_SIMPLE,
		150*PRE_WIDTH/336,  230*PRE_HEIGHT/390,
		100*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        0,
        "Fixed Font Size",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_COMBOBOX,
        WS_TABSTOP | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_READONLY | CBS_NOTIFY,
		250*PRE_WIDTH/336,  230*PRE_HEIGHT/390,
		60*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_FIXED_FONT_SIZE,
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY,
		10*PRE_WIDTH/336,  260*PRE_HEIGHT/390,
		150*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_AUTOLOAD_IMAGES,
        "Autoload images",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY,
		170*PRE_WIDTH/336,  260*PRE_HEIGHT/390,
		150*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_ENABLE_JS,
        "Enable Javascript",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY,
		10*PRE_WIDTH/336,  290*PRE_HEIGHT/390,
		150*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_ENABLE_COOKIES,
        "Enable Cookies",
        0, WS_EX_TRANSPARENT
    },
#if ENABLE_PLUGIN	
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY,
		130*PRE_WIDTH/336,  290*PRE_HEIGHT/390,
		110*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
        IDC_SET_ENABLE_PLUGIN,
        "Enable Plug-Ins",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_NOTIFY,
		245*PRE_WIDTH/336,  290*PRE_HEIGHT/390,
		80*PRE_WIDTH/336,  20*PRE_HEIGHT/390, 
        IDC_SET_PLUGIN,
        "Plug-Ins",
        0, WS_EX_TRANSPARENT
    },
#endif
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX | BS_NOTIFY,
		170*PRE_WIDTH/336,  290*PRE_HEIGHT/390,
		150*PRE_WIDTH/336,  20*PRE_HEIGHT/390,
		//10*PRE_WIDTH/336,  320*PRE_HEIGHT/390,
		//120*PRE_WIDTH/336,  20*PRE_HEIGHT/390, 
        IDC_SET_BLOCK_POPUPWINDOW,
        "Block PopupWindows",
        0, WS_EX_TRANSPARENT
    },
    
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
		51*PRE_WIDTH/336,  320*PRE_HEIGHT/390,
		//135*PRE_WIDTH/336,  320*PRE_HEIGHT/390,
		92*PRE_WIDTH/336,  25*PRE_HEIGHT/390,
		//60*PRE_WIDTH/336,  25*PRE_HEIGHT/390,
        IDOK, 
        "Ok",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
		170*PRE_WIDTH/336,  320*PRE_HEIGHT/390,
		//210*PRE_WIDTH/336,  320*PRE_HEIGHT/390,
		92*PRE_WIDTH/336,  25*PRE_HEIGHT/390,
		//70*PRE_WIDTH/336,  25*PRE_HEIGHT/390,
        IDCANCEL, 
        "Cancel",
        0
    }
};

//static int SettingsProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)	// gengyue
static LRESULT SettingsProc (HWND hDlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    switch (message) {
        case MSG_INITDIALOG: 
            init_setup_info (hDlg);
            GetWindowRect ((HWND)(lParam), &rc);
            //MoveWindow (hDlg , (rc.right-rc.left)/2-168 , (rc.bottom-rc.top)/2-195, 336 , 390 , TRUE);
            return 1;
        case MSG_COMMAND:
            switch (wParam) {
#if ENABLE_PLUGIN	
                case IDC_SET_PLUGIN:
					MessageBox(hDlg,"you should using plugin is static", "plugin", MB_OK);
                    //create_plugins_manage_window(hDlg);
                    break;
#endif
                case IDOK:
                    {
                            mdolphin_import_setup(mdolphin_hwnd, &setup_info);
                        EndDialog (hDlg, wParam);
                    }
                    break;
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

void create_settings_window(HWND hParent)
{
    SettingsWinData.controls = CtrlInitProgress;
    DialogBoxIndirectParam (&SettingsWinData, hParent, SettingsProc, (DWORD)hParent);
}

