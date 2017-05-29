#include "mdolphin_app.h"
#include "mdolphin_skin.h"
#include "mdolphin_transform.h"
#include "mdolphin_animation.h"


extern void SetBrowserCallbacks(HWND hParent);
extern void HandleLButtonMessage(int x_pos, int y_pos);
extern BOOL HandleMouseMessage(int message, WPARAM wParam, LPARAM lParam);
extern BOOL DispatchMouseMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

extern HMENU CreatPopupMenu();
extern void CreateSkBorad(HWND hParent);

static int win_width = WIN_WIDTH;
static int win_height = WIN_HEIGHT;

static PLOGFONT cap_font = NULL;
static PLOGFONT ctrl_font = NULL;
static PLOGFONT utf8_font = NULL;

TransContext* g_pCtxt = NULL;

static void init_mdolphin()
{
    cap_font = g_SysLogFont[SYSLOGFONT_CAPTION];
    ctrl_font = g_SysLogFont[SYSLOGFONT_CONTROL];

    utf8_font = CreateLogFontByName("*-Arial-rrncnn-*-12-UTF-8");

    g_SysLogFont[SYSLOGFONT_CAPTION] = utf8_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = utf8_font;

    local_SysText = GetSysTextInUTF8("zh_CN");
}

static void release_mdolphin()
{
    g_SysLogFont[SYSLOGFONT_CAPTION] = cap_font;
    g_SysLogFont[SYSLOGFONT_CONTROL] = ctrl_font;

    DestroyLogFont(utf8_font);
}

#ifdef __ECOS__
const char* home_url = "file:///homepage.html";
#else
const char* home_url = "http://www.minigui.com";
#endif 

HWND hMainWnd;
HWND slide_hwnd;
HWND location_hwnd;
HWND hPageView;
HWND hAnimationWnd;
HMENU popup_menu;

#ifdef _IME_GB2312
#if USE_SW_KB
GHANDLE ime_cont;
#else
HWND ime_hwnd;
#endif
#endif

int MDolphinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    switch (message) {
        case MSG_CREATE: {
            RECT rc;
            HWND hBrowser;

            hMainWnd = hWnd;

            GetElementGeometry(MDOLPHIN_ELEMENT, &rc);

            hPageView = CreatePageView(hWnd, WS_CHILD, &rc);
            hAnimationWnd = CreateAnimationWindow(hWnd);
            hBrowser = CreateMdolphinWindow("", WS_CHILD| WS_VISIBLE);
            mdolphin_set_scale_factor(hBrowser, 1.0, 1.0);
			CreateSkBorad(hWnd);

            SetBrowserCallbacks(hBrowser);
            mdolphin_navigate(hBrowser, NAV_GOTO, home_url, FALSE);
            break;
        }
        case MSG_COMMAND:
            break;
        case MSG_KEYDOWN: {
			HWND hwnd = GetActiveWindowInPageView(); 
#if 0
            if (!IsWindowVisible(hAnimationWnd))
                break;

            switch (wParam){
				case SCANCODE_CURSORBLOCKLEFT:
					ScrollPage(hwnd, MOVETO_PREV);
					break;
				case SCANCODE_CURSORBLOCKRIGHT:
					ScrollPage(hwnd, MOVETO_NEXT);
					break;
				case SCANCODE_HOME:
					ScrollPage(hwnd, MOVETO_FIRST);
					break;
				case SCANCODE_END:			
					ScrollPage(hwnd, MOVETO_LAST);
					break;
			}
#else
            int currentX = -1, currentY = -1;
            mdolphin_get_contents_position(hwnd, &currentX, &currentY);
            switch (wParam){
				case SCANCODE_CURSORBLOCKLEFT:
                    currentX -= 10;
                    mdolphin_set_contents_position(hwnd, currentX, currentY);
					break;
				case SCANCODE_CURSORBLOCKRIGHT:
                    currentX += 10;
                    mdolphin_set_contents_position(hwnd, currentX, currentY);
					break;
				case SCANCODE_CURSORBLOCKUP:
                    currentY -= 10;
                    mdolphin_set_contents_position(hwnd, currentX, currentY);
					break;
				case SCANCODE_CURSORBLOCKDOWN:
                    currentY += 10;
                    mdolphin_set_contents_position(hwnd, currentX, currentY);
					break;
				case SCANCODE_HOME:
                    mdolphin_set_contents_position(hwnd, 0, 0);
					break;
				case SCANCODE_END:			
					break;
			}
#endif
            break;
        }
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            HandlePaintMessage(hWnd, hdc);
            EndPaint(hWnd, hdc);
            return 0;
		case MSG_MOUSEMOVE:
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            if (DispatchMouseMessage(hWnd, message, wParam, lParam))
                return 0;
            break;
        case MSG_DESTROY:
            DestroyAllControls(hWnd);
            break;
        case MSG_CLOSE:
            PostQuitMessage(hWnd);
            DestroyMainWindow(hWnd);
            return 0;
        case MSG_ERASEBKGND: {
                HDC hdc = (HDC)wParam;
                const RECT* clip = (const RECT*) lParam;
                BOOL fGetDC = FALSE;
                RECT rcTemp;

                if (hdc == 0) {
                    hdc = GetClientDC (hWnd);
                    fGetDC = TRUE;
                }

                if (clip) {
                    rcTemp = *clip;
                    ScreenToClient(hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient(hWnd, &rcTemp.right, &rcTemp.bottom);
                    IncludeClipRect(hdc, &rcTemp);
                }

                PaintMainWndBackground(hdc);

                HandlePaintTitle(hdc);

                HandlePaintBottom(hdc);

                if (fGetDC)
                    ReleaseDC (hdc);
                return 0;
            }
    }
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int MiniGUIMain(int args, const char* argv[])
{
	if (args > 1 && argv[1])
		home_url = argv[1];

    MSG Msg;
    MAINWINCREATE CreateInfo;
	if (!RegisterMPhone_AddressControl ())
		return 0;

#if _MINIGUI_VERSION_CODE >= _VERSION_CODE (2, 0, 2)
#if defined (_MGRM_PROCESSES) 
    int i;
    const char* layer = NULL;

    for (i = 1; i < args; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i + 1];
            break;
        }
    }

    GetLayerInfo (layer, NULL, NULL, NULL);
    if (JoinLayer(layer, argv[0], 0, 0) == INV_LAYER_HANDLE){
        printf("JoinLayer: invalid layer handle.\n");
        exit(1);
    }
#endif
#endif

    init_mdolphin();

    RegisterMyAnimation();
    RegisterPageViewControl();
    RegisterMDolphinControl();

    InitSkinElements();

    CreateInfo.dwStyle = WS_VISIBLE;
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
    CreateInfo.iBkColor = 0x37d8a;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    hMainWnd = CreateMainWindow(&CreateInfo);
    if (hMainWnd == HWND_INVALID)
       return -1;
 

#if !defined(_LITE_VERSION) && !defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
#ifdef _IME_GB2312
    ime_hwnd = GBIMEWindow (hMainWnd);
#endif
#endif

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    InvalidateRect (hMainWnd, NULL, TRUE);
    
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnregisterMyAnimation();
    UnregisterPageViewControl();
    UnregisterMDolphinControl();
	UnregisterMPhone_AddressControl();

    release_mdolphin();

    DestroySkinElements();

    MainWindowThreadCleanup (hMainWnd);
    return 0;
}

#ifdef _USE_MINIGUIENTRY 
void* start_routine( void *arg)
{
	minigui_entry(0,NULL);
}

int main(int args, const char* argv[])
{
	if (args > 1 && argv[1])
		home_url = argv[1];

	pthread_attr_t tattr;
	pthread_t tid;
	void *arg;
	int ret;

	/* initialized with default attributes */
	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize(&tattr, 128 * 1024);

	/* default behavior specified*/
	ret = pthread_create(&tid, &tattr, start_routine, NULL);
	pthread_join(tid, NULL);
	return ret;
}
#endif 

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
