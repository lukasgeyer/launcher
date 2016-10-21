/*!
 * \file linkitemproxymodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <algorithm>
#include <iterator>

#include "groupitem.h"
#include "itemmodel.h"
#include "linkgroupitem.h"
#include "linkitem.h"
#include "linkitemproxymodel.h"

LinkItemProxyModel::LinkItemProxyModel(QObject* parent) : QAbstractProxyModel(parent)
{
}

void LinkItemProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
   if (this->sourceModel() != nullptr)
   {
      //
      // If an item model was set previously disconnect from the reset signal, so the cache is
      // no longer rebuilt when this model changes.
      //
      disconnect(this->sourceModel(), &QAbstractItemModel::rowsInserted, this, &LinkItemProxyModel::reset_);
      disconnect(this->sourceModel(), &QAbstractItemModel::rowsRemoved, this, &LinkItemProxyModel::reset_);
      disconnect(this->sourceModel(), &QAbstractItemModel::modelReset, this, &LinkItemProxyModel::reset_);
   }

   //
   // Set the source model.
   //
   QAbstractProxyModel::setSourceModel(sourceModel);

   if (this->sourceModel() != nullptr)
   {
      //
      // If a valid item model has been set connect to the reset signal, so the cache is rebuilt
      // when this model changes.
      //
      connect(this->sourceModel(), &QAbstractItemModel::rowsInserted, this, &LinkItemProxyModel::reset_);
      connect(this->sourceModel(), &QAbstractItemModel::rowsRemoved, this, &LinkItemProxyModel::reset_);
      connect(this->sourceModel(), &QAbstractItemModel::modelReset, this, &LinkItemProxyModel::reset_);

      //
      // Immediately reset the model from the set source model and rebuild the cache.
      //
      reset_();
   }
}

QModelIndex LinkItemProxyModel::index(int row, int column, const QModelIndex& /* parent */) const
{
   QModelIndex index;

   if (isValid_(row, column))
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
   QModelIndex proxyIndex;

   auto cacheEntry = std::find_if(std::cbegin(cache_), std::cend(cache_), [sourceIndex](const CacheEntry_ &cacheEntry)
   {
      return (cacheEntry.index == sourceIndex);
   });

   if (cacheEntry != std::cend(cache_))
   {
      proxyIndex = createIndex(std::distance(std::cbegin(cache_), cacheEntry), sourceIndex.column());
   }

   return proxyIndex;
}

QModelIndex LinkItemProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
   QModelIndex sourceIndex;

   if (isValid_(proxyIndex))
   {
      sourceIndex = cache_[proxyIndex.row()].index;
   }

   return sourceIndex;
}

int LinkItemProxyModel::rowCount(const QModelIndex& /* parent */) const
{
   return cache_.size();
}

int LinkItemProxyModel::columnCount(const QModelIndex& /* parent */) const
{
   return 2;
}

QVariant LinkItemProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
   QVariant data;

   const LinkItem* item = this->item(proxyIndex);
   if (item != nullptr)
   {
      switch (proxyIndex.column())
      {
      case 0:
      {
         switch (role)
         {
         case Qt::DisplayRole:
            data = item->name();
            break;
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->link();
            break;
         case Qt::ForegroundRole:
            data = cache_[proxyIndex.row()].brush;
            break;
         }
         break;
      }
      case 1:
      {
         switch(role)
         {
         case Qt::DisplayRole:
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = cache_[proxyIndex.row()].tagString;
            break;
         case Qt::UserRole:
            data = cache_[proxyIndex.row()].tagStringList;
            break;
         case Qt::TextAlignmentRole:
            data = QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
            break;
         }
         break;
      }
      }
   }

   return data;
}

LinkItem* LinkItemProxyModel::item(const QModelIndex& proxyIndex)
{
   LinkItem* item = nullptr;

   ItemModel* itemModel = static_cast<ItemModel*>(sourceModel());
   if ((itemModel != nullptr) && (isValid_(proxyIndex)))
   {
      item = Item::cast<LinkItem>(itemModel->item(cache_[proxyIndex.row()].index));
   }

   return item;
}

const LinkItem* LinkItemProxyModel::item(const QModelIndex& proxyIndex) const
{
   return const_cast<const LinkItem*>(const_cast<LinkItemProxyModel*>(this)->item(proxyIndex));
}

void LinkItemProxyModel::reset_()
{
   beginResetModel();

   //
   // Clear the cache.
   //
   cache_.clear();

   //
   // Recursively add any link items to the cache.
   //
   if (auto itemModel = qobject_cast<ItemModel*>(sourceModel()))
   {
      itemModel->apply<LinkItem>([this](const QModelIndex& index, LinkItem* item)
      {
         CacheEntry_ cacheEntry{index, {}, item->tags(), item->brush()};

         for (auto parentItem = item->parent(); parentItem != nullptr; parentItem = parentItem->parent())
         {
            if (auto parentLinkGroupItem = Item::cast<LinkGroupItem>(parentItem))
            {
               cacheEntry.tagStringList.append(parentLinkGroupItem->tags());

               if (cacheEntry.brush.style() == Qt::NoBrush)
               {
                  cacheEntry.brush = parentLinkGroupItem->brush();
               }
            }
         }

         cacheEntry.tagString = cacheEntry.tagStringList.join(QStringLiteral(", "));

         cache_.append(cacheEntry);
      });
   }

   endResetModel();
}
