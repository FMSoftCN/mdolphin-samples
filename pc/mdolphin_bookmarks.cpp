
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mdolphin/mdolphin.h>
#include "mdolphin_app.h"
#include "mdolphin_propsheet.h"

#define IDC_BOOK_NAME  640
#define IDC_BOOK_LIST  641
#define IDC_BOOK_DELETE  642

extern HWND propsheet_hwnd;
extern HWND location_hwnd;
extern HMENU main_menu;

static char staticFileName[512];
static int currentPosition = 0;

int get_bookmark_currentNum()
{
    return currentPosition;
}

static void append_menu (HMENU menu, unsigned int type, char* title, int id)
{
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = type;
    if (mii.type != MFT_SEPARATOR) {
        mii.state = MFS_ENABLED;
        mii.typedata = (DWORD)title;
        mii.hsubmenu = 0;
        mii.id = id;
    }
    InsertMenuItem (menu, GetMenuItemCount(menu), TRUE, &mii);
}

static char * file_name()
{
    memset(staticFileName, 0, 512);
    char * current_dir = get_current_dir_name();
    if (current_dir) {
        snprintf(staticFileName, 511, "%s%s%s", current_dir, "/.mDolphin/", "bookmarks.dat");
        staticFileName[511]='\0';
        free (current_dir);
    }
    return staticFileName;
}

static BOOL checkFile()
{
    FILE *fp = NULL;
    BOOL result = FALSE;
    fp = fopen(file_name(), "r");
    if (fp != NULL) {
        char head[18];
        fread(head, 1, 18, fp);
        if (memcmp(head, "mDolphin BookMarks", 18) == 0) 
            result = TRUE;
        fclose(fp);
    }
    return result;
}

static void addBookMarkItem(const char* buf, HMENU menu)
{
    int bufsize = strlen(buf);
    char *pos = NULL;
    char *title = NULL;
    int len = 0;
    int id=1;
    while ((pos = (char*)memchr(buf+len, ';', bufsize-len))) {
        int len1 = pos-buf-len+1;
        if (id%2!=0) {
            title = (char*)malloc(len1);
            if (title) {
                memset(title, 0, len1);
                memcpy(title, buf+len, len1-1);
                title[len1-1]='\0';
                append_menu (menu, MFT_STRING, title, IDM_BOOK_FIRST + currentPosition);
                free(title);
                currentPosition ++;
            }
        }
        len+=len1;
        id++;
    }
}

static char* file_content()
{
    FILE *fp = NULL;
    char *buf = NULL;
    long begin, end, total;
    fp = fopen(file_name(), "r");
    if (fp != NULL) {
        fseek(fp, 18, SEEK_SET);
        begin = ftell(fp);
        fseek(fp, 0, SEEK_END);
        end = ftell(fp);
        total = end-begin;
        buf = (char*)malloc(total+1);
        if (buf) {
            memset(buf, 0, total+1);
            fseek(fp, 18, SEEK_SET);
            fread(buf, 1, total, fp);
            buf[total] = '\0';
        }
        fclose(fp);
    }
    return buf;
}

HMENU create_bookmarks_submenu(void)
{
    HMENU menu;
    MENUITEMINFO mii;
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;
    mii.typedata = (DWORD)"";
    menu = CreatePopupMenu(&mii);

    if (checkFile()) {
        char* buf = file_content();
        if (buf) {
            addBookMarkItem(buf, menu);
            free(buf);
        }
    }
    return StripPopupHead(menu);
}

static char* get_url_from_content(const char* buf, int key)
{
    long bufsize = strlen(buf);
    long begin = 0;
    long end =0;
    char *pos = NULL;
    char *url = NULL;
    long len = 0;
    int id=1;
    
    while ((pos = (char*)memchr(buf+len, ';', bufsize-len))) {
        len = pos-buf+1;
        if (id == (key*2+1))
            begin = len;
        if (id == (key*2+2)){
            end = len-1;
            break;
        }
        id++;
    }
    if (begin && end){
        url = (char*)malloc(end-begin+1);
        if (url) {
            memcpy(url, buf+begin, end-begin);
            url[end-begin]='\0';
        }
    }
    return url;
}

void bookmark_click_proc(int id)
{
    if (checkFile()) {
        char* buf = file_content();
        if (buf) {
            char* url = NULL;
            url = get_url_from_content(buf, id-IDM_BOOK_FIRST);
            if (url) {
                HWND mdolphin_cHwnd = get_current_mdolphin_hwnd();
                if (mdolphin_cHwnd)
                    mdolphin_navigate(mdolphin_cHwnd, NAV_GOTO, url, FALSE);
                free(url);
            }
            free(buf);
        }
    }
}

static void initAddMarkDialog(HWND hDlg)
{
    int lenU = 0;
    int lenT = 0;
    lenU = GetWindowTextLength (location_hwnd);
    int index = SendMessage(propsheet_hwnd, PSM_GETACTIVEINDEX, 0, 0);
    lenT = SendMessage(propsheet_hwnd, PSM_GETTITLELENGTH, index, 0);
    if (lenT == PS_ERR)
        return;
    if (lenU && lenT) {
        char *title = (char *)malloc(lenT+1);
        if (title) {
            memset(title, 0, lenT+1);
            SendMessage(propsheet_hwnd, PSM_GETTITLE, index, (LPARAM)title);
            title[lenT] = '\0';
            if(strcmp(title,"") != 0) 
                SetWindowText(GetDlgItem (hDlg, IDC_BOOK_NAME), title);
            SendMessage(GetDlgItem(hDlg, IDC_BOOK_NAME), EM_SELECTALL, 0, 0);
            free (title);
        }
    }
}

static BOOL checkIsSaved(char *url)
{
    BOOL result = FALSE;
    if (checkFile()) {
        char* buf = file_content();
        if (buf) {
            int len = strlen(url)+2;
            if (len > 0) {
                char* full = (char *)malloc(len+1);
                if (full) {
                    strcpy(full, ";");
                    strcat(full, url);
                    strcat(full, ";");
                    full[len] = '\0';
                    if(strstr(buf, full))
                        result = TRUE;
                    free(full);
                }
            }
            free(buf);
        }
    }
    return result;
}

static void writeToFile(FILE * fp, const char* title, const char* url)
{
    fwrite(title, 1, strlen(title), fp);
    fwrite(";", 1, 1, fp);
    fwrite(url, 1, strlen(url), fp);
    fwrite(";", 1, 1, fp);
}

static void addItemToLocalFile(const char* title, const char* url)
{
    FILE *fp = NULL;
    char *fileName = file_name(); 
    if (checkFile()) {
        fp = fopen(fileName,"a+");
        if (fp) {
            writeToFile (fp, title, url);
            fclose(fp);
        }
    } else {
        char dirPath[512];
        DIR *dir;
        char *current_dir = get_current_dir_name();
        if (current_dir) {
            snprintf(dirPath, 512, "%s%s", current_dir, "/.mDolphin/");
            dir = opendir(dirPath);
            if ( dir == NULL)
                mkdir(dirPath, 01777);
            else
                closedir(dir);
            fp = fopen(fileName, "w");
            if (fp) {
                fwrite("mDolphin BookMarks", 1, strlen("mDolphin BookMarks"), fp);
                writeToFile (fp, title, url);
                fclose(fp);
            }
            free(current_dir);
        }
    }
}

static void addNewItem(HWND hDlg)
{
    int lenT=0, lenU=0;
    lenU = GetWindowTextLength (location_hwnd);
    lenT = GetWindowTextLength (GetDlgItem (hDlg, IDC_BOOK_NAME));
    if (lenU <= 0 || lenT <= 0)
        return;

    char *title = (char *)malloc(lenT+1);
    char *url = (char *)malloc(lenU+1);
    if (title && url) {
        memset(title, 0, lenT+1);
        memset(url, 0, lenU+1);
        GetWindowText(GetDlgItem (hDlg, IDC_BOOK_NAME), title, lenT);
        GetWindowText(location_hwnd, url, lenU);
        title[lenT] = '\0';
        url[lenU] = '\0';
        if (!checkIsSaved(url))
            addItemToLocalFile(title, url);
    }
    if (title)
        free (title);
    if (url)
        free (url);
}

static DLGTEMPLATE addMarkData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , 310, 140,
     "Add Bookmark",
     0, 0,
     4,
     NULL, 0
};

static CTRLDATA addMarkInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        10, 5, 290, 60,
        0,
        "Name",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_READONLY | ES_AUTOSELECT,
        30, 30, 260, 23,
        IDC_BOOK_NAME,
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        160, 80, 60, 25,
        IDOK, 
        "Add",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        230, 80, 60, 25,
        IDCANCEL, 
        "Cancel",
        0
    }
};


//static int addMarkProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)	// gengyue
static LRESULT addMarkProc (HWND hDlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    switch (message) {
        case MSG_INITDIALOG: 
            initAddMarkDialog(hDlg);
            GetWindowRect ((HWND)(lParam), &rc);
            MoveWindow (hDlg , (rc.right-rc.left)/2-155 , (rc.bottom-rc.top)/2-70 , 310 , 140 , TRUE);
            return 1;
        case MSG_COMMAND:
            switch (wParam) {
                case IDOK: 
                    addNewItem(hDlg);
                    MENUITEMINFO mmii;
                    MENUITEMINFO smii;
                    char buf[200];
                    mmii.mask = MIIM_SUBMENU;
                    smii.mask = MIIM_SUBMENU;
                    mmii.typedata = (DWORD)buf; 
                    mmii.cch = 199;
                    GetMenuItemInfo(main_menu, POS_BOOKMARKS_MENU, MF_BYPOSITION, &mmii);
                    smii.typedata = (DWORD)buf; 
                    smii.cch = 199;
                    GetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);
                    if (smii.hsubmenu)
                        DestroyMenu(smii.hsubmenu);
                    currentPosition = 0;
                    smii.hsubmenu = create_bookmarks_submenu();
                    SetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);
                    EndDialog (hDlg, wParam);
                    break;
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

int create_add_bookmark_window(HWND hParent)
{
    addMarkData.controls = addMarkInitProgress;
    return DialogBoxIndirectParam (&addMarkData, hParent, addMarkProc, (DWORD)hParent);
}

/*Manage Bookmarks*/
static void initBookMarksListFromFile(HWND hDlg, const char* buf)
{
    long bufsize = strlen(buf);
    long begin = 0;
    long end =0;
    char *pos = NULL;
    char *title = NULL;
    long len = 0;
    int id=1;

    while ((pos = (char*)memchr(buf+len, ';', bufsize-len))) {
        len = pos-buf+1;
        if (id%2 != 0) {
            end = len;
            title = (char *)malloc(end-begin+1);
            if (title) {
                memcpy(title, buf+begin, end-begin);
                title[end-begin] = '\0';
                SendMessage(GetDlgItem (hDlg, IDC_BOOK_LIST), LB_ADDSTRING, 0, (LPARAM)title);
                free(title);
            }
        } else {
            begin = len;
        }
        id++;
    }
}

static void initBookMarksListBox(HWND hDlg)
{
    if (checkFile()) {
        char* buf = file_content();
        if (buf) {
            initBookMarksListFromFile(hDlg, buf);
            free(buf);
        }
    }
}

static void saveNewFile(const char* buf, const char* title)
{
    FILE *fp = NULL;
    fp = fopen(file_name(), "w");
    if (fp != NULL) {
        const char *pos = strstr(buf, title);
        if (pos) {
            int bufsize = strlen(buf);
            int begin = pos-buf;
            pos = strchr(pos+strlen(title)+1, ';');
            if (pos) {
                int end = pos-buf+1;
                fwrite("mDolphin BookMarks", 1, strlen("mDolphin BookMarks"), fp);
                fwrite(buf, 1, begin, fp);
                fwrite(buf+end, 1, bufsize-end, fp);
            }
        }
        fclose(fp);
    }
}

static void deleteBookMarkFromFile(const char *title)
{
    if (checkFile()) {
        char* buf = file_content();
        if (buf) {
            saveNewFile(buf, title);
            free(buf);
        }
    }
}

static void deleteBookMarkItems(HWND hDlg)
{
    int index = SendMessage(GetDlgItem (hDlg, IDC_BOOK_LIST), LB_GETCURSEL, 0, 0);
    if (index == LB_ERR)
        return;
    int length = SendMessage(GetDlgItem (hDlg, IDC_BOOK_LIST), LB_GETTEXTLEN, index, 0);
    char* title = (char *)malloc(length+1);
    if (title) {
        SendMessage(GetDlgItem (hDlg, IDC_BOOK_LIST), LB_GETTEXT, index, (LPARAM)title);
        title[length]='\0';
        deleteBookMarkFromFile(title);
        free(title);
    }
    SendMessage(GetDlgItem (hDlg, IDC_BOOK_LIST), LB_DELETESTRING, index, 0);
}

static DLGTEMPLATE manageMarkData =
{
     WS_CAPTION | WS_BORDER,
     WS_EX_NONE, 
     0, 0 , 450, 280,
     "Bookmarks Manager",
     0, 0,
     4,
     NULL, 0
};

static CTRLDATA manageMarkInitProgress [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE | SS_GROUPBOX,
        10, 5, 430, 200,
        0,
        "Bookmarks List",
        0, WS_EX_TRANSPARENT
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE | WS_VSCROLL | WS_BORDER,
        25, 30, 400, 160,
        IDC_BOOK_LIST,
        "",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        160, 220, 60, 25,
        IDC_BOOK_DELETE, 
        "Delete",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        230, 220, 60, 25,
        IDOK, 
        "OK",
        0
    }
};

//static int manageMarkProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)		// gengyue
static LRESULT manageMarkProc (HWND hDlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
    char buf[200];
    RECT rc;
    switch (message) {
        case MSG_INITDIALOG: 
            initBookMarksListBox(hDlg);
            GetWindowRect ((HWND)(lParam), &rc);
            MoveWindow (hDlg , (rc.right-rc.left)/2-155 , (rc.bottom-rc.top)/2-140 , 450 , 280 , TRUE);
            return 1;
        case MSG_KEYDOWN:
            if (wParam == SCANCODE_ESCAPE)
            {
                PostMessage(hDlg, MSG_CLOSE, 0, 0);
                return 0;
            }
            break;
        case MSG_COMMAND:
            switch (wParam) {
                case IDC_BOOK_DELETE: 
                    deleteBookMarkItems(hDlg);
                    break;
                case IDOK:
                    MENUITEMINFO mmii;
                    MENUITEMINFO smii;
                    mmii.mask = MIIM_SUBMENU;
                    smii.mask = MIIM_SUBMENU;
                    smii.typedata = (DWORD)buf; 
                    smii.cch = 199;
                    mmii.typedata = (DWORD)buf; 
                    mmii.cch = 199;
                    GetMenuItemInfo(main_menu, POS_BOOKMARKS_MENU, MF_BYPOSITION, &mmii);
                    GetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);
                    if (smii.hsubmenu)
                        DestroyMenu(smii.hsubmenu);
                    currentPosition = 0;
                    smii.hsubmenu = create_bookmarks_submenu();
                    SetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);
                    EndDialog (hDlg, wParam);
                    break;
            }
            break;
        case MSG_CLOSE:
            {
                MENUITEMINFO mmii;
                MENUITEMINFO smii;
                mmii.mask = MIIM_SUBMENU;
                smii.mask = MIIM_SUBMENU;
                smii.typedata = (DWORD)buf; 
                smii.cch = 199;
                mmii.typedata = (DWORD)buf; 
                mmii.cch = 199;
                GetMenuItemInfo(main_menu, POS_BOOKMARKS_MENU, MF_BYPOSITION, &mmii);
                GetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);
                if (smii.hsubmenu)
                    DestroyMenu(smii.hsubmenu);
                currentPosition = 0;
                smii.hsubmenu = create_bookmarks_submenu();
                SetMenuItemInfo(mmii.hsubmenu, POS_BOOKMARKS, MF_BYPOSITION, &smii);

                EndDialog (hDlg, IDCANCEL);
                break;
            }
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int create_manage_bookmarks_window(HWND hParent)
{
    manageMarkData.controls = manageMarkInitProgress;
    return DialogBoxIndirectParam (&manageMarkData, hParent, manageMarkProc, (DWORD)hParent);
}
