#############################################################################
# Makefile for building: libCHM.so.1.0.0
# Generated by qmake (1.06c) (Qt 3.2.3) on: Sun Jul 25 08:37:00 2004
# Project:  CHM.pro
# Template: lib
# Command: $(QMAKE) -o CHM.mak CHM.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -w -O2 -fomit-frame-pointer -pipe -march=i586 -mcpu=pentiumpro -D_REENTRANT -fPIC  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT
CXXFLAGS = -pipe -w -O2 -fomit-frame-pointer -pipe -march=i586 -mcpu=pentiumpro -D_REENTRANT -fPIC  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/lib/qt3/mkspecs/default -I. -IQREADERINC -I$(QTDIR)/include -IQREADERMOCS/
LINK     = g++
LFLAGS   = -shared -Wl,-soname,libCHM.so.1
LIBS     = $(SUBLIBS) -L$(QTDIR)/lib -L/usr/X11R6/lib -L$(READERDIR)/lib -lreader_codec -lqt-mt -lXext -lX11 -lm -lpthread
AR       = ar cqs
RANLIB   = 
MOC      = $(QTDIR)/bin/moc
UIC      = $(QTDIR)/bin/uic
QMAKE    = qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = QREADEROBJS/

####### Files

HEADERS = CHM.h \
		chm_lib.h \
		lzx.h
SOURCES = CHM.cpp \
		chm_lib.c \
		lzx.c
OBJECTS = QREADEROBJS/CHM.o \
		QREADEROBJS/chm_lib.o \
		QREADEROBJS/lzx.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = 
OBJMOC = 
DIST	   = CHM.pro
QMAKE_TARGET = CHM
DESTDIR  = $(READERDIR)/codecs/
TARGET   = libCHM.so.1.0.0
TARGETA	= $(READERDIR)/codecs/libCHM.a
TARGETD	= libCHM.so.1.0.0
TARGET0	= libCHM.so
TARGET1	= libCHM.so.1
TARGET2	= libCHM.so.1.0

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: CHM.mak  $(READERDIR)/codecs/$(TARGET)

$(READERDIR)/codecs/$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC) $(SUBLIBS) $(OBJCOMP)  
	test -d $(READERDIR)/codecs/ || mkdir -p $(READERDIR)/codecs/
	-$(DEL_FILE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS) $(OBJCOMP)
	-ln -s $(TARGET) $(TARGET0)
	-ln -s $(TARGET) $(TARGET1)
	-ln -s $(TARGET) $(TARGET2)
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET)
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET0)
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET1)
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET2)
	-$(MOVE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2) $(READERDIR)/codecs/



staticlib: $(TARGETA)

$(TARGETA):  $(UICDECLS) $(OBJECTS) $(OBJMOC) $(OBJCOMP)
	-$(DEL_FILE) $(TARGETA) 
	$(AR) $(TARGETA) $(OBJECTS) $(OBJMOC)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc ; $(MAKE) )

CHM.mak: CHM.pro  /usr/lib/qt3/mkspecs/default/qmake.conf 
	$(QMAKE) -o CHM.mak CHM.pro
qmake: 
	@$(QMAKE) -o CHM.mak CHM.pro

dist: 
	@mkdir -p QREADEROBJS/CHM && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) QREADEROBJS/CHM/ && ( cd `dirname QREADEROBJS/CHM` && $(TAR) CHM.tar CHM && $(GZIP) CHM.tar ) && $(MOVE) `dirname QREADEROBJS/CHM`/CHM.tar.gz . && $(DEL_FILE) -r QREADEROBJS/CHM

mocclean:

uiclean:

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET) $(TARGET)
	-$(DEL_FILE) $(READERDIR)/codecs/$(TARGET0) $(READERDIR)/codecs/$(TARGET1) $(READERDIR)/codecs/$(TARGET2) $(TARGETA)


FORCE:

####### Compile

QREADEROBJS/CHM.o: CHM.cpp CHM.h \
		chm_lib.h \
		QREADERINC/static.h \
		QREADERINC/useqpe.h \
		CExpander.h \
		my_list.h \
		config.h \
		StyleConsts.h \
		Markups.h \
		names.h \
		linktype.h \
		ustring.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o QREADEROBJS/CHM.o CHM.cpp

QREADEROBJS/chm_lib.o: chm_lib.c chm_lib.h \
		lzx.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o QREADEROBJS/chm_lib.o chm_lib.c

QREADEROBJS/lzx.o: lzx.c lzx.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o QREADEROBJS/lzx.o lzx.c

####### Install

install: all  

uninstall:  
