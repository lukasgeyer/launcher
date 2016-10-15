/*!
 * \file linkitem.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "linkitem.h"

LinkItem::LinkItem() : Item(Item::Type::Link)
{
}

void LinkItem::setName(const QString& name)
{
   name_ = name;
}

QString LinkItem::name() const
{
   return name_;
}

void LinkItem::setLink(const QString& link)
{
   link_ = link;
}

QString LinkItem::link() const
{
   return link_;
}

void LinkItem::setBrush(const QBrush& brush)
{
   brush_ = brush;
}

QBrush LinkItem::brush() const
{
   return brush_;
}

void LinkItem::appendTag(const QString& tag)
{
   tags_.append(tag);
}

void LinkItem::setTags(const QStringList& tags)
{
   tags_ = tags;
}

const QStringList& LinkItem::tags() const
{
   return tags_;
}

QDebug operator<<(QDebug stream, const LinkItem& item)
{
   return (stream << item.name() << item.link() << item.brush() << item.tags());
}
