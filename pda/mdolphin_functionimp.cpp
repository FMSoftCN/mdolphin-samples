#ifndef _MDOLPHIN_FUNCTION_H
#define _MDOLPHIN_FUNCTION_H

#include "mdolphin_app.h"

#ifdef _IME_GB2312
	#if USE_SW_KB 
		#include "mgime.h"
		#include "keyboard.h"
	#endif
#endif


#if LoadResFromFile
#else
#include "mdolphin_app_res.h"
#endif
extern void create_find_window(HWND hParent);
extern void	create_settings_window(HWND hParent);
extern void find_again_with_last_result();
extern void create_proxy_window(HWND hParent);
extern BOOL importCertificate(HWND hParent);
extern void notify_viewmode_callback(int viewMode);

extern int win_width;
extern int win_height;

extern RECT Title_Rect;
extern RECT Close_Rect;
extern RECT Home_Rect;
extern RECT Location_Rect;
extern RECT GoURL_Rect;
extern RECT Mdolphin_Rect;
extern RECT Toolbar_Rect;
extern RECT Back_Rect;
extern RECT Ime_Rect;
extern RECT Menu_Rect;

extern HWND hMainWnd;
#ifdef _IME_GB2312
#if USE_SW_KB
extern GHANDLE ime_cont;
#else
extern HWND ime_hwnd;
#endif

#endif
extern HWND mdolphin_hwnd;
extern HMENU popup_menu;
extern HWND location_hwnd;
extern HWND toolbar_hwnd;

extern const char * home_url;

extern int viewmode;

static int charMenuPos = 0;
static int menuCheckPos = 1;

BITMAP title_bitmap;
BITMAP close_bitmap;
BITMAP b_close_bitmap;
BITMAP go_bitmap;
BITMAP home_bitmap;
BITMAP b_go_bitmap;
BITMAP b_home_bitmap;
BITMAP loading_bitmap;
BITMAP back_bitmap;
BITMAP ime_bitmap;
BITMAP menu_bitmap;
BITMAP b_loading_bitmap;
BITMAP b_back_bitmap;
BITMAP b_ime_bitmap;
BITMAP b_menu_bitmap;

BITMAP title_offset_bitmap;
BITMAP bottom_offset_bitmap;
bool state_ime = false;
bool b_close = false;
bool b_go = false;
bool b_home = false;
bool state_load = false;
bool b_loading = false;
bool b_back = false;
bool b_menu = false;
bool b_ime = false;

int prompt_box(HWND parent, const char *info, const char *definp, char *buf, int buflen);


static const char* edit_menu_item[] = {
    "Find in This Page...",
    "Find Again",
    "Separator",
    "Preferences",
};

static const char* view_menu_item[] = {
    "Small View",/*Pos 0*/
    "Normal View",/*Pos 1*/
    "Global View",/*Pos 2*/
    "Separator",
	"Page Source"
};

static const char* text_menu_item[] = {
    "Increase ++ ",
    "Decrease -- ",
    "Normal    "
};

static const char* char_menu_item[] = {
    "Auto-Detect ",
    "Western (ISO-8859-1) ",
    "Unicode (UTF-8) ",
    "Chinese Simplified (GB2312) ",
    "Chinese Traditional (BIG5) "
};


static DLGTEMPLATE DlgInitProgress =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_TOPMOST,
    0, 0, WIN_WIDTH - 60, WIN_HEIGHT - 50,
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
        0, 0, WIN_WIDTH - 60, WIN_HEIGHT - 50,
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

HWND my_create_new_window(const char* url, DWORD styles, int x, int y, int width, int height)
{
	HWND ret;
	if (styles & CWS_MODAL) 
		ret = my_create_dialog_window(hMainWnd);
	else 
		ret = mdolphin_hwnd;
	mdolphin_navigate(ret, NAV_GOTO, url, FALSE);

	return ret;
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
                HWND newHwnd = my_create_new_window(NULL, 0x7f, 0, 0, -1, -1);
                if (newHwnd) {
                    mdolphin_set_in_view_source_mode(newHwnd, TRUE);
                    mdolphin_navigate(newHwnd, NAV_GOTO, url, FALSE);
                }
                free(url);
            }
        }
    }
}

void location_entry_open_url(HWND hwnd, int id, int nc, DWORD add_data)
{
    int length;
    char* url_string = NULL;
    if (nc == EN_ENTER)
    {
        length = SendMessage(hwnd, MSG_GETTEXTLENGTH, 0, 0);
        url_string = (char*)malloc (length + 1);
        memset (url_string, 0, length+1);
        GetWindowText (hwnd, url_string, length);
        if (url_string)
        {
            mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, url_string, FALSE);
            SetFocus(mdolphin_hwnd);
			free (url_string);
        }
    }
}

void create_location_window(HWND hParent)
{
    location_hwnd = CreateWindowEx (CTRL_SLEDIT, "",
            WS_CHILD | WS_BORDER | WS_VISIBLE,
            0,IDC_LOCATION,
	Location_Rect.left,Location_Rect.top,Location_Rect.right,Location_Rect.bottom,
            hParent, 0);

    if(location_hwnd == HWND_INVALID)
        printf ("mDolphin: Can not create location bar window.\n");

    SetNotificationCallback (location_hwnd, location_entry_open_url);
}


void create_mdolphin_window(HWND hParent)
{
    mdolphin_hwnd = CreateWindow (MDOLPHIN_CTRL,
            "",
            WS_VISIBLE | WS_HSCROLL| WS_VSCROLL| WS_CHILD ,
            IDC_MDOLPHIN,
	Mdolphin_Rect.left,Mdolphin_Rect.top,Mdolphin_Rect.right,Mdolphin_Rect.bottom,
			hParent, 0);
}


static bool IsPointInRECT(int pos_x,int pos_y,RECT * rect)
{
	if (pos_x >= rect->left && pos_x <= rect->right+rect->left
			&& pos_y >= rect->top && pos_y <= rect->bottom+rect->top )
		return true;
	return false;
};

void  HandlePaintMessage(HDC hdc)
{
	if(b_close)
		FillBoxWithBitmap(hdc,
				Close_Rect.left, Close_Rect.top, Close_Rect.right,Close_Rect.bottom,
				&b_close_bitmap);
	else
		FillBoxWithBitmap(hdc,
				Close_Rect.left, Close_Rect.top, Close_Rect.right,Close_Rect.bottom,
				&close_bitmap);

	if(b_go)
		FillBoxWithBitmap(hdc,
				GoURL_Rect.left, GoURL_Rect.top, GoURL_Rect.right,GoURL_Rect.bottom,
				&b_go_bitmap);
	else
		FillBoxWithBitmap(hdc,
				GoURL_Rect.left, GoURL_Rect.top, GoURL_Rect.right,GoURL_Rect.bottom,
				&go_bitmap);
	if(b_home)
		FillBoxWithBitmap(hdc,
				Home_Rect.left, Home_Rect.top, Home_Rect.right,Home_Rect.bottom,
				&b_home_bitmap);
	else
		FillBoxWithBitmap(hdc,
				Home_Rect.left, Home_Rect.top, Home_Rect.right,Home_Rect.bottom,
				&home_bitmap);
	if(!state_load)
	{
		if(b_back)
			FillBoxWithBitmap(hdc,
					Back_Rect.left, Back_Rect.top, Back_Rect.right,Back_Rect.bottom,
					&b_back_bitmap);
		else
			FillBoxWithBitmap(hdc,
					Back_Rect.left, Back_Rect.top, Back_Rect.right,Back_Rect.bottom,
					&back_bitmap);
	}
	else
	{
		if(b_loading)
			FillBoxWithBitmap(hdc,
					Back_Rect.left, Back_Rect.top, Back_Rect.right,Back_Rect.bottom,
					&b_loading_bitmap);
		else
			FillBoxWithBitmap(hdc,
					Back_Rect.left, Back_Rect.top, Back_Rect.right,Back_Rect.bottom,
					&loading_bitmap);
	}

	if(b_menu)    
		FillBoxWithBitmap(hdc,
				Menu_Rect.left, Menu_Rect.top, Menu_Rect.right,Menu_Rect.bottom,
				&b_menu_bitmap);
	else
		FillBoxWithBitmap(hdc,
				Menu_Rect.left, Menu_Rect.top, Menu_Rect.right,Menu_Rect.bottom,
				&menu_bitmap);

	if(b_ime)
		FillBoxWithBitmap(hdc,
				Ime_Rect.left, Ime_Rect.top, Ime_Rect.right,Ime_Rect.bottom,
				&b_ime_bitmap);
	else
		FillBoxWithBitmap(hdc,
				Ime_Rect.left, Ime_Rect.top, Ime_Rect.right,Ime_Rect.bottom,
				&ime_bitmap);
}

void  HandldLButtonMessage(int x_pos, int y_pos)
{
	if(IsPointInRECT ( x_pos,y_pos,&GoURL_Rect) )
		location_entry_open_url(location_hwnd, 0, EN_ENTER,0);

	if(IsPointInRECT ( x_pos,y_pos,&Close_Rect) )
		SendMessage(hMainWnd,MSG_CLOSE,0,0); 

    if(IsPointInRECT ( x_pos,y_pos,&Home_Rect) )
        mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);

	if(IsPointInRECT ( x_pos,y_pos,&Back_Rect) )
        mdolphin_navigate(mdolphin_hwnd, NAV_BACKWARD, NULL, FALSE);

	if(IsPointInRECT ( x_pos,y_pos,&Menu_Rect) ) {
		RENDERING_MODE mode;
		mdolphin_get_rendering_mode(mdolphin_hwnd, &mode);
		notify_viewmode_callback(mode);

		TrackPopupMenu(popup_menu ,TPM_LEFTALIGN|TPM_BOTTOMALIGN,
				x_pos,y_pos,hMainWnd);	
	}

#ifdef _IME_GB2312
	if(IsPointInRECT ( x_pos,y_pos,&Ime_Rect) )
	{
		state_ime = !state_ime;
		if (state_ime) {
			#if USE_SW_KB
			mgiSetActiveIMEWindow(ime_cont, "softkeyboard");
			#else
			SendNotifyMessage(ime_hwnd, MSG_IME_OPEN, 0, 0);
			#endif

		} else { 
			#if USE_SW_KB
            mgiSetActiveIMEWindow(ime_cont, "");
			#else
			SendNotifyMessage(ime_hwnd, MSG_IME_CLOSE, 0, 0);
			#endif
		}
	}			
#endif

}

int handle_key_ecos(int wParam)
{
#ifdef __ECOS__
	if(wParam == SCANCODE_F1) {
		viewmode = (viewmode%3) + 1;
		if(viewmode == 1)  //screen view
			mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SCREENVIEW_MODE, 0);
		if(viewmode == 2)  //virtual view
			mdolphin_set_rendering_mode(mdolphin_hwnd, MD_VIRTUALVIEW_MODE, WIN_WIDTH - 40);
		if(viewmode == 3)  // small view 
			mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SMALLVIEW_MODE, WIN_WIDTH-18);
		SetFocus(mdolphin_hwnd);
		return 0;
	}

	if(wParam == SCANCODE_F2) {
		mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
		return 0;
	}
	if(wParam == SCANCODE_F3) {
		mdolphin_navigate(mdolphin_hwnd, NAV_BACKWARD, NULL, FALSE);
		return 0;
	}

	if(viewmode != 2) {
		if(wParam == SCANCODE_CURSORBLOCKRIGHT ) {
			SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), 0);
			return 0;
		}
		if(wParam == SCANCODE_CURSORBLOCKLEFT)	{
			SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), (LPARAM)KS_SHIFT);
			return 0;
		}
	}
#endif  //end of __ECOS__
	return 1;

}
int handle_key_ipaq5450(int wParam)
{
#ifdef __IPAQ5450__ 
			if(wParam == SCANCODE_F1 )	{
				SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), 0);
				return 0;
			}
			if(wParam == SCANCODE_F2)	{
				SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), (LPARAM)KS_SHIFT);
				return 0;
			}
			if(wParam == SCANCODE_F3) {
				viewmode = (viewmode%3) + 1;
				if(viewmode == 1)  //screen view
					mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SCREENVIEW_MODE, 0);
				if(viewmode == 2)  //virtual view
					mdolphin_set_rendering_mode(mdolphin_hwnd, MD_VIRTUALVIEW_MODE, WIN_WIDTH-40);
				if(viewmode == 3)  // small view 
					mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SMALLVIEW_MODE, WIN_WIDTH-18);
				SetFocus(mdolphin_hwnd);
				return 0;
			}
#endif
	return 1;
}
static void change_char_menu_status(int newPos)
{
    MENUITEMINFO mii;
    HMENU  child_menu;
	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
	GetMenuItemInfo(popup_menu, 4, MF_BYPOSITION, &mii);
	child_menu = mii.hsubmenu;

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
	GetMenuItemInfo(child_menu, charMenuPos, MF_BYPOSITION, &mii);
	mii.state = MFS_UNCHECKED;
	mii.typedata = (DWORD)char_menu_item[charMenuPos];	
	SetMenuItemInfo( child_menu, charMenuPos, MF_BYPOSITION, &mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
	GetMenuItemInfo(child_menu, newPos, MF_BYPOSITION, &mii);
	mii.state = MFS_CHECKED;
	mii.typedata = (DWORD)char_menu_item[newPos];	
	SetMenuItemInfo( child_menu, newPos, MF_BYPOSITION, &mii);
	charMenuPos = newPos;
}

void notify_viewmode_callback(int viewMode)
{
    int newPos = 1;
    if (viewMode == MD_SMALLVIEW_MODE)
        newPos = 0;
    else if (viewMode == MD_SCREENVIEW_MODE)
        newPos = 1;
    else if (viewMode == MD_VIRTUALVIEW_MODE)
        newPos = 2;

    MENUITEMINFO mii;
    HMENU sub_menu;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    
	mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    GetMenuItemInfo(popup_menu, 5, MF_BYPOSITION, &mii);
	    sub_menu = mii.hsubmenu;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
	GetMenuItemInfo(sub_menu, menuCheckPos, MF_BYPOSITION, &mii);
	mii.state = MFS_UNCHECKED;
	mii.typedata = (DWORD)view_menu_item[menuCheckPos];	
	SetMenuItemInfo( sub_menu, menuCheckPos, MF_BYPOSITION, &mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
	GetMenuItemInfo(sub_menu, newPos, MF_BYPOSITION, &mii);
	mii.state = MFS_CHECKED;
	mii.typedata = (DWORD)view_menu_item[newPos];	
	SetMenuItemInfo( sub_menu, newPos, MF_BYPOSITION, &mii);
	menuCheckPos = newPos;

}
void  HandleCommandMessage(int id)
{
	switch(id)
	{
		case IDM_VIEW_SMALLVIEW:
            mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SMALLVIEW_MODE, WIN_WIDTH-18);
            notify_viewmode_callback(MD_SMALLVIEW_MODE);
			SetFocus(mdolphin_hwnd);
			break;
		case IDM_VIEW_SCREENVIEW:
            mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SCREENVIEW_MODE, 0);
            notify_viewmode_callback(MD_SCREENVIEW_MODE);
			SetFocus(mdolphin_hwnd);
			break;
		case IDM_VIEW_VIRTUALVIEW:
            mdolphin_set_rendering_mode(mdolphin_hwnd, MD_VIRTUALVIEW_MODE, WIN_WIDTH - 40);
            notify_viewmode_callback(MD_VIRTUALVIEW_MODE);
			SetFocus(mdolphin_hwnd);
			break;

		case IDM_VIEW_PAGE_SOURCE:
			view_page_in_source_mode(mdolphin_hwnd);
			break;
		case IDM_VIEW_TEXTSIZE_ZOOMOUT:
			{
				int percent;
                mdolphin_get_text_size_multiplier(mdolphin_hwnd, &percent);
				percent -=10;
                mdolphin_set_text_size_multiplier(mdolphin_hwnd, percent);
			}
			break;
		case IDM_VIEW_TEXTSIZE_ZOOMIN:
			{
				int percent;
                mdolphin_get_text_size_multiplier(mdolphin_hwnd, &percent);
				percent +=10;
                mdolphin_set_text_size_multiplier(mdolphin_hwnd, percent);
			}
			break;
		case IDM_VIEW_TEXTSIZE_NORMAL:
            mdolphin_set_text_size_multiplier(mdolphin_hwnd, 100);
			break;
		case IDM_VIEW_CHARENCODE_AUTO:
			{
				int newPos = 0;
				change_char_menu_status(newPos);
                mdolphin_set_text_encoding_name(mdolphin_hwnd, MD_CHARSET_AUTODETECT);
			}
			break;
		case IDM_VIEW_CHARENCODE_ISO8859_1:
			{
				int newPos = 1;
				change_char_menu_status(newPos);
                mdolphin_set_text_encoding_name(mdolphin_hwnd, MD_CHARSET_ISO8859_1);
			}
			break;
		case IDM_VIEW_CHARENCODE_UTF8:
			{
				int newPos = 2;
				change_char_menu_status(newPos);
                mdolphin_set_text_encoding_name(mdolphin_hwnd, MD_CHARSET_UTF8);
			}
			break;
		case IDM_VIEW_CHARENCODE_GB2312:
			{
				int newPos = 3;
				change_char_menu_status(newPos);
                mdolphin_set_text_encoding_name(mdolphin_hwnd, MD_CHARSET_GB2312_0);
			}
			break;
		case IDM_VIEW_CHARENCODE_BIG5:
			{
				int newPos = 4;
				change_char_menu_status(newPos);
                mdolphin_set_text_encoding_name(mdolphin_hwnd, MD_CHARSET_BIG5);
			}
			break;
		case IDM_MAIN_BACK:
            mdolphin_navigate(mdolphin_hwnd, NAV_BACKWARD, NULL, FALSE);
			break;
		case IDM_MAIN_RELOAD:
            mdolphin_navigate(mdolphin_hwnd, NAV_RELOAD, NULL, FALSE);
			break;
		case IDM_MAIN_GO_HOME:
            mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
			break;

		case IDM_EDIT_FIND:
			create_find_window(hMainWnd);
			break;
		case IDM_EDIT_FINDAGAIN:
			find_again_with_last_result();
			break;
		case IDM_EDIT_PREFERENCES:
			create_settings_window(hMainWnd);
			break;
	}
}


void ModifyImageShowState(int x_pos,int y_pos)
{
	bool shouldRePaint = false;

	if( IsPointInRECT ( x_pos,y_pos,&Close_Rect) != b_close) {
		b_close = !b_close;
		shouldRePaint = true;
	}

	if( IsPointInRECT ( x_pos,y_pos,&GoURL_Rect) != b_go) {
		b_go = !b_go ;
		shouldRePaint = true;
	}

	if( IsPointInRECT ( x_pos,y_pos,&Home_Rect) != b_home ) {
		b_home = !b_home ;
		shouldRePaint = true;
	}


	if (!state_load) {
		if (IsPointInRECT ( x_pos,y_pos,&Back_Rect) != b_back) {
			b_back = !b_back ;
			shouldRePaint = true;
		}
	} else {
		if(IsPointInRECT ( x_pos,y_pos,&Back_Rect) != b_loading ) {
			b_loading = !b_loading ;
			shouldRePaint = true;
		}
	}

	if (IsPointInRECT ( x_pos,y_pos,&Menu_Rect) != b_menu) {
		b_menu = !b_menu;
		shouldRePaint = true;
	}

	if(IsPointInRECT ( x_pos,y_pos,&Ime_Rect) != b_ime ) {
		b_ime = !b_ime ;
		shouldRePaint = true;
	}

	if(shouldRePaint) 
		SendMessage(hMainWnd, MSG_PAINT, 0, 0);
}

static HMENU create_submenu(const char* menu_item[], int nr_item, int idm)
{
    HMENU menu;
    MENUITEMINFO mii;
    int i;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"";
    menu = CreatePopupMenu(&mii);

    memset (&mii, 0, sizeof(mii));
    mii.type = MFT_STRING;
    for (i = 0; i < nr_item; ++i)
    {
        mii.type = MFT_STRING;
        mii.state = MFS_ENABLED;
        mii.typedata = (DWORD)"";
        mii.hsubmenu = 0;
        mii.id = idm + i;
        if (strstr( menu_item[i],"Separator")) {
            mii.type = MFT_SEPARATOR;
            InsertMenuItem (menu, i, TRUE, &mii);
            continue;
		}
        mii.typedata = (DWORD)menu_item[i];
        InsertMenuItem (menu, i, TRUE, &mii);
    }
    menu = StripPopupHead(menu);
    return menu;
}

HMENU creat_popupmenu()
{
	MENUITEMINFO mii;
    HMENU  tmp_menu;

	memset (&mii, 0, sizeof(MENUITEMINFO));

	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN;
	mii.typedata    = (DWORD)"main";
	popup_menu = CreatePopupMenu(&mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_GO_HOME;
	mii.typedata    = (DWORD)"HomePage";
	InsertMenuItem(popup_menu, 0, TRUE, &mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_BACK;
	mii.typedata    = (DWORD)"Back";
	InsertMenuItem(popup_menu, 1, TRUE, &mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_RELOAD;
	mii.typedata    = (DWORD)"Reload";
	InsertMenuItem(popup_menu, 2, TRUE, &mii);


	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_TEXTSIZE;
	mii.typedata    = (DWORD)"TextSize";
	mii.hsubmenu    = create_submenu(text_menu_item, TABLESIZE(text_menu_item), IDM_VIEW_TEXTSIZE_ZOOMIN);
	InsertMenuItem(popup_menu, 3, TRUE, &mii);


	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_CHARACTER;
	mii.typedata    = (DWORD)"Character Encoding";
	mii.hsubmenu    = create_submenu(char_menu_item, TABLESIZE(char_menu_item), IDM_VIEW_CHARENCODE_AUTO);
	InsertMenuItem(popup_menu, 4, TRUE, &mii);


	tmp_menu = mii.hsubmenu;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;

    GetMenuItemInfo(tmp_menu, 0, MF_BYPOSITION, &mii);
    mii.state = MFS_CHECKED;
	mii.typedata = (DWORD)char_menu_item[0];
    SetMenuItemInfo( tmp_menu,0, MF_BYPOSITION, &mii);

	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_VIEW;
	mii.typedata    = (DWORD)"View";
	mii.hsubmenu    = create_submenu(view_menu_item, TABLESIZE(view_menu_item), IDM_VIEW_SMALLVIEW);
	InsertMenuItem(popup_menu, 5, TRUE, &mii);

	tmp_menu = mii.hsubmenu;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;

    GetMenuItemInfo(tmp_menu, 1, MF_BYPOSITION, &mii);
    mii.state = MFS_CHECKED;
	mii.typedata = (DWORD)view_menu_item[1];
    SetMenuItemInfo(tmp_menu, 1, MF_BYPOSITION, &mii);
	memset (&mii, 0, sizeof(MENUITEMINFO));
	mii.type        = MFT_STRING;
	mii.id          = IDM_MAIN_BACK;
	mii.typedata    = (DWORD)"Edit";
	mii.hsubmenu    = create_submenu(edit_menu_item, TABLESIZE(edit_menu_item), IDM_EDIT_FIND);
	InsertMenuItem(popup_menu, 6, TRUE, &mii);

	popup_menu = StripPopupHead(popup_menu);

	return popup_menu;
}

int LoadBitmapResource()
{
	memset (&b_close_bitmap, 0, sizeof(b_close_bitmap));
#if LoadResFromFile
	if ( LoadBitmap(HDC_SCREEN ,&b_close_bitmap ,ResPATH"b_close.gif"))
#else
	if ( LoadBitmapFromMem (HDC_SCREEN, &b_close_bitmap, b_close_data, b_close_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_close\n" );
		return 0;
	}

	memset (&close_bitmap, 0, sizeof(close_bitmap));
#if LoadResFromFile
	if( LoadBitmap(HDC_SCREEN ,&close_bitmap ,ResPATH"close.gif"))
#else
	if( LoadBitmapFromMem (HDC_SCREEN, &close_bitmap, close_data, close_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for close\n" );
		return 0;
	}

	memset (&title_bitmap, 0, sizeof(title_bitmap));
#if LoadResFromFile
	if( LoadBitmap(HDC_SCREEN ,&title_bitmap ,ResPATH"title.gif"))
#else
	if( LoadBitmapFromMem (HDC_SCREEN, &title_bitmap, title_data, title_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for title\n" );
		return 0;
	}

	memset (&go_bitmap, 0, sizeof(go_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&go_bitmap ,ResPATH"go.gif"))
#else
	if ( LoadBitmapFromMem (HDC_SCREEN, &go_bitmap,go_data ,go_datasize, "gif") )
#endif
	{
		fprintf (stderr, "Can not load bitmap for go\n");
		return 0;
	}

	memset (&home_bitmap, 0, sizeof(home_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&home_bitmap ,ResPATH"home.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &home_bitmap,home_data ,home_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for home \n");
		return 0;
	}


	memset (&b_go_bitmap, 0, sizeof(b_go_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_go_bitmap ,ResPATH"b_go.gif"))
#else
	if ( LoadBitmapFromMem (HDC_SCREEN, &b_go_bitmap,b_go_data ,b_go_datasize, "gif") )
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_go\n");
		return 0;
	}

	memset (&b_home_bitmap, 0, sizeof(b_home_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_home_bitmap ,ResPATH"b_home.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &b_home_bitmap,b_home_data ,b_home_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_home \n");
		return 0;
	}

	memset (&loading_bitmap, 0, sizeof(loading_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&loading_bitmap ,ResPATH"loading.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &loading_bitmap,loading_data ,loading_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for loading\n");
		return 0;
	}

	memset (&back_bitmap, 0, sizeof(back_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&back_bitmap ,ResPATH"back.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &back_bitmap,back_data ,back_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for back\n");
		return 0;
	}

	memset (&menu_bitmap, 0, sizeof(menu_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&menu_bitmap ,ResPATH"menu.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &menu_bitmap,menu_data ,menu_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for menu \n");
		return 0;
	}

	memset(&ime_bitmap,0,sizeof(ime_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&ime_bitmap ,ResPATH"ime.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &ime_bitmap,ime_data ,ime_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for ime\n");
		return 0;
	}

	memset (&b_loading_bitmap, 0, sizeof(b_loading_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_loading_bitmap ,ResPATH"b_loading.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &b_loading_bitmap,b_loading_data ,b_loading_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_loading\n");
		return 0;
	}

	memset (&b_back_bitmap, 0, sizeof(b_back_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_back_bitmap ,ResPATH"b_back.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &b_back_bitmap,b_back_data ,b_back_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_back\n");
		return 0;
	}

	memset (&b_menu_bitmap, 0, sizeof(b_menu_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_menu_bitmap ,ResPATH"b_menu.gif"))
#else
	if(LoadBitmapFromMem (HDC_SCREEN, &b_menu_bitmap,b_menu_data ,b_menu_datasize, "gif"))
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_menu \n");
		return 0;
	}
	memset (&title_offset_bitmap, 0, sizeof(title_offset_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&title_offset_bitmap ,ResPATH"title_offset.gif"))
#else
		if(LoadBitmapFromMem (HDC_SCREEN, &title_offset_bitmap,title_offset_data ,title_offset_datasize, "gif"))
#endif
		{
			fprintf (stderr, "Can not load bitmap for title_offset_bitmap \n");
			return 0;
		}
	memset (&bottom_offset_bitmap, 0, sizeof(bottom_offset_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&bottom_offset_bitmap ,ResPATH"bottom_offset.gif"))
#else
		if(LoadBitmapFromMem (HDC_SCREEN, &bottom_offset_bitmap,bottom_offset_data ,bottom_offset_datasize, "gif"))
#endif
		{
			fprintf (stderr, "Can not load bitmap for title_offset_bitmap \n");
			return 0;
		}
	memset(&b_ime_bitmap,0,sizeof(b_ime_bitmap));
#if LoadResFromFile
	if(LoadBitmap(HDC_SCREEN ,&b_ime_bitmap ,ResPATH"b_ime.gif"))
#else
	if ( LoadBitmapFromMem (HDC_SCREEN, &b_ime_bitmap,b_ime_data ,b_ime_datasize, "gif") )
#endif
	{
		fprintf (stderr, "Can not load bitmap for b_ime\n");
		return 0;
	}
	return 1;
}






// follow codes was moved from mdolphin_callback.cpp

static char prompt_text [256];

static void set_location_text (HWND hControl, const char * text)
{
	if(text)
    SetWindowText (location_hwnd, text);
	else
    SetWindowText (location_hwnd, "");
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
    else
        return NULL;
}

static void err_msg (HWND hWnd, int err, const char* url)
{
    MessageBox(GetParent(hWnd), "Error!", url, MB_OK);
}

static void set_backmenu_status( BOOL status)
{
   MENUITEMINFO mii;
	memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    GetMenuItemInfo(popup_menu, 1, MF_BYPOSITION, &mii);
    mii.typedata = (DWORD)"Back";
    if (status)
        mii.state = MFS_ENABLED;
    else
        mii.state = MFS_GRAYED;
    SetMenuItemInfo( popup_menu, 1, MF_BYPOSITION, &mii);
}


static void set_histroy_status (HWND hControl, unsigned int bcount, unsigned int fcount, unsigned int max)
{
    if (!bcount)
        set_backmenu_status( FALSE);
     else 
        set_backmenu_status( TRUE);
}

static void set_loading_status (HWND hControl, BOOL load, unsigned int progress)
{
    if (load) 
		state_load = true ;
	else
		state_load = false ;
	 SendMessage(hMainWnd,MSG_PAINT,0,0);
}

static BOOL set_ime_window(BOOL show)
{
#ifdef _IME_GB2312
	if (show) {
		#if USE_SW_KB
		mgiSetActiveIMEWindow(ime_cont, "softkeyboard");
		#else
		SendNotifyMessage(ime_hwnd, MSG_IME_OPEN, 0, 0);
		#endif
	} else {
		#if USE_SW_KB
		mgiSetActiveIMEWindow(ime_cont, "");
		#else
		SendNotifyMessage(ime_hwnd, MSG_IME_CLOSE, 0, 0);
		#endif
	}
#endif
	return TRUE;
}


static char *inputboxbuf;
static int inputboxbuflen;
static int InputBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    if ( message == MSG_COMMAND ) {
        if ( wParam == IDOK ) {
            GetWindowText (GetDlgItem (hDlg, IDC_INPUT), inputboxbuf, inputboxbuflen);
            EndDialog (hDlg, 1);
        }else if (wParam == IDCANCEL){
            EndDialog (hDlg, 0);
        } 
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int prompt_box(HWND parent, const char *info, const char *definp, char *buf, int buflen)
{
	static DLGTEMPLATE DlgBoxInputLen =
	{
    	WS_BORDER | WS_CAPTION, 
	    WS_EX_NONE,
    	0, 41, 170, 100, 
		"Javascript input",
	    0, 0,
    	4, NULL,
	    0
	};

	static CTRLDATA CtrlInputLen [] =
	{ 
    	{
        	CTRL_STATIC,
	        WS_VISIBLE | SS_SIMPLE,
    	    10, 5, 150, 20, 
        	IDC_STATIC, 
	        "InputBox",
    	    0
	    },
    	{
	        CTRL_EDIT,
    	    WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        	10, 23, 150, 20,
	        IDC_INPUT,
    	    NULL,
	        0
    	},
	    {
    	    CTRL_BUTTON,
        	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
	        10, 50, 70, 20,
    	    IDOK, 
        	"Ok",
	        0
    	},
	    {
    	    CTRL_BUTTON,
        	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
	        90, 50, 70, 20,
    	    IDCANCEL, 
        	"Cancel",
	        0
    	}
	};

	CtrlInputLen[0].caption = info;
	CtrlInputLen[1].caption = definp;
    DlgBoxInputLen.controls = CtrlInputLen;
	inputboxbuf = buf;
	inputboxbuflen = buflen;

    return DialogBoxIndirectParam (&DlgBoxInputLen, parent, InputBoxProc, 0);
}


void set_callback_info(HWND hWnd)
{
    CB_INFO cb_info;
    memset (&cb_info, 0, sizeof(CB_INFO));
    cb_info.CB_MESSAGE_BOX = my_message_callback;
    cb_info.CB_CONFIRM_BOX = my_confirm_callback;
    cb_info.CB_PROMPT_BOX = my_prompt_callback;
    cb_info.CB_SET_LOCATION = set_location_text;
    cb_info.CB_OPEN_WINDOW = my_create_new_window;
    cb_info.CB_SET_LOADING_STATUS = set_loading_status;
    cb_info.CB_SET_HISTORY_STATUS = set_histroy_status;
#ifdef _IME_GB2312
    cb_info.CB_SET_IME_STATUS = set_ime_window;
#endif	
    cb_info.CB_ERROR = err_msg;
    mdolphin_set_callback_info(mdolphin_hwnd, &cb_info);
}

#endif
