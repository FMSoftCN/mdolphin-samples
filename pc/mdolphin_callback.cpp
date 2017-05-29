#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mdolphin_app.h"
#ifdef MINIGUI_V3
#include <mgutils/mgutils.h>
#else
#include <minigui/mgext.h>
#include <minigui/newfiledlg.h>
#include <minigui/mywindows.h>
#endif

#include <mdolphin/mdolphin.h>
#include "mdolphin_cert.h"
#include "mdolphin_menustate.h"
#include "mdolphin_propsheet.h"
#include "mdolphin_menustate.h"
#include "mdolphin_tooltipwin.h"


extern int prompt_box(HWND parent, const char *info, const char *definp, char *buf, int buflen);
extern BOOL my_provide_auth_callbackv3(const char* title, char *username, size_t nameLen,
        char *password, size_t passwordLen);
extern BOOL my_provide_auth_callback(const char* title, char *username, char *password);
extern BOOL my_savefile_callback (int id, const char * url, const void* data, size_t datasize,
       unsigned long totalsize,const char *FileName, const char *MimeType,  BOOL isFinish);
extern BOOL my_create_popup_menu_callback(HWND hWnd, const POPUP_MENU_DATA *data);
extern void my_error_code_callback(HWND hWnd, int errCode, const char* url);
extern BOOL  my_saveas_callback(char * filename);

extern HWND propsheet_hwnd;
extern HWND location_hwnd;
extern HWND status_hwnd;
extern HWND ime_hwnd;

static char title_text[256];
static char prompt_text [256];
static char url_text [256];

static void set_location_text (HWND hWnd, const char * text)
{
    int len = (255 > strlen(text)) ? strlen(text) : 255;
    strncpy(url_text, text, len);
    url_text[len] = '\0';
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_LOCATION, (LPARAM)text);

    HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (active != GetParent(hWnd))
        return;

    if (text)
        SetWindowText (location_hwnd, text);
    else
        SetWindowText (location_hwnd, "");
}

static void set_title_text(HWND hWnd, const char * text)
{
    if (!text) //you minigui not support utf-8 codecs
        return;

    if(mdolphin_in_view_source_mode(hWnd))
    {
        text="SRC";
    }
    else
    {
       char strTmp[256];
       GetWindowText (location_hwnd, strTmp,sizeof(strTmp));
       if(strncmp(strTmp,"ftp://",6)==0)
           text=strTmp;
    }

    int index =SendMessage(propsheet_hwnd, PSM_GETPAGEINDEX, GetParent(hWnd), 0);
    SendMessage(propsheet_hwnd, PSM_SETTITLE, index, (LPARAM)text);

    memset(title_text, 0 , sizeof(title_text));
    strcpy (title_text, "mDolphin - ");
    strncat (title_text, text, 244);
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_TITLE, (LPARAM)title_text);

    HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (active != GetParent(hWnd))
        return;

    SetWindowText(GetParent(propsheet_hwnd), title_text);
}

static void set_status_text (HWND hWnd, const char * text)
{
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_STATUS, (LPARAM)text);

    HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (active != GetParent(hWnd))
        return;
    if (text)
        SetWindowText (status_hwnd, text);
    else
        SetWindowText (status_hwnd, "");
}

static void my_message_callback (HWND parent, const char * text, const char * caption)
{
    MessageBox (parent, text, caption, MB_OK);
}

static BOOL my_confirm_callback (HWND parent, const char * text, const char * caption)
{
    if (MessageBox (parent, text, caption, MB_OKCANCEL|MB_ICONINFORMATION) == IDOK)
        return TRUE;
    return FALSE;
}

static char * my_prompt_callback (HWND parent, const char* text, const char* defaultval, const char* caption)
{
    memset (prompt_text, 0 , sizeof(prompt_text));
    if (prompt_box(parent, text, defaultval, prompt_text, sizeof(prompt_text)))
        return prompt_text;
    return NULL;
}

static void set_loading_status (HWND hWnd, BOOL load, unsigned int progress)
{
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_LOAD, (LPARAM)load);
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_PROGRESS, (LPARAM)progress);

    HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (active != GetParent(hWnd))
        return;
    change_loading_menu_status(load, progress);
}

static void set_histroy_status (HWND hWnd, unsigned int bcount, unsigned int fcount, unsigned int capacity)
{
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_BACK, (LPARAM)bcount);
    SendMessage(GetParent(hWnd), MSG_COMMAND, USER_PMS_FORWARD, (LPARAM)fcount);

    HWND active = SendMessage(propsheet_hwnd, PSM_GETACTIVEPAGE, 0, 0);
    if (active != GetParent(hWnd))
        return;

    change_history_menu_status(bcount, fcount);
}

BOOL my_choosefile_callbackv3 (HWND hWnd, char* filename, size_t bufsize)
{
	NEWFILEDLGDATA newfiledlg =
	{
#ifdef MINIGUI_V3
            FALSE, FALSE,
#else
	        FALSE,
#endif
			"",
			"",
			"./",
			"All file(*.*)",
			1
	};

	newfiledlg.IsSave = 0;
	int retvalue = ShowOpenDialog (hWnd, 100, 100, 350, 250, &newfiledlg);
	if (retvalue == IDOK)
		strcpy (filename, newfiledlg.filefullname);
	else
		return false;

	return true;

}

BOOL my_choosefile_callback (HWND hWnd, char* filename, size_t bufsize, BOOL IsSave)
{
	NEWFILEDLGDATA newfiledlg =
	{
#ifdef MINIGUI_V3
            FALSE, FALSE,
#else
	        FALSE,
#endif
			"",
			"",
			"./",
			"All file(*.*)",
			1
	};

	if(IsSave)
		newfiledlg.IsSave = 1;   // IsSave =0 show open file , =1 show save file
	else
		newfiledlg.IsSave = 0;   // IsSave =0 show open file , =1 show save file
	int retvalue = ShowOpenDialog (hWnd, 100, 100, 350, 250, &newfiledlg);
	if (retvalue == IDOK)
		strcpy (filename, newfiledlg.filefullname);
	else
		return false;

	return true;

}

static BOOL set_ime_window(BOOL show)
{
    if (show) {
        SendNotifyMessage(ime_hwnd, MSG_IME_OPEN, 0, 0);
    } else {
		SendNotifyMessage(ime_hwnd, MSG_IME_CLOSE, 0, 0);
	}
	return TRUE;
}

static HWND tooltip_hwnd = 0;
static void my_tooltip(HWND hWnd, int x, int y, const char *text, BOOL bShow)
{
    if (bShow) {
        ClientToScreen(hWnd, &x, &y);
        y += GetSysCharHeight();
        if (!tooltip_hwnd)
            tooltip_hwnd = mdolphin_createToolTipWin(hWnd, x, y, 2000, text);
        else
            mdolphin_resetToolTipWin(tooltip_hwnd, x, y, text);
    } else
        mdolphin_hideToolTip(tooltip_hwnd);
}

static const char *my_useragent(const char *url)
{
    if (strstr(url, ".google.com"))
        return "Mozilla/5.0 (MiniGUI/2.04; U; Linux i686; en) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+) mDolphin";
        return "Mozilla/5.0 (MiniGUI/2.04; U; Linux i686; en) AppleWebKit/525.1+ (KHTML, like Gecko, Safari/525.1+) mDolphin";
    //return "Mozilla/5.0 (MiniGUI/2.04; U; linux-i686; en; rv:1.8.1.12) mDolphin/2.00 Compatible Gecko/20070601";
}

#if ENABLE_SSLFILE
static BOOL my_provide_client_cert(MD_CERT_DATA **x509, MD_SSL_PKEY **pkey, const MD_CERT_NAME **names, int count)
{
    int i;
    if (count) {
        for (i = 0; i < count; i++) {
            if (readCertificateHome(X509_NAME_hash((X509_NAME *)names[i]), (X509**)x509, (EVP_PKEY**)pkey))
                return TRUE;
        }
        MessageBox(GetParent(propsheet_hwnd),
                "No client certificate! Please select your certificate!", "Alert", MB_OK);

        if (importCertificate(GetParent(propsheet_hwnd))) {
            if (count) {
                for (i = 0; i < count; i++) {
                    if (readCertificateHome(X509_NAME_hash((X509_NAME *)names[i]), (X509**)x509, (EVP_PKEY**)pkey))
                        return TRUE;
                }
            }
        }
    }
    return FALSE;
}
#endif
#if ENABLE_SSL
static MD_CERT_RESULT my_verify_server_cert(MD_CERT_RESULT result, MD_CERT_DATA *x509)
{
    return verifyCertificate(GetParent(propsheet_hwnd), result, (X509*)x509);
}
#endif

void set_callback_info(HWND hWnd)
{
    CB_INFO cb_info;
    memset (&cb_info, 0, sizeof(CB_INFO));

    cb_info.CB_MESSAGE_BOX = my_message_callback;
    cb_info.CB_CONFIRM_BOX = my_confirm_callback;
    cb_info.CB_PROMPT_BOX = my_prompt_callback;
    cb_info.CB_SET_TITLE = set_title_text;
    cb_info.CB_SET_LOCATION = set_location_text;
    cb_info.CB_SET_STATUS = set_status_text;
    cb_info.CB_SET_LOADING_STATUS = set_loading_status;
    cb_info.CB_SET_HISTORY_STATUS = set_histroy_status;
#if MDOLPHIN_V3
    cb_info.CB_CHOOSEFILE_BOX = my_choosefile_callbackv3;
#else
    cb_info.CB_CHOOSEFILE_BOX = my_choosefile_callback;
#endif
    cb_info.CB_SAVE_FILE_DATA = my_savefile_callback;
    //cb_info.CB_SET_IME_STATUS = NULL; //set_ime_window;
    cb_info.CB_ERROR = my_error_code_callback;
#if MDOLPHIN_V3
    cb_info.CB_PROVIDE_AUTH = my_provide_auth_callbackv3;
#else
    cb_info.CB_PROVIDE_AUTH = my_provide_auth_callback;
#endif
    cb_info.CB_CREATE_POPUP_MENU = my_create_popup_menu_callback;
    cb_info.CB_OPEN_WINDOW = my_create_new_window;
    cb_info.CB_CLOSE_WINDOW = my_close_tab_window;
    cb_info.CB_SET_TOOLTIP = my_tooltip;
    cb_info.CB_CUSTOM_USERAGENT = my_useragent;
    cb_info.CB_SAVE_AS_FILE_DATA = my_saveas_callback;
#if ENABLE_SSL
#if ENABLE_SSLFILE
    cb_info.CB_PROVIDE_CLIENT_CERT = my_provide_client_cert;
#else
    cb_info.CB_PROVIDE_CLIENT_CERT = NULL;
#endif
    cb_info.CB_VERIFY_SERVER_CERT = my_verify_server_cert;
#endif
    HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
    if (mdolphin_cHwnd)
        mdolphin_set_callback_info(mdolphin_cHwnd, &cb_info);
}

