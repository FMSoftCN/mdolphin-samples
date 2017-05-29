noinst_PROGRAMS = mdolphin_app

mdolphin_app_SOURCES = mdolphin_app.cpp mdolphin_app.h \
					   mdolphin_app_res.h \
					   mdolphin_functionimp.cpp \
					   mdolphin_find.cpp \
					   mdolphin_preferences.cpp \
					   mdolphin_plugins.cpp 

if MINIGUI_V3
MGLIBS= -lmgutils -lmgplus
else
MGLIBS= -lmgext
endif

mdolphin_app_LDFLAGS =  $(MGLIBS) $(LIBS)

