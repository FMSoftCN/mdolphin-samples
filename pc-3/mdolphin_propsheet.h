#ifndef _MDOLPHIN_PROPSHEET_H
#define _MDOLPHIN_PROPSHEET_H

HWND my_create_new_window(const char* url, DWORD styles, int x, int y, int width, int height);

void my_close_tab_window(HWND hWnd);

HWND get_current_mdolphin_hwnd(void);

void view_page_in_source_mode(HWND hWnd);
#endif
