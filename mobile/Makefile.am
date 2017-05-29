noinst_PROGRAMS = mdolphin_app

mdolphin_app_SOURCES = mdolphin_app.cpp mdolphin_app.h

if MINIGUI_V3
MGLIBS= -lmgutils -lmgplus
else
MGLIBS= -lmgext
endif

mdolphin_app_LDFLAGS =  $(MGLIBS) $(LIBS)
