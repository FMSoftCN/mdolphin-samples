#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgutils/mgutils.h>

#include <mdolphin/mdolphin.h>

extern HWND mdolphin_hwnd;

static char title_text[256];
static char prompt_text [256];
static char url_text [256];


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
#if 0
    memset (prompt_text, 0 , sizeof(prompt_text));
    if (prompt_box(parent, text, defaultval, prompt_text, sizeof(prompt_text)))
        return prompt_text;
#endif
    return NULL;
}


HWND my_create_window(const char* url, DWORD styles, int x, int y, int width, int height)
{
    return mdolphin_hwnd;
}


void set_callback_info(HWND hWnd)
{
    CB_INFO cb_info;
    memset (&cb_info, 0, sizeof(CB_INFO));
    
    cb_info.CB_MESSAGE_BOX = my_message_callback;
    cb_info.CB_CONFIRM_BOX = my_confirm_callback;
    cb_info.CB_PROMPT_BOX = NULL;
    cb_info.CB_SET_TITLE = NULL;
    cb_info.CB_SET_LOCATION = NULL;
    cb_info.CB_SET_STATUS = NULL;
    cb_info.CB_SET_LOADING_STATUS = NULL;
    cb_info.CB_SET_HISTORY_STATUS = NULL;
    cb_info.CB_CHOOSEFILE_BOX = NULL;
    cb_info.CB_SAVE_FILE_DATA = NULL;
    //cb_info.CB_SET_IME_STATUS = NULL; //set_ime_window;
    cb_info.CB_ERROR = NULL;
    cb_info.CB_PROVIDE_AUTH = NULL;
    cb_info.CB_CREATE_POPUP_MENU = NULL;
    cb_info.CB_OPEN_WINDOW = NULL;
    cb_info.CB_CLOSE_WINDOW = NULL;
    cb_info.CB_SET_TOOLTIP = NULL;
    cb_info.CB_CUSTOM_USERAGENT = NULL;
    cb_info.CB_SAVE_AS_FILE_DATA = NULL;
#if ENABLE_SSL
#if ENABLE_SSLFILE
    cb_info.CB_PROVIDE_CLIENT_CERT = NULL;
#else
    cb_info.CB_PROVIDE_CLIENT_CERT = NULL;
#endif
    cb_info.CB_VERIFY_SERVER_CERT = NULL;
#endif
    HWND mdolphin_cHwnd = mdolphin_hwnd;
    if (mdolphin_cHwnd)
        mdolphin_set_callback_info(mdolphin_cHwnd, &cb_info);
}

