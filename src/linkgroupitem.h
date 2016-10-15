/*!
 * \file linkgroupitem.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef LINKGROUPITEM_H
#define LINKGROUPITEM_H

#include <QBrush>
#include <QString>

#include "groupitem.h"
#include "linkitem.h"

/*!
 * \brief A group of items.
 */
class LinkGroupItem : public GroupItem
{
public:
   /*!
    * Constructs a link group item.
    */
   LinkGroupItem();

   /*!
    * Appends the tag \a tag to the list of tags common for the items in the group.
    */
   void appendTag(const QString& tag);
   /*!
    * Sets the list of tags.
    */
   void setTags(const QStringList& tags);
   /*!
    * Returns the list of tags common for the items in this group.
    */
   const QStringList& tags() const;

   /*!
    * Sets the brush for the item group to \a brush.
    */
   void setBrush(const QBrush& brush);
   /*!
    * Returns the brush for the item group.
    */
   const QBrush& brush() const;

   static inline bool isItemType(Item::ItemType itemType)
   {
      return (itemType == Item::ItemType::LinkGroup);
   }

private:
   /*!
    * The list of tags common for the items in the group.
    */
   QStringList tags_;

   /*!
    * The name of the item group.
    */
   QString name_;
   /*!
    * The brush used to represent items in the item group.
    */
   QBrush brush_;

   /*!
    * Inserts the item group \a itemGroup into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const LinkGroupItem& itemGroup);
};

#endif // LINKGROUPITEM_H
