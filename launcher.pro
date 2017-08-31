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

VERSION = 1.1.4
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
    src/itemdelegate.cpp \
    src/application.cpp \
    src/itemmodel.cpp \
    src/systemlock.cpp \
    src/systemhotkey.cpp \
    src/searchexpression.cpp \
    src/xmlitemsource.cpp \
    src/csvitemsource.cpp \
    src/searchwindow.cpp \
    src/linkitem.cpp \
    src/importitem.cpp \
    src/groupitem.cpp \
    src/linkgroupitem.cpp \
    src/importitemreader.cpp \
    src/searchitemproxymodel.cpp \
    src/searchitemfiltermodel.cpp \
    src/itemsourceeditor.cpp \
    src/itemsourcehighlighter.cpp \
    src/itemsourceposition.cpp \
    src/searchbarwidget.cpp

HEADERS += \
    src/itemdelegate.h \
    src/event.h \
    src/application.h \
    src/itemmodel.h \
    src/systemlock.h \
    src/systemhotkey.h \
    src/searchexpression.h \
    src/itemsource.h \
    src/xmlitemsource.h \
    src/itemsourcefactory.h \
    src/csvitemsource.h \
    src/searchwindow.h \
    src/linkitem.h \
    src/item.h \
    src/importitem.h \
    src/groupitem.h \
    src/importgroupitem.h \
    src/linkgroupitem.h \
    src/itemeditor.h \
    src/itemfactory.h \
    src/factory.h \
    src/importitemreader.h \
    src/searchitemproxymodel.h \
    src/searchitemfiltermodel.h \
    src/itemsourceeditor.h \
    src/itemsourcehighlighter.h \
    src/itemsourceposition.h \
    src/metatype.h \
    src/searchbarwidget.h \
    src/searchresultwidget.h

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
