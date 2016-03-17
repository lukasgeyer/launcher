/*!
 * \file itemview.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QApplication>
#include <QDesktopWidget>

#include "itemdelegate.h"
#include "itemview.h"

ItemView::ItemView(QWidget* parent) : QListView(parent)
{
   setItemDelegate(new ItemDelegate);
   setStyleSheet(QStringLiteral("QListView { background-color: transparent; border: none; }"));
}

QSize ItemView::sizeHint() const
{
   return { QListView::sizeHint().width(), (QListView::sizeHintForRow(0) * model()->rowCount()) };
}
