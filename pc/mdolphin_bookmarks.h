#ifndef _MDOLPHIN_BOOKMARKS_H
#define _MDOLPHIN_BOOKMARKS_H

HMENU create_bookmarks_submenu(void);

int get_bookmark_currentNum(void);

void bookmark_click_proc(int id);

int create_add_bookmark_window(HWND hParent);

int create_manage_bookmarks_window(HWND hParent);

#endif
