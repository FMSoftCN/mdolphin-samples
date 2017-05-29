noinst_PROGRAMS = mdolphin_app

mdolphin_app_SOURCES = mdolphin_app.cpp mdolphin_app.h \
                        mdolphin_app_res.h \
                        mdolphin_pageview.h \
                        mdolphin_pageview.cpp \
                        mdolphin_skin.h \
                        mdolphin_skin.cpp \
                        mdolphin_transform.h \
                        mdolphin_transform.cpp \
                        mdolphin_animation.h \
                        mdolphin_animation.cpp \
                        mdolphin_functionimp.cpp \
                        mphone_addressField.c \
                        mphone_sk_support.c \
                        mphone_softkey_info.h \
                        mphone_addressField.h \
                        mphone_sk_support.h \
                        ftimer/ft_common.h \
                        ftimer/ft_config.h \
                        ftimer/ft_list.h \
                        ftimer/ft_timer.h \
                        ftimer/ft_timer.c \
                        ftimer/ft_tasklet.h \
                        ftimer/ft_tasklet.c \
                        ftimer/ft_malloc.h \
                        ftimer/ft_malloc.c

if MINIGUI_V3
MGLIBS= -lmgutils -lmgplus
else
MGLIBS= -lmgext
endif

mdolphin_app_LDFLAGS = $(MGLIBS) $(LIBS)
EXTRA_DIST = \
			 res/addr_disable.png \
			 res/addr_disable_pt.png \
			 res/addr_hover.png \
			 res/addr_hover_pt.png \
			 res/addr.png \
			 res/addr_pt.png \
			 res/back_disable.png \
			 res/back_disable_pt.png \
			 res/background.png \
			 res/background_pt.png \
			 res/back_hover.png \
			 res/back_hover_pt.png \
			 res/back.png \
			 res/back_pt.png \
			 res/char.png \
			 res/char_pt.png \
			 res/closepage.png \
			 res/dir1.png \
			 res/dir2.png \
			 res/forward_disable.png \
			 res/forward_disable_pt.png \
			 res/forward_hover.png \
			 res/forward_hover_pt.png \
			 res/forward.png \
			 res/forward_pt.png \
			 res/goback_disable.png \
			 res/goback_disable_pt.png \
			 res/goback_hover.png \
			 res/goback_hover_pt.png \
			 res/goback.png \
			 res/goback_pt.png \
			 res/gohome.png \
			 res/goto.png \
			 res/goto_pt.png \
			 res/local.png \
			 res/local_pt.png \
			 res/mdolphin_background.png \
			 res/mdolphin_background_pt.png \
			 res/newpage_disable.png \
			 res/newpage_disable_pt.png \
			 res/newpage_hover.png \
			 res/newpage_hover_pt.png \
			 res/newpage.png \
			 res/newpage_pt.png \
			 res/number.png \
			 res/number_pt.png \
			 res/pageview_disable.png \
			 res/pageview_disable_pt.png \
			 res/pageview_hover.png \
			 res/pageview_hover_pt.png \
			 res/pageview.png \
			 res/pageview_pt.png \
			 res/refresh_disable.png \
			 res/refresh_disable_pt.png \
			 res/refresh_hover.png \
			 res/refresh_hover_pt.png \
			 res/refresh.png \
			 res/refresh_pt.png \
			 res/stop_disable.png \
			 res/stop_disable_pt.png \
			 res/stop_hover.png \
			 res/stop_hover_pt.png \
			 res/stop.png \
			 res/stop_pt.png \
			 res/zoomin_disable.png \
			 res/zoomin_disable_pt.png \
			 res/zoomin_hover.png \
			 res/zoomin_hover_pt.png \
			 res/zoomin.png \
			 res/zoomin_pt.png \
			 res/zoomout_disable.png \
			 res/zoomout_disable_pt.png \
			 res/zoomout_hover.png \
			 res/zoomout_hover_pt.png \
			 res/zoomout.png \
			 res/zoomout_pt.png 
