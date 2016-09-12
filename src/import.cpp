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

Import::Import(const QString& file, const QString& type) : file_(file), type_(type)
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

void Import::setType(const QString& type)
{
    type_ = type;
}

QString Import::type() const
{
   return type_;
}

QDebug operator<<(QDebug stream, const Import& import)
{
   return (stream << import.file() << import.type());
}

