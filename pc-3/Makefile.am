noinst_PROGRAMS = mdolphin_app

mdolphin_app_SOURCES = \
	mdolphin_app.cpp mdolphin_app.h mdolphin_app_res.h \
	mdolphin_callback.cpp prompt_dialog.cpp mdolphin_proxy.cpp \
	mdolphin_cert.h mdolphin_clientcert.cpp  \
	mdolphin_verifycert.cpp mdolphin_auth.cpp \
	mdolphin_savefile.cpp progress_dialog.cpp \
	mdolphin_preferences.cpp mdolphin_popupmenu.cpp \
	mdolphin_bookmarks.h mdolphin_bookmarks.cpp\
	mdolphin_propsheet.h mdolphin_propsheet.cpp \
	mdolphin_menustate.h mdolphin_menustate.cpp \
	mdolphin_dialogwindow.cpp mdolphin_find.cpp \
	mdolphin_scheme.h mdolphin_scheme.cpp \
	mdolphin_plugins.h mdolphin_plugins.cpp \
	mdolphin_tooltipwin.h mdolphin_tooltipwin.cpp \
	mdolphin_errcode.cpp mdolphin_native.cpp mdolphin_native.h
if MINIGUI_V3
MGLIBS=-lmgutils -lmgplus
else
MGLIBS=-lmgext
endif

mdolphin_app_LDFLAGS =  $(MGLIBS) $(LIBS)

