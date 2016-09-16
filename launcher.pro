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

VERSION = 1.1.0
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
    src/main.cpp \
    src/itemview.cpp \
    src/itemdelegate.cpp \
    src/itemfiltermodel.cpp \
    src/itemedit.cpp \
    src/geometrystore.cpp \
    src/application.cpp \
    src/itemmodel.cpp \
    src/systemlock.cpp \
    src/systemhotkey.cpp \
    src/searchexpression.cpp \
    src/itemsource.cpp \
    src/xmlitemsource.cpp \
    src/importer.cpp \
    src/csvitemsource.cpp \
    src/searchwindow.cpp \
    src/itemeditdialog.cpp \
    src/linkitem.cpp \
    src/importitem.cpp \
    src/groupitem.cpp \
    src/importgroupitem.cpp \
    src/linkgroupitem.cpp \
    src/linkitemproxymodel.cpp

HEADERS += \
    src/itemview.h \
    src/itemdelegate.h \
    src/itemfiltermodel.h \
    src/itemedit.h \
    src/geometrystore.h \
    src/event.h \
    src/application.h \
    src/itemmodel.h \
    src/systemlock.h \
    src/systemhotkey.h \
    src/searchexpression.h \
    src/itemsource.h \
    src/xmlitemsource.h \
    src/itemsourcefactory.h \
    src/importer.h \
    src/csvitemsource.h \
    src/searchwindow.h \
    src/metatype.h \
    src/itemeditdialog.h \
    src/linkitem.h \
    src/item.h \
    src/importitem.h \
    src/groupitem.h \
    src/importgroupitem.h \
    src/linkgroupitem.h \
    src/linkitemproxymodel.h

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
