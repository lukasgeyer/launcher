/*!
 * \file linkitemfiltermodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QVariant>

#include "itemfiltermodel.h"
#include "itemmodel.h"
#include "linkitemproxymodel.h"
#include "searchexpression.h"

LinkItemFilterModel::LinkItemFilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

LinkItem* LinkItemFilterModel::item(const QModelIndex& proxyIndex)
{
   return static_cast<LinkItemProxyModel*>(sourceModel())->item(mapToSource(proxyIndex));
}

const LinkItem* LinkItemFilterModel::item(const QModelIndex& proxyIndex) const
{
   return const_cast<const LinkItem*>(const_cast<LinkItemFilterModel*>(this)->item(proxyIndex));
}

void LinkItemFilterModel::setSearchExpression(const QString& expression)
{
   searchExpression_.setExpression(expression);

   invalidateFilter();
}

bool LinkItemFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
   bool matches = false;

   const LinkItemProxyModel* itemModel = static_cast<const LinkItemProxyModel*>(sourceModel());
   if (itemModel != nullptr)
   {
      matches = searchExpression_.matches(itemModel->data(itemModel->index(sourceRow, 0, sourceParent), Qt::DisplayRole).toString(),
                                          itemModel->data(itemModel->index(sourceRow, 1, sourceParent), Qt::UserRole).toStringList());
   }

   return matches;
}

bool LinkItemFilterModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
   bool isLessThan = false;

   const LinkItemProxyModel* itemModel = static_cast<const LinkItemProxyModel*>(sourceModel());

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
