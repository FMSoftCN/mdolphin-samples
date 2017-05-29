#include <unistd.h>
#include <stdio.h>
#include "mdolphin_scheme.h"

#if ENABLE_SCHEMEEXTENSION

#define RegisterScheme(scheme, cb, param) mdolphin_register_scheme_handler(scheme, cb, param)
#define UnregisterScheme(scheme)  mdolphin_unregister_scheme_handler(scheme)

static BOOL schemeProc(const char *url, void *param)
{
    printf("invoke: url=%s, parma=0x%x\n", url, (unsigned int)param);
    return TRUE;
}

static int mailtoProc(const char *url, void *param)
{
    int pid;
    if((pid = fork())==0) {
        execl("/usr/bin/gnome-open", "gnome-open", url, NULL); 
        perror("execl gnome-open error!");
    }
    return 0;
}

static schemeHandler schemes[] =
{
    {"dtv", &schemeProc},
    {"rtsp:", &schemeProc},
    {"dvb", &schemeProc},
    {"mailto", &mailtoProc},
};

void register_schemes(void)
{
    size_t i;
    for (i=0; i<sizeof(schemes)/sizeof(schemes[0]); i++) {
        if (! RegisterScheme(schemes[i].scheme, schemes[i].proc, (void*)&schemes[i]))
            fprintf(stderr, "Register scheme:%s failed!\n", schemes[i].scheme);
    }
}

void unregister_schemes(void)
{
    size_t i;
    for (i=0; i<sizeof(schemes)/sizeof(schemes[0]); i++)
        UnregisterScheme(schemes[i].scheme);
}

#endif
