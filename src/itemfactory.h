/*!
 * \file itemfactory.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "factory.h"
#include "item.h"

/*!
 * \brief An item factory.
 */
class ItemFactory : public Factory<Item::Type, Item, QString>
{
   /*!
    * Additional information supplied with this factory.
    */
   enum class AdditionalInformation
   {
      TypeName /*< The type name of the item */
   };

public:
   /*!
    * Returns the item types registered in this factory.
    */
   QList<Item::Type> types() const
   {
      return ids();
   }

   /*!
    * Returns the type name for the type \a itemType.
    */
   QString typeName(Item::Type type)
   {
      return additionalInformation<static_cast<std::size_t>(AdditionalInformation::TypeName)>(type);
   }
};

#endif // ITEMFACTORY_H
