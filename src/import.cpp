/*!
 * \file import.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "import.h"
#include "metatype.h"

REGISTER_METATYPE(Import)

Import::Import(const QString& file, const QString& mimeType) : file_(file), mimeType_(mimeType)
{
}

void Import::setFile(const QString& file)
{
    file_ = file;
}

QString Import::file() const
{
    return file_;
}

void Import::setMimeType(const QString& type)
{
    mimeType_ = type;
}

QString Import::mimeType() const
{
   return mimeType_;
}

QDebug operator<<(QDebug stream, const Import& import)
{
   return (stream << import.file() << import.mimeType());
}

