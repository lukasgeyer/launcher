/*!
 * \file itemeditdialog.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDialog>

#ifndef ITEMEDITDIALOG_H
#define ITEMEDITDIALOG_H

class QStackedLayout;
class QTreeView;

class ItemModel;

/*!
 * \brief A dialog used to edit items.
 */
class ItemEditDialog : public QDialog
{
   Q_OBJECT

public:
   /*!
    * Constructs an item edit dialog with the parent \a parent and the flags \a windowFlags.
    */
   ItemEditDialog(ItemModel *itemModel, QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

private:
   /*!
    * The layout containing the item editors.
    */
   QStackedLayout* editorLayout_ = nullptr;
};

#endif // ITEMEDITDIALOG_H
