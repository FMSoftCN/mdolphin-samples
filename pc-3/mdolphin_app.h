#ifndef _MDOLPHIN_APP_H
#define _MDOLPHIN_APP_H

#include <minigui/mgconfig.h>

#if defined(MINIGUI_MAJOR_VERSION)&&(MINIGUI_MAJOR_VERSION == 3)
#define MINIGUI_V3 1
#endif


#include <mdolphin/mdconfig.h>


/* control height and width of browser window */
#define TOOLBAR_WIDTH       150
#define TOOLBAR_HEIGHT      32
#define LOCATION_HEIGHT     24
#define STATUSBAR_HEIGHT    18
#define PROGRESSBAR_WIDTH   120

/* ID of toolbar */
enum {
    OP_NAV_BACKWARD =10,
    OP_NAV_FORWARD,
    OP_NAV_RELOAD,
    OP_NAV_STOP,
    OP_NAV_HOME,
};

/* The position of the main menu item*/
enum {
    POS_FILE_MENU,
    POS_EDIT_MENU,
    POS_VIEW_MENU,
    POS_GO_MENU,
    POS_BOOKMARKS_MENU,
    POS_HELP_MENU,
};

/* The position of the POS_FILE_MENU submenu item*/
enum {
   POS_FILE_NEWTAB,
   POS_FILE_OPENFILE,
   POS_FILE_CLOSETAB,
   POS_FILE_SEPARATOR,
   POS_FILE_QUIT,
};

/* The position of the POS_GO_MENU submenu item*/
enum {
   POS_GO_BACK,
   POS_GO_FORWARD,
   POS_GO_RELOAD,
   POS_GO_STOP,
   POS_GO_HOME,
};

/* The position of the POS_VIEW_MENU submenu item*/
enum {
   POS_TEXT_SIZE,
   POS_CHARACTER_ENCODING,
   POS_VIEW_SEPARATOR_AGAIN,
   POS_VIEW_PAGE_SOURCE
};

/* The position of the POS_CHARACTER_ENCODING submenu item*/
enum {
   POS_AUTO_DETECT,
   POS_CHAR_SEPARATOR,
   POS_WESTERN,
   POS_UNICODE,
   POS_GB2312,
   POS_BIG5,
   POS_GBK,
};

/* The position of the POS_BOOKMARKS submenu item*/
enum {
   POS_ADD_BOOKS,
   POS_BOOK_MANAGES,
   POS_BOOK_SEPARATOR,
   POS_BOOKMARKS,
};
enum {
    /* id of control */
    IDC_PROPSHEET = 101, 
    IDC_MDOLPHIN, 
    IDC_TOOLBAR,
    IDC_STATUSBAR,
    IDC_LOCATION,
    IDC_PROGRESS,
    IDC_PROGRESSBAR,
    /* id of main menu */
    IDM_MAIN_FILE,
    IDM_MAIN_EDIT,
    IDM_MAIN_VIEW,
    IDM_MAIN_GO,
    IDM_MAIN_BOOK,
    IDM_MAIN_HELP,

    /* id of file submenu */
    IDM_FILE_NEW_TAB,
    IDM_FILE_OPENFILE,
    //IDM_FILE_SAVEAS,
    IDM_FILE_CLOSE_TAB,
    IDM_FILE_SEPARATOR,
    IDM_FILE_QUIT,

    /* id of edit submenu */
    IDM_EDIT_PROXY,
#if ENABLE_SSLFILE
    IDM_EDIT_CERTIFICATE,
#endif
    IDM_EDIT_SEPARATOR,
    IDM_EDIT_FIND,
    IDM_EDIT_FINDAGAIN,
    IDM_EDIT_SEPARATOR_AGAIN,
    IDM_EDIT_PREFERENCES,

    /* id of view submenu */
    IDM_VIEW_TEXTSIZE,
    IDM_VIEW_CHARENCODE,
    IDM_VIEW_SEPARATOR_AGAIN,
    IDM_VIEW_PAGE_SOURCE,

    /*id of text size*/
    IDM_VIEW_TEXTSIZE_ZOOMIN,
    IDM_VIEW_TEXTSIZE_ZOOMOUT,
    IDM_VIEW_TEXTSIZE_NORMAL,
 
    /*id of character encoding*/
    IDM_VIEW_CHARENCODE_AUTO,
    IDM_VIEW_CHARENCODE_SEPARATOR,
    IDM_VIEW_CHARENCODE_ISO8859_1,
    IDM_VIEW_CHARENCODE_UTF8,
    IDM_VIEW_CHARENCODE_GB2312,
    IDM_VIEW_CHARENCODE_BIG5,
    IDM_VIEW_CHARENCODE_GBK,

    /* id of go submenu */
    IDM_GO_BACK,
    IDM_GO_FORWARD,
    IDM_GO_RELOAD,
    IDM_GO_STOP,
    IDM_GO_HOME,
   
    IDM_BOOK_ADD,
    IDM_BOOK_MANAGE,
    IDM_BOOK_SEPARATOR,
    IDM_BOOK_MARKS,

    /* id of help submenu */
    IDM_HELP_ABOUT,
    /* for propsheet*/
    USER_PMS_RESIZE,
    USER_PMS_STATUS,
    USER_PMS_LOAD,
    USER_PMS_PROGRESS,
    USER_PMS_LOCATION,
    USER_PMS_TITLE,
    USER_PMS_BACK,
    USER_PMS_FORWARD,
    USER_PMS_VIEWMODE,
    USER_PMS_ENCODING,
    USER_PMS_CLOSE_SEARCHFOR,
    /* user defined popup menu items*/
    IDM_POP_NEW_TAB,
    IDM_POP_CLOSE_TAB,
    IDM_POP_SAVE_AS,

    /* id for bookmarks list*/
    IDM_BOOK_FIRST,
    /* id for popup menu*/
    IDM_POPUP_MENU_BASE = 1000,
};

#endif
