#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"

#ifdef MINIGUI_V3
#include <mgutils/mgutils.h>
#else
#include <minigui/mgext.h>
#include <minigui/newfiledlg.h>
#endif



extern HWND toolbar_hwnd;
extern HWND progress_hwnd;
extern HMENU main_menu;
extern const char **pfile_menu_item;
extern const char **pgo_menu_item;

static int charMenuPos = 0;

static void set_menu_item_enable_status(int subpos, int pos, BOOL status)
{
    char buf[200];
    MENUITEMINFO mii;
    HMENU sub_menu;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    sub_menu = GetSubMenu(main_menu, subpos);
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    mii.typedata = (DWORD)buf; 
    mii.cch = 199;
    GetMenuItemInfo(sub_menu, pos, MF_BYPOSITION, &mii);
    if (subpos == POS_GO_MENU)
        mii.typedata = (DWORD)pgo_menu_item[pos];
    else if (subpos == POS_FILE_MENU)
        mii.typedata = (DWORD)pfile_menu_item[pos];

    if (status)
        mii.state = MFS_ENABLED;
    else
        mii.state = MFS_GRAYED;
    SetMenuItemInfo( sub_menu, pos, MF_BYPOSITION, &mii);
}

void change_loading_menu_status(BOOL load, unsigned int progress)
{
    if (load) {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_STOP, TRUE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_STOP, TRUE);
        ShowWindow(progress_hwnd, SW_SHOW);
        SendMessage(progress_hwnd, PBM_SETPOS, progress*10, 0L);
    } else {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_STOP, FALSE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_STOP, FALSE);
        SendMessage(progress_hwnd, PBM_SETPOS, 1000, 0);
        ShowWindow(progress_hwnd, SW_HIDE);
    }
}

void change_history_menu_status(unsigned int bcount, unsigned int fcount)
{
    if (!bcount) {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_BACKWARD, FALSE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_BACK, FALSE);
    } else {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_BACKWARD, TRUE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_BACK, TRUE);
    }

    if (!fcount) {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_FORWARD, FALSE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_FORWARD, FALSE);
    } else {
        SendMessage (toolbar_hwnd, NTBM_ENABLEITEM, OP_NAV_FORWARD, TRUE);
        set_menu_item_enable_status( POS_GO_MENU, POS_GO_FORWARD, TRUE);
    }
}

void change_view_menu_status(HWND hwnd)
{
    char buf[200];
    MENUITEMINFO mii;
    HMENU sub_menu;
    memset (&mii, 0, sizeof(MENUITEMINFO));

    for (unsigned i = 0; i < 3; i++) {
        sub_menu = GetSubMenu(main_menu, POS_VIEW_MENU);
        mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
        mii.typedata = (DWORD)buf; 
        mii.cch = 199;
        GetMenuItemInfo(sub_menu, i, MF_BYPOSITION, &mii);
        mii.state = MFS_UNCHECKED;
        SetMenuItemInfo(sub_menu, i, MF_BYPOSITION, &mii);
    }

    RENDERING_MODE mode;
    unsigned int newPos = 1;
    mdolphin_get_rendering_mode(hwnd, &mode);
    switch (mode) {
        case MD_SMALLVIEW_MODE:
            newPos = 0;
            break;
        case MD_SCREENVIEW_MODE:
            newPos = 1;
            break;
        case MD_VIRTUALVIEW_MODE:
            newPos = 2;
            break;
    }

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.typedata = (DWORD)buf; 
    mii.cch = 199;
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    GetMenuItemInfo(sub_menu, newPos, MF_BYPOSITION, &mii);
    mii.state = MFS_CHECKED;
    SetMenuItemInfo( sub_menu, newPos, MF_BYPOSITION, &mii);
}

void change_encoding_menu_status(unsigned int newPos)
{
    char buf[200];
    MENUITEMINFO mii;
    HMENU sub_menu, child_menu;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    sub_menu = GetSubMenu(main_menu, POS_VIEW_MENU);
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    mii.typedata = (DWORD)buf; 
    mii.cch = 199;
    GetMenuItemInfo(sub_menu, POS_CHARACTER_ENCODING, MF_BYPOSITION, &mii);
    child_menu = mii.hsubmenu;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    mii.typedata = (DWORD)buf; 
    mii.cch = 199;
    if (!GetMenuItemInfo(child_menu, charMenuPos, MF_BYPOSITION, &mii)){
        mii.state = MFS_UNCHECKED;
        SetMenuItemInfo( child_menu, charMenuPos, MF_BYPOSITION, &mii);
    }

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.mask = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE | MIIM_DATA;
    mii.typedata = (DWORD)buf; 
    mii.cch = 199;
    if (!GetMenuItemInfo(child_menu, newPos, MF_BYPOSITION, &mii)) {
        mii.state = MFS_CHECKED;
        SetMenuItemInfo( child_menu, newPos, MF_BYPOSITION, &mii);
    }
    charMenuPos = newPos;
}

void change_file_menu_status(int count, BOOL add)
{
    if (add && count == 2) 
        set_menu_item_enable_status(POS_FILE_MENU, POS_FILE_CLOSETAB, TRUE);

    if (!add && count == 1) 
        set_menu_item_enable_status(POS_FILE_MENU, POS_FILE_CLOSETAB, FALSE);
}


