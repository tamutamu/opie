#############################################################################
# Makefile for building: libreader_pluckerbase.so.1.0.0
# Generated by qmake (1.06c) (Qt 3.2.3) on: Sun Jul 25 08:40:18 2004
# Project:  pluckerbaselib.pro
# Template: lib
# Command: $(QMAKE) -o pluckerbaselib.mak pluckerbaselib.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -Wall -W -O2 -fomit-frame-pointer -pipe -march=i586 -mcpu=pentiumpro -D_REENTRANT -fPIC  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT
CXXFLAGS = -pipe -Wall -W -O2 -fomit-frame-pointer -pipe -march=i586 -mcpu=pentiumpro -D_REENTRANT -fPIC  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/lib/qt3/mkspecs/default -I. -IQREADERINC -I$(QTDIR)/include -IQREADERMOCS/
LINK     = g++
LFLAGS   = -shared -Wl,-soname,libreader_pluckerbase.so.1
LIBS     = $(SUBLIBS) -L$(QTDIR)/lib -L/usr/X11R6/lib -lqt-mt -lXext -lX11 -lm -lpthread
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

HEADERS = pdb.h
SOURCES = plucker_base.cpp
OBJECTS = QREADEROBJS/plucker_base.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = 
OBJMOC = 
DIST	   = pluckerbaselib.pro
QMAKE_TARGET = reader_pluckerbase
DESTDIR  = $(READERDIR)/lib/
TARGET   = libreader_pluckerbase.so.1.0.0
TARGETA	= $(READERDIR)/lib/libreader_pluckerbase.a
TARGETD	= libreader_pluckerbase.so.1.0.0
TARGET0	= libreader_pluckerbase.so
TARGET1	= libreader_pluckerbase.so.1
TARGET2	= libreader_pluckerbase.so.1.0

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

all: pluckerbaselib.mak  $(READERDIR)/lib/$(TARGET)

$(READERDIR)/lib/$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC) $(SUBLIBS) $(OBJCOMP)  
	test -d $(READERDIR)/lib/ || mkdir -p $(READERDIR)/lib/
	-$(DEL_FILE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS) $(OBJCOMP)
	-ln -s $(TARGET) $(TARGET0)
	-ln -s $(TARGET) $(TARGET1)
	-ln -s $(TARGET) $(TARGET2)
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET)
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET0)
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET1)
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET2)
	-$(MOVE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2) $(READERDIR)/lib/



staticlib: $(TARGETA)

$(TARGETA):  $(UICDECLS) $(OBJECTS) $(OBJMOC) $(OBJCOMP)
	-$(DEL_FILE) $(TARGETA) 
	$(AR) $(TARGETA) $(OBJECTS) $(OBJMOC)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc ; $(MAKE) )

pluckerbaselib.mak: pluckerbaselib.pro  /usr/lib/qt3/mkspecs/default/qmake.conf 
	$(QMAKE) -o pluckerbaselib.mak pluckerbaselib.pro
qmake: 
	@$(QMAKE) -o pluckerbaselib.mak pluckerbaselib.pro

dist: 
	@mkdir -p QREADEROBJS/reader_pluckerbase && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) QREADEROBJS/reader_pluckerbase/ && ( cd `dirname QREADEROBJS/reader_pluckerbase` && $(TAR) reader_pluckerbase.tar reader_pluckerbase && $(GZIP) reader_pluckerbase.tar ) && $(MOVE) `dirname QREADEROBJS/reader_pluckerbase`/reader_pluckerbase.tar.gz . && $(DEL_FILE) -r QREADEROBJS/reader_pluckerbase

mocclean:

uiclean:

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET) $(TARGET)
	-$(DEL_FILE) $(READERDIR)/lib/$(TARGET0) $(READERDIR)/lib/$(TARGET1) $(READERDIR)/lib/$(TARGET2) $(TARGETA)


FORCE:

####### Compile

QREADEROBJS/plucker_base.o: plucker_base.cpp QREADERINC/useqpe.h \
		QREADERINC/static.h \
		plucker_base.h \
		Aportis.h \
		hrule.h \
		CExpander.h \
		ztxt.h \
		pdb.h \
		CBuffer.h \
		my_list.h \
		Navigation.h \
		config.h \
		StyleConsts.h \
		Markups.h \
		names.h \
		linktype.h \
		ustring.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o QREADEROBJS/plucker_base.o plucker_base.cpp

####### Install

install: all  

uninstall:  

