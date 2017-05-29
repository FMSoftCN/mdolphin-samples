#ifndef mdolphin_plugins_h
#define mdolphin_plugins_h

#include <mdolphin/mdconfig.h>

#if ENABLE_PLUGIN

void register_all_plugins(void);

void unregister_all_plugins(void);

#endif
#endif /* mdolphin_plugins_h */
