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

   if (!searchExpression_.isEmpty() || !filterEmptySearchExpression_)
   {
      const SearchItemProxyModel* itemModel = static_cast<const SearchItemProxyModel*>(sourceModel());
      if (itemModel != nullptr)
      {
         matches = searchExpression_.matches(itemModel->data(itemModel->index(sourceRow, 0, sourceParent), Qt::DisplayRole).toString(),
                                             itemModel->data(itemModel->index(sourceRow, 1, sourceParent), Qt::UserRole).toStringList());

         if (matches && filterParameterMismatch_)
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
   bool isLessThan = false;

   const SearchItemProxyModel* itemModel = static_cast<const SearchItemProxyModel*>(sourceModel());

   const LinkItem* itemLeft = itemModel->item(sourceLeft);
   const LinkItem* itemRight = itemModel->item(sourceRight);

   if ((itemLeft != nullptr) && (itemRight != nullptr))
   {
      //
      // A tagged item is sorted up, an non-tagged item is sorted down, if both are of the same
      // type sort lexically.
      //
      isLessThan = ((!itemLeft->tags().isEmpty()) && ( itemRight->tags().isEmpty())) ? true  :
                   (( itemLeft->tags().isEmpty()) && (!itemRight->tags().isEmpty())) ? false :
                   (( itemLeft->name() < itemRight->name()));
   }

   return isLessThan;
}
