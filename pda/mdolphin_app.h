#ifndef _MDOLPHIN_APP_H
#define _MDOLPHIN_APP_H

#include <minigui/mgconfig.h>

#if defined(MINIGUI_MAJOR_VERSION)&&(MINIGUI_MAJOR_VERSION == 3)
#define MINIGUI_V3 1
#endif

#define __IPAQ5450__ 1
#undef __ECOS__           //if use on eCos platform should define __ECOS__ maco

#define USE_SW_KB 0 //note: must enable IME_GB2312 in minigui firt, if you want to ues soft keyboard 
#define LoadResFromFile 0 //if want to load the Res from file ,define LoadResFromFile  1  

/* control height and width of browser window */
#if 0
#define WIN_WIDTH 320 
#define WIN_HEIGHT 240
#define FIND_WIDTH  (WIN_WIDTH*2/3)
#define FIND_HEIGHT (WIN_HEIGHT/2)
#else
extern int win_width;
extern int win_height;

#define WIN_WIDTH  win_height 
#define WIN_HEIGHT win_height
//#define WIN_WIDTH 240 
//#define WIN_HEIGHT 320
#define FIND_WIDTH  (WIN_WIDTH)
#define FIND_HEIGHT (WIN_HEIGHT/2)
#endif


#define PRE_WIDTH (WIN_WIDTH)
#define PRE_HEIGHT (WIN_HEIGHT)
#define PLUG_WIDTH (WIN_WIDTH)
#define PLUG_HEIGHT (WIN_HEIGHT)
#define PROX_WIDTH  (WIN_WIDTH)        
#define PROX_HEIGHT  (WIN_HEIGHT)      

#define CAPTION_HEIGHT      20 
#define LOCATION_HEIGHT     20
#define BOTTOM_HEIGHT		22 

#if LoadResFromFile
#define ResPATH "./res/"  // here setting the image res path
#endif

#ifdef __ECOS__
#include <cyg/kernel/kapi.h>

#include <dirent.h>
#include <cyg/fileio/fileio.h>
#include <unistd.h> 

#include <semaphore.h>

#endif // #ifdef __ECOS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#ifdef MINIGUI_V3
#include <mgutils/mgutils.h>
#else
#include <minigui/mgext.h>
#include <minigui/newfiledlg.h>
#endif
#include <mdolphin/mdolphin.h>

enum {
    /* id of control */
    IDC_MDOLPHIN = 101, 
    IDC_INPUT,
    IDC_LOCATION,
    /* id of main menu */
    IDM_MAIN,
    IDM_MAIN_GO_HOME,
    IDM_MAIN_BACK,
    IDM_MAIN_RELOAD,
    IDM_MAIN_VIEW,
    IDM_MAIN_TEXTSIZE,
	IDM_MAIN_CHARACTER,
    /* id of view submenu */

    IDM_VIEW_SMALLVIEW,
    IDM_VIEW_SCREENVIEW,
    IDM_VIEW_VIRTUALVIEW,
    IDM_VIEW_SEPARATOR,
    IDM_VIEW_PAGE_SOURCE,

    /*id of text size*/
    IDM_VIEW_TEXTSIZE_ZOOMIN,
    IDM_VIEW_TEXTSIZE_ZOOMOUT,
    IDM_VIEW_TEXTSIZE_NORMAL,
 
    /*id of character encoding*/
    IDM_VIEW_CHARENCODE_AUTO,
    IDM_VIEW_CHARENCODE_ISO8859_1,
    IDM_VIEW_CHARENCODE_UTF8,
    IDM_VIEW_CHARENCODE_GB2312,
    IDM_VIEW_CHARENCODE_BIG5,

    /* id of edit submenu */
    IDM_EDIT_PROXY,
    IDM_EDIT_SEPARATOR,
    IDM_EDIT_FIND,
    IDM_EDIT_FINDAGAIN,
    IDM_EDIT_SEPARATOR_AGAIN,
    IDM_EDIT_PREFERENCES,

};

#endif
