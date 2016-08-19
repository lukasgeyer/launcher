/*!
 * \file import.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "import.h"

Import::Import(const QString& file) : file_(file)
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

bool operator==(const Import& lhs, const Import& rhs)
{
   return (lhs.file() == rhs.file());
}

bool operator!=(const Import& lhs, const Import& rhs)
{
   return ((lhs == rhs) == false);
}

QDebug operator<<(QDebug stream, const Import &import)
{
   return (stream << "file:" << import.file());
}
