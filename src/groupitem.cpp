/*!
 * \file groupitem.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QtAlgorithms>

#include "groupitem.h"

GroupItem::GroupItem() : Item(Item::Type::Group)
{
}

GroupItem::~GroupItem()
{
   qDeleteAll(items_);
}

void GroupItem::setName(const QString& name)
{
   name_ = name;
}

QString GroupItem::name() const
{
   return name_;
}

void GroupItem::insertItem(Item* item, int position)
{
   if (item != nullptr)
   {
      item->setParent(this);
   }

   items_.insert(position, item);
}

bool GroupItem::removeItem(Item* item)
{
   return items_.removeOne(item);
}

void GroupItem::removeItems()
{
   items_.clear();
}

void GroupItem::setItems(const QVector<Item*>& items)
{
   qDeleteAll(items_);

   items_ = items;
}

const QVector<Item*>& GroupItem::items() const
{
   return items_;
}

int GroupItem::itemCount() const
{
   return items_.size();
}

Item* GroupItem::item(int index)
{
   Item* item = nullptr;

   if ((index >= 0) && (index < items_.size()))
   {
      item = items_[index];
   }

   return item;
}

const Item* GroupItem::item(int index) const
{
   return const_cast<const Item*>(const_cast<GroupItem*>(this)->item(index));
}

int GroupItem::indexOf(Item* item) const
{
   return items_.indexOf(item);
}

GroupItem::GroupItem(Type type) : Item(type)
{
}
