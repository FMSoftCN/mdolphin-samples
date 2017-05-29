#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "mdolphin_app.h"

#ifdef MINIGUI_V3
#include <mgutils/mgutils.h>
#else
#include <minigui/mgext.h>
#include <minigui/newfiledlg.h>
#endif

#include <mdolphin/mdolphin.h>
#include "mdolphin_urldatabase.h"

extern HWND mdolphin_hwnd;
extern HWND hMainWnd;
extern char *currentURL;

void loadNewUrl()
{
	//i don't know the ResetTimer how to work, i just run kill and start the timer 
	KillTimer (hMainWnd, TIMER_ID);

	if(getURL(currentURL) ) {
		printf("get the url is %s \n", currentURL);
		mdolphin_navigate(mdolphin_hwnd, NAV_GOTO, currentURL, FALSE);
		SetTimer (hMainWnd, TIMER_ID, TIMERS);
	} else {
		saveLogmsg("call the getURL function , get invalid URL!!" );
		fprintf(stderr, "!!!!!!error of get the url...!!!! \n");
	}

}

static void my_message_callback (HWND parent, const char * text, const char * caption)
{
	fprintf(stderr,"Messageb Box text %s, caption %s\n", text, caption);
}

static HWND my_create_new_window(const char* url, DWORD styles, int x, int y, int width, int height)
{
	fprintf(stderr, "******will open a new url [%s]!!!!!!!!!\n", url);
	return HWND_INVALID;
}

static void my_error_code_callback(HWND hWnd, int errCode, const char* url)
{
	char*log;
	asprintf(&log, "open the url [%s] error!", url);
	saveLogmsg(log);
	if(log)
		free(log);

	fprintf(stderr,"******get the url [%s] error!!!!!!\n", url);
	loadNewUrl();
}

#if ENABLE_SSL
static int my_verify_server_cert(CERT_RESULT result, CERT_DATA *x509)
{
    return result;
}
#endif

static BOOL  get_all_url(const char* url)
{
	saveURL(url);
	return FALSE;
}

static void set_loading_status (HWND hWnd, BOOL load, unsigned int progress)
{ 
	if(load)
		return;

	loadNewUrl();
}

void set_callback_info(HWND hWnd)
{
    CB_INFO cb_info;
    memset (&cb_info, 0, sizeof(CB_INFO));
    
	cb_info.CB_URL_IS_VISITED = get_all_url;
	cb_info.CB_ERROR = my_error_code_callback;
	cb_info.CB_SET_LOADING_STATUS = set_loading_status;
	//cb_info.CB_OPEN_WINDOW = my_create_new_window;
	//cb_info.CB_CLOSE_WINDOW = my_close_tab_window;

#if ENABLE_SSL
#if ENABLE_SSLFILE
    cb_info.CB_PROVIDE_CLIENT_CERT = NULL;
#else
    cb_info.CB_PROVIDE_CLIENT_CERT = NULL;
#endif
    cb_info.CB_VERIFY_SERVER_CERT = my_verify_server_cert;
#endif

    if (mdolphin_hwnd)
        mdolphin_set_callback_info(mdolphin_hwnd, &cb_info);
}

