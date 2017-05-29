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
#endif

#include <mdolphin/mdolphin.h>
#include "mdolphin_app_res.h"
#include "mdolphin_cert.h"
#include "mdolphin_bookmarks.h"
#include "mdolphin_propsheet.h"
#include "mdolphin_menustate.h"
#include "mdolphin_scheme.h"
#include "mdolphin_plugins.h"

#if ENABLE_JSNATIVEBINDING
#include "mdolphin_native.h"
#endif
#include <mdolphin/IMDWebView.h>
#include <iostream>

BITMAP ntb_bitmap;

HWND propsheet_hwnd;
HWND toolbar_hwnd ;
HWND location_hwnd;
HWND progressbar_hwnd;
HWND progress_hwnd;
HWND status_hwnd;
HWND hMainWnd;
HMENU main_menu;

const char* lang="zh_cn";

static int win_width = 800;
static int win_height = 600;

const char **pmain_menu = NULL;
const char **pfile_menu_item =NULL;
const char **pedit_menu_item =NULL;
const char **pview_menu_item =NULL;
const char **ptext_menu_item =NULL;
const char **pchar_menu_item =NULL;
const char **pgo_menu_item =NULL;
const char **pbookmark_menu_item =NULL;
const char **phelp_menu_item =NULL;


static const char * home_url = "http://www.minigui.com/";
extern void set_callback_info(HWND hParent);
extern void create_proxy_window(HWND hParent);
extern void create_settings_window(HWND hParent);
extern void create_find_window(HWND hParent);
extern void find_again_with_last_result(void);
/*popup menu click process function*/
extern void popup_menu_click_proc(int id);

static PLOGFONT cap_font = NULL;
static PLOGFONT ctrl_font = NULL;
static PLOGFONT menu_font = NULL;
static PLOGFONT utf8_font = NULL;


static const char* main_menu_cn[] = {
    "文件",
    "编辑",
    "查看",
    "转到",
    "书签",
    "帮助"
};

static const char* main_menu_tw[] = {
    "文件",
    "編輯",
    "查看",
    "轉到",
    "書簽",
    "幫助"
};

static const char* main_menu_en[] = {
    "File",
    "Edit",
    "View",
    "Go",
    "Bookmarks",
    "Help"
};

/************************************************
 *simple chinese menu
 *
 * **********************************************/
static const char* file_menu_item_cn[] = {
    "新建标签页   Ctrl+T",
    "打开文件...  Ctrl+O",
    "关闭标签页   Ctrl+W",
    "Separator",
    "退出"
};

static const char* edit_menu_item_cn[] = {
    "代理设置...",
#if ENABLE_SSLFILE
    "载入证书...",
#endif
    "Separator",
    "在当前页查找... Ctrl+F",
    "查找            Ctrl+G",
    "Separator",
    "选项   Ctrl+N",
};

static const char* view_menu_item_cn[] = {
    "文字大小",
    "字符编码",
    "Separator",
    "页面源代码  Ctrl+U"
};

static const char* text_menu_item_cn[] = {
    "放大 Ctrl++ ",
    "缩小 Ctrl+- ",
    "正常 Ctrl+0 "
};

static const char* char_menu_item_cn[] = {
    "自动检测",
    "Separator",
    "西方 (ISO-8859-1)",
    "Unicode (UTF-8)",
    "简体中文 (GB2312)",
    "繁体中文 (BIG5)",
    "简体中文 (GBK)"
};

static const char* go_menu_item_cn[] = {
    "后退",
    "前进",
    "重新载入",
    "停止",
    "主页"
};

static const char* bookmark_menu_item_cn[] = {
    "添加本页... Ctrl+D",
    "管理书签...",
    "Separator",
    "书签"
};

static const char* help_menu_item_cn[] = {
    "关于"
};

/************************************************
 *traditional chinese menu
 *
 * **********************************************/

static const char* file_menu_item_tw[] = {
    "新建標簽頁   Ctrl+T",
    "打開文件...  Ctrl+O",
    "關閉標簽頁   Ctrl+W",
    "Separator",
    "退出"
};

static const char* edit_menu_item_tw[] = {
    "代理設置...",
#if ENABLE_SSLFILE
    "載入証書...",
#endif
    "Separator",
    "在當前頁查找... Ctrl+F",
    "查找   Ctrl+G",
    "Separator",
    "選項   Ctrl+N",
};

static const char* view_menu_item_tw[] = {
    "文字大小",
    "字符編碼",
    "Separator",
    "頁面源代碼  Ctrl+U"
};

static const char* text_menu_item_tw[] = {
    "放大 Ctrl++ ",
    "縮小 Ctrl+- ",
    "正常 Ctrl+0 "
};

static const char* char_menu_item_tw[] = {
    "自動檢測 ",
    "Separator",
    "西方 (ISO-8859-1) ",
    "Unicode (UTF-8) ",
    "簡體中文 (GB2312) ",
    "繁體中文 (BIG5) ",
    "簡體中文 (GBK) "
};

static const char* go_menu_item_tw[] = {
    "後退",
    "前進",
    "重新載入",
    "停止",
    "主頁"
};

static const char* bookmark_menu_item_tw[] = {
    "添加本頁... Ctrl+D",
    "管理書簽...",
    "Separator",
    "書簽"
};

static const char* help_menu_item_tw[] = {
    "關於"
};

/************************************************
 *english chinese menu
 *
 * **********************************************/
static const char* file_menu_item[] = {
    "New Tab      Ctrl+T",
    "Open File... Ctrl+O",
    "Close Tab    Ctrl+W",
    "Separator",
    "Quit"
};

static const char* edit_menu_item[] = {
    "Proxy Setting...",
#if ENABLE_SSLFILE
    "Import Certificate...",
#endif
    "Separator",
    "Find in This Page... Ctrl+F",
    "Find Again           Ctrl+G",
    "Separator",
    "Preferences          Ctrl+N",
};

static const char* view_menu_item[] = {
    "Text Size",
    "Character Encoding",
    "Separator",
    "Page Source        Ctrl+U"
};

static const char* text_menu_item[] = {
    "Increase Ctrl++ ",
    "Decrease Ctrl+- ",
    "Normal   Ctrl+0 "
};

static const char* char_menu_item[] = {
    "Auto-Detect ",
    "Separator",
    "Western (ISO-8859-1) ",
    "Unicode (UTF-8) ",
    "Chinese Simplified (GB2312) ",
    "Chinese Traditional (BIG5) ",
    "Chinese Simplified (GBK) "
};

static const char* go_menu_item[] = {
    "Back",
    "Forward",
    "Reload",
    "Stop",
    "Home"
};

static const char* bookmark_menu_item[] = {
    "Bookmark This Page... Ctrl+D",
    "Manage Bookmarks...",
    "Separator",
    "Bookmarks"
};

static const char* help_menu_item[] = {
    "About"
};

static void init_mdolphin()
{
    cap_font = g_SysLogFont[SYSLOGFONT_CAPTION];
    ctrl_font = g_SysLogFont[SYSLOGFONT_CONTROL];
    menu_font = g_SysLogFont[SYSLOGFONT_MENU];

    utf8_font = CreateLogFontByName("*-Arial-rrncnn-*-12-UTF-8");

    g_SysLogFont[SYSLOGFONT_CAPTION] = utf8_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = utf8_font;
    g_SysLogFont[SYSLOGFONT_MENU] = utf8_font;

    if(strcasecmp(lang,"zh_cn")==0){
        local_SysText = GetSysTextInUTF8("zh_CN");
    }
    else if(strcasecmp(lang,"zh_tw")==0){
        local_SysText = GetSysTextInUTF8("zh_TW");
    }
    else
       local_SysText = GetSysTextInUTF8("EN");

#if defined (_MGRM_PROCESSES) 
   InitVectorialFonts(); 
#endif
}

static void release_mdolphin()
{
    g_SysLogFont[SYSLOGFONT_CAPTION] = cap_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = ctrl_font;
    g_SysLogFont[SYSLOGFONT_MENU] = menu_font;

    DestroyLogFont(utf8_font);
}


static HMENU create_submenu(const char* menu_item[], int nr_item, int idm)
{
    HMENU menu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"";
    menu = CreatePopupMenu(&mii);

    for (int i = 0; i < nr_item; ++i)
    {
        memset (&mii, 0, sizeof(mii));
        mii.type = MFT_STRING;
        mii.state = MFS_ENABLED;
        mii.hsubmenu = 0;
        mii.id = idm + i;
        if (strstr( menu_item[i],"Separator")) {
            mii.type = MFT_SEPARATOR;
            InsertMenuItem (menu, i, TRUE, &mii);
            continue;
        }
        if (mii.id == IDM_VIEW_CHARENCODE_AUTO || mii.id == IDM_VIEW_CHARENCODE_ISO8859_1 
                || mii.id == IDM_VIEW_CHARENCODE_UTF8 || mii.id == IDM_VIEW_CHARENCODE_GB2312 
                || mii.id == IDM_VIEW_CHARENCODE_BIG5
                || mii.id == IDM_VIEW_CHARENCODE_GBK)
            mii.type = MFT_RADIOCHECK;

        if (mii.id == IDM_VIEW_CHARENCODE_ISO8859_1 
                || mii.id == IDM_VIEW_CHARENCODE_UTF8 || mii.id == IDM_VIEW_CHARENCODE_BIG5 || mii.id == IDM_VIEW_CHARENCODE_GB2312 
                || mii.id == IDM_VIEW_CHARENCODE_GBK) 
            mii.state = MFS_UNCHECKED; 

        if (mii.id == IDM_VIEW_CHARENCODE_AUTO )
            mii.state = MFS_CHECKED;

        if (mii.id == IDM_GO_BACK || mii.id == IDM_GO_FORWARD || mii.id == IDM_FILE_CLOSE_TAB) 
            mii.state = MFS_GRAYED; 
        
        if (mii.id == IDM_VIEW_TEXTSIZE)
            mii.hsubmenu = create_submenu(text_menu_item, TABLESIZE(text_menu_item), IDM_VIEW_TEXTSIZE_ZOOMIN);
        
        if (mii.id == IDM_VIEW_CHARENCODE)
            mii.hsubmenu = create_submenu(char_menu_item, TABLESIZE(char_menu_item), IDM_VIEW_CHARENCODE_AUTO);

        if (mii.id == IDM_BOOK_MARKS) 
            mii.hsubmenu = create_bookmarks_submenu();

        mii.typedata = (DWORD)menu_item[i];
        InsertMenuItem (menu, i, TRUE, &mii);
    }
    menu = StripPopupHead(menu);
    return menu;
}

static HMENU create_menu()
{
    MENUITEMINFO mii;

    main_menu = CreateMenu();

    if(strcasecmp(lang,"zh_cn")==0){
        pmain_menu = main_menu_cn;
        pfile_menu_item = file_menu_item_cn;
        pedit_menu_item = edit_menu_item_cn;
        pview_menu_item = view_menu_item_cn;
        ptext_menu_item = text_menu_item_cn;
        pchar_menu_item = char_menu_item_cn;
        pgo_menu_item = go_menu_item_cn;
        pbookmark_menu_item = bookmark_menu_item_cn;
        phelp_menu_item = help_menu_item_cn;
    }
    else if(strcasecmp(lang,"zh_tw")==0){
        pmain_menu = main_menu_tw;
        pfile_menu_item = file_menu_item_tw;
        pedit_menu_item = edit_menu_item_tw;
        pview_menu_item = view_menu_item_tw;
        ptext_menu_item = text_menu_item_tw;
        pchar_menu_item = char_menu_item_tw;
        pgo_menu_item = go_menu_item_tw;
        pbookmark_menu_item = bookmark_menu_item_tw;
        phelp_menu_item = help_menu_item_tw;
    }
    else{
        pmain_menu = main_menu_en;
        pfile_menu_item = file_menu_item;
        pedit_menu_item = edit_menu_item;
        pview_menu_item = view_menu_item;
        ptext_menu_item = text_menu_item;
        pchar_menu_item = char_menu_item;
        pgo_menu_item = go_menu_item;
        pbookmark_menu_item = bookmark_menu_item;
        phelp_menu_item = help_menu_item;
    }

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_FILE;
    mii.typedata    = (DWORD)pmain_menu[0];
    mii.hsubmenu    = create_submenu(pfile_menu_item, TABLESIZE(file_menu_item), IDM_FILE_NEW_TAB);
    InsertMenuItem(main_menu, POS_FILE_MENU, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_EDIT;
    mii.typedata    = (DWORD)pmain_menu[1];
    mii.hsubmenu    = create_submenu(pedit_menu_item, TABLESIZE(edit_menu_item), IDM_EDIT_PROXY);
    InsertMenuItem(main_menu, POS_EDIT_MENU, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_VIEW;
    mii.typedata    = (DWORD)pmain_menu[2];
    mii.hsubmenu    = create_submenu(pview_menu_item, TABLESIZE(view_menu_item), IDM_VIEW_TEXTSIZE);
    InsertMenuItem(main_menu, POS_VIEW_MENU, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_GO;
    mii.typedata    = (DWORD)pmain_menu[3];
    mii.hsubmenu    = create_submenu(pgo_menu_item, TABLESIZE(go_menu_item), IDM_GO_BACK);
    InsertMenuItem(main_menu, POS_GO_MENU, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_BOOK;
    mii.typedata    = (DWORD)pmain_menu[4];
    mii.hsubmenu    = create_submenu(pbookmark_menu_item, TABLESIZE(bookmark_menu_item), IDM_BOOK_ADD);
    InsertMenuItem(main_menu, POS_BOOKMARKS_MENU, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_MAIN_HELP;
    mii.typedata    = (DWORD)pmain_menu[5];
    mii.hsubmenu    = create_submenu(phelp_menu_item, TABLESIZE(help_menu_item), IDM_HELP_ABOUT);
    InsertMenuItem(main_menu, POS_HELP_MENU, TRUE, &mii);
    return main_menu;
}

static void create_status_window(HWND hwnd_parent, int width, int height)
{
    status_hwnd = CreateWindow (CTRL_STATIC, "",
            WS_VISIBLE | SS_LEFT | SS_NOTIFY | WS_BORDER,
            IDC_STATUSBAR, 0,
            height - STATUSBAR_HEIGHT,
            width - PROGRESSBAR_WIDTH, STATUSBAR_HEIGHT, hwnd_parent, 0);
}

static void create_progress_window(HWND hwnd_parent, int width, int height)
{
    progressbar_hwnd = CreateWindow (CTRL_STATIC, "",
            WS_VISIBLE | WS_BORDER,
            IDC_PROGRESSBAR,
            win_width - PROGRESSBAR_WIDTH,
            height - STATUSBAR_HEIGHT,
            PROGRESSBAR_WIDTH-5, STATUSBAR_HEIGHT-1, hwnd_parent, 0);
    progress_hwnd = CreateWindow (CTRL_PROGRESSBAR, "",
            WS_CHILD,
            IDC_PROGRESS,
            0, 0, 
            PROGRESSBAR_WIDTH-5, STATUSBAR_HEIGHT-2, progressbar_hwnd, 0);
    SendMessage (progress_hwnd, PBM_SETRANGE, 0, 1000);
}

void location_entry_open_url(HWND hwnd, int id, int nc, DWORD add_data)
{
    int length;
    char* url_string = NULL;
    if (nc == EN_ENTER)
    {
        HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
        if (!mdolphin_cHwnd)
            return;
        length = SendMessage(hwnd, MSG_GETTEXTLENGTH, 0, 0);
        url_string = (char*)malloc (length + 1);
        if (url_string) {
            memset (url_string, 0, length+1);
            GetWindowText (hwnd, url_string, length);
            url_string[length]='\0';
			SendMessage(hwnd, MSG_KILLFOCUS, 0, 0);
            mdolphin_navigate(mdolphin_cHwnd, NAV_GOTO, url_string, FALSE);
            //IMDWebView *view =  GetWebViewInstance(mdolphin_cHwnd);
            //view->loadURL(url_string);
            printf("url = %s\n", url_string);
            SetFocus(mdolphin_cHwnd);
            free (url_string);
        }
    }
}

static void create_location_window(HWND hParent, int width, int height)
{
    location_hwnd = CreateWindowEx (CTRL_SLEDIT, "",
            WS_CHILD | WS_BORDER | WS_VISIBLE,
            0, IDC_LOCATION, TOOLBAR_WIDTH +2, 5, width - TOOLBAR_WIDTH -20, LOCATION_HEIGHT,
            hParent, 0);

    if(location_hwnd == HWND_INVALID)
        printf ("mDolphin: Can not create location bar window.\n");

    SetNotificationCallback (location_hwnd, location_entry_open_url);
}

static void toolbar_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    HWND parent = GetParent (hwnd);

    switch (nc) {
        case OP_NAV_BACKWARD:
            SendMessage (parent, MSG_COMMAND, IDM_GO_BACK, 0);
            break;
        case OP_NAV_FORWARD:
            SendMessage (parent, MSG_COMMAND, IDM_GO_FORWARD, 0);
            break;
        case OP_NAV_RELOAD:
            SendMessage (parent, MSG_COMMAND, IDM_GO_RELOAD, 0);
            break;
        case OP_NAV_STOP:
            SendMessage (parent, MSG_COMMAND, IDM_GO_STOP, 0);
            break;
        case OP_NAV_HOME:
            SendMessage (parent, MSG_COMMAND, IDM_GO_HOME, 0);
            break;
    }
}

static void create_toolbar_window(HWND hParent, int width, int height)
{
    int i;
    NTBINFO ntb_info;
    NTBITEMINFO ntbii;
   
    //FIXME: Do we need the following sentence? 
    memset (&ntb_bitmap, 0, sizeof(ntb_bitmap));
    if (LoadBitmapFromMem (HDC_SCREEN, &ntb_bitmap, new_toolbar_data, toolbar_datasize, "gif")) {
        fprintf (stderr, "Can not load bitmap for toolbar.\n");
        return;
    }

    ntb_info.image = &ntb_bitmap;
    ntb_info.nr_cells = 5;
    ntb_info.nr_cols = 0;
    ntb_info.w_cell = 0;
    ntb_info.h_cell = 0;

    toolbar_hwnd = CreateWindow (CTRL_NEWTOOLBAR,
            "",
            WS_CHILD | WS_VISIBLE | NTBS_HORIZONTAL,
            IDC_TOOLBAR,
            0, 0, TOOLBAR_WIDTH, TOOLBAR_HEIGHT,
            hParent,
            (DWORD) &ntb_info);

    memset (&ntbii, 0, sizeof (ntbii));
    for (i=0; i<5; i++)
    {
        if (i<2 || i ==3)
            ntbii.flags = NTBIF_PUSHBUTTON | NTBIF_DISABLED;
        else
            ntbii.flags = NTBIF_PUSHBUTTON;

        ntbii.id = OP_NAV_BACKWARD+i;
        ntbii.bmp_cell = i;
#ifdef MINIGUI_V3
        SendMessage(toolbar_hwnd, NTBM_ADDITEM, 0, (LPARAM)&ntbii);
#else
        SendMessage(toolbar_hwnd, TBM_ADDITEM, 0, (LPARAM)&ntbii);
#endif
    }
   
    SetNotificationCallback (toolbar_hwnd, toolbar_notif_proc);
}

static void destroy_toolbar_window()
{
    printf("destroy toolbar window\n");
    UnloadBitmap (&ntb_bitmap);
}


void my_owner_draw(HWND hWnd, HDC hdc, const RECT* prc)
{
#if 0
    printf("my owner draw\n");
    HDC mydc = GetClientDC(hWnd);
    SetBrushColor(mydc, PIXEL_lightwhite);
    FillBox(mydc, 0, 0, 800, 600);
    BitBlt(mydc, 0, 0, 0, 0, hdc, 0, 0, 0);
    ReleaseDC(mydc);
#endif
}

static void create_propsheet_window(HWND hParent, int width, int height)
{
    propsheet_hwnd = CreateWindow (CTRL_PROPSHEET,
            "",
            WS_VISIBLE | WS_CHILD | PSS_SIMPLE,
            IDC_PROPSHEET,
            0, TOOLBAR_HEIGHT, width, height - STATUSBAR_HEIGHT - TOOLBAR_HEIGHT, hParent, 0);
}

static void set_preference_info(HWND mdolpin_current_hwnd)
{
//	if (mdolpin_current_hwnd)
    {
		SETUP_INFO setup_info;
		//mdolphin_fetch_setup(mdolpin_current_hwnd, &setup_info);
        setup_info.medium_fontsize = 16;
        setup_info.medium_fixed_fontsize = 16;
		//setup_info.default_encoding = MD_CHARSET_GB2312_0;
		//mdolphin_import_setup(mdolpin_current_hwnd, &setup_info);
		//mdolphin_fetch_setup(mdolpin_current_hwnd, &setup_info);
	}
}
HWND mdolphin_hwnd;

void init_webview(IMDWebView* view)
{
    //TODO: set delegates here.
}
int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case MSG_CREATE:
            { 
                RECT cRect; 
                GetClientRect(hWnd, &cRect); 
                create_toolbar_window(hWnd, RECTW(cRect), RECTH(cRect));
                create_location_window(hWnd, RECTW(cRect), RECTH(cRect));
                create_propsheet_window(hWnd, RECTW(cRect), RECTH(cRect)); 
                create_progress_window(hWnd, RECTW(cRect), RECTH(cRect)); 
                create_status_window(hWnd, RECTW(cRect), RECTH(cRect));
#if ENABLE_SCHEMEEXTENSION
                register_schemes();
#endif
#if ENABLE_JSNATIVEBINDING
                define_nativeobjects();

#endif
                HWND ret = my_create_new_window(home_url, 0x3f, 0, 0, -1, -1);
                IMDWebView *view =  mdGetWebViewInstance(ret);
                view->loadURL(home_url);
            }
            break;
        case MSG_ACTIVEMENU:
            break;
        case MSG_COMMAND:
            {
                HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                switch(LOWORD(wParam))
                {
                    /*Menu Item*/
                    case IDM_FILE_OPENFILE:
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
                                "All file(*.*)|html file(*.html)",
                                1
                            };
                            int ret = ShowOpenDialog (hWnd, 100, 100, 350, 250, &newfiledlg);
                            if(ret == IDOK){
                                char * filename = NULL;
                                int len = strlen(newfiledlg.filefullname)+8;
                                filename = (char *)malloc (len*sizeof(char));
                                if (filename) {
                                    strcpy(filename, "file://");
                                    strcat(filename, newfiledlg.filefullname);
                                    if (mdolphin_cHwnd) {
                                        IMDWebView *view =  mdGetWebViewInstance(mdolphin_cHwnd);
                                        view->loadURL(filename);
                                        //mdolphin_navigate(mdolphin_cHwnd, NAV_GOTO, filename, FALSE);
                                    }
                                    free(filename);
                                }
                            }
                        }
                        break;
                    case IDM_FILE_NEW_TAB:
						my_create_new_window(NULL, 0x3f, 0, 0, -1, -1);
						SetFocus(location_hwnd);
                        break;
                    case IDM_FILE_CLOSE_TAB:
                        if (mdolphin_cHwnd)
                            my_close_tab_window(mdolphin_cHwnd);
                        break;
                    case IDM_FILE_QUIT:
                        PostQuitMessage (hWnd);
                        DestroyMainWindow (hWnd);
                        return 0;
                    case IDM_EDIT_PROXY:
                        create_proxy_window(hWnd);
                        break;
                    case IDM_EDIT_FIND:
                        create_find_window(hWnd);
                        break;
                    case IDM_EDIT_FINDAGAIN:
                        find_again_with_last_result();
                        break;
                    case IDM_EDIT_PREFERENCES:
                        create_settings_window(hWnd);
                        break;
                    case IDM_VIEW_TEXTSIZE_ZOOMOUT:
                        {
                            if (!mdolphin_cHwnd)
                                break;
                            int percent;
                            mdolphin_get_text_size_multiplier(mdolphin_cHwnd, &percent);
                            percent -=10;
                            if (percent < 25) {
                                percent = 25;
                            }
                            mdolphin_set_text_size_multiplier(mdolphin_cHwnd, percent);
                        }
                        break;
                    case IDM_VIEW_TEXTSIZE_ZOOMIN:
                        {
                            if (!mdolphin_cHwnd)
                                break;
                            int percent;
                            mdolphin_get_text_size_multiplier(mdolphin_cHwnd, &percent);
                            percent +=10;
                            if (percent >= 350) {
                                percent = 350;
                            }
                            mdolphin_set_text_size_multiplier(mdolphin_cHwnd, percent);
                        }
                        break;
                    case IDM_VIEW_TEXTSIZE_NORMAL:
                        if (mdolphin_cHwnd)
                            //mdolphin_set_text_size_multiplier(mdolphin_cHwnd, 100);
                        break;
                    case IDM_VIEW_CHARENCODE_AUTO:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_AUTODETECT);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_AUTODETECT);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_AUTO_DETECT);
                        change_encoding_menu_status(POS_AUTO_DETECT);
                        break;
                    case IDM_VIEW_CHARENCODE_ISO8859_1:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_ISO8859_1);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_ISO8859_1);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_WESTERN);
                        change_encoding_menu_status(POS_WESTERN);
                        break;
                    case IDM_VIEW_CHARENCODE_UTF8:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_UTF8);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_UTF8);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_UNICODE);
                        change_encoding_menu_status(POS_UNICODE);
                        break;
                    case IDM_VIEW_CHARENCODE_GB2312:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_GB2312_0);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_GB2312_0);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_GB2312);
                        change_encoding_menu_status(POS_GB2312);
                        break;
                    case IDM_VIEW_CHARENCODE_BIG5:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_BIG5);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_BIG5);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_BIG5);
                        change_encoding_menu_status(POS_BIG5);
                        break;
                    case IDM_VIEW_CHARENCODE_GBK:
                        if (!mdolphin_cHwnd)
                            break;
                        //mdolphin_set_text_encoding_name(mdolphin_cHwnd, MD_CHARSET_GBK);
                        mdSetTextEncodingName(mdolphin_cHwnd, MD_CHARSET_GBK);
                        SendMessage(GetParent(mdolphin_cHwnd), MSG_COMMAND, USER_PMS_ENCODING, (LPARAM)POS_GBK);
                        change_encoding_menu_status(POS_GBK);
                        break;
                    case IDM_VIEW_PAGE_SOURCE:
                        if (mdolphin_cHwnd)
                            view_page_in_source_mode(mdolphin_cHwnd);
                        break;
                    case IDM_GO_BACK:
                        if (mdolphin_cHwnd)
                            //mdolphin_navigate(mdolphin_cHwnd, NAV_BACKWARD, NULL, FALSE);
                            mdNavigate(mdolphin_cHwnd, NAV_BACKWARD, NULL);
                        break;
                    case IDM_GO_FORWARD:
                        if (mdolphin_cHwnd)
                            //mdolphin_navigate(mdolphin_cHwnd, NAV_FORWARD, NULL, FALSE);
                            mdNavigate(mdolphin_cHwnd, NAV_FORWARD, NULL);
                        break;
                    case IDM_GO_RELOAD:
                        if (mdolphin_cHwnd)
                            //mdolphin_navigate(mdolphin_cHwnd, NAV_RELOAD, NULL, FALSE);
                            mdNavigate(mdolphin_cHwnd, NAV_RELOAD, NULL);
                        break;
                    case IDM_GO_STOP:
                        if (mdolphin_cHwnd)
                            //mdolphin_navigate(mdolphin_cHwnd, NAV_STOP, NULL, FALSE);
                            mdNavigate(mdolphin_cHwnd, NAV_STOP, NULL);
                        break;
                    case IDM_GO_HOME:
                        if (mdolphin_cHwnd)
                            //mdolphin_navigate(mdolphin_cHwnd, NAV_GOTO, home_url, FALSE);
                            mdNavigate(mdolphin_cHwnd, NAV_GOTO, NULL);
                        break;
                    case IDM_BOOK_ADD:
                        create_add_bookmark_window(hWnd);
                        break;
                    case IDM_BOOK_MANAGE:
                        create_manage_bookmarks_window(hWnd);
                        break;
                    case IDM_HELP_ABOUT:
                        {
                            if(strcasecmp(lang,"zh_cn")==0){
                                MessageBox(hWnd, " mDolphin 2.0\n\n"
                                   " Copyright (C) 2008, 北京飞漫软件技术有限公司\n\n 保留所有权利",
                                    "关于 mDolphin", MB_OK);
                           }
                            else if(strcasecmp(lang,"zh_tw")==0){
                                MessageBox(hWnd, " mDolphin 2.0\n\n"
                                   " Copyright (C) 2008, 北京飛漫軟件技術有限公司\n\n 保留所有權利",
                                    "關於 mDolphin", MB_OK);

                            }
                            else{
                                MessageBox(hWnd, "mDolphin 2.0\n\n"
                                        "Copyright (C) 2008, Beijing Feynman Software Technology Co., Ltd.. All Rights Reserved.",
                                        "About mDolphin", MB_OK);
                            }
                        }
                        break;
                    case IDM_POP_NEW_TAB:
                        my_create_new_window(NULL, 0x3f, 0, 0, -1, -1);
                        break;
                    case IDM_POP_SAVE_AS:
                        printf("save as\n");
                        //mdolphin_save_current_page(mdolphin_cHwnd, false);
                        break;
                    case IDM_POP_CLOSE_TAB:
                        if (mdolphin_cHwnd) 
                            my_close_tab_window(mdolphin_cHwnd);
                        break;
                }
#if 1
                if ((LOWORD(wParam)>=IDM_BOOK_FIRST)&& 
                        (LOWORD(wParam)<=(IDM_BOOK_FIRST+get_bookmark_currentNum()))) 
                    bookmark_click_proc(LOWORD(wParam));
                //if ((LOWORD(wParam) >= IDM_POPUP_MENU_BASE)&& 
                        //(LOWORD(wParam) < MDPE_BASE_APPLICATION_TAG)) 
                    //popup_menu_click_proc(LOWORD(wParam));
#endif
            }
            break;
        case MSG_CSIZECHANGED:
            {
                RECT location_rc, progress_rc, status_rc, propsheet_rc;
                int width = (int)wParam;
                int height = (int)lParam;
               
                GetWindowRect(location_hwnd, &location_rc);
                location_rc.right = location_rc.left + width - TOOLBAR_WIDTH -15;
                MoveWindow (location_hwnd, location_rc.left, location_rc.top,
                             RECTW(location_rc), RECTH(location_rc), TRUE);

                status_rc.left = 0;
                status_rc.top = height - STATUSBAR_HEIGHT;
                status_rc.right = (int)(width*0.8);
                status_rc.bottom = height;
                MoveWindow (status_hwnd, status_rc.left, status_rc.top,
                             RECTW(status_rc), RECTH(status_rc), TRUE);
                
                progress_rc.left = status_rc.right;
                progress_rc.top = status_rc.top;
                progress_rc.right = width;
                progress_rc.bottom = height;
                MoveWindow (progressbar_hwnd, progress_rc.left, progress_rc.top,
                             RECTW(progress_rc), RECTH(progress_rc), TRUE);
                MoveWindow (progress_hwnd, 0, 0,
                             RECTW(progress_rc), RECTH(progress_rc), TRUE);
                
                propsheet_rc.left = 0;
                propsheet_rc.top = TOOLBAR_HEIGHT;
                propsheet_rc.right = width;
                propsheet_rc.bottom = height - STATUSBAR_HEIGHT;
                MoveWindow (propsheet_hwnd, propsheet_rc.left, propsheet_rc.top,
                             RECTW(propsheet_rc) , RECTH(propsheet_rc), TRUE);
                SendMessage(propsheet_hwnd, PSM_SHEETCMD, USER_PMS_RESIZE, 0);
                break;
            }
        case MSG_KEYDOWN:
            {
                RENDERING_MODE mode;
                HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                //mdolphin_get_rendering_mode(mdolphin_cHwnd, &mode);
                //if (mode == MD_VIRTUALVIEW_MODE && wParam == SCANCODE_ENTER)
                    //mdolphin_set_rendering_mode(mdolphin_cHwnd, MD_SCREENVIEW_MODE, 0);
            }
            break;
        case MSG_KEYUP:
            switch (wParam){
                case SCANCODE_CURSORBLOCKUP:
                    {
                        int x, y;
                        HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                        //mdolphin_get_contents_position(mdolphin_cHwnd , &x, &y);
                        y -= 50; //  50;
                        //mdolphin_set_contents_position(mdolphin_cHwnd, x, y);
                        return 0;
                    }
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
                    {
                        int x, y;
                        HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                        //mdolphin_get_contents_position(mdolphin_cHwnd , &x, &y);
                        y += 50; //  50;
                        //mdolphin_set_contents_position(mdolphin_cHwnd , x, y);
                        return 0;
                    }
                    break;
                case SCANCODE_T:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_FILE_NEW_TAB, 0);
                    break;
                case SCANCODE_O:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_FILE_OPENFILE, 0);
                    break;
                case SCANCODE_W:
                    if (lParam & KS_CTRL) {
                        int count = SendMessage(propsheet_hwnd, PSM_GETPAGECOUNT, 0, 0);
                        if (count >1)
                            SendMessage(hWnd, MSG_COMMAND, IDM_FILE_CLOSE_TAB, 0);
                    }
                    break;
                case SCANCODE_F:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_EDIT_FIND, 0);
                    break;
                case SCANCODE_G:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_EDIT_FINDAGAIN, 0);
                    break;
                case SCANCODE_N:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_EDIT_PREFERENCES, 0);
                    break;
                case SCANCODE_MINUS:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_VIEW_TEXTSIZE_ZOOMOUT, 0);
                    break;
                case SCANCODE_EQUAL:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_VIEW_TEXTSIZE_ZOOMIN, 0);
                    break;
                case SCANCODE_0:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_VIEW_TEXTSIZE_NORMAL, 0);
                    break;
                case SCANCODE_U:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_VIEW_PAGE_SOURCE, 0);
                    break;
                case SCANCODE_D:
                    if (lParam & KS_CTRL) 
                        SendMessage(hWnd, MSG_COMMAND, IDM_BOOK_ADD, 0);
                    break;
            }
            break;
        case MSG_DESTROY:
            destroy_toolbar_window();
            DestroyAllControls (hWnd);
            break;
        case MSG_LBUTTONDBLCLK:
            {
                //mdolphin_navigate(mdolphin_hwnd, NAV_STOP, NULL, FALSE);
                //UnregisterMDolphinControl ();
                mdolphin_unregister_minigui_ctrlclass ();
                //mdolphin_cleanup();
                DestroyMainWindow(mdolphin_hwnd);
            }
            return 0;
		case MSG_CLOSE:
            {
                HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                while (mdolphin_cHwnd)
                {
                    my_close_tab_window(mdolphin_cHwnd);
                    mdolphin_cHwnd = get_current_mdolphin_hwnd();
                }
#if ENABLE_SCHEMEEXTENSION
                unregister_schemes();
#endif

                PostQuitMessage (hWnd);
                DestroyMainWindow (hWnd);
                return 0;
            }
    }
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}


int MiniGUIMain (int args, const char* argv[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;

    if (argv[1] && argv[1][0] != '-')
        home_url = argv[1];

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (2, 0, 2)
#if defined (_MGRM_PROCESSES) 
    const char* layer = NULL;

    for (int i = 0; i < args; i++) {
        if (argv[i+1] && strcmp (argv[i+1], "-layer") == 0) {
            layer = argv[i+2];
        }
        else if(argv[i+1] && strcmp( argv[i+1], "-lang") == 0)
             lang = argv[i+2];
    }

    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        return -1;
    }
#endif
#endif

    init_mdolphin();

    mdRegisterMDolphinControl();

#ifdef MINIGUI_V3
    SetDefaultWindowElementRenderer(argv[2]);
#else
    if (!InitMiniGUIExt()) {
        fprintf(stderr, "Can not initialize MiniGUI Ext!\n");
        return -1;
    }
#endif
    
    win_width = g_rcScr.right;
    win_height = g_rcScr.bottom;

    CreateInfo.dwStyle = WS_VISIBLE|WS_CAPTION|WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mDolphin";
    CreateInfo.hMenu = create_menu();
    CreateInfo.hCursor = GetSystemCursor(IDC_WAIT);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MDolphinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = win_width;
    CreateInfo.by = win_height;
#ifdef MINIGUI_V3
    CreateInfo.iBkColor = DWORD2PIXEL(HDC_SCREEN, GetWindowElementAttr(HWND_DESKTOP, WE_MAINC_THREED_BODY));
#else
    CreateInfo.iBkColor = GetWindowElementColor (BKC_CONTROL_DEF);
#endif
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
       return -1;
 
    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    mdUnregisterMDolphinControl();

    release_mdolphin();

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
