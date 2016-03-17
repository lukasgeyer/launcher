/*!
 * \file itemdelegate.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QSize>
#include <QStyledItemDelegate>

/*!
 * \brief A delegate painting an item.
 */
class ItemDelegate : public QStyledItemDelegate
{
public:
   /*!
    * Constructs an ItemDelegate with the parent \a parent.
    */
   ItemDelegate(QObject* parent = nullptr);

   /*!
    * \reimp
    */
   void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

   /*!
    * \reimp
    */
   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // ITEMDELEGATE_H
