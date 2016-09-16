/*!
 * \file itemmodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <algorithm>
#include <chrono>

#include <QFileInfo>
#include <QUuid>

#include "application.h"
#include "importgroupitem.h"
#include "itemmodel.h"
#include "linkgroupitem.h"

namespace {

static const std::chrono::milliseconds DEFAULT_IMPORT_RETRY_TIMEOUT_ = std::chrono::seconds(10);

} // namespace

ItemModel::ItemModel(QObject* parent) : QAbstractItemModel(parent)
{
}

ItemModel::~ItemModel()
{
}

bool ItemModel::read(QIODevice* device)
{
   beginResetModel();

   bool result = XmlItemSource::read(device);

   endResetModel();

   // Process import.

   return result;
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex& parent) const
{
   QModelIndex index;

   GroupItem* parentItem = const_cast<ItemModel*>(this);
   if (parent.isValid() == true)
   {
      parentItem = static_cast<GroupItem*>(parent.internalPointer());
   }

   if ((row >= 0) && (row < parentItem->items().size()) && (column == 0))
   {
      index = createIndex(row, column, parentItem->item(row));
   }

   return index;
}

QModelIndex ItemModel::parent(const QModelIndex& child) const
{
   QModelIndex index;

   if (child.isValid() == true)
   {
      Item* parentItem = static_cast<Item*>(child.internalPointer())->parent();
      if ((parentItem != nullptr) && (parentItem != this))
      {
         index = createIndex(child.row(), 0, parentItem);
      }
   }

   return index;
}

int ItemModel::rowCount(const QModelIndex& parent) const
{
   int rowCount = 0;

   GroupItem* parentItem = const_cast<ItemModel*>(this);
   if (parent.isValid() == true)
   {
      parentItem = static_cast<GroupItem*>(parent.internalPointer());
   }

   if (parentItem->cast<GroupItem>() != nullptr)
   {
      rowCount = parentItem->items().size();
   }

   return rowCount;
}

int ItemModel::columnCount(const QModelIndex& parent) const
{
   return 1;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   QVariant data;

   if (index.isValid() == true)
   {
      if (auto item = static_cast<Item*>(index.internalPointer())->cast<LinkItem>())
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
         }
      }
      else if (auto item = static_cast<Item*>(index.internalPointer())->cast<ImportItem>())
      {
         switch (role)
         {
         case Qt::DisplayRole:
            data = QFileInfo(item->file()).fileName();
            break;
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->file();
            break;
         }
      }
      else if (auto item = static_cast<Item*>(index.internalPointer())->cast<GroupItem>())
      {
         switch (role)
         {
         case Qt::DisplayRole:
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->name();
            break;
         }
      }
   }

   return data;
}

Item* ItemModel::item(const QModelIndex& index)
{
   Item* item = nullptr;

   if (index.isValid() == true)
   {
      item = static_cast<Item*>(index.internalPointer());
   }

   return item;
}

const Item* ItemModel::item(const QModelIndex& index) const
{
   return const_cast<const Item*>(const_cast<ItemModel*>(this)->item(index));
}
