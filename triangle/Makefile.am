noinst_PROGRAMS = triangle
noinst_PROGRAMS += triangledaemon

triangle_SOURCES = \
	URLAccessPolicy.h \
	URLAccessPolicy.cpp \
	URLDepository.cpp \
	URLDepository.h \
	URLStorageController.h \
	URLStorage.cpp \
	urlStorage.h \
	URLDataBase.cpp \
	URLDataBase.h \
	Triangle.cpp \
	Triangle.h \
	TriangleLogging.cpp \
	TriangleLogging.h \
	FrameLoadDelegate.cpp \
	FrameLoadDelegate.h \
	sqlite3.c \
	sqlite3.h \
	\
	CustomDelegate.cpp \
	CustomDelegate.h \
	UIDelegate.cpp	\
	UIDelegate.h	\
	\
	fifo.cpp \
	\
	main.cpp 

triangledaemon_SOURCES = triangledaemon.cpp
if MINIGUI_V3
MGLIBS= -lmgutils -lmgplus
else
MGLIBS= -lmgext
endif

triangle_LDFLAGS =  $(MGLIBS) $(LIBS)
triangledaemon_LDFLAGS = 

