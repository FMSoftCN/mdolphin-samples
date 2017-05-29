

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mdolphin/mdolphin.h>

#include "UIDelegate.h"

#include <stdio.h>
#include <string>

#include "TriangleLogging.h"

UIDelegate::UIDelegate(TriangleLogging *log) 
{
    m_log = log;
}

void UIDelegate::webViewAddMessageToConsole(IMDWebView* webView,  const char* message, int line, const char* url, int level, int source_type)
{
    //do nothing
}

void UIDelegate::runJavaScriptAlertPanelWithMessage(IMDWebView* webView, const char* strMessage)
{
}


BOOL UIDelegate::runJavaScriptConfirmPanelWithMessage(IMDWebView* webView, const char* strMessage)
{
    return TRUE;
}

char* UIDelegate::runJavaScriptTextInputPanelWithPrompt(IMDWebView* webView, const char* strMessage, const char* strDefValue)
{
    return NULL; 
}

void UIDelegate::setStatusText(IMDWebView* webView, const char* strStatusText)
{
}

HWND UIDelegate::createWebViewWithRequest(IMDWebView* , MDFrameLoaderInfo*, MDWindowFeatureInfo*)
{
    return HWND_INVALID;
}

void UIDelegate::webViewClose(IMDWebView* webView)
{
    if (webView) {
        HWND hostWindow = GetParent(webView->viewWindow());
        DestroyMainWindow(hostWindow);
    }
}

//===================================================
BOOL UIDelegate::webViewFrame(IMDWebView* webView, RECT *rc)
{
    HWND hwnd = webView->viewWindow();
    if (!rc || hwnd == HWND_INVALID)
        return FALSE;

    GetClientRect(hwnd, rc);
    ClientToScreen(hwnd, &rc->left, &rc->top);
    ClientToScreen(hwnd, &rc->right, &rc->bottom);

    return TRUE;

}

BOOL UIDelegate::canTakeFocus(IMDWebView * webView, BOOL bForward)
{
    //do nothing
    return FALSE;
}

BOOL UIDelegate::takeFocus(IMDWebView* webView, BOOL bForward)
{
    //do nothing
    return FALSE;
}

BOOL UIDelegate::canRunModal(IMDWebView* webView)
{
    return FALSE;
}

BOOL UIDelegate::runModal(IMDWebView* wevView)
{
    return FALSE;
}

BOOL UIDelegate::isBarVisible(IMDWebView* webView, MDEBarType bar)
{
    //do nothing
    return FALSE;
}

BOOL UIDelegate::shouldInterruptJavaScript(IMDWebView* webView)
{
    return FALSE;
}

BOOL UIDelegate::chooseFiles(IMDWebView* webView, MDChooseFilesInfo* chooseInfo)
{
    return FALSE;
}

void UIDelegate::printFrame(IMDWebView *webView, IMDWebFrame *webFrame)
{
    //do nothing
}

void UIDelegate::mouseDidMoveOverElement(IMDWebView* view, const char* msg)
{
}

BOOL UIDelegate::hasCustomerIME()
{
    return FALSE;
}

void UIDelegate::setCustomerIME(IMDWebView* view, BOOL active, const MDEditorElement* editorInfo)
{
}

void UIDelegate::didChangeCaret(IMDWebView*, const RECT* rect)
{
}

BOOL UIDelegate::hasCustomMenuImplementation()
{
    return FALSE;
}

void UIDelegate::trackCustomPopupMenu(IMDWebView* view, const MDPopupMenu* data)
{
}

