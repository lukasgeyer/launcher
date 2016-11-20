/*!
 * \file csvitemsource.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "csvitemsource.h"

bool CsvItemSource::read(QIODevice* /* device */, const QString& identifier)
{
   ItemSource::setIdentifier(identifier);

   return false;
}

bool CsvItemSource::write(QIODevice* /* device */) const
{
   return false;
}

QString CsvItemSource::errorString() const
{
   return QString();
}

QPoint CsvItemSource::errorPosition() const
{
   return QPoint();
}
