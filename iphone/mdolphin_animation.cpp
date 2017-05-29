
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mdolphin_app.h"
#include "mdolphin_animation.h"
#include "mdolphin_transform.h"
#include "mdolphin_pageview.h"
#include "mdolphin_skin.h"
#if USE_FTIMER
#include "ftimer/ft_timer.h"
#endif

#define MAN_SETANIMATION (MSG_USER+1)
#define MAN_GETANIMATION (MAN_SETANIMATION+1)
#define ID_MYANIMATION    0x200

static BOOL isLBDown = FALSE;
static int ld_down_x = -1;
static int ld_down_y = -1;
#if USE_FTIMER
static pthread_t g_timerTask;
#endif

extern HWND hAnimationWnd;
void SetAnimationCtxt(HWND hwnd, void* pCtxt)
{
    SendMessage(hwnd,  MAN_SETANIMATION, 0, (WPARAM)pCtxt);
}

HWND CreateAnimationWindow(HWND hParent)
{
    RECT rc;
    GetElementGeometry(MDOLPHIN_ELEMENT, &rc);
#if USE_FTIMER
    ft_start_timer_task(&g_timerTask, 0);
#endif
    return CreateWindowEx(MYANIMATION_CTRL, "animation", WS_CHILD,
            WS_EX_NONE, ID_MYANIMATION, 
            rc.left, rc.top, RECTW(rc), RECTH(rc), hParent, 0);
}

static int MyAnimationProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	TransContext* pCtxt = (TransContext*)GetWindowAdditionalData2(hWnd);
    switch (message) {
        case MSG_CREATE:
			break;
        case MSG_TIMER:
            ProcessTransform(pCtxt);
            break;
#if 0
        case MSG_PAINT: {
            HDC hdc = BeginPaint(hWnd);			

            if (pCtxt) {
                UpdateTransActionRect(pCtxt, hdc); 
                if (!pCtxt->current) {
                    if (pCtxt->bAutoStop) {
                        EndTransform(pCtxt);
                        DestroyTransContext(pCtxt);
                    }
                }
            }

            EndPaint(hWnd, hdc);
			return 0;
		}
#endif
        case MSG_LBUTTONDBLCLK: {
            RECT rc;
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
            HWND hBrowser = GetActiveWindowInPageView();
			if (GetViewMode(hBrowser) != PAGE_VIEW)
				break;

            ClientToScreen(hWnd, &x, &y);

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

		case MSG_MOUSEMOVE: {
		   if (!isLBDown)
			   break;
		   HWND hwnd = GetActiveWindowInPageView(); 
		   HWND hBrowser = GetActiveWindowInPageView();
		   if (GetViewMode(hBrowser) != PAGE_VIEW)
			   break;

		   int x = LOSWORD (lParam);
		   int y = HISWORD (lParam);

		   ClientToScreen(hWnd, &x, &y);

           if (abs(x - ld_down_x) > 5) {
               if ((x - ld_down_x) > 5)
                   ScrollPage(hwnd, MOVETO_PREV);
               else 
                   ScrollPage(hwnd, MOVETO_NEXT);

               isLBDown = FALSE;
               ld_down_x = -1;
               break;
           }
		   break;
		}
        case MSG_LBUTTONUP: {
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);
		
            ClientToScreen(hWnd, &x, &y);

            isLBDown = FALSE;

			if (ld_down_x == x && ld_down_y == y) {
				HWND hBrowser = GetActiveWindowInPageView();
				if (GetViewMode(hBrowser) != PAGE_VIEW)
					break;

				SwitchViewMode(hAnimationWnd, GetViewMode(hBrowser) , NORMAL_VIEW);
				SetViewMode(hBrowser, NORMAL_VIEW);
			}
			break;
		}
        case MSG_LBUTTONDOWN: {
            RECT rc;
            int x = LOSWORD (lParam);
            int y = HISWORD (lParam);

			ClientToScreen(hWnd, &x, &y);

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
                isLBDown = TRUE;
                ld_down_x = x;
                ld_down_y = y;
			}
			break;
		}

        case MSG_CLOSE:
#if USE_FTIMER
            ft_finish_timer_task(&g_timerTask);
#endif
            PostQuitMessage (hWnd);
            return 0;
		case MSG_ERASEBKGND: 
			return 0;
		case MAN_SETANIMATION: 
			SetWindowAdditionalData2(hWnd, lParam);	
			break;
		case MAN_GETANIMATION:
			return GetWindowAdditionalData2(hWnd);
	}

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

void UnregisterMyAnimation()
{
	UnregisterWindowClass(MYANIMATION_CTRL);
}

BOOL RegisterMyAnimation()
{
    WNDCLASS gbClass;
    gbClass.spClassName = MYANIMATION_CTRL;
    gbClass.dwStyle = WS_NONE;
    gbClass.dwExStyle = WS_EX_NONE;
    gbClass.hCursor = GetSystemCursor (IDC_ARROW);
    gbClass.iBkColor = COLOR_blue;
    gbClass.WinProc = MyAnimationProc;
    return RegisterWindowClass (&gbClass);
}
