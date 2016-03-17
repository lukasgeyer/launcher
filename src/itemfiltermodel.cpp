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

ItemFilterModel::ItemFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

void ItemFilterModel::setFilterRegularExpressionPattern(const QString& regularExpressionPattern)
{
   ///
   /// As the regular expression is matched against each item optimize immediately.
   ///
   regularExpression_ = QRegularExpression(regularExpressionPattern, QRegularExpression::CaseInsensitiveOption);
   regularExpression_.optimize();

   invalidateFilter();
}

bool ItemFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
   const auto& name = sourceModel()->index(sourceRow, 0, sourceParent).data(ItemModel::NameRole).toString();
   const auto& tags = sourceModel()->index(sourceRow, 0, sourceParent).data(ItemModel::TagsRole).toStringList();

   ///
   /// A regular expression matches if it is either empty and a non-tagged item is requested or
   /// if it is not empty the name or one of the tags matches.
   ///
   return (((regularExpression_.pattern().isEmpty() == true) && (tags.isEmpty() == true)) ||
           ((regularExpression_.pattern().isEmpty() == false) &&
            ((regularExpression_.match(name).hasMatch()) ||
             (std::any_of(tags.begin(), tags.end(), [&](const QString& tag) { return regularExpression_.match(tag).hasMatch(); })))));
}

bool ItemFilterModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
   const auto& nameLeft = sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()).data(ItemModel::NameRole).toString();
   const auto& tagsLeft = sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()).data(ItemModel::TagsRole).toStringList();

   const auto& nameRight = sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()).data(ItemModel::NameRole).toString();
   const auto& tagsRight = sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()).data(ItemModel::TagsRole).toStringList();

   ///
   /// A tagged item is sorted up, an non-tagged item is sorted down, if both are of the same
   /// type sort lexically.
   ///
   return ((tagsLeft.isEmpty() == false) && (tagsRight.isEmpty() == true )) ? true  :
          ((tagsLeft.isEmpty() == true ) && (tagsRight.isEmpty() == false)) ? false :
          ((nameLeft < nameRight));
}
