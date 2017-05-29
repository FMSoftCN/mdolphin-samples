#ifndef _MDOLPHIN_APP_H
#define _MDOLPHIN_APP_H

#include <string.h>

#include <minigui/mgconfig.h>

#if defined(MINIGUI_MAJOR_VERSION)&&(MINIGUI_MAJOR_VERSION == 3)
#define MINIGUI_V3 1
#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define TIMER_ID 100
#define TIMERS 60*100  //60 seconds

/* ID of toolbar */
enum {
    OP_NAV_BACKWARD =10,
    OP_NAV_FORWARD,
    OP_NAV_RELOAD,
    OP_NAV_STOP,
    OP_NAV_HOME,
};

enum {
    /* id of control */
    IDC_MDOLPHIN = 101, 
    IDC_TOOLBAR,
    IDC_STATUSBAR,
    IDC_LOCATION,
    IDC_PROGRESSBAR,
    /* id of main menu */
    IDM_MAIN_FILE,
    IDM_MAIN_EDIT,
    IDM_MAIN_VIEW,
    IDM_MAIN_GO,
    IDM_MAIN_HELP,

    /* id of file submenu */
    IDM_FILE_NEWWIN,
    IDM_FILE_OPENLOC,
    IDM_FILE_OPENFILE,
    IDM_FILE_CLOSEWIN,
    IDM_FILE_SAVEPAGEAS,
    IDM_FILE_QUIT,

    /* id of edit submenu */
    IDM_EDIT_PROXY,
    IDM_EDIT_FINDIN,
    IDM_EDIT_FINDAGAIN,

    /* id of view submenu */

    IDM_VIEW_SMALLVIEW,
    IDM_VIEW_TEXTSIZE,
 
    /* id of go submenu */
    IDM_GO_BACK,
    IDM_GO_FORWARD,
    IDM_GO_RELOAD,
    IDM_GO_STOP,
    IDM_GO_HOME,
    IDM_GO_HISTORY,
   
    /* id of help submenu */
    IDM_HELP_ABOUT,
};

#endif
