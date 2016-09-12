/*!
 * \file itemgroup.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <QBrush>

#include "items.h"
#include "tags.h"

/*!
 * \brief A group of items.
 */
class ItemGroup
{
public:
   /*!
    * Appends the item \a item to the item group.
    */
   void appendItem(const Item& item);
   /*!
    * Returns the list of items in the item group.
    */
   const Items& items() const;

   /*!
    * Appends the tag \a tag to the list of tags common for the items in the group.
    */
   void appendTag(const Tag& tag);
   /*!
    * Returns the list of tags common for the items in this group.
    */
   const Tags& tags() const;

   /*!
    * Sets the brush for the item group to \a brush.
    */
   void setBrush(const QBrush& brush);
   /*!
    * Returns the brush for the item group.
    */
   const QBrush& brush() const;

private:
   /*!
    * The list of items in the item group.
    */
   Items items_;

   /*!
    * The list of tags common for the items in the group.
    */
   Tags tags_;

   /*!
    * The brush used to represent items in the item group.
    */
   QBrush brush_;
};

#endif // ITEMGROUP_H
