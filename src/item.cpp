/*!
 * \file item.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "item.h"

QString Item::applyProcessEnvironment(const QString &string, const QProcessEnvironment &processEnvironment)
{
   QString appliedString = string;

   const auto& keys = processEnvironment.keys();
   for (const auto& key : keys)
   {
      appliedString.replace((QStringLiteral("{") + key + QStringLiteral("}")), processEnvironment.value(key));
   }

   return appliedString;
}
