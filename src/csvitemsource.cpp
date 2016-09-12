/*!
 * \file csvitemsource.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "csvitemsource.h"

CsvItemSource::CsvItemSource(const QString& identifier) : ItemSource(identifier)
{
}

bool CsvItemSource::read(QIODevice* device)
{
   return false;
}

bool CsvItemSource::write(QIODevice* device) const
{
   return false;
}

void CsvItemSource::reset()
{
}

const ItemGroups& CsvItemSource::itemGroups() const
{
   return ItemGroups();
}

const Imports& CsvItemSource::imports() const
{
   return Imports();
}

QString CsvItemSource::errorString() const
{
   return QString();
}

QPoint CsvItemSource::errorPosition() const
{
   return QPoint();
}
