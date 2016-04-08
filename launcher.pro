#
# \file launcher.pro
#
# \copyright 2016 Lukas Geyer. All rights reseverd.
# \license This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License version 3 as
#          published by the Free Software Foundation.
#

QT += core gui sql widgets

TARGET = launcher
TEMPLATE = app

CONFIG += c++11 warn_on

SOURCES += \
   src/main.cpp\
   src/itemmodel.cpp \
   src/itemview.cpp \
   src/itemdelegate.cpp \
   src/itemfiltermodel.cpp \
   src/itemedit.cpp \
   src/itemhotkey.cpp \
   src/itemwindow.cpp \
   src/itemlock.cpp

HEADERS += \
   src/itemmodel.h \
   src/itemview.h \
   src/itemdelegate.h \
   src/itemfiltermodel.h \
   src/itemedit.h \
   src/itemhotkey.h \
   src/itemwindow.h \
   src/itemlock.h

RESOURCES += \
    resources/resources.qrc

DISTFILES += \
   resources/sql/schema.sql \
   resources/sql/data.sql \
   launcher.xml

unix {
   LIBS += -lxcb
   QT += x11extras
}

win32 {
   LIBS += user32.lib
   RC_FILE = resources/resources.rc
}
