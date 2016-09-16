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
   bool matches = false;

//   const auto& sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
//   if (sourceIndex.isValid() == true)
//   {
//      const auto& name = sourceIndex.data(static_cast<int>(ItemModel::Role::Name)).value<QString>();
//      const auto& tags = sourceIndex.data(static_cast<int>(ItemModel::Role::Tags)).value<QStringList>();

//      matches = searchExpression_.Matches(name, tags);
//   }

   return matches;
}

bool ItemFilterModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
   bool isLessThan = false;

//   const auto& leftSourceIndex = sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent());
//   const auto& rightSourceIndex = sourceModel()->index(sourceRight.row(), 0, sourceRight.parent());

//   if ((leftSourceIndex.isValid() == true) && (rightSourceIndex.isValid() == true))
//   {
//      const auto& leftName = leftSourceIndex.data(static_cast<int>(ItemModel::Role::Name)).value<QString>();
//      const auto& leftTags = leftSourceIndex.data(static_cast<int>(ItemModel::Role::Tags)).value<QStringList>();

//      const auto& rightName = rightSourceIndex.data(static_cast<int>(ItemModel::Role::Name)).value<QString>();
//      const auto& rightTags = rightSourceIndex.data(static_cast<int>(ItemModel::Role::Tags)).value<QStringList>();

//      //
//      // A tagged item is sorted up, an non-tagged item is sorted down, if both are of the same
//      // type sort lexically.
//      //
//      isLessThan = ((leftTags.isEmpty() == false) && (rightTags.isEmpty() == true )) ? true  :
//                   ((leftTags.isEmpty() == true ) && (rightTags.isEmpty() == false)) ? false :
//                   ((leftName < rightName));
//   }

   return isLessThan;
}
