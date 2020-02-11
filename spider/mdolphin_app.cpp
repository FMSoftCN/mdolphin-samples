#include "mdolphin_app.h"
#include <mdolphin/mdolphin.h>
#include "mdolphin_urldatabase.h"
#include <signal.h>

extern void loadNewUrl();
extern void set_callback_info(HWND hWnd);

HWND mdolphin_hwnd;
HWND hMainWnd;
char *currentURL;

static int win_width = 800;
static int win_height = 600;
const char *home_url = "http://www.google.com";

void dump(int signo)
{
	char *sig ;
	if(signo == SIGSEGV)
		sig = "SIGEGV";
	if(signo == SIGTERM)
		sig = "SIGTERM";
	if(signo == SIGINT)
		sig = "SIGINT, may be ctrl+c";

	char*log;
	asprintf(&log, "program error when run [%s] , error is [%s]!", 
			currentURL, sig);

	saveLogmsg(log);  //save error msg

	if(log)
		free(log);

	destroy_db();
	exit(0);
}


void registSignal()
{
	signal(SIGSEGV, dump);
	signal(SIGTERM, dump);
	signal(SIGINT, dump);
}

static void create_mdolphin_window(HWND hParent)
{
    mdolphin_hwnd = CreateWindow (MDOLPHIN_CTRL,
            "",
            WS_VISIBLE | WS_HSCROLL| WS_VSCROLL| WS_CHILD,
            IDC_MDOLPHIN,
            0, 0, win_width-5,win_height-5, hParent, 0);
}

//int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)	// gengyue
LRESULT MDolphinProc (HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_CREATE:
			hMainWnd = hWnd; // a bad way....
			create_mdolphin_window(hWnd);
			set_callback_info(hWnd);
			saveURL(home_url);
			loadNewUrl();
			break;

        case MSG_TIMER:
			if (wParam == TIMER_ID) {
				char*log;
				asprintf(&log, "open the url [%s] timeout!", currentURL);
				saveLogmsg(log);
				if(log)
					free(log);

				loadNewUrl();
			}
            break;

		case MSG_DESTROY:
			DestroyAllControls (hWnd);
			return 0;

		case MSG_CLOSE:
			KillTimer (hWnd, TIMER_ID);
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			destroy_db();
			return 0;
	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* argv[])
{
	if(argv[1])
		home_url=argv[1];

	currentURL = (char*)malloc(sizeof(char)*256);

	registSignal();

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

    CreateInfo.dwStyle = WS_VISIBLE|WS_BORDER;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "mDolphin NetSpider";
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

#ifdef _USE_MINIGUIENTRY 
void * start_routine ( void *arg)
{
	minigui_entry(0,NULL);
}

int main (int args, const char* argv[])
{
	if(args>1 && argv[1])
		home_url=argv[1];

	pthread_attr_t tattr;
	pthread_t tid;
	void *arg;
	int ret;


	/* initialized with default attributes */
	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize (&tattr, 256 * 1024);

	/* default behavior specified*/
	ret = pthread_create(&tid, &tattr, start_routine, NULL);
	pthread_join(tid, NULL);

	return ret;
}
#endif 
