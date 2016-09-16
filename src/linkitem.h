/*!
 * \file linkitem.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef LINKITEM_H
#define LINKITEM_H

#include <memory>

#include <QBrush>
#include <QDebug>
#include <QString>

#include "item.h"

class LinkGroupItem;

/*!
 * An item as found in a source.
 */
class LinkItem : public Item
{
public:
   /*!
    * Creates an item for the item group \a parent.
    */
   explicit LinkItem();

   /*!
    * Sets the name of the item to \a name.
    */
   void setName(const QString& name);
   /*!
    * Returns the name of the item.
    */
   QString name() const;

   /*!
    * Sets the link the item refers to to \a link.
    */
   void setLink(const QString& link);
   /*!
    * Returns the link the item refers to.
    */
   QString link() const;

   /*!
    * Sets the brush of the item to \a brush.
    */
   void setBrush(const QBrush& brush);
   /*!
    * Returns the bursh of the item or an Qt::NoBrush if no brush is set.
    */
   QBrush brush() const;

   /*!
    * Appends the tag \a tag to the list of tags for the item.
    */
   void appendTag(const QString& tag);
   /*!
    * Sets the tags to \a tags.
    */
   void setTags(const QStringList& tags);
   /*!
    * Returns the tags attached to the item.
    */
   const QStringList& tags() const;

   /*!
    * The type of the item.
    */
   static inline bool isItemType(Item::ItemType itemType)
   {
      return (itemType == Item::ItemType::Link);
   }

private:
   /*!
    * The name of the item.
    */
   QString name_;
   /*!
    * The link the item refers to.
    */
   QString link_;

   /*!
    * The brush that shall be used to represent the item.
    */
   QBrush brush_;

   /*!
    * The tags attached to the item.
    */
   QStringList tags_;

   /*!
    * Inserts the item \a item into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const LinkItem& item);
};


#endif // LINKITEM_H
