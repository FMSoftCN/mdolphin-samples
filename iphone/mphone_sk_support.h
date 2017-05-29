#ifndef MINIGUI_MPHION_SK_SUPPORT__H_
#define MINIGUI_MPHION_SK_SUPPORT__H_

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    SFT_SCANCODE,
    SFT_STRING,
} MP_SOFTKEYTYPE;

typedef struct _MP_SOFTKEY_INFO {
    MP_SOFTKEYTYPE type;
    char* str;
    int scancode;
    RECT rect;
    int status;
} MP_SOFTKEY_INFO;

typedef struct _MP_SOFTKEY {
	MP_SOFTKEY_INFO *keysChar;  //leter keyboard
	MP_SOFTKEY_INFO *keysNumber;//number keyboard
    int N_nr_keys;  //the keys of number keyboard
    int C_nr_keys;  //the keys of leter keyboard;
	int sk_IsNumber;       /* 1 for number, 0 for char */
	int sk_IsFullChar;     /* 0 for half char, 1 for full char */
	int offx;
	int offy;
} MP_SOFTKEY;

int init_soft_keyboard();
int destroy_soft_keyboard();

void setoffset_sk(int offx, int offy);
PRECT soft_keyboard_mouse_down(int x, int y);

//PRECT soft_keyboard_get_rect(int x, int y);

int soft_keyboard_is_shift();

void paint_sk_pic();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
