
#include "mdolphin_app.h"
#include "mphone_sk_support.h"

#ifndef mphone_softkey_h
#define mphone_softkey_h

#define SCANCODE_CHANGE 1009
#define SCANCODE_GOTO   1010

#if IS_PORTRAIT

//this file setting the softkey info
static MP_SOFTKEY_INFO default_keyboard_Char[] = {
    {SFT_STRING, "q", 0, {12, 7, 24, 33}, 0},
    {SFT_STRING, "w", 0, {31, 7, 43, 33}, 0},
    {SFT_STRING, "e", 0, {50, 7, 62, 33}, 0},
    {SFT_STRING, "r", 0, {69, 7, 82, 33}, 0},
    {SFT_STRING, "t", 0, {88, 7, 100, 33}, 0},
    {SFT_STRING, "y", 0, {107, 7, 120, 33}, 0},
    {SFT_STRING, "u", 0, {126, 7, 139, 33}, 0},
    {SFT_STRING, "i", 0, {146, 7, 158, 33}, 0},
    {SFT_STRING, "o", 0, {165, 7, 177, 33}, 0},
    {SFT_STRING, "p", 0, {184, 7, 196, 33}, 0},

    {SFT_STRING, "a", 0, {20, 42, 33, 68}, 0},
    {SFT_STRING, "s", 0, {40, 42, 52, 68}, 0},
    {SFT_STRING, "d", 0, {59, 42, 71, 68}, 0},
    {SFT_STRING, "f", 0, {78, 42, 91, 68}, 0},
    {SFT_STRING, "g", 0, {97, 42, 110, 68}, 0},
    {SFT_STRING, "h", 0, {117, 42, 129, 68}, 0},
    {SFT_STRING, "j", 0, {136, 42, 148, 68}, 0},
    {SFT_STRING, "k", 0, {155, 42, 168, 68}, 0},
    {SFT_STRING, "l", 0, {174, 42, 187, 68}, 0},

    {SFT_STRING, "z", 0, {40, 76, 52, 102}, 0},
    {SFT_STRING, "x", 0, {59, 76, 71, 102}, 0},
    {SFT_STRING, "c", 0, {78, 76, 91, 102}, 0},
    {SFT_STRING, "v", 0, {97, 76, 110, 102}, 0},
    {SFT_STRING, "b", 0, {117, 76, 129, 102}, 0},
    {SFT_STRING, "n", 0, {136, 76, 148, 102}, 0},
    {SFT_STRING, "m", 0, {155, 76, 168, 102}, 0},

    {SFT_STRING, ".", 0, {98, 109, 120, 136}, 0},
    {SFT_STRING, "/", 0, {126, 109, 148, 136}, 0},
    {SFT_STRING, ".com", 0, {155, 109, 191, 136}, 0},
    {SFT_STRING, "www", 0, {56, 109, 91, 136}, 0},

    {SFT_SCANCODE, NULL, SCANCODE_LEFTSHIFT, {11, 75, 30, 102}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSLASH, {177, 75, 196, 102}, 0},
    //{SFT_SCANCODE, NULL, SCANCODE_CHANGE, {26, 109, 115, 135}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_CHANGE, {16, 109, 49, 135}, 0},
	//{SFT_SCANCODE, NULL, SCANCODE_GOTO, {302, 109, 395, 135}, 0}
};

static MP_SOFTKEY_INFO default_keyboard_Number[] = {
    {SFT_STRING, "1", 0, {12, 7, 24, 33}, 0},
    {SFT_STRING, "2", 0, {31, 7, 43, 33}, 0},
    {SFT_STRING, "3", 0, {50, 7, 62, 33}, 0},
    {SFT_STRING, "4", 0, {70, 7, 82, 33}, 0},
    {SFT_STRING, "5", 0, {89, 7, 101, 33}, 0},
    {SFT_STRING, "6", 0, {108, 7, 120, 33}, 0},
    {SFT_STRING, "7", 0, {127, 7, 139, 33}, 0},
    {SFT_STRING, "8", 0, {147, 7, 159, 33}, 0},
    {SFT_STRING, "9", 0, {166, 7, 177, 33}, 0},
    {SFT_STRING, "0", 0, {185, 7, 196, 33}, 0},

    {SFT_STRING, "@", 0, {12, 40, 33, 68}, 0},
    {SFT_STRING, "&", 0, {45, 40, 66, 68}, 0},
    {SFT_STRING, "%", 0, {77, 40, 98, 68}, 0},
    {SFT_STRING, "?", 0, {110, 40, 131, 68}, 0},
    {SFT_STRING, ",", 0, {142, 40, 164, 68}, 0},
    {SFT_STRING, "=", 0, {175, 40, 196, 68}, 0},

    {SFT_STRING, "_", 0, {40, 75, 64, 102}, 0},
    {SFT_STRING, ":", 0, {75, 75, 99, 102}, 0},
    {SFT_STRING, "-", 0, {110, 75, 133, 102}, 0},
    {SFT_STRING, "+", 0, {144, 75, 168, 102}, 0},

    {SFT_STRING, ".", 0, {98, 109, 120, 136}, 0},
    {SFT_STRING, "/", 0, {126, 109, 148, 136}, 0},
    {SFT_STRING, ".com", 0, {155, 109, 192, 136}, 0},
    {SFT_STRING, "www", 0, {56, 109, 91, 136}, 0},

    {SFT_SCANCODE, NULL, SCANCODE_LEFTSHIFT, {12, 75, 31, 100}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSLASH, {178, 75, 196, 100}, 0},
    //{SFT_SCANCODE, NULL, SCANCODE_CHANGE, {26, 109, 115, 136}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_CHANGE, {17, 109, 49, 136}, 0},
//    {SFT_SCANCODE, NULL, SCANCODE_GOTO, {302, 109, 395, 135}, 0}
};

#else

//this file setting the softkey info
static MP_SOFTKEY_INFO default_keyboard_Char[] = {
    {SFT_STRING, "q", 0, {23, 7, 47, 33}, 0},
    {SFT_STRING, "w", 0, {62, 7, 84, 33}, 0},
    {SFT_STRING, "e", 0, {100, 7, 123, 33}, 0},
    {SFT_STRING, "r", 0, {140, 7, 162, 33}, 0},
    {SFT_STRING, "t", 0, {178, 7, 201, 33}, 0},
    {SFT_STRING, "y", 0, {216, 7, 240, 33}, 0},
    {SFT_STRING, "u", 0, {255, 7, 278, 33}, 0},
    {SFT_STRING, "i", 0, {293, 7, 317, 33}, 0},
    {SFT_STRING, "o", 0, {330, 7, 355, 33}, 0},
    {SFT_STRING, "p", 0, {369, 7, 393, 33}, 0},


    {SFT_STRING, "a", 0, {42, 42, 67, 68}, 0},
    {SFT_STRING, "s", 0, {82, 42, 104, 68}, 0},
    {SFT_STRING, "d", 0, {119, 42, 143, 68}, 0},
    {SFT_STRING, "f", 0, {159, 42, 182, 68}, 0},
    {SFT_STRING, "g", 0, {197, 42, 221, 68}, 0},
    {SFT_STRING, "h", 0, {235, 42, 259, 68}, 0},
    {SFT_STRING, "j", 0, {273, 42, 297, 68}, 0},
    {SFT_STRING, "k", 0, {312, 42, 336, 68}, 0},
    {SFT_STRING, "l", 0, {350, 42, 374, 68}, 0},

    {SFT_STRING, "z", 0, {82, 76, 105, 102}, 0},
    {SFT_STRING, "x", 0, {119, 76, 144, 102}, 0},
    {SFT_STRING, "c", 0, {158, 76, 183, 102}, 0},
    {SFT_STRING, "v", 0, {195, 76, 220, 102}, 0},
    {SFT_STRING, "b", 0, {236, 76, 259, 102}, 0},
    {SFT_STRING, "n", 0, {273, 76, 297, 102}, 0},
    {SFT_STRING, "m", 0, {313, 76, 336, 102}, 0},

    {SFT_STRING, ".", 0, {182, 109, 226, 136}, 0},
    {SFT_STRING, "/", 0, {239, 109, 282, 136}, 0},
    {SFT_STRING, ".com", 0, {296, 109, 339, 136}, 0},


    {SFT_SCANCODE, NULL, SCANCODE_LEFTSHIFT, {25, 75, 60, 102}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSLASH, {358, 75, 393, 102}, 0},
    //{SFT_SCANCODE, NULL, SCANCODE_CHANGE, {26, 109, 115, 135}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_CHANGE, {77, 109, 168, 135}, 0},
	//{SFT_SCANCODE, NULL, SCANCODE_GOTO, {302, 109, 395, 135}, 0}
};

static MP_SOFTKEY_INFO default_keyboard_Number[] = {
    {SFT_STRING, "1", 0, {24, 7, 50, 33}, 0},
    {SFT_STRING, "2", 0, {64, 7, 87, 33}, 0},
    {SFT_STRING, "3", 0, {102, 7, 125, 33}, 0},
    {SFT_STRING, "4", 0, {141, 7, 164, 33}, 0},
    {SFT_STRING, "5", 0, {179, 7, 203, 33}, 0},
    {SFT_STRING, "6", 0, {217, 7, 242, 33}, 0},
    {SFT_STRING, "7", 0, {256, 7, 279, 33}, 0},
    {SFT_STRING, "8", 0, {295, 7, 319, 33}, 0},
    {SFT_STRING, "9", 0, {332, 7, 357, 33}, 0},
    {SFT_STRING, "0", 0, {370, 7, 394, 33}, 0},


    {SFT_STRING, "@", 0, {24, 40, 68, 68}, 0},
    {SFT_STRING, "&", 0, {89, 40, 133, 68}, 0},
    {SFT_STRING, "%", 0, {154, 40, 197, 68}, 0},
    {SFT_STRING, "?", 0, {220, 40, 264, 68}, 0},
    {SFT_STRING, ",", 0, {285, 40, 329, 68}, 0},
    {SFT_STRING, "=", 0, {350, 40, 394, 68}, 0},

    {SFT_STRING, "_", 0, {80, 75, 131, 102}, 0},
    {SFT_STRING, ":", 0, {150, 75, 199, 102}, 0},
    {SFT_STRING, "-", 0, {220, 75, 270, 102}, 0},
    {SFT_STRING, "+", 0, {290, 75, 338, 102}, 0},

    {SFT_STRING, ".", 0, {182, 109, 227, 136}, 0},
    {SFT_STRING, "/", 0, {238, 109, 283, 136}, 0},
    {SFT_STRING, ".com", 0, {294, 109, 339, 136}, 0},

    {SFT_SCANCODE, NULL, SCANCODE_LEFTSHIFT, {25, 75, 63, 100}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSLASH, {358, 75, 395, 100}, 0},
    //{SFT_SCANCODE, NULL, SCANCODE_CHANGE, {26, 109, 115, 136}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_CHANGE, {74, 109, 167, 136}, 0},
//    {SFT_SCANCODE, NULL, SCANCODE_GOTO, {302, 109, 395, 135}, 0}
};

#endif // !IS_PORTRAIT
#endif // mphone_softkey_h

