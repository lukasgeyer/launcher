/*!
 * \file importgroupitem.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORTGROUPITEM_H
#define IMPORTGROUPITEM_H

#include "importitem.h"
#include "groupitem.h"

/*!
 * \brief A group of items.
 */
class ImportGroupItem : public GroupItem
{
public:
   /*!
    * Creates an item group.
    */
   ImportGroupItem() : GroupItem(Item::Type::ImportGroup)
   {
   }

   /*!
    * \reimp
    */
   bool isContainerOf(Item::Type type) const override
   {
      return (type == Item::Type::Import);
   }

   /*!
    * \reimp
    */
   static bool isBaseOf(Item::Type type)
   {
      return (type == Item::Type::ImportGroup);
   }

private:
   /*!
    * Inserts the item group \a itemGroup into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const ImportGroupItem& itemGroup);
};

#endif // IMPORTGROUPITEM_H
