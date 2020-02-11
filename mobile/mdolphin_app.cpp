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


HWND mdolphin_hwnd;
HWND location_hwnd;

static bool focusOnMdolphin = false; 
static int win_width = 128;
static int win_height = 160;
const char * home_url = "http://www.google.com";


//void location_entry_open_url(HWND hwnd, int id, int nc, DWORD add_data)	// gengyue
void location_entry_open_url(HWND hwnd, long int id, int nc, DWORD add_data)
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

static void create_location_window(HWND hParent)
{
    location_hwnd = CreateWindowEx (CTRL_SLEDIT, "",
            WS_CHILD | WS_BORDER | WS_VISIBLE,
             0,IDC_LOCATION, 0, 0, win_width -5, LOCATION_HEIGHT,
            hParent, 0);

    if(location_hwnd == HWND_INVALID)
        printf ("mDolphin: Can not create location bar window.\n");

    SetNotificationCallback (location_hwnd, location_entry_open_url);
}



static void create_mdolphin_window(HWND hParent)
{
    mdolphin_hwnd = CreateWindow (MDOLPHIN_CTRL,
            "",
            WS_VISIBLE | WS_HSCROLL| WS_VSCROLL| WS_CHILD,
            IDC_MDOLPHIN,
            0, 20, win_width-5,MDOLPHIN_HEIGHT, hParent, 0);
}
static void my_message_callback (HWND parent, const char * text, const char * caption)
{
//	fprintf(stderr,"MessageBox is run the text is [%s],caption is [%s] \n",text,caption);
	    MessageBox (parent, text, caption, MB_OK);
}

//int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)		// gengyue
LRESULT MDolphinProc (HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam)
{
/*	if(hWnd == mdolphin_hwnd)
		    fprintf(stderr," get message %d, Wparam %d ,Lparam %d \n",message,wParam,lParam);	
*/
	switch (message)
	{
		case MSG_CREATE:
			create_location_window(hWnd);
			create_mdolphin_window(hWnd);
			
			CB_INFO cb_info;
			memset (&cb_info, 0, sizeof(CB_INFO));

			cb_info.CB_MESSAGE_BOX = my_message_callback;
            mdolphin_set_callback_info(mdolphin_hwnd, &cb_info);
			
            mdolphin_set_rendering_mode(mdolphin_hwnd, MD_SMALLVIEW_MODE, 115);
            mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
			break;
		case MSG_KEYDOWN:
			if(wParam == SCANCODE_F3)
			{ 
				if(location_hwnd == GetFocusChild(hWnd))
				{
					int length;
					char* url_string = NULL;
					length = SendMessage(location_hwnd, MSG_GETTEXTLENGTH, 0, 0);
					url_string = (char*)malloc (length + 1);
					memset (url_string, 0, length+1);
					GetWindowText (location_hwnd, url_string, length);

					if (url_string)
					{
                        mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, url_string, FALSE);
						SetFocus(mdolphin_hwnd);
						free (url_string);
					}

				}
				else
				SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_ENTER), 0);

			}
			if(wParam == SCANCODE_F2)
			{
				focusOnMdolphin =! focusOnMdolphin;
				if(focusOnMdolphin)
					SetFocusChild(mdolphin_hwnd);
				else
					SetFocusChild(location_hwnd);
				return 0 ;
			}
			if(wParam == SCANCODE_CURSORBLOCKRIGHT )
			{
	//			fprintf(stderr,"SCANCODE_CURSORBLOCKRIGHT\n");
				SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), 0);
				return 0;
			}
			if(wParam == SCANCODE_CURSORBLOCKLEFT)
			{
	//			fprintf(stderr,"SCANCODE_CURSORBLOCKLEFT\n");
				SendMessage (mdolphin_hwnd, MSG_KEYDOWN, WPARAM(SCANCODE_TAB), (LPARAM)KS_SHIFT);
				return 0;
			}


                 break;
		case MSG_DESTROY:
			DestroyAllControls (hWnd);
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			return 0;
	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* argv[])
{


	if(argv[1])
		        home_url=argv[1];

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (2, 0, 2)
#if defined (_MGRM_PROCESSES) 
    if (JoinLayer (NULL, argv[0], 0, 0) == INV_LAYER_HANDLE){
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif
#endif

    MSG Msg;
    MAINWINCREATE CreateInfo;


    RegisterMDolphinControl ();

#ifdef MINIGUI_V3
    SetDefaultWindowElementRenderer(argv[2]);
#else
    if (!InitMiniGUIExt()) {
        fprintf(stderr, "Can not initialize MiniGUI Ext!\n");
        return -11;
    }
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE|WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mDolphin";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hCursor = 0;
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MDolphinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = win_width;
    CreateInfo.by = win_height;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    HWND hMainWnd;

    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
       return -1;
 

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    SetWindowBkColor (hMainWnd, RGB2Pixel (HDC_SCREEN, 230, 246, 255));
    InvalidateRect (hMainWnd, NULL, TRUE);
    
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnregisterMDolphinControl ();

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}
