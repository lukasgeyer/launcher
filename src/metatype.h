/*!
 * \file metatype.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef METATYPE_H
#define METATYPE_H

#include <QMetaEnum>

namespace Enum {

/*!
 * Returns the string representation of the enum value \a value.
 */
template <typename EnumType> inline QString toString(EnumType value)
{
   return QMetaEnum::fromType<EnumType>().valueToKey(value);
}

/*!
 * Returns the enum value of the string representation of the enum value \a valueString or \a
 * defaultValue if there is no such conversion possible.
 */
template <typename EnumType> inline EnumType toValue(const QString& valueString, EnumType defaultValue)
{
   bool conversionPossible = false;

   EnumType value = QMetaEnum::fromType<EnumType>().keyToValue(valueString.toLatin1().data(), &conversionPossible);
   if (conversionPossible == false)
   {
      value = defaultValue;
   }

   return value;
}

} // namespace Enum

#endif // METATYPE_H
