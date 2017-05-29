
#include <mdolphin/mdconfig.h>
#if ENABLE_PLUGIN	

#include <stdio.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mdolphin/mdolphin_plugin.h>

#define IDC_SET_PLUGIN_LIST 658
#define IDC_PLUGIN_LIST_DESC 659
#define PLUGINS_PATH    "/.mDolphin/plugins/"

typedef struct PREG_LIST
{
	char *mimeType;
	char *mimeDes;
    PLUGIN_REGISTER *regPgn;
	PREG_LIST * next;
};

static PREG_LIST* prBegin = NULL;
static PREG_LIST* prEnd = NULL;

static PREG_LIST * get_from_reglist(const char* mimeType)
{
    for(PREG_LIST* node = prBegin; node != NULL; node = node->next) {
        if (strcmp(node->mimeType, mimeType) == 0) {
            return node;
        }
    }
    return NULL;
}

static char * get_mimeType_from_mimeDes(const char* mimeDes)
{
    char *mime = NULL;
    const char *p = strchr(mimeDes, ':');
    if (p) {
        int len = p-mimeDes;
        if (len>0) {
            mime = (char*)malloc(len +1);
            if (mime) {
                memcpy(mime, mimeDes, len);
                mime[len] = '\0';
            }
        }
    }
    return mime;
}

static char * get_detail_from_mimeDes(const char* mimeDes)
{
    char *detail = NULL;
    const char *p = strrchr(mimeDes, ':');
    if (p) {
        int len = strlen(mimeDes)-(p-mimeDes+1);
        if (len>0) {
            detail = (char*)malloc(len +1);
            if (detail) {
                memcpy(detail, p+1, len);
                detail[len] = '\0';
            }
        }
    }
    return detail;
}

static char * get_suffixes_from_mimeDes(const char* mimeDes)
{
    char *suf = NULL;
    const char *p = strchr(mimeDes, ':');
    if (p) {
        int begin = p - mimeDes + 1;
        p = strchr(p+1, ':');
        if (p) {
            int end = p - mimeDes;
            int len = end-begin;
            if (len>0) {
                suf = (char*)malloc(len +1);
                if (suf) {
                    memcpy(suf, mimeDes+begin, len);
                    suf[len] = '\0';
                }
            }
        }
    }
    return suf;
}

static char * get_name_from_fullpath(const char* fullpath)
{
    char *name = NULL;
    const char *p = strrchr(fullpath, '/');
    if (p) {
        int len = strlen(fullpath)-(p-fullpath+1);
        if (len>0) {
            name = (char*)malloc(len +1);
            if (name) {
                memcpy(name, p+1, len);
                name[len] = '\0';
            }
        }
    }
    return name;
}

static void add_to_reglist(const char *mimeType, const char *mimeDes, PLUGIN_REGISTER *regPgn)
{
    PREG_LIST *node = get_from_reglist(mimeType);
    if (!node) {
        PREG_LIST* node = (PREG_LIST*)malloc(sizeof(PREG_LIST));
        if (node) {
            memset(node, 0, sizeof(PREG_LIST));
            node->mimeType = strdup(mimeType);
            node->mimeDes = strdup(mimeDes);
            node->regPgn = regPgn;
            node->next = NULL;
            if (prBegin == NULL)
                prBegin = prEnd = node;
            else {
                prEnd->next = node;
                prEnd = node;
            }
        }
    }
}

static void delete_from_reglist(PREG_LIST* node)
{
    if(node->mimeType)
        free(node->mimeType);
    if(node->mimeDes)
        free(node->mimeDes);
    if(node->regPgn)
        free(node->regPgn);
    free(node);
}

static DLGTEMPLATE PluginsWinData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , 300, 350,
     "Plug-Ins",
     0, 0,
     5,
     NULL, 0
};

static CTRLDATA PluginsInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_LEFT,
        10, 20, 280, 60,
        0,
        "Plug-Ins are currently available for the following file types:",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE | LBS_NOTIFY | LBS_AUTOCHECKBOX,
        10, 70, 280, 110,
        IDC_SET_PLUGIN_LIST,
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_LEFT,
        10, 180, 280, 100,
        IDC_PLUGIN_LIST_DESC,
        "Disabling Plug-Ins for a file type will cause files of that type to be download instead of being viewed in the Plug-In.",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        50, 280, 92, 25,
        IDOK, 
        "Ok",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        170, 280, 92, 25,
        IDCANCEL, 
        "Cancel",
        0
    }
};

static char* get_mime_info_from_plugin_so(void *handle)
{
    char *error;
    char *mimeInfo = NULL;

    if (handle) {
        dlerror();    /* Clear any existing error */
        NP_GetMIMEDescriptionProcPtr getMimeInfoFunction = 0;
        getMimeInfoFunction = (NP_GetMIMEDescriptionProcPtr)dlsym(handle, "NP_GetMIMEDescription");
        if (getMimeInfoFunction) {
            mimeInfo = strdup((*getMimeInfoFunction)());
            if ((error = dlerror()) != NULL) {
                if (mimeInfo)
                    free(mimeInfo);
                mimeInfo = NULL;
            }
        }
    }
    return mimeInfo;
}

static BOOL register_plugin(const char *mimeDes, const char *file, void *handle)
{
    BOOL result = FALSE;
    if (handle) {
        NP_InitializeProcPtr init = 0;
        NP_ShutdownProcPtr shutdown = 0;
        NP_GetMIMEDescriptionProcPtr desc = 0;
        NP_GetValueProcPtr getval = 0;

        init = (NP_InitializeProcPtr)dlsym(handle, "NP_Initialize");
        shutdown = (NP_ShutdownProcPtr)dlsym(handle, "NP_Shutdown");
        desc = (NP_GetMIMEDescriptionProcPtr)dlsym(handle, "NP_GetMIMEDescription");
        getval = (NP_GetValueProcPtr)dlsym(handle, "NP_GetValue");

        if (init && shutdown && mimeDes) {
            char *mimeType = get_mimeType_from_mimeDes(mimeDes);
            if (mimeType) {
                PLUGIN_REGISTER *RegPgn;
                RegPgn = (PLUGIN_REGISTER *)malloc(sizeof(PLUGIN_REGISTER));
                if (RegPgn) {
                    memset(RegPgn, 0, sizeof(PLUGIN_REGISTER)); 
                    RegPgn->init = init; 
                    RegPgn->shutdown = shutdown; 
                    RegPgn->desc = desc; 
                    RegPgn->getval = getval; 

                    char *name = get_name_from_fullpath(file);
                    if (name) {
                        strncpy(RegPgn->name, name, MAX_LEN_PLUGINNAME-1);
                        RegPgn->name[MAX_LEN_PLUGINNAME-1]='\0';
                        free(name);
                    }

                    strncpy(RegPgn->mimetype, mimeType, MAX_LEN_MIMETYPE-1);
                    RegPgn->mimetype[MAX_LEN_MIMETYPE-1]='\0';

                    char *suffixes = get_suffixes_from_mimeDes(mimeDes);
                    if (suffixes) {
                        strncpy(RegPgn->suffixes, suffixes, MAX_LEN_SUFFIXES-1);
                        RegPgn->suffixes[MAX_LEN_SUFFIXES-1]='\0';
                        free(suffixes);
                    }

                    if (mdolphin_register_plugin(RegPgn)) {
                        add_to_reglist(mimeType, mimeDes, RegPgn);
                        result = TRUE;
                    }else
                        free(RegPgn);
                }
                free(mimeType);
            }
        }
    }
    return result;
}

static void unregister_plugin(const char* mimeType)
{
    HPGN plugin = mdolphin_get_plugin_from_mimetype(mimeType);
    if (plugin)
        mdolphin_unregister_plugin(plugin);
}

static void update_plugins_register_status(HWND hWnd)
{
    BOOL reg, result;
    for (int i = 0; i < SendMessage (hWnd, LB_GETCOUNT, 0, 0L); i++) {
        if (SendMessage (hWnd, LB_GETCHECKMARK, i, 0) == CMFLAG_CHECKED)
            reg = TRUE;
        else
            reg = FALSE;
        int len = SendMessage (hWnd, LB_GETTEXTLEN, i, 0);
        if (len >0) {
            char *buffer = (char *)malloc(len+1);
            if (buffer) {
                SendMessage (hWnd, LB_GETTEXT, i, (LPARAM)buffer);
                buffer[len]='\0';
                if (mdolphin_get_plugin_from_mimetype(buffer))
                    result = TRUE;
                else
                    result = FALSE;
                if (reg != result) {
                    PREG_LIST *node = get_from_reglist(buffer);
                    if (node) {
                        if (reg)
                            mdolphin_register_plugin(node->regPgn);
                        else
                            unregister_plugin(node->mimeType);
                    }
                }
                free(buffer);
            }
        }
    }
}

static void init_plugins_manage_list(HWND hwnd)
{
    LISTBOXITEMINFO lbii;
    for(PREG_LIST* node = prBegin; node != NULL; node = node->next) {
        memset(&lbii, 0, sizeof(LISTBOXITEMINFO));
        lbii.hIcon = 0;
        if (mdolphin_get_plugin_from_mimetype(node->mimeType)) 
            lbii.cmFlag = CMFLAG_CHECKED;
        else
            lbii.cmFlag = CMFLAG_BLANK;
        lbii.string = node->mimeType;
        SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)&lbii);
    }
}

static void plugins_notif_proc(HWND hWnd, int id, int nc, DWORD add_data)
{
    if (nc == LBN_CLICKED) {
        int index = SendMessage(hWnd, LB_GETCURSEL, 0, 0);
        if (index != LB_ERR) {
            int len = SendMessage(hWnd, LB_GETTEXTLEN, index, 0);
            if (len>0) {
                char *text = (char *)malloc(len+1);
                if (text) {
                    memset(text, 0, len+1);
                    SendMessage(hWnd, LB_GETTEXT, index, (LPARAM)text);
                    text[len]='\0';
                    PREG_LIST *node = get_from_reglist(text);
                    if (node && node->mimeDes) {
                        char *detail = get_detail_from_mimeDes(node->mimeDes);
                        if (detail) {
                            SetWindowText(GetDlgItem(GetParent(hWnd), IDC_PLUGIN_LIST_DESC), detail);
                            free(detail);
                        }else
                            SetWindowText(GetDlgItem(GetParent(hWnd), IDC_PLUGIN_LIST_DESC), "");
                    }
                    free(text);
                }
            }
        }
    }
}

static int PluginsProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    switch (message) {
        case MSG_INITDIALOG: 
            {
                GetWindowRect ((HWND)(lParam), &rc);
                int x = rc.left + (rc.right - rc.left)/2-150;
                int y = rc.top + (rc.bottom - rc.top)/2-150;
                MoveWindow (hDlg, x, y, 300, 350 , TRUE);
                init_plugins_manage_list(GetDlgItem(hDlg, IDC_SET_PLUGIN_LIST));
                SetNotificationCallback(GetDlgItem (hDlg, IDC_SET_PLUGIN_LIST), plugins_notif_proc);
            }
            return 1;
        case MSG_COMMAND:
            switch (wParam) {
                case IDOK:
                    update_plugins_register_status(GetDlgItem(hDlg, IDC_SET_PLUGIN_LIST));
                case IDCANCEL:
                    EndDialog (hDlg, wParam);
                    break;
            }
            break;
        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

void create_plugins_manage_window(HWND hParent)
{
    PluginsWinData.controls = PluginsInitProgress;
    DialogBoxIndirectParam (&PluginsWinData, hParent, PluginsProc, (DWORD)hParent);
}

BOOL register_plugins_from_so(const char *mimeInfo, const char *file, void *handle)
{
    char *dl = NULL;
    char *buf = strdup(mimeInfo);
    BOOL success = FALSE;
    if (buf) {
        char *p = strtok_r(buf, ";", &dl);
        if (p) {
            if (register_plugin(p, file, handle))
                success = TRUE;
        }
        while((p = strtok_r(NULL, ";", &dl))) {
            if (p) {
                if (register_plugin(p, file, handle))
                    success = TRUE;
            }
        }
        free(buf);
    }
    return success;
}

void register_all_plugins()
{
    struct dirent* dir_ent;
    DIR* dir;
    char *path = NULL;
    char *mimeInfo = NULL;
    char *file = NULL;
    void *handle;
    char *current_dir = get_current_dir_name();
    if (current_dir) {
        asprintf(&path ,"%s%s", current_dir, PLUGINS_PATH);
        free(current_dir);
    }
    if (path) {
        if ((dir = opendir (path)) != NULL) {
            while ((dir_ent = readdir(dir)) != NULL ) {
                asprintf(&file ,"%s%s", path, dir_ent->d_name);
                if (file) {
                    printf ("plugin file: %s\n", file);
                    handle = dlopen(file, RTLD_LAZY);
                    if (handle) {
                        BOOL result = FALSE;
                        mimeInfo = get_mime_info_from_plugin_so(handle);
                        if (mimeInfo) {
                            result = register_plugins_from_so(mimeInfo, file, handle);
                            free(mimeInfo);
                        }
                        if (!result)
                            dlclose(handle);
                    }
                    free(file);
                    file = NULL;
                }
            }
            closedir(dir);
        }
        free(path);
    }
}

void unregister_all_plugins()
{
    for(PREG_LIST* node = prBegin; node != NULL; node = node->next) {
        HPGN plugin = mdolphin_get_plugin_from_mimetype(node->mimeType);
        if (plugin) {
            mdolphin_unregister_plugin(plugin);
            delete_from_reglist(node);
        }
    }
}

#endif
