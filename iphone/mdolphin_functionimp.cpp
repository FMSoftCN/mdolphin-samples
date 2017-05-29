#include <unistd.h>
#include <math.h>
#include "mdolphin_app.h"
#include "mdolphin_pageview.h"
#include "mdolphin_skin.h"
#include "mdolphin_transform.h"
#include "mphone_addressField.h"
#include "mdolphin_skin.h"

#define MAX_DRAG_STEP   20
#define LONG_PRESS_TIME   15

extern void CreateFindWindow(HWND hParent);
extern void	CreateSettingsWindow(HWND hParent);
extern void find_again_with_last_result();
extern void CreateProxyWindow(HWND hParent);
extern void draw_addr_btn_status(BOOL enable);
extern void draw_zoom_btn_status(BOOL enable);

static BOOL capture_mouse_message(int message, WPARAM wParam, LPARAM lParam);

static int prompt_box(HWND parent, const char* info, const char* definp, char* buf, int len);
int SwitchViewMode(HWND hWnd, int from, int to);

extern HWND hMainWnd;
extern HWND hAnimationWnd;
extern HWND hNavigatorWnd;
extern HWND location_hwnd;

extern ELEMENT_DATA skin_elements[MAX_ELEMENT]; 
extern TransContext* g_pCtxt;
extern const char* home_url;

static char prompt_text[256];
static char *inputboxbuf;
static int inputboxbuflen;

static BOOL g_panView = TRUE;
typedef struct CaptureContxt {
    unsigned int t;
    int press;
    WPARAM wParam;
    LPARAM lParam;
}CaptureContext;

static CaptureContext m_captureContext = {0, 0, 0, 0};

void CreateSkBorad(HWND hParent)
{
	RECT rc;
	GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
	CreateWindowEx(CTRL_MPhone_Address, "", WS_CHILD, 0x20000000L, IDC_ADDRESS,
			rc.left, rc.top, RECTW(rc), RECTH(rc), hParent, 0);
}

static void draw_pan_view_status()
{
    if (skin_elements[PANVIEW_ELEMENT].bEnabled != g_panView) {
        skin_elements[PANVIEW_ELEMENT].bEnabled = g_panView ;
        InvalidateRect(hMainWnd, skin_elements[PANVIEW_ELEMENT].prc, TRUE);
    }
}

void HandleLButtonMessage(int x, int y)
{
	int index = GetElementByPostion(x, y);
    HWND hBrowser = GetActiveWindowInPageView();
    if (hBrowser == HWND_INVALID|| hBrowser == HWND_NULL)
        return; 
    
    switch (index) {
       case BACK_ELEMENT:
            if (hBrowser)
                mdolphin_navigate(hBrowser, NAV_BACKWARD, NULL, FALSE);
            break;
        case FORWARD_ELEMENT:
            if (hBrowser)      
                mdolphin_navigate(hBrowser, NAV_FORWARD, NULL, FALSE);
            break;
        case REFRESH_ELENENT:
            if (hBrowser)
                mdolphin_navigate(hBrowser, NAV_RELOAD, NULL, FALSE);
            break;
        case STOP_ELEMENT:
            if (hBrowser)
                mdolphin_navigate(hBrowser, NAV_STOP, NULL, FALSE);
            break;
        case ADDR_ELEMENT:
			if (GetViewMode(hBrowser) == PAGE_VIEW)
				return ;

			if (isAddressVisible()) {
				ShowWindow(GetAddrWnd(), SW_HIDE);
				SetFocus(GetBrowserHwnd());
			} else {
				ShowWindow(GetAddrWnd(), SW_SHOW);
				SetFocus(GetAddrWnd());
			}

            SetAddressVisible(!isAddressVisible());
            break;
        case ZOOMIN_ELEMENT:
            if (hBrowser) {
#if 1
                float x, y;
                mdolphin_get_scale_factor(hBrowser, &x, &y);
                x -= 0.2;
                y -= 0.2;
                mdolphin_set_scale_factor(hBrowser, x, y);
#else
                int percent;
                mdolphin_get_text_size_multiplier(hBrowser, &percent);
                percent -=10;
                mdolphin_set_text_size_multiplier(hBrowser, percent);
#endif                
            }
            break;
        case ZOOMOUT_ELEMENT:
            if (hBrowser) {
#if 1
                float x, y;
                mdolphin_get_scale_factor(hBrowser, &x, &y);
                x += 0.2;
                y += 0.2;
                mdolphin_set_scale_factor(hBrowser, x, y);
#else
                int percent;
                mdolphin_get_text_size_multiplier(hBrowser, &percent);
                percent +=10;
                mdolphin_set_text_size_multiplier(hBrowser, percent);
#endif                
            }
            break;
        case PAGEVIEW_ELEMENT: {
			int mode = GetViewMode(hBrowser);
            switch (mode) {
                case NORMAL_VIEW:
				    SwitchViewMode(hAnimationWnd, NORMAL_VIEW, PAGE_VIEW);
					SetViewMode(hBrowser, PAGE_VIEW);
                    break;
                case PAGE_VIEW:
				    SwitchViewMode(hAnimationWnd, mode, NORMAL_VIEW);
					SetViewMode(hBrowser, NORMAL_VIEW);
                    break;
                default:
                    break;
            }
			break;
        }
        case NEWPAGE_ELEMENT:
            (void)my_create_mdolphin_window("about:blank", WS_CHILD, 0, 0, -1, -1);
            break;
        case PANVIEW_ELEMENT: 
            {
                if (g_panView)
                    g_panView = FALSE;
                else
                    g_panView = TRUE;
                draw_pan_view_status();
                break;
            }
#if 0
#if 0            
            PostMessage(hMainWnd, MSG_CLOSE, 0, 0);
            return;
#else
            _exit(0);
#endif
#endif
        case CLOSEPAGE_ELEMENT: {
			RECT rc;
            if (GetViewMode(hBrowser) != PAGE_VIEW)
				break;

            GetElementGeometry(CLOSEPAGE_ELEMENT, &rc); 
            if (PtInRect(&rc, x, y)) {
                HWND hActive = GetActiveWindowInPageView();
                PAGE_DATA* page = FindPage(hActive, MOVETO_NEXT);
                int dir = MOVETO_PREV;

                if (GetPageCount() == 1)
                    break;

                if (!page) {
                    page = FindPage(hActive, MOVETO_PREV);
                    dir = MOVETO_NEXT;
                }

                my_close_mdolphin_window(hActive);
                ScrollPageInView(page->hwnd, dir);
            } else {
				    SwitchViewMode(hAnimationWnd, GetViewMode(hBrowser) , NORMAL_VIEW);
				    SetViewMode(hBrowser, NORMAL_VIEW);
			}
            break;
        }
        case HOME_ELEMENT:
            if (hBrowser)
                mdolphin_navigate(hBrowser, NAV_GOTO, home_url, FALSE);
            break;
        case GOTO_ELEMENT:
            break;
        case MENU_ELEMENT: {
            break;
        }
        default:
            break;
    }
}

static BOOL begin_mouse_capture(HWND hBrowser, int message, WPARAM wParam, LPARAM lParam)
{
    memset(&m_captureContext, 0, sizeof(CaptureContext));
    m_captureContext.t = GetTickCount();
    m_captureContext.press = 1;
    m_captureContext.wParam = wParam;
    m_captureContext.lParam = lParam;
    return TRUE;
}

static void PanView(HWND hWnd, LPARAM lParam)
{
    int currentX =0;
    int currentY =0;
    int offsetX = LOSWORD (m_captureContext.lParam) - LOSWORD (lParam);
    int offsetY = HISWORD (m_captureContext.lParam) - HISWORD (lParam);
    float sx, sy;
   
    mdolphin_get_scale_factor(hWnd, &sx, &sy);
    if (ABS(offsetX) > MAX_DRAG_STEP|| ABS(offsetY)> MAX_DRAG_STEP) {
        offsetX = (int)roundf((float)offsetX / sx);
        offsetY = (int)roundf((float)offsetY / sy);
        mdolphin_get_contents_position(hWnd, &currentX, &currentY);
        mdolphin_set_contents_position(hWnd, currentX + offsetX , currentY +  offsetY);
        m_captureContext.lParam = lParam;
    }
}

BOOL DispatchMouseMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int x_pos = LOSWORD (lParam);
	int y_pos = HISWORD (lParam);
    switch (message) {
		case MSG_MOUSEMOVE:
			if (!isAddressVisible() && g_panView) {
				if (capture_mouse_message(message, wParam, lParam))
					return TRUE;
            }
            if (hWnd == hMainWnd) //only handle main window message
                ModifyImageShowState(x_pos, y_pos);
            break;
        case MSG_LBUTTONDOWN:
            if (hWnd == hMainWnd) //only handle main window message
                HandleLButtonMessage(x_pos, y_pos);
        case MSG_LBUTTONUP:
			if (!isAddressVisible() && g_panView) {
				if (capture_mouse_message(message, wParam, lParam))
					return TRUE;
			}
            break;
        default:
            return FALSE;
    }
    return FALSE;
}

static BOOL end_mouse_capture(HWND hBrowser, int message, LPARAM lParam) 
{
    BOOL result = FALSE;
#if 1
    if (message == MSG_MOUSEMOVE && m_captureContext.press) {
        result = TRUE;
        m_captureContext.press = 2;
        PanView(hBrowser, lParam);
    }

    if (message == MSG_LBUTTONUP ) {
        if(m_captureContext.press == 1) 
            SendMessage(hBrowser, MSG_LBUTTONDOWN, m_captureContext.wParam, m_captureContext.lParam);
        else
            result = TRUE;

        memset(&m_captureContext, 0, sizeof(CaptureContext));
    }
#else
    if (m_captureContext.t) {
        if ((GetTickCount() - m_captureContext.t) > LONG_PRESS_TIME) {
            m_captureContext.press = 2;
            SendMessage(hBrowser, MSG_LBUTTONDOWN, m_captureContext.wParam, m_captureContext.lParam);
        }
        m_captureContext.t = 0;
    }

    if (m_captureContext.press == 1) {
        result = TRUE;
        if (message == MSG_MOUSEMOVE)
            PanView(hBrowser, lParam);
    }

    if (message == MSG_LBUTTONUP)
        memset(&m_captureContext, 0, sizeof(CaptureContext));
#endif
    return result;
}

BOOL capture_mouse_message(int message, WPARAM wParam, LPARAM lParam)
{
    int index = GetElementByPostion(LOSWORD (lParam), HISWORD (lParam));
    if (index != MDOLPHIN_ELEMENT)
        return FALSE;

    HWND hBrowser = GetActiveWindowInPageView();
    if (hBrowser == HWND_INVALID|| hBrowser == HWND_NULL)
        return FALSE;

    if (GetViewMode(hBrowser) != NORMAL_VIEW)
        return FALSE;

	RECT rc;
	GetClientRect(hBrowser, &rc);
	if (!PtInRect(&rc, LOSWORD(lParam), HISWORD(lParam)))
		return FALSE;

    switch (message) {
        case MSG_LBUTTONDOWN:
            return begin_mouse_capture(hBrowser, message, wParam, lParam);
        case MSG_MOUSEMOVE:
        case MSG_LBUTTONUP:
            return end_mouse_capture(hBrowser, message, lParam);
    }
    return FALSE;
}

static void my_message_callback (HWND parent, const char * text, const char * caption)
{
    MessageBox(parent, text, caption, MB_OK);
}

static void set_title_text(HWND hWnd, const char * text)
{
    if (!text || !strcmp(text, "about:blank") || !strlen(text))
        text = "无标题";

    PAGE_DATA* pd = GetPageData(hWnd);
    if (!pd)
        return;

    if (pd->title)
        free(pd->title);
    pd->title = strdup(text);
}

static void set_location_text(HWND hWnd, const char * text)
{
    if (!text || !strcmp(text, "about:blank") || !strlen(text))
        return;

    PAGE_DATA* pd = GetPageData(hWnd);
    if (!pd)
        return;

    if (pd->url)
        free(pd->url);
    pd->url= strdup(text);
}

static BOOL my_confirm_callback (HWND parent, const char * text, const char * caption)
{
    if (MessageBox (parent, text, caption, MB_OKCANCEL|MB_ICONINFORMATION) == IDOK)
        return TRUE;
    return FALSE;
}

static char * my_prompt_callback (HWND parent, const char* text, const char* defaultval, const char* caption)
{
    memset(prompt_text, 0 , sizeof(prompt_text));
    if (prompt_box(parent, text, defaultval, prompt_text, sizeof(prompt_text)))
        return prompt_text;
    return NULL;
}

static void err_msg(HWND hWnd, int err, const char* url)
{
    MessageBox(GetParent(hWnd), "Error!", url, MB_OK);
}

void draw_addr_btn_status(BOOL enable)
{
    skin_elements[ADDR_ELEMENT].bEnabled = enable;
    InvalidateRect(hMainWnd, skin_elements[ADDR_ELEMENT].prc, TRUE);
}

void draw_zoom_btn_status(BOOL enable)
{
    skin_elements[ZOOMIN_ELEMENT].bEnabled = enable;
    InvalidateRect(hMainWnd, skin_elements[ZOOMIN_ELEMENT].prc, TRUE);
    skin_elements[ZOOMOUT_ELEMENT].bEnabled = enable;
    InvalidateRect(hMainWnd, skin_elements[ZOOMOUT_ELEMENT].prc, TRUE);
}

void draw_histroy_status(size_t bcount, size_t fcount)
{
	if(bcount) {
		if (!skin_elements[BACK_ELEMENT].bEnabled) {
			skin_elements[BACK_ELEMENT].bEnabled = TRUE;
			InvalidateRect(hMainWnd, skin_elements[BACK_ELEMENT].prc, TRUE);
		}

	} else {
		if (skin_elements[BACK_ELEMENT].bEnabled) {
			skin_elements[BACK_ELEMENT].bEnabled = FALSE;
			InvalidateRect(hMainWnd, skin_elements[BACK_ELEMENT].prc, TRUE);
		}
	}

	if(fcount) {
		if (!skin_elements[FORWARD_ELEMENT].bEnabled) {
			skin_elements[FORWARD_ELEMENT].bEnabled = TRUE;
			InvalidateRect(hMainWnd, skin_elements[FORWARD_ELEMENT].prc, TRUE);
		}

	} else {
		if(skin_elements[FORWARD_ELEMENT].bEnabled) {
			skin_elements[FORWARD_ELEMENT].bEnabled =FALSE;
			InvalidateRect(hMainWnd, skin_elements[FORWARD_ELEMENT].prc, TRUE);
		}
	}
}

static void set_histroy_status(HWND hControl, size_t bcount, size_t fcount, size_t max)
{
    PAGE_DATA* pd = get_page_data(hControl);
    if (pd) {
        pd->back = bcount;
        pd->forward = fcount;

        HWND hActive = GetActiveWindowInPageView();
        if (hActive == hControl) {
            if (pd->mode == PAGE_VIEW)
                draw_histroy_status(0, 0);
            else
                draw_histroy_status(pd->back, pd->forward);
        }
    }
}

void draw_loading_status(BOOL load, BOOL refresh)
{
    if (skin_elements[STOP_ELEMENT].bEnabled != load) {
        skin_elements[STOP_ELEMENT].bEnabled = load ;
        InvalidateRect(hMainWnd, skin_elements[STOP_ELEMENT].prc, TRUE);
    }

    if (skin_elements[REFRESH_ELENENT].bEnabled != refresh) {
        skin_elements[REFRESH_ELENENT].bEnabled = refresh;
        InvalidateRect(hMainWnd, skin_elements[REFRESH_ELENENT].prc, TRUE);
    }
}

static void set_loading_status(HWND hControl, BOOL load, size_t progress)
{
    PAGE_DATA* pd = get_page_data(hControl);
    if (pd) {
        pd->load = load;

        HWND hActive = GetActiveWindowInPageView();
        if (hActive == hControl) {
            if (pd->mode == PAGE_VIEW)
                draw_loading_status(false, false);
            else
                draw_loading_status(pd->load, !pd->load);
        }
    }
}

static int InputBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_COMMAND) {
        if (wParam == IDOK) {
            GetWindowText(GetDlgItem (hDlg, IDC_INPUT), inputboxbuf, inputboxbuflen);
            EndDialog(hDlg, 1);
        }else if (wParam == IDCANCEL){
            EndDialog(hDlg, 0);
        } 
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static int prompt_box(HWND parent, const char *info, const char *definp, char *buf, int buflen)
{
	static DLGTEMPLATE DlgBoxInputLen = {
    	WS_BORDER | WS_CAPTION, 
	    WS_EX_NONE,
    	0, 41, 170, 100, 
		"Javascript input",
	    0, 0,
    	4, NULL,
	    0
	};

	static CTRLDATA CtrlInputLen [] = { 
    	{
        	CTRL_STATIC,
	        WS_VISIBLE | SS_SIMPLE,
    	    10, 5, 150, 20, 
        	IDC_STATIC, 
	        "InputBox",
    	    0
	    },
    	{
	        CTRL_EDIT,
    	    WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        	10, 23, 150, 20,
	        IDC_INPUT,
    	    NULL,
	        0
    	},
	    {
    	    CTRL_BUTTON,
        	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
	        10, 50, 70, 20,
    	    IDOK, 
        	"Ok",
	        0
    	},
	    {
    	    CTRL_BUTTON,
        	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
	        90, 50, 70, 20,
    	    IDCANCEL, 
        	"Cancel",
	        0
    	}
	};

	CtrlInputLen[0].caption = info;
	CtrlInputLen[1].caption = definp;
    DlgBoxInputLen.controls = CtrlInputLen;
	inputboxbuf = buf;
	inputboxbuflen = buflen;

    return DialogBoxIndirectParam (&DlgBoxInputLen, parent, InputBoxProc, 0);
}

void SetBrowserCallbacks(HWND hWnd)
{
    CB_INFO cb_info;
    memset(&cb_info, 0, sizeof(CB_INFO));
    
    cb_info.CB_MESSAGE_BOX = my_message_callback;
    cb_info.CB_SET_TITLE = set_title_text;
    cb_info.CB_SET_LOCATION = set_location_text;
    cb_info.CB_CONFIRM_BOX = my_confirm_callback;
    cb_info.CB_PROMPT_BOX = my_prompt_callback;
    cb_info.CB_OPEN_WINDOW = my_create_mdolphin_window;
    cb_info.CB_CLOSE_WINDOW = my_close_mdolphin_window;
    cb_info.CB_SET_LOADING_STATUS = set_loading_status;
    cb_info.CB_SET_HISTORY_STATUS = set_histroy_status;
    cb_info.CB_CAPTURE_IMAGE= my_capture_window_image;
    cb_info.CB_ERROR = err_msg;
    mdolphin_set_callback_info(hWnd, &cb_info);
}

