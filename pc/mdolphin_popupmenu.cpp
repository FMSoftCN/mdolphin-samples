#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
#include "mdolphin_propsheet.h"
extern HWND propsheet_hwnd;
static HMENU popupMenu=0;
static HMENU fontSubMenu=0;
static HMENU writeSubMenu=0;
static HWND mHwnd_popupMenu =0;

typedef enum {
    ITEM_ACTION,
    ITEM_SUBMENU,
    ITEM_SEPARATOR,
}ITEM_TYPE;

/* font submenu for editable content */
static const char* FontSubMenu ="FontSubMenu";
static const char* Fonts = "Fonts";
static const char* Bold = "Bold";
static const char* Italic = "Italic";
static const char* Underline = "Underline";
/* text submenu for editable content */
static const char* TextSubMenu = "TextSubMenu";
static const char* Direction = "Direction";
static const char* Default = "Default";
static const char* LTR = "LTR";
static const char* RTL = "RTL";
/* popup menu */
static const char* ContextMenu = "ContextMenu";
/* Link menu items */
static const char* OpenLink = "Open Link";
static const char* OpenInNewWindow = "Open in New Window";
static const char* SaveLink = "Save Link...";
static const char* CopyLink = "Copy Link";
/* Image menu items */
static const char* OpenImage = "Open Image";
static const char* SaveImage = "Save Image";
static const char* CopyImage = "Copy Image";
/* Document menu items */
static const char* GoBack = "Go Back";
static const char* GoForward = "Go Forward";
static const char* Stop = "Stop";
static const char* Reload = "Reload";
static const char* OpenFrame = "Open Frame";
/* Text menu items */
static const char* SearchTheWeb = "Search With Google";
static const char* Copy = "Copy";
static const char* Cut = "Cut";
static const char* Paste = "Paste";
static const char* Inspect = "Inspect";

static void add_user_menu_items(HMENU menu)
{
    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_SEPARATOR;
    InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
#if 1
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.hsubmenu = 0;
    mii.id = IDM_POP_NEW_TAB;
    mii.state = MFS_ENABLED;
    mii.cch = strlen("New Tab");
    mii.typedata = (DWORD)"New Tab";
    InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
#endif
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.hsubmenu = 0;
    mii.id =  IDM_POP_SAVE_AS;
    mii.state = MFS_ENABLED;
    mii.cch = strlen("Save As...");
    mii.typedata = (DWORD)"Save As...";
    InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
    
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.hsubmenu = 0;
    mii.id = IDM_POP_CLOSE_TAB;
    
    int count = SendMessage(propsheet_hwnd, PSM_GETPAGECOUNT, 0, 0);
    if (count >1) {
        mii.state &= ~MFS_DISABLED;
        mii.state |= MFS_ENABLED;
    } else {
        mii.state &= ~MFS_ENABLED;
        mii.state |= MFS_DISABLED;
    }
    mii.cch = strlen("Close Tab");
    mii.typedata = (DWORD)"Close Tab";
    InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
}

static void appendPopUpMenuItem(HMENU menu, const char* title, unsigned long flag, ITEM_TYPE type, POPUP_MENU_EVENT_TYPE action, HMENU submenu, const POPUP_MENU_DATA *data)
{
    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(MENUITEMINFO));

    mii.mask = MIIM_TYPE;

    if (type == ITEM_SEPARATOR) {
        mii.type = MFT_SEPARATOR;
        InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
        return;
    }

    if (submenu) {
        mii.mask |= MIIM_SUBMENU;
        mii.hsubmenu = submenu;
    } else
        mii.mask |= MIIM_ID;

    mii.type = MFT_STRING;

    if (type == ITEM_ACTION)
        mii.id = IDM_POPUP_MENU_BASE + action;

    mii.mask |= MIIM_STATE;
    BOOL check = data->itemsChecked & flag;
    BOOL enable = data->itemsEnabled & flag;

    if (check) {
        mii.state &= ~MFS_UNCHECKED;
        mii.state |= MFS_CHECKED;
    } else {
        mii.state &= ~MFS_CHECKED;
        mii.state |= MFS_UNCHECKED;
    }
    if (enable) {
        mii.state &= ~MFS_DISABLED;
        mii.state |= MFS_ENABLED;
    } else {
        mii.state &= ~MFS_ENABLED;
        mii.state |= MFS_DISABLED;
    }

    mii.cch = strlen(title);
    mii.typedata = (DWORD)title;
    InsertMenuItem(menu, GetMenuItemCount(menu), TRUE, &mii);
}

static void createFontSubMenu(HMENU menu, const POPUP_MENU_DATA *data)
{
    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.id = 0;
    mii.typedata = (DWORD)FontSubMenu;
    HMENU fsub = CreatePopupMenu(&mii);

    appendPopUpMenuItem(menu, NULL, MDPI_NO, ITEM_SEPARATOR, MDPE_NO, 0, data);

    if (data->itemsFlag & MDPI_BOLD)
        appendPopUpMenuItem(fsub, Bold, MDPI_BOLD, ITEM_ACTION, MDPE_BOLD, 0, data);
    if (data->itemsFlag & MDPI_ITALIC)
        appendPopUpMenuItem(fsub, Italic, MDPI_ITALIC, ITEM_ACTION, MDPE_ITALIC, 0, data);
    if (data->itemsFlag & MDPI_UNDERLINE)
        appendPopUpMenuItem(fsub, Underline, MDPI_UNDERLINE, ITEM_ACTION, MDPE_UNDERLINE, 0, data);
    
    fontSubMenu=StripPopupHead(fsub);
    appendPopUpMenuItem(menu, Fonts, MDPI_NO, ITEM_SUBMENU, MDPE_NO, fontSubMenu, data);
}

static void createWritingDirectionSubMenu(HMENU menu, const POPUP_MENU_DATA *data)
{
    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.id = 0;
    mii.typedata = (DWORD)TextSubMenu;
    HMENU tsub = CreatePopupMenu(&mii);

    if (data->itemsFlag & MDPI_DEFAULT_DIRECTION)
        appendPopUpMenuItem(tsub, Default, MDPI_DEFAULT_DIRECTION, ITEM_ACTION, MDPE_DEFAULT_DIRECTION, 0, data);
    if (data->itemsFlag & MDPI_LEFT_TO_RIGHT)
        appendPopUpMenuItem(tsub, LTR, MDPI_LEFT_TO_RIGHT, ITEM_ACTION, MDPE_LEFT_TO_RIGHT, 0, data);
    if (data->itemsFlag & MDPI_RIGHT_TO_LEFT)
        appendPopUpMenuItem(tsub, RTL, MDPI_RIGHT_TO_LEFT, ITEM_ACTION, MDPE_RIGHT_TO_LEFT, 0, data);

    writeSubMenu = StripPopupHead(tsub);
    appendPopUpMenuItem(menu, Direction, MDPI_NO, ITEM_ACTION, MDPE_NO, writeSubMenu, data);
}

BOOL my_create_popup_menu_callback(HWND hWnd, const POPUP_MENU_DATA *data)
{
    int index = SendMessage(propsheet_hwnd, PSM_GETPAGEINDEX, GetParent(hWnd), 0);
    if (index == PS_ERR) 
        return FALSE;

    mHwnd_popupMenu = hWnd;
    if (popupMenu) {
        DestroyMenu(popupMenu);
        popupMenu = 0;
    }

    MENUITEMINFO mii;
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.id = 0;
    mii.typedata = (DWORD)ContextMenu;
    HMENU menu = CreatePopupMenu(&mii);

    if (data->itemsFlag & MDPI_OPEN_LINK) 
        appendPopUpMenuItem(menu, OpenLink, MDPI_OPEN_LINK, ITEM_ACTION, MDPE_OPEN_LINK, 0, data);
    if (data->itemsFlag & MDPI_OPEN_LINK_IN_NEW_WINDOW) 
        appendPopUpMenuItem(menu, OpenInNewWindow, MDPI_OPEN_LINK_IN_NEW_WINDOW, ITEM_ACTION, MDPE_OPEN_LINK_IN_NEW_WINDOW, 0, data);
    if (data->itemsFlag & MDPI_DOWNLOAD_LINK_TO_DISK)
        appendPopUpMenuItem(menu, SaveLink, MDPI_DOWNLOAD_LINK_TO_DISK, ITEM_ACTION, MDPE_DOWNLOAD_LINK_TO_DISK, 0, data);
    if (data->itemsFlag & MDPI_COPY_LINK_TO_CLIPBOARD)  
        appendPopUpMenuItem(menu, CopyLink, MDPI_COPY_LINK_TO_CLIPBOARD, ITEM_ACTION, MDPE_COPY_LINK_TO_CLIPBOARD, 0, data);
    if (data->itemsFlag & MDPI_OPEN_IMAGE_IN_NEW_WINDOW) {
        if (data->itemsFlag & MDPI_OPEN_LINK) 
            appendPopUpMenuItem(menu, NULL, MDPI_NO, ITEM_SEPARATOR, MDPE_NO, 0, data);
        appendPopUpMenuItem(menu, OpenImage, MDPI_OPEN_IMAGE_IN_NEW_WINDOW, ITEM_ACTION, MDPE_OPEN_IMAGE_IN_NEW_WINDOW, 0, data);
    }
    if (data->itemsFlag & MDPI_DOWNLOAD_IMAGE_TO_DISK) 
        appendPopUpMenuItem(menu, SaveImage, MDPI_DOWNLOAD_IMAGE_TO_DISK, ITEM_ACTION, MDPE_DOWNLOAD_IMAGE_TO_DISK, 0, data);
    if (data->itemsFlag & MDPI_COPY_IMAGE_TO_CLIPBOARD) 
        appendPopUpMenuItem(menu, CopyImage, MDPI_COPY_IMAGE_TO_CLIPBOARD, ITEM_ACTION, MDPE_COPY_IMAGE_TO_CLIPBOARD, 0, data);

    if (data->itemsFlag & MDPI_GOBACK) 
        appendPopUpMenuItem(menu, GoBack, MDPI_GOBACK, ITEM_ACTION, MDPE_GOBACK, 0, data);

    if (data->itemsFlag & MDPI_GOFORWARD) 
        appendPopUpMenuItem(menu, GoForward, MDPI_GOFORWARD, ITEM_ACTION, MDPE_GOFORWARD, 0, data);

    if (data->itemsFlag & MDPI_STOP) 
        appendPopUpMenuItem(menu, Stop, MDPI_STOP, ITEM_ACTION, MDPE_STOP, 0, data);

    if (data->itemsFlag & MDPI_RELOAD) 
        appendPopUpMenuItem(menu, Reload, MDPI_RELOAD, ITEM_ACTION, MDPE_RELOAD, 0, data);

    if (data->itemsFlag & MDPI_OPEN_FRAME_IN_NEW_WINDOW) 
        appendPopUpMenuItem(menu, OpenFrame, MDPI_OPEN_FRAME_IN_NEW_WINDOW, ITEM_ACTION, MDPE_OPEN_FRAME_IN_NEW_WINDOW, 0, data);
    if (data->itemsFlag & MDPI_SEARCH_WEB) {
        appendPopUpMenuItem(menu, SearchTheWeb, MDPI_SEARCH_WEB, ITEM_ACTION, MDPE_SEARCH_WEB, 0, data);
        appendPopUpMenuItem(menu, NULL, MDPI_NO, ITEM_SEPARATOR, MDPE_NO, 0, data);
    }

    if (data->itemsFlag & MDPI_COPY)  
        appendPopUpMenuItem(menu, Copy, MDPI_COPY, ITEM_ACTION, MDPE_COPY, 0, data);

    if (data->itemsFlag & MDPI_CUT) 
        appendPopUpMenuItem(menu, Cut, MDPI_CUT, ITEM_ACTION, MDPE_CUT, 0, data);

    if (data->itemsFlag & MDPI_PASTE)  
        appendPopUpMenuItem(menu, Paste, MDPI_PASTE, ITEM_ACTION, MDPE_PASTE, 0, data);

    Uint32 font = 0;
    font |= MDPI_BOLD | MDPI_ITALIC | MDPI_UNDERLINE;
    if (data->itemsFlag & font) 
        createFontSubMenu(menu, data);

    Uint32 writing = 0;
    writing |= MDPI_DEFAULT_DIRECTION | MDPI_LEFT_TO_RIGHT | MDPI_RIGHT_TO_LEFT;
    if (data->itemsFlag & writing) 
        createWritingDirectionSubMenu(menu, data);

    if (data->itemsFlag & MDPI_INSPECT_ELEMENT) 
        appendPopUpMenuItem(menu, Inspect, MDPI_INSPECT_ELEMENT, ITEM_ACTION, MDPE_INSPECT_ELEMENT, 0, data);

    add_user_menu_items(menu);

    int x = data->x;
    int y = data->y;
    ClientToScreen(hWnd, &x, &y);
    UINT flags = TPM_TOPALIGN | TPM_LEFTALIGN;

    popupMenu = StripPopupHead(menu);
    TrackPopupMenu(popupMenu, flags, x, y, GetParent(propsheet_hwnd));
    return TRUE;
}


void popup_menu_click_proc(int commandId)
{
    POPUP_MENU_EVENT_TYPE action = (POPUP_MENU_EVENT_TYPE)(commandId-IDM_POPUP_MENU_BASE);
    if (mHwnd_popupMenu) {
        mdolphin_perform_popup_menu_event(mHwnd_popupMenu, action);
        mHwnd_popupMenu = 0;
    }
}
