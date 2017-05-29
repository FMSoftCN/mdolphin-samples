noinst_PROGRAMS = mdolphin_app

mdolphin_app_SOURCES = main.cpp mdolphin_callback.cpp
if MINIGUI_V3
MGLIBS=-lmgutils -lmgplus
else
MGLIBS=-lmgext
endif

mdolphin_app_LDFLAGS =  $(MGLIBS) $(LIBS)

