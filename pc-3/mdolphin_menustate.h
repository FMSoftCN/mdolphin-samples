#ifndef _MDOLPHIN_MENUSTATE_H
#define _MDOLPHIN_MENUSTATE_H

void change_loading_menu_status(BOOL load, unsigned int progress);

void change_history_menu_status(unsigned int bcount, unsigned int fcount);

void change_view_menu_status(HWND hwnd);

void change_encoding_menu_status(unsigned int newPos);

void change_file_menu_status(int count, BOOL add);
#endif
