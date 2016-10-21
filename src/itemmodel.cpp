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

#include <QtGlobal>
#include <QFileInfo>
#include <QUuid>

#include "application.h"
#include "importgroupitem.h"
#include "itemmodel.h"
#include "linkgroupitem.h"
#include "xmlitemsource.h"

namespace {

static const std::chrono::milliseconds DEFAULT_IMPORT_RETRY_TIMEOUT_ = std::chrono::seconds(10);

} // namespace

ItemModel::ItemModel(QObject* parent) : QAbstractItemModel(parent)
{

}

ItemModel::~ItemModel()
{
}

void ItemModel::read(const QString& fileName)
{
   QFile file(fileName);
   if (file.open(QIODevice::ReadOnly))
   {
      beginResetModel();

      auto source = new XmlItemSource;
      source->read(&file);

      GroupItem::insertItem(source, itemCount());

      endResetModel();
   }

   //< TODO: Implement recursive reading.
}

void ItemModel::write(const QString& fileName)
{
   QFile file(fileName);
   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      for (auto item : items())
      {
         if (auto itemSource = Item::cast<ItemSource>(item))
         {
            Q_UNUSED(itemSource);

            //< TODO: Implement recursive writing.
         }
      }
   }
}

bool ItemModel::insertItem(Item* item, int row, const QModelIndex& parent)
{
   bool itemInserted = false;

   if (GroupItem* groupItem = Item::cast<GroupItem>(static_cast<Item*>(parent.internalPointer())))
   {
      row = qMin(row, groupItem->itemCount());

      beginInsertRows(parent, row, row);

      groupItem->insertItem(item, row);

      endInsertRows();
   }

   return itemInserted;
}

bool ItemModel::removeItem(const QModelIndex& index)
{
   bool itemRemoved = false;

   if (auto item = static_cast<Item*>(index.internalPointer()))
   {
      if (auto groupItem = Item::cast<GroupItem>(item->parent()))
      {
         beginRemoveRows(index.parent(), index.row(), index.row());

         groupItem->removeItem(item);

         endRemoveRows();
      }
   }

   return itemRemoved;
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex& parent) const
{
   QModelIndex index;

   const GroupItem* groupItem = this;
   if (parent.isValid())
   {
      groupItem = static_cast<const GroupItem*>(parent.internalPointer());
   }

   Item* item = const_cast<Item*>(groupItem->item(row));
   if (item != nullptr)
   {
      index = createIndex(row, column, item);
   }

   return index;
}

QModelIndex ItemModel::parent(const QModelIndex& child) const
{
   QModelIndex index;

   Item* item = static_cast<GroupItem*>(child.internalPointer());
   if (item != nullptr)
   {
      GroupItem* parentItem = static_cast<GroupItem*>(item->parent());
      if (parentItem != this)
      {
         index = createIndex(0, 0, item->parent());
      }
   }

   return index;
}

int ItemModel::rowCount(const QModelIndex& parent) const
{
   int rowCount = 0;

   auto item = ((!parent.isValid()) ? (this) : (Item::cast<GroupItem>(static_cast<Item*>(parent.internalPointer()))));
   if (item != nullptr)
   {
      rowCount = item->items().count();
   }

   return rowCount;
}

int ItemModel::columnCount(const QModelIndex& /* parent */) const
{
   return 1;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   QVariant data;

   if (index.isValid())
   {
      if (auto item = Item::cast<LinkItem>(static_cast<Item*>(index.internalPointer())))
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
      else if (auto item = Item::cast<ImportItem>(static_cast<Item*>(index.internalPointer())))
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
      else if (auto item = Item::cast<GroupItem>(static_cast<Item*>(index.internalPointer())))
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

   if (index.isValid())
   {
      item = static_cast<Item*>(index.internalPointer());
   }

   return item;
}

const Item* ItemModel::item(const QModelIndex& index) const
{
   return const_cast<const Item*>(const_cast<ItemModel*>(this)->item(index));
}
