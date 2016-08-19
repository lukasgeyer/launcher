#
# \file launcher.pro
#
# \copyright 2016 Lukas Geyer. All rights reseverd.
# \license This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License version 3 as
#          published by the Free Software Foundation.
#

QT += core gui widgets

TARGET = launcher
TEMPLATE = app

VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = "Launcher"
QMAKE_TARGET_COMPANY = "Lukas Geyer"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2016 Lukas Geyer. All rights reserved."
QMAKE_TARGET_DOMAIN = "https://github.com/lukasgeyer/launcher"

CONFIG += c++11 warn_on

DEFINES += APPLICATION_NAME=\\\"\"$$QMAKE_TARGET_PRODUCT\\\"\"
DEFINES += APPLICATION_VERSION=\\\"\"$$VERSION\\\"\"
DEFINES += ORGANIZATION_NAME=\\\"\"$$QMAKE_TARGET_COMPANY\\\"\"
DEFINES += ORGANIZATION_DOMAIN=\\\"\"$$QMAKE_TARGET_DOMAIN\\\"\"

SOURCES += \
    src/main.cpp\
    src/itemview.cpp \
    src/itemdelegate.cpp \
    src/itemfiltermodel.cpp \
    src/itemedit.cpp \
    src/itemwindow.cpp \
    src/sourceeditor.cpp \
    src/sourcehighlighter.cpp \
    src/geometrystore.cpp \
    src/application.cpp \
    src/sourceposition.cpp \
    src/indication.cpp \
    src/sourceerrorindication.cpp \
    src/source.cpp \
    src/sourcereader.cpp \
    src/sourceerror.cpp \
    src/item.cpp \
    src/itemmodel.cpp \
    src/systemlock.cpp \
    src/systemhotkey.cpp \
    src/import.cpp

HEADERS += \
    src/itemview.h \
    src/itemdelegate.h \
    src/itemfiltermodel.h \
    src/itemedit.h \
    src/itemwindow.h \
    src/sourceeditor.h \
    src/sourcehighlighter.h \
    src/geometrystore.h \
    src/event.h \
    src/application.h \
    src/indication.h \
    src/sourceposition.h \
    src/sourceerrorindication.h \
    src/indicator.h \
    src/source.h \
    src/sourcereader.h \
    src/sourceerror.h \
    src/item.h \
    src/items.h \
    src/itemmodel.h \
    src/systemlock.h \
    src/systemhotkey.h \
    src/import.h \
    src/imports.h

RESOURCES += \
    resources/resources.qrc

unix {
   LIBS += -lxcb
   QT += x11extras
}

win32 {
   LIBS += user32.lib
   RC_ICONS  = resources/images/logo.ico
}
