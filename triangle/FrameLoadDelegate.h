
#ifndef FrameLoadDelegate_h
#define FrameLoadDelegate_h

#include <mdolphin/IMDWebFrameLoadDelegate.h>

class TriangleLogging;
class FrameLoadDelegate : public IMDWebFrameLoadDelegate
{
public:
    FrameLoadDelegate(TriangleLogging *log);
    void didReceiveTitle(IMDWebView*, IMDWebFrame*, const char* title);
    BOOL didClearWindowObject(IMDWebView*,IMDWebFrame*, JSContextRef cotext, JSObjectRef windowObject, IMDWebFrame*);
    void didFinishDocumentLoad(IMDWebView*, IMDWebFrame*);
    void didStartProvisionalLoadForFrame(IMDWebView*, IMDWebFrame*);
    void didCommitLoadForFrame(IMDWebView*, IMDWebFrame*);
    void didHandleOnloadEventsForFrame(IMDWebView*, IMDWebFrame*);
    void didFinishLoadForFrame(IMDWebView*, IMDWebFrame*);
    void didFailLoad(IMDWebView*, IMDWebFrame*, int code, const char* url);

    BOOL didClearWindowObject(IMDWebView* webView, IMDWebFrame* webFrame, JSContextRef context, JSObjectRef windowObject);
    void didReceiveResponse(IMDWebView*, IMDWebFrame*, unsigned long identifier, MDHResponse response);
    void identifierForInitialRequest(IMDWebView*, IMDWebFrame*, MDHRequest, unsigned long identifier);
    void didFailProvisionalLoad(IMDWebView*, IMDWebFrame*, int code, const char* url);
    void didProgressStarted(IMDWebView*, IMDWebFrame*);
    void didProgressEstimateChanged(IMDWebView*, IMDWebFrame*, int percent);
    void didProgressFinished(IMDWebView*, IMDWebFrame*);
    void didMainFrameDocumentReady(IMDWebView*);
    void didChangeLocationWithinPageForFrame(IMDWebView*, IMDWebFrame*);
    void didPerformFirstNavigation(IMDWebView*, IMDWebFrame*);

private:
    TriangleLogging* m_log;
};
#endif
