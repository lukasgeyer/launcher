/*!
 * \file linkgroupitem.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "linkgroupitem.h"

LinkGroupItem::LinkGroupItem() : GroupItem(Item::ItemType::LinkGroup)
{
}

void LinkGroupItem::appendTag(const QString& tag)
{
   tags_.append(tag);
}

void LinkGroupItem::setTags(const QStringList& tags)
{
   tags_ = tags;
}

const QStringList& LinkGroupItem::tags() const
{
   return tags_;
}

void LinkGroupItem::setBrush(const QBrush& brush)
{
   brush_ = brush;
}

const QBrush&LinkGroupItem::brush() const
{
   return brush_;
}

QDebug operator<<(QDebug stream, const LinkGroupItem& itemGroup)
{
   return (stream << itemGroup.name() << itemGroup.brush() << itemGroup.tags());
}
