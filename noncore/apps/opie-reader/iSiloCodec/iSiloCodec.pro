DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
#HEADERS		= Aportis.h

SOURCES		= iSilo.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= iSilo
LIBS            += -lreader_pdb -lreader_codec

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )

