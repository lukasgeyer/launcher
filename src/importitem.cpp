/*!
 * \file import.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "importitem.h"

ImportItem::ImportItem(const QString& file, const QString& mimeType) : Item(Item::Type::Import), file_(file), mimeType_(mimeType)
{
}

void ImportItem::setFile(const QString& file)
{
    file_ = file;
}

QString ImportItem::file() const
{
    return file_;
}

void ImportItem::setMimeType(const QString& type)
{
    mimeType_ = type;
}

QString ImportItem::mimeType() const
{
   return mimeType_;
}

QDebug operator<<(QDebug stream, const ImportItem& import)
{
   return (stream << import.file() << import.mimeType());
}

