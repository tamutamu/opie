TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= inlineedit.h  filebrowser.h filePermissions.h
SOURCES		= filebrowser.cpp inlineedit.cpp filePermissions.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	=
TRANSLATIONS = ../i18n/de/filebrowser.ts
TRANSLATIONS += ../i18n/pt_BR/filebrowser.ts
TRANSLATIONS   += ../i18n/en/filebrowser.ts
TRANSLATIONS   += ../i18n/hu/filebrowser.ts
TRANSLATIONS   += ../i18n/ja/filebrowser.ts
TRANSLATIONS   += ../i18n/fr/filebrowser.ts
TRANSLATIONS   += ../i18n/sl/filebrowser.ts
TRANSLATIONS   += ../i18n/ko/filebrowser.ts
TRANSLATIONS   += ../i18n/no/filebrowser.ts
TRANSLATIONS   += ../i18n/zh_CN/filebrowser.ts
TRANSLATIONS   += ../i18n/zh_TW/filebrowser.ts
