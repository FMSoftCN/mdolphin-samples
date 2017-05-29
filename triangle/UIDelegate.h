#ifndef UIDelegate_h
#define UIDelegate_h

#include <mdolphin/IMDWebUIDelegate.h>

class TriangleLogging;

class UIDelegate : public IMDWebUIDelegate {
public:
    virtual HWND createWebViewWithRequest(IMDWebView*, MDFrameLoaderInfo*, MDWindowFeatureInfo*);

    virtual void webViewShow(IMDWebView* ) {} ;
    virtual void setFrame(IMDWebView*, const RECT*) {} ;
    virtual BOOL webViewFrame(IMDWebView*, RECT *);
    virtual void webViewFocus(IMDWebView*, BOOL bFocus) {} ;
    virtual BOOL canTakeFocus(IMDWebView*, BOOL bForward);
    virtual BOOL takeFocus(IMDWebView*, BOOL bForward);
    virtual BOOL canRunModal(IMDWebView*);
    virtual BOOL runModal(IMDWebView*);
    virtual void setBarVisible(IMDWebView*, MDEBarType barType, BOOL bVisible) {} ;
    virtual BOOL isBarVisible(IMDWebView*, MDEBarType barType);
    virtual void setResizeable(IMDWebView*, bool resizeable) {} ;

    virtual void webViewAddMessageToConsole(IMDWebView*, const char* message, int line, const char* url, int level, int source_type);
    virtual void runJavaScriptAlertPanelWithMessage(IMDWebView*, const char* strMessage);
    virtual BOOL runJavaScriptConfirmPanelWithMessage(IMDWebView*, const char* string);
    virtual char* runJavaScriptTextInputPanelWithPrompt(IMDWebView*, const char* strMessage, const char* strDefValue);
    virtual void setStatusText(IMDWebView*, const char* strStatusText);
    virtual void webViewClose(IMDWebView*);

    virtual BOOL shouldInterruptJavaScript(IMDWebView*);
    virtual BOOL chooseFiles(IMDWebView*, MDChooseFilesInfo*) ;
    virtual void printFrame(IMDWebView*, IMDWebFrame*);
    virtual void mouseDidMoveOverElement(IMDWebView*, const char*);
    virtual BOOL hasCustomerIME();
    virtual void setCustomerIME(IMDWebView*, BOOL active, const MDEditorElement* editorInfo);
    virtual void didChangeCaret(IMDWebView*, const RECT* rect);
    virtual BOOL hasCustomMenuImplementation();
    virtual void trackCustomPopupMenu(IMDWebView*, const MDPopupMenu*);
    virtual BOOL receivedAuthentication(IMDWebView*, const char* title, char* user, char* pass) {}

public:
    UIDelegate(TriangleLogging *log);
    ~UIDelegate(){}

private:
    TriangleLogging* m_log;

};
#endif
