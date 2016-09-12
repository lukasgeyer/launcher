/*!
 * \file item.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "item.h"

void Item::setName(const QString& name)
{
   name_ = name;
}

QString Item::name() const
{
   return name_;
}

void Item::setLink(const QUrl& link)
{
   link_ = link;
}

QUrl Item::link() const
{
   return link_;
}

void Item::setBrush(const QBrush& color)
{
   brush_ = color;
}

QBrush Item::brush() const
{
   return brush_;
}

void Item::appendTag(const Tag& tag)
{
   tags_.append(tag);
}

const Tags& Item::tags() const
{
   return tags_;
}

QDebug operator<<(QDebug stream, const Item& item)
{
   return (stream << "name:" << item.name() << "link:" << item.link().toString() << "brush:" << item.brush() << "tags:" << item.tags());
}
