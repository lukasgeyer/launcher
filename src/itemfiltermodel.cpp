/*!
 * \file filtermodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "itemfiltermodel.h"
#include "itemmodel.h"
#include "searchexpression.h"

ItemFilterModel::ItemFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

void ItemFilterModel::setSearchExpression(const QString& expression)
{
   searchExpression_.setExpression(expression);

   invalidateFilter();
}

bool ItemFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
   const auto &name = sourceModel()->index(sourceRow, 0, sourceParent).data(ItemModel::NameRole).value<QString>();
   const auto &tags = sourceModel()->index(sourceRow, 0, sourceParent).data(ItemModel::TagsRole).value<QStringList>();

   bool matches = searchExpression_.Matches(name, tags);

   return matches;
}

bool ItemFilterModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
   const auto& nameLeft = sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()).data(ItemModel::NameRole).value<QString>();
   const auto& tagsLeft = sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()).data(ItemModel::TagsRole).value<QStringList>();

   const auto& nameRight = sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()).data(ItemModel::NameRole).value<QString>();
   const auto& tagsRight = sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()).data(ItemModel::TagsRole).value<QStringList>();

   //
   // A tagged item is sorted up, an non-tagged item is sorted down, if both are of the same
   // type sort lexically.
   //
   return ((tagsLeft.isEmpty() == false) && (tagsRight.isEmpty() == true )) ? true  :
          ((tagsLeft.isEmpty() == true ) && (tagsRight.isEmpty() == false)) ? false :
          ((nameLeft < nameRight));
}
