/*!
 * \file linkitemproxymodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "linkitemproxymodel.h"

void LinkItemProxyModel::setSourceModel(QAbstractItemModel* model)
{
   //< CONTINUE HERE
}

QModelIndex LinkItemProxyModel::index(int row, int column, const QModelIndex& /* parent */) const
{
   QModelIndex index;

   if ((row >= 0) && (row < fromSourceIndexCache_.size()) && (column == 0))
   {
      index = createIndex(row, column);
   }

   return index;
}

QModelIndex LinkItemProxyModel::parent(const QModelIndex& /* child */) const
{
   return QModelIndex();
}

QModelIndex LinkItemProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
   QModelIndex index;

   if ((sourceIndex.isValid() == true) && (sourceIndex.row() >= 0) && (sourceIndex.row() < fromSourceIndexCache_.size()) && (sourceIndex.column() == 0))
   {
      index = fromSourceIndexCache_[sourceIndex.row()];
   }

   return index;
}

QModelIndex LinkItemProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
   QModelIndex index;

   if ((proxyIndex.isValid() == true) && (proxyIndex.row() >= 0) && (proxyIndex.row() < toSourceIndexCache_.size()) && (proxyIndex.column() == 0))
   {
      index = toSourceIndexCache_[proxyIndex.row()];
   }

   return index;
}

int LinkItemProxyModel::rowCount(const QModelIndex& /* parent */) const
{
   return fromSourceIndexCache_.size();
}

int LinkItemProxyModel::columnCount(const QModelIndex& parent) const
{
   return 1;
}

QVariant LinkItemProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
   QVariant data;

   return data;
}
