/*!
 * \file searchitemfiltermodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QVariant>

#include "searchitemfiltermodel.h"
#include "itemmodel.h"
#include "searchitemproxymodel.h"
#include "searchexpression.h"

SearchItemFilterModel::SearchItemFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

LinkItem* SearchItemFilterModel::item(const QModelIndex& proxyIndex)
{
   return static_cast<SearchItemProxyModel*>(sourceModel())->item(mapToSource(proxyIndex));
}

const LinkItem* SearchItemFilterModel::item(const QModelIndex& proxyIndex) const
{
   return const_cast<const LinkItem*>(const_cast<SearchItemFilterModel*>(this)->item(proxyIndex));
}

void SearchItemFilterModel::setSearchExpression(const QString& expression)
{
   searchExpression_.setExpression(expression);

   invalidateFilter();
}

bool SearchItemFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
   bool matches = false;

   if (!searchExpression_.isEmpty() || !(filter_ & EmptySearchExpressionFilter))
   {
      const SearchItemProxyModel* itemModel = static_cast<const SearchItemProxyModel*>(sourceModel());
      if (itemModel != nullptr)
      {
         matches = searchExpression_.matches(itemModel->data(itemModel->index(sourceRow, 0, sourceParent), Qt::DisplayRole).toString(),
                                             itemModel->data(itemModel->index(sourceRow, 1, sourceParent), Qt::UserRole).toStringList());

         if (matches && (filter_ & ParameterMismatchFilter))
         {
            if (auto linkItem = Item::cast<LinkItem>(itemModel->item(itemModel->index(sourceRow, 0, sourceParent))))
            {
               matches = (linkItem->linkMatches(searchExpression_.parameters()) == LinkItem::LinkMatch::SufficientParameters);
            }
         }
      }
   }

   return matches;
}

bool SearchItemFilterModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
   bool isLessThan = true;

   SortAlgorithm sortAlgorithm = static_cast<SortAlgorithm>(sourceLeft.column());

   switch (sortAlgorithm)
   {
   case NameSortAlgorithm:
   {
      const auto& leftName = sourceModel()->data(sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()), Qt::DisplayRole).toString();
      const auto& rightName = sourceModel()->data(sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()), Qt::DisplayRole).toString();

      isLessThan = (QString::compare(leftName, rightName, Qt::CaseInsensitive) < 0);

      break;
   }
   case TagSortAlgorithm:
   {
      const auto& leftName = sourceModel()->data(sourceModel()->index(sourceLeft.row(), 0, sourceLeft.parent()), Qt::DisplayRole).toString();
      const auto& leftTags = sourceModel()->data(sourceModel()->index(sourceLeft.row(), 1, sourceLeft.parent()), Qt::UserRole).toStringList();

      const auto& rightName = sourceModel()->data(sourceModel()->index(sourceRight.row(), 0, sourceRight.parent()), Qt::DisplayRole).toString();
      const auto& rightTags = sourceModel()->data(sourceModel()->index(sourceRight.row(), 1, sourceRight.parent()), Qt::UserRole).toStringList();

      for (auto leftTag = std::begin(leftTags), rightTag = std::begin(rightTags); ; ++leftTag, ++rightTag)
      {
         if (leftTag != std::end(leftTags) && rightTag != std::end(rightTags))
         {
            int comparisonResult = QString::compare(*leftTag, *rightTag, Qt::CaseInsensitive);
            if (comparisonResult != 0)
            {
               isLessThan = (comparisonResult < 0);

               break;
            }
         }
         else if (leftTag == std::end(leftTags) && rightTag == std::end(rightTags))
         {
            isLessThan = (QString::compare(leftName, rightName, Qt::CaseInsensitive) < 0);

            break;
         }
         else if (leftTag != std::end(leftTags) && rightTag == std::end(rightTags))
         {
            isLessThan = true;

            break;
         }
         else if (leftTag == std::end(leftTags) && rightTag != std::end(rightTags))
         {
            isLessThan = false;

            break;
         }
      }

      break;
   }
   }

   return isLessThan;
}
