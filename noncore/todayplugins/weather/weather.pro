TEMPLATE = lib
#CONFIG -= moc
CONFIG += qt plugin 

HEADERS = weatherplugin.h       \
          weatherpluginimpl.h   \
          weatherpluginwidget.h \
          weatherconfig.h

SOURCES = weatherplugin.cpp       \
          weatherpluginimpl.cpp   \
          weatherpluginwidget.cpp \
          weatherconfig.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayweatherplugin

include( $(OPIEDIR)/include.pro )
