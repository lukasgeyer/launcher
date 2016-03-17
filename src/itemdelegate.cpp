/*!
 * \file itemdelegate.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QPainter>

#include "itemdelegate.h"
#include "itemmodel.h"

ItemDelegate::ItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   ///
   /// Save painter.
   ///
   painter->save();

   ///
   /// Enable anti aliasing.
   ///
   painter->setRenderHint(QPainter::Antialiasing, true);
   painter->setRenderHint(QPainter::TextAntialiasing, true);

   ///
   /// Draw item border.
   ///
   painter->setPen(Qt::darkGray);
   painter->drawRect(option.rect);

   ///
   /// Draw item background.
   ///
   if ((option.state & QStyle::State_Selected) == 0)
   {
      painter->fillRect(option.rect.adjusted(1, 0, -1, 0), ((index.row() % 2) ? (QColor(236, 236, 236)) : (QColor(255, 255, 255))));
      painter->setPen(Qt::black);
   }
   else
   {
      painter->fillRect(option.rect.adjusted(1, 0, -1, 0), option.palette.highlight());
      painter->setPen(QPen(Qt::white));
   }

   ///
   /// Draw item name.
   ///
   painter->drawText(QRect((option.rect.left() + 4),
                           (option.rect.top() + 4),
                           (option.rect.width() / 2) - 8,
                           (option.rect.height() - 2)),
                           index.data(ItemModel::NameRole).toString(),
                           QTextOption(Qt::AlignLeft));
   ///
   /// Draw item tags.
   ///
   painter->setPen(QPen(Qt::lightGray));
   painter->drawText(QRect((option.rect.left() + 4) + (option.rect.width() / 2),
                           (option.rect.top() + 4),
                           (option.rect.width() / 2) - 8,
                           (option.rect.height() - 2)),
                           index.data(ItemModel::TagsRole).toStringList().join(QStringLiteral(", ")),
                           QTextOption(Qt::AlignRight));
   ///
   /// Restore painter.
   ///
   painter->restore();
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   QSize sizeHint = QStyledItemDelegate::sizeHint(option, index);

   return { sizeHint.width(), sizeHint.height() + 4 };
}

