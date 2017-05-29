#ifndef MINIGUI_MPHION_ADDRESS__H_
#define MINIGUI_MPHION_ADDRESS__H_

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "mdolphin_app.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CTRL_MPhone_Address  ("MPhone_Address") 

#if IS_PORTRAIT
#define XSCALE 1
#define YSCALE 1

#define SCALE_X 1
#define SCALE_Y 1

#define InputShowLen 36

//soft keyboard
#define SK_OFFX 13
#define SK_OFFY 105
#define SK_WIDTH 421*XSCALE  
#define SK_HEIGHT 147*YSCALE  

//local input
#define RL_X    13
#define RL_Y    36
#define RL_W    185
#define RL_H	37

//go button
#define GO_X    202
#define GO_Y    38
#define GO_W    20
#define GO_H	35

//home button
#define HOME_X    397*XSCALE
#define HOME_Y    21*YSCALE
#define HOME_W    55*XSCALE
#define HOME_H	  35*YSCALE

//shift button
#define SHIFT_X   (55-5)*XSCALE   
#define SHIFT_Y   (145-5)*YSCALE   
#define SHIFT_W   (60+55+5)*XSCALE
#define SHIFT_H	  (102+145+5)*YSCALE

#else

#define XSCALE WIN_WIDTH/480
#define YSCALE WIN_HEIGHT/272

#define SCALE_X 480/WIN_WIDTH
#define SCALE_Y 272/WIN_HEIGHT

#define InputShowLen 32*XSCALE  

//soft keyboard
#define SK_OFFX 30*XSCALE  
#define SK_OFFY 70*YSCALE    
#define SK_WIDTH 421*XSCALE  
#define SK_HEIGHT 147*YSCALE  

//local input
#define RL_X    30*XSCALE  
#define RL_Y    20*YSCALE   
#define RL_W    299*XSCALE  
#define RL_H	37*YSCALE   

//go button
#define GO_X    338*XSCALE   
#define GO_Y    21*YSCALE  
#define GO_W    55*XSCALE  
#define GO_H	35*YSCALE  

//home button
#define HOME_X    397*XSCALE   
#define HOME_Y    21*YSCALE   
#define HOME_W    55*XSCALE  
#define HOME_H	  35*YSCALE  

//shift button
#define SHIFT_X   (55-5)*XSCALE   
#define SHIFT_Y   (145-5)*YSCALE   
#define SHIFT_W   (60+55+5)*XSCALE
#define SHIFT_H	  (102+145+5)*YSCALE
#endif /* !IS_PORTRAIT */

BOOL RegisterMPhone_AddressControl (void);
void UnregisterMPhone_AddressControl (void);

HWND GetAddrWnd();
BOOL isAddressVisible(); //minigui visible function have bug..
void SetAddressVisible(BOOL flag);

void  SK_SetLocation(const char *url);
const char* SK_GetLocation();
void  SK_Location();

HWND  GetBrowserHwnd();
void  SK_BrowserGoto(const char* url);

void HandldLButtonMessage(int x_pos, int y_pos, int type);
void HandldMoveMessage(int x_pos, int y_pos);

char* GetInputData(BOOL isGetShowData);
void CleanInputData();
BOOL AddInputData(char*, int len, int ifUp);
BOOL DeleteInputData(int number);

RECT GetSK_Rect();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
