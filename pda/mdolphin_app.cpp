#include "mdolphin_app.h"

#ifdef _IME_GB2312
	#if USE_SW_KB
		#include "mgime.h"
		#include "keyboard.h"
	#endif
#endif

#ifdef __ECOS__
MTAB_ENTRY( romfs_mte1,
		"/",
		"romfs",
		"",
		(CYG_ADDRWORD)/*&filedata[0]*/ 0x50100000 /*0x34200000*/  );
#endif

extern int handle_key_ecos(int wParam);
extern int handle_key_ipaq5450(int wParam);
extern void set_callback_info(HWND hParent);
extern void HandlePaintMessage(HDC hdc);
extern void HandldLButtonMessage(int x_pos, int y_pos);
extern void HandleCommandMessage(int id);
extern void ModifyImageShowState(int x_pos,int y_pos);
extern int LoadBitmapResource();

extern void location_entry_open_url(HWND hwnd, int id, int nc, DWORD add_data);
extern void create_location_window(HWND hParent);
extern void create_mdolphin_window(HWND hParent);
extern HMENU create_submenu(const char* menu_item[], int nr_item, int idm);
extern HMENU creat_popupmenu();
extern BITMAP title_bitmap;
extern BITMAP title_offset_bitmap;
extern BITMAP bottom_offset_bitmap;


int viewmode = 1 ; // 1 =" screen view" 2 = "virtual" 3 = "samll view"
//int win_width = WIN_WIDTH;
//int win_height = WIN_HEIGHT;
int win_width = 0;
int win_height = 0;

static PLOGFONT cap_font = NULL;
static PLOGFONT ctrl_font = NULL;
static PLOGFONT utf8_font = NULL;

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

RECT Title_offset_Rect = {200,0,win_width-220, CAPTION_HEIGHT};

RECT Bottom_offset1_Rect = {80,win_height -BOTTOM_HEIGHT,win_width/2-40-80, BOTTOM_HEIGHT};
RECT Bottom_offset2_Rect = {win_width/2+40,win_height -BOTTOM_HEIGHT,win_width/2-40-80, BOTTOM_HEIGHT};

RECT Title_Rect = {0,0,220, CAPTION_HEIGHT};
RECT Close_Rect = {win_width -20 , 0, 20-1,CAPTION_HEIGHT-1};

RECT Home_Rect =  {0, CAPTION_HEIGHT,20, LOCATION_HEIGHT};
RECT Location_Rect ={20,CAPTION_HEIGHT, win_width -20-20-2 , LOCATION_HEIGHT};
RECT GoURL_Rect = {win_width - 22, CAPTION_HEIGHT,20, LOCATION_HEIGHT};

RECT Mdolphin_Rect = {0,LOCATION_HEIGHT+1+CAPTION_HEIGHT, win_width-4,win_height-BOTTOM_HEIGHT-CAPTION_HEIGHT-LOCATION_HEIGHT-1};

RECT Back_Rect = {0,win_height -BOTTOM_HEIGHT ,80,BOTTOM_HEIGHT };
RECT Ime_Rect= {win_width/2-40,win_height -BOTTOM_HEIGHT,80,BOTTOM_HEIGHT};
RECT Menu_Rect = {win_width-80,win_height -BOTTOM_HEIGHT,80,BOTTOM_HEIGHT};

#ifdef __ECOS__
const char * home_url = "file:///homepage.html";
#else
const char * home_url = "http://www.minigui.com";
#endif 

HWND hMainWnd;
#ifdef _IME_GB2312
	#if USE_SW_KB
		GHANDLE ime_cont;
	#else
		HWND ime_hwnd;
	#endif
#endif

HWND mdolphin_hwnd;
HMENU popup_menu;
HWND location_hwnd;

int MDolphinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
	switch (message)
	{
		case MSG_COMMAND:
			HandleCommandMessage(LOWORD(wParam));
			break;
		case MSG_CREATE:
			create_location_window(hWnd);
			create_mdolphin_window(hWnd);
			creat_popupmenu();
			set_callback_info(hWnd);
            mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, home_url, FALSE);
			break;

		case MSG_KEYDOWN:
			if( !handle_key_ecos(wParam) )
				return 0;
			if( !handle_key_ipaq5450(wParam) )
				return 0;
			break;

		case MSG_PAINT:
			hdc = BeginPaint (hWnd);
			HandlePaintMessage(hdc); 
			EndPaint (hWnd, hdc);
			return 0;

		case MSG_MOUSEMOVE:
			if(hWnd == hMainWnd) //only handle main window message
				ModifyImageShowState(x_pos,y_pos);
			break;

		case MSG_LBUTTONDOWN:
			if(hWnd == hMainWnd) //only handle main window message
				HandldLButtonMessage( x_pos,  y_pos);
			break;

		case MSG_DESTROY:
			DestroyAllControls (hWnd);
			return 0;

		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			return 0;
		case MSG_ERASEBKGND:
			{
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
					ScreenToClient (hWnd, &rcTemp.left, &rcTemp.top);
					ScreenToClient (hWnd, &rcTemp.right, &rcTemp.bottom);
					IncludeClipRect (hdc, &rcTemp);
				}
				FillBoxWithBitmap(hdc,
						Title_Rect.left, Title_Rect.top, Title_Rect.right,Title_Rect.bottom,
						&title_bitmap);

				FillBoxWithBitmap(hdc,
						Title_offset_Rect.left, Title_offset_Rect.top, Title_offset_Rect.right,
						Title_offset_Rect.bottom,&title_offset_bitmap);
				FillBoxWithBitmap(hdc,
						Bottom_offset1_Rect.left,Bottom_offset1_Rect.top,Bottom_offset1_Rect.right,
						Bottom_offset1_Rect.bottom,&bottom_offset_bitmap);
				FillBoxWithBitmap(hdc,
						Bottom_offset2_Rect.left,Bottom_offset2_Rect.top,Bottom_offset2_Rect.right,
						Bottom_offset2_Rect.bottom,&bottom_offset_bitmap);

				if (fGetDC)
					ReleaseDC (hdc);
				return 0;
			}
	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}


void initRect()
{
	Title_offset_Rect.left = 200;
	Title_offset_Rect.top = 0;
	Title_offset_Rect.right = win_width - 220;

	Bottom_offset1_Rect.top = win_height - BOTTOM_HEIGHT;
	Bottom_offset1_Rect.right = win_width/2-40-80;

	Bottom_offset2_Rect.left = win_width/2+40;
	Bottom_offset2_Rect.top = win_height - BOTTOM_HEIGHT;
	Bottom_offset2_Rect.right = win_width/2-40-80;

	Close_Rect.left = win_width - 20;

	Location_Rect.right = win_width -20-20-2;

	GoURL_Rect.left = win_width - 22;
	Mdolphin_Rect.right = win_width-4;
	Mdolphin_Rect.bottom = win_height-BOTTOM_HEIGHT-CAPTION_HEIGHT-LOCATION_HEIGHT-1;

	Back_Rect.top = win_height -BOTTOM_HEIGHT;

	Ime_Rect.left = win_width/2-40;
	Ime_Rect.top = win_height -BOTTOM_HEIGHT;
	Menu_Rect.left = win_width-80 ; 
	Menu_Rect.top = win_height -BOTTOM_HEIGHT ;
}

int MiniGUIMain (int args, const char* argv[])
{

#ifdef __ECOS__
	//here is test the file system of ecos
	FILE* fd = NULL;
	if ( !(fd = fopen ("/homepage.html", "r")))
		printf("\n open file /homepage.html error \n");
	else
		printf("\nopen file /homepage.html success\n");
	fclose(fd);
#else
	if(args>1 && argv[1])
		home_url=argv[1];
#endif

    MSG Msg;
    MAINWINCREATE CreateInfo;

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

    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE){
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

#endif

    init_mdolphin();
    RegisterMDolphinControl();

	if (!LoadBitmapResource()) {
		printf("LoadBitmapResource fail \n");
		return 1;
	}

	win_width = g_rcScr.right;
	win_height = g_rcScr.bottom;

	initRect();

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
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
       return -1;
 
#if !defined(_LITE_VERSION) && !defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
#ifdef _IME_GB2312
	#if USE_SW_KB
	HWND sk_ime_hwnd;
	MGI_SOFTKEYBOARD_INFO sk_info;
	int ret;
	bzero(&sk_info, sizeof(MGI_SOFTKEYBOARD_INFO));
	sk_ime_hwnd = mgiCreateSoftKeyboardIME(&sk_info, NULL);
	//TEST_INFO("mgiCreateSoftKeyboardIME  finished");

	ime_cont = mgiCreateIMEContainer(2, FALSE);
	//TEST_INFO("mgiCreateIMEContainer  finished");

	ret = mgiAddIMEWindow(ime_cont, sk_ime_hwnd, "softkeyboard");
	//TEST_VAL(ret, %d);
	ret = mgiAddSKBTranslateOps(sk_ime_hwnd, &quanpin_trans_ops);
	#else
		ime_hwnd = GBIMEWindow (hMainWnd);
	#endif
#endif
#endif


    ShowWindow(hMainWnd, SW_SHOWNORMAL);
	SetWindowBkColor(hMainWnd,RGB2Pixel (HDC_SCREEN, 0, 84, 236));
    InvalidateRect (hMainWnd, NULL, TRUE);
    
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnregisterMDolphinControl ();
    release_mdolphin();

#if !defined(_LITE_VERSION) && !defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
#ifdef _IME_GB2312
	#if USE_SW_KB
    mgiDestroyIMEContainer (ime_cont);
	#endif
#endif
#endif
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

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
