#include <string.h>
#include <ctype.h>

#include "mdolphin_app.h"
#include "mdolphin_skin.h"
#include "mdolphin_pageview.h"
#include "mphone_addressField.h"
#include "mphone_sk_support.h"

#if IS_PORTRAIT
#define IMG_LOCATION "./res/local_pt.png"
#define IMG_GOTO     "./res/goto_pt.png"
#define IMG_GOHOME   "./res/gohome_pt.png"
#else
#define IMG_LOCATION "./res/local.png"
#define IMG_GOTO     "./res/goto.png"
#define IMG_GOHOME   "./res/gohome.png"
#endif

static void HandleLButtonMessage(int x_pos, int y_pos, int type);

#define OFF_ADD   5
#define URL_MAXLEN 1024

enum BUTTON_STATE{
    BTN_DOWN = 1, 
    BTN_UP, 
};

static RECT rcLocation = {RL_X, RL_Y, RL_W+RL_X, RL_H+RL_Y};
static RECT rcURLText = {RL_X+4, RL_Y+(RL_H/4), RL_W+RL_X+4, (RL_H/2)+RL_Y+(RL_H/4)};

static RECT rcGoToBtn = {GO_X, GO_Y, GO_W+GO_X, GO_H+GO_Y};
static RECT rcHomeBtn = {HOME_X, HOME_Y, HOME_W+HOME_X, HOME_H+HOME_Y};

static RECT rcSK = {SK_OFFX, SK_OFFY, SK_WIDTH+SK_OFFX, SK_HEIGHT+SK_OFFY};

static BOOL isAddressShow = FALSE;

static BITMAP bmpLocation;
static BITMAP bmpGoToBtn;
static BITMAP bmpHomtBtn;

static HWND hAddrWnd = HWND_INVALID;

static int input_offset = 0;
static char inputdata[URL_MAXLEN+2]; //here i using the array..  

static int LBtnState = 0;
static PRECT prcFocusKey = NULL;
static HDC tmp_dc = 0;
#if 0
static HDC shift_alphadc = 0;
static HDC shift_dc = 0;
static int shift_state = 0;
//static RECT Shift_Rect = {SHIFT_X, SHIFT_Y, SHIFT_W, SHIFT_H}; 
#endif

RECT GetrcSK()
{
	return rcSK; 
}

const char* SK_GetLocation()
{
    PAGE_DATA* pd = GetActivePage();
    if (pd && pd->wasUserGesture)
        return pd->url;

    return NULL;
}

HWND GetAddrWnd()
{
    return hAddrWnd;
}

//minigui visible function have bug..
BOOL isAddressVisible()
{
	return isAddressShow;
}

void SetAddressVisible(BOOL flag)
{
	isAddressShow = flag;
}

char* GetInputData(BOOL isGetShowData)
{
	if (!isGetShowData)
		return inputdata;

	if (input_offset > InputShowLen)
		return inputdata + (input_offset-InputShowLen);

    return inputdata;
}

void CleanInputData()
{
	input_offset = 0;
	inputdata[input_offset] = '\0';
}

BOOL AddInputData(char* data, int len, int isUp)
{
	int i = 0;

    if (input_offset + len > URL_MAXLEN)
		return FALSE;

	for (i = 0; i< len; i++) {
		if (isUp)
			inputdata[input_offset] = toupper (data[i]);
		else
			inputdata[input_offset] = data[i];
		input_offset++;
	}

    inputdata[input_offset] = '\0';

    InvalidateRect(hAddrWnd, &rcLocation, FALSE);
	return TRUE;
}

BOOL DeleteInputData(int number)
{
	if (number > input_offset)
		return FALSE;

	input_offset -= number;
	inputdata[input_offset] = '\0';
	InvalidateRect(hAddrWnd, &rcLocation, FALSE);

	return TRUE;
}

HWND GetBrowserHwnd()
{
	return GetActiveWindowInPageView();
}

void SK_BrowserGoto(const char* url)
{
    PAGE_DATA* pd = GetActivePage();
	if (pd)
        pd->wasUserGesture = TRUE;

    mdolphin_navigate(GetBrowserHwnd(), NAV_GOTO, url, FALSE);
	SetAddressVisible(FALSE);
	ShowWindow(hAddrWnd, SW_HIDE);
	SetFocus(GetBrowserHwnd());
	CleanInputData();
}

static void PaintFocusKey(HDC hdc)
{
	static HDC mem_dc = 0;

    if (!prcFocusKey)
		return;

	if (LBtnState == BTN_DOWN) {
#if 0
        PainShift(hdc);
#endif        
		HDC sub_dc = CreateSubMemDC(hdc, prcFocusKey->left, prcFocusKey->top,
                RECTWP(prcFocusKey), RECTHP(prcFocusKey), TRUE);

		//svar the dc of the click area 
		if (mem_dc)
			DeleteMemDC(mem_dc);

        mem_dc = CreateCompatibleDC(sub_dc);
		BitBlt(hdc, prcFocusKey->left, prcFocusKey->top,
			   RECTWP(prcFocusKey), RECTHP(prcFocusKey), mem_dc, 0, 0, 0);
		if (!tmp_dc) {
			tmp_dc = CreateCompatibleDC(CreateSubMemDC(hdc, 0, 0, 91, 26, TRUE));
	
            //paint the transparence keyboard on click area
			SetMemDCAlpha(tmp_dc, MEMDC_FLAG_SRCALPHA, 40);
		}
		BitBlt(tmp_dc, 0, 0, RECTWP(prcFocusKey), RECTHP(prcFocusKey),
				hdc, prcFocusKey->left, prcFocusKey->top, 0);
        // DeleteMemDC(sub_dc); should not delete the sub_dc...
	} else if (LBtnState == BTN_UP) {
        if (mem_dc) {
			//recover the dc of the click area 
			BitBlt(mem_dc, 0, 0, RECTWP(prcFocusKey), RECTHP(prcFocusKey),
				    hdc, prcFocusKey->left, prcFocusKey->top, 0);
			DeleteMemDC(mem_dc);
			mem_dc = 0;
		}
		prcFocusKey = NULL;
	}
}

void HandldMoveMessage(int x_pos, int y_pos)
{
	LBtnState = 0;
}

static void HandleLButtonMessage(int x_pos, int y_pos, int type)
{
	static RECT rcKeyBox;

    LBtnState = type;
	if (type == BTN_DOWN) {
		if (PtInRect(&rcSK, x_pos, y_pos)) {
			PRECT sk_rect_down = soft_keyboard_mouse_down(x_pos, y_pos);
			if (sk_rect_down) {
				rcKeyBox.left = sk_rect_down->left+SK_OFFX - 5;
				rcKeyBox.top = sk_rect_down->top+SK_OFFY - 5;
				rcKeyBox.right = sk_rect_down->right+SK_OFFX + 6;
				rcKeyBox.bottom = sk_rect_down->bottom+SK_OFFY + 6;
				prcFocusKey = &rcKeyBox;
				InvalidateRect(hAddrWnd, prcFocusKey, FALSE);
			}
		}

		if (PtInRect(&rcGoToBtn, x_pos, y_pos))
			SK_BrowserGoto(GetInputData(FALSE));

		if (PtInRect(&rcHomeBtn, x_pos, y_pos))
			SK_BrowserGoto(SK_GetLocation());
    } else if (type == BTN_UP) {
		if (prcFocusKey)
			InvalidateRect(hAddrWnd, prcFocusKey, FALSE);
	}
}

static void PaintBackGround(HDC hdc, LOGFONT *logfont)
{
	FillBoxWithBitmap(hdc, rcLocation.left, rcLocation.top,
            RECTW(rcLocation), RECTH(rcLocation), &bmpLocation);
	FillBoxWithBitmap(hdc, rcGoToBtn.left, rcGoToBtn.top,
            RECTW(rcGoToBtn), RECTH(rcGoToBtn), &bmpGoToBtn);
	FillBoxWithBitmap(hdc, rcHomeBtn.left, rcHomeBtn.top,
            RECTW(rcHomeBtn), RECTH(rcHomeBtn), &bmpHomtBtn);

	SetBkMode(hdc, BM_TRANSPARENT);
	SelectFont(hdc, logfont);
	DrawText(hdc, GetInputData(TRUE), -1, &rcURLText, DT_LEFT | DT_WORDBREAK);
}

int MPhone_AddressCtrlProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static LOGFONT *logfont = NULL;

    switch (message) {
        case MSG_CREATE:
            if (LoadBitmap(HDC_SCREEN, &bmpLocation, IMG_LOCATION))
                return -1;
            if (LoadBitmap(HDC_SCREEN, &bmpGoToBtn, IMG_GOTO))
				return -1;
#if 0
            if (LoadBitmap(HDC_SCREEN, &bmpHomtBtn, IMG_GOHOME))
				return -1;
#endif
			IncludeWindowExStyle(hWnd, WS_EX_CLIPCHILDREN);
			hAddrWnd = hWnd;

			init_soft_keyboard();
			setoffset_sk(SK_OFFX, SK_OFFY);

			logfont = CreateLogFont(NULL, "song", "UTF-8", FONT_WEIGHT_BOLD,
                    FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 16, 0);
			break;
		case MSG_LBUTTONDOWN:
			HandleLButtonMessage(LOSWORD(lParam), HISWORD(lParam), BTN_DOWN);
			break;
		case MSG_LBUTTONUP:
			HandleLButtonMessage(LOSWORD(lParam), HISWORD(lParam), BTN_UP);
			break;
		case MSG_MOUSEMOVE:
			HandldMoveMessage(LOSWORD(lParam), HISWORD(lParam));
			break;
		case MSG_ERASEBKGND: {
            RECT rc;
		    BOOL    fGetDC = FALSE;
		    HDC     hdc = (HDC) wParam;
            if (!hdc) {
                hdc = GetClientDC(hWnd);
                fGetDC = TRUE;
            }

			GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
            ClipRectIntersect(hdc, &rc);

            HDC mem_dc = CreateCompatibleDC(hdc);
            SetMemDCAlpha(mem_dc, MEMDC_FLAG_SRCALPHA, 150);
            BitBlt(mem_dc, rc.left, rc.top, RECTW(rc), RECTH(rc), hdc, 0, 0, 0);
            DeleteMemDC(mem_dc);

            PaintBackGround(hdc, logfont);

            if (fGetDC)
                ReleaseDC(hdc);

            paint_sk_pic();
            return 0;
	    }	
		case MSG_PAINT: {
            HDC hdc = BeginPaint(hWnd);
            PaintBackGround(hdc, logfont);
            PaintFocusKey(hdc);
            EndPaint(hWnd, hdc);
            return 0;
	    }
		case MSG_DOESNEEDIME:
			return TRUE;
        case MSG_DESTROY:
            UnloadBitmap(&bmpLocation);
            UnloadBitmap(&bmpGoToBtn);
            //UnloadBitmap(&bmpHomtBtn);
            destroy_soft_keyboard();
            break;
        case MSG_CLOSE:
            DestroyLogFont(logfont);
            if(tmp_dc)
				DeleteMemDC(tmp_dc);
			break;
	}

	return DefaultControlProc (hWnd, message, wParam, lParam);
}

BOOL RegisterMPhone_AddressControl(void)
{
	WNDCLASS MPAddressClass;

	MPAddressClass.spClassName = CTRL_MPhone_Address;
	MPAddressClass.dwStyle = WS_NONE;
	MPAddressClass.dwExStyle = WS_EX_NONE;
	MPAddressClass.hCursor = GetSystemCursor(IDC_ARROW);
	MPAddressClass.iBkColor = COLOR_lightwhite;
	MPAddressClass.WinProc = MPhone_AddressCtrlProc;

	return RegisterWindowClass(&MPAddressClass);
}

void UnregisterMPhone_AddressControl(void)
{
	UnregisterWindowClass(CTRL_MPhone_Address);
}

#if 0
void PainShift(HDC hdc)
{
	if (shift_state == soft_keyboard_is_shift())
		return;

	shift_state = soft_keyboard_is_shift();
	if (!shift_dc) {
		shift_dc = CreateCompatibleDC(CreateSubMemDC(hdc, 
					Shift_Rect.left, Shift_Rect.top,
                    Shift_Rect.right, Shift_Rect.bottom, TRUE));
        BitBlt(hdc, Shift_Rect.left, Shift_Rect.top, 
			    RECTW(Shift_Rect), RECTH(Shift_Rect), shift_dc,  0, 0, 0);
	}

	if (!shift_alphadc) {
		shift_alphadc = CreateCompatibleDC(CreateSubMemDC(hdc, 
					Shift_Rect.left, Shift_Rect.top, Shift_Rect.right, Shift_Rect.bottom,
					TRUE));

        //paint the transparence keyboard on click area
		SetMemDCAlpha(shift_alphadc, MEMDC_FLAG_SRCALPHA, 40);
	}

	if (shift_state)
		BitBlt(shift_alphadc, 0, 0, RECTW(Shift_Rect), RECTH(Shift_Rect),
				hdc, Shift_Rect.left, Shift_Rect.top, 0);
	else 
		BitBlt(shift_dc, 0, 0, RECTW(Shift_Rect), RECTH(Shift_Rect),
				hdc, Shift_Rect.left, Shift_Rect.top, 0);
}
#endif

