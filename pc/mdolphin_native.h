
#ifndef mdolphin_binding_h
#define mdolphin_binding_h

#include <mdolphin/mdconfig.h>

#if ENABLE_JSNATIVEBINDING

#define TRACE(msg...) fprintf(stderr, msg)

void define_nativeobjects(void);

#endif /* ENABLE_JSNATIVEBINDING */
#endif /* mdolphin_binding_h */
