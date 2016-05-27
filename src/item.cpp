/*!
 * \file sourceitem.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "item.h"

Item::Item(const QString& sourceFile) : sourceFile_(sourceFile)
{
}

void Item::setSource(const QString& sourceFile)
{
   sourceFile_ = sourceFile;
}

QString Item::sourceFile() const
{
   return sourceFile_;
}

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

void Item::setLinkPosition(const SourcePosition& linkPosition)
{
   linkPosition_ = linkPosition;
}

SourcePosition Item::linkPosition() const
{
   return linkPosition_;
}

void Item::appendTag(const QString& tag)
{
   tags_.append(tag);
}

void Item::appendTags(const QStringList& tags)
{
   tags_.append(tags);
}

void Item::setTags(const QStringList& tags)
{
   tags_ = tags;
}

QStringList Item::tags() const
{
   return tags_;
}

QDebug operator<<(QDebug stream, const Item &item)
{
   return (stream << "name:" << item.name() <<
                     "link:" << item.link().toString() <<
                     "tags:" << item.tags());
}
