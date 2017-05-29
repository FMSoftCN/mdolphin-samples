#ifndef mdolphin_scheme_h
#define mdolphin_scheme_h

#include <minigui/common.h>
#include <minigui/minigui.h>
#include "mdolphin_app.h"
#include <mdolphin/mdolphin.h>

#if ENABLE_SCHEMEEXTENSION

struct schemeHandler {
    const char *scheme;
    CB_SCHEME_HANDLER proc;
};

void register_schemes(void);
void unregister_schemes(void);

#endif
#endif /* mdolphin_scheme_h */
