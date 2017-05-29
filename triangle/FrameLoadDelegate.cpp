#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mdolphin/mdolphin.h>
#include <mdolphin/IMDWebView.h>
#include <mdolphin/IMDWebFrameLoadDelegate.h>
#include "FrameLoadDelegate.h"
#include "TriangleLogging.h"

#include <stdio.h>
#include <string>

using std::string;


FrameLoadDelegate::FrameLoadDelegate(TriangleLogging *log)
    : m_log(log)
{

}

void FrameLoadDelegate::didFinishLoadForFrame(IMDWebView* webView, IMDWebFrame* webFrame)
{
}

void FrameLoadDelegate::didFailLoad(IMDWebView* webView, IMDWebFrame* webFrame,
        int code, const char* url)
{
    m_log->error("%s    %s   %s   %d\n", url, "didFailLoad", "failed", code);
}

void FrameLoadDelegate::didStartProvisionalLoadForFrame(IMDWebView* webView, 
        IMDWebFrame* webFrame)
{
}

void FrameLoadDelegate::didCommitLoadForFrame(IMDWebView* webView, 
        IMDWebFrame* webFrame)
{
}

void FrameLoadDelegate::didReceiveTitle(IMDWebView* webView,
    IMDWebFrame* webFrame, const char* title)
{
}

void FrameLoadDelegate::didFinishDocumentLoad(IMDWebView* webView, 
        IMDWebFrame* webFrame)
{
}

void FrameLoadDelegate::didHandleOnloadEventsForFrame(IMDWebView* webView, 
        IMDWebFrame* webFrame)
{
}

BOOL FrameLoadDelegate::didClearWindowObject(IMDWebView* webView,
        IMDWebFrame* webFrame, JSContextRef context,
        JSObjectRef windowObject)
{
    return TRUE;
}

void  FrameLoadDelegate::didReceiveResponse(IMDWebView*, IMDWebFrame*, 
          unsigned long identifier, MDHResponse response) 
{
/*
    char* url = mdRespGetURL(response);
    m_log->debug("%s    %s   %s\n", url, "response", "Succed");
    //remember!
    free(url);
*/
} 

void  FrameLoadDelegate::identifierForInitialRequest(IMDWebView*, IMDWebFrame*, 
        MDHRequest, unsigned long identifier) 
{
}

void  FrameLoadDelegate::didFailProvisionalLoad(IMDWebView*, IMDWebFrame*, 
        int code, const char* url) 
{
    m_log->error("%s    %s   %s   %d\n", url, "didFailProvisionalLoad", "failed", code);
}

void  FrameLoadDelegate::didProgressStarted(IMDWebView*, IMDWebFrame*) 
{
}

void  FrameLoadDelegate::didProgressEstimateChanged(IMDWebView*, IMDWebFrame*, 
        int percent) 
{
} 

void  FrameLoadDelegate::didProgressFinished(IMDWebView* view, IMDWebFrame* frame) 
{
    char *url = frame->url();
    if (url) {
        m_log->debug("%s   %s  %s\n", url,  "   ", "load: completed");
        free(url);
    }
}

void  FrameLoadDelegate::didMainFrameDocumentReady(IMDWebView*) 
{
}

void  FrameLoadDelegate::didChangeLocationWithinPageForFrame(IMDWebView*, IMDWebFrame*) 
{
}

void  FrameLoadDelegate::didPerformFirstNavigation(IMDWebView*, IMDWebFrame*) 
{

}

