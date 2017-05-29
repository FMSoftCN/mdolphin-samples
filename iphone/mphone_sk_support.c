#include <string.h>
#include "mphone_sk_support.h"
#include "mphone_softkey_info.h"
#include "mphone_addressField.h"
#include "mdolphin_app.h"

#if  IS_PORTRAIT
#define IMG_SK_NUMBER "./res/number_pt.png"
#define IMG_SK_CHAR   "./res/char_pt.png"
#else
#define IMG_SK_NUMBER "./res/number.png"
#define IMG_SK_CHAR   "./res/char.png"
#endif

static BOOL WriteToAddressEdit(MP_SOFTKEY_INFO *keys, int key); 

static BITMAP bmpNumPanel;
static BITMAP bmpCharPanel;

static MP_SOFTKEY *sk_info = NULL;
static RECT rcMouseDown;

int init_soft_keyboard()
{
	if (LoadBitmap(HDC_SCREEN, &bmpNumPanel, IMG_SK_NUMBER)) {
		fprintf(stderr, "not find the "IMG_SK_NUMBER"\n");
		return 1;
	}

	if (LoadBitmap(HDC_SCREEN, &bmpCharPanel, IMG_SK_CHAR)) {
		fprintf(stderr, "not find the "IMG_SK_CHAR"\n");
		return 1;
	}

    sk_info = (MP_SOFTKEY* )calloc(1, sizeof(MP_SOFTKEY));
	sk_info->keysChar = default_keyboard_Char;
	sk_info->keysNumber = default_keyboard_Number;
    sk_info->C_nr_keys = TABLESIZE(default_keyboard_Char);
    sk_info->N_nr_keys = TABLESIZE(default_keyboard_Number);
	sk_info->sk_IsNumber = 0;       /* 1 for number, 0 for char */
	sk_info->sk_IsFullChar = 0;     /* 0 for half char, 1 for full char */

    return 0;
}

int destroy_soft_keyboard()
{
	UnloadBitmap(&bmpNumPanel);
	UnloadBitmap(&bmpCharPanel);
    if (sk_info) {
        free(sk_info);
        sk_info = NULL;
    }
    return 0;
}

void setoffset_sk(int offx, int offy)
{
	if (!sk_info)
		return;

	sk_info->offx = offx;
	sk_info->offy = offy;
}

PRECT soft_keyboard_mouse_down(int x, int y)
{
	int i = 0; 
	int nr_key = 0;
	MP_SOFTKEY_INFO *keys = NULL;

	if (!sk_info)
		return NULL;

	if (sk_info->sk_IsNumber) {
		keys = sk_info->keysNumber;
		nr_key = sk_info->N_nr_keys;
	} else {
		keys = sk_info->keysChar;
		nr_key = sk_info->C_nr_keys;
	}

	for (i = 0; i < nr_key; i++) {
		if (PtInRect(&keys[i].rect, (x - sk_info->offx)*SCALE_X, (y- sk_info->offy)*SCALE_Y)) {
			if (WriteToAddressEdit(keys, i)) {
				//return &keys[i].rect;
				rcMouseDown.left = keys[i].rect.left*XSCALE;
				rcMouseDown.top = keys[i].rect.top*YSCALE;
				rcMouseDown.right = keys[i].rect.right*XSCALE;
				rcMouseDown.bottom = keys[i].rect.bottom*YSCALE;
				return &rcMouseDown;
			}
			return NULL;
		}
	}

	return NULL;
}

static BOOL WriteToAddressEdit(MP_SOFTKEY_INFO *keys, int index)
{
	if (!sk_info)
		return FALSE;

    if (keys[index].type == SFT_STRING && keys[index].str != NULL) {
		AddInputData(keys[index].str, strlen(keys[index].str), sk_info->sk_IsFullChar);
	} else {
		if(keys[index].type == SFT_SCANCODE) {
			switch(keys[index].scancode)
			{
				case SCANCODE_BACKSLASH:
					DeleteInputData(1);
					break;
				case SCANCODE_CHANGE:
					sk_info->sk_IsNumber = !sk_info->sk_IsNumber;
					sk_info->sk_IsFullChar = 0;
					paint_sk_pic();
					break;
				case SCANCODE_LEFTSHIFT:
					sk_info->sk_IsFullChar = !sk_info->sk_IsFullChar;
					break;
				case SCANCODE_GOTO:
					SK_BrowserGoto(GetInputData(FALSE));
					return FALSE;
			}
		}
	}

    return TRUE;
}

void paint_sk_pic()
{
	BITMAP *bmp = NULL;
	int x = sk_info->offx;
	int y = sk_info->offy;

	if (sk_info->sk_IsNumber) 
		bmp = &bmpNumPanel;
	else
		bmp = &bmpCharPanel;

	HDC hdc = GetClientDC(GetAddrWnd());
	FillBoxWithBitmap(hdc, x, y, bmp->bmWidth*XSCALE, bmp->bmHeight*YSCALE, bmp);
    ReleaseDC(hdc);
}

int soft_keyboard_is_shift()
{
	return sk_info->sk_IsFullChar;
}

#if 0
PRECT soft_keyboard_get_rect(int x, int y)
{
	if (!sk_info)
		return NULL;

	MP_SOFTKEY_INFO *keys = NULL;
	int nr_key = 0;
	int i = 0; 

	if(sk_info->sk_IsNumber) {
		keys = sk_info->keysNumber;
		nr_key = sk_info->N_nr_keys;
	} else {
		keys = sk_info->keysChar;
		nr_key = sk_info->C_nr_keys;
	}

	for (i = 0; i < nr_key; i++)
		if (PtInRect(&keys[i].rect, (x - sk_info->offx)/XSCALE, (y - sk_info->offy)/YSCALE))
			return &keys[i].rect; 
	return NULL;
}
#endif

