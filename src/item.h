/*!
 * \file item.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEM_H
#define ITEM_H

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "sourceposition.h"

/*!
 * An item as found in a source.
 */
class Item
{
public:
   /*!
    * Constructs an item which has been read from the source \a source.
    */
   explicit Item(const QString& sourceFile = QString());

   /*!
    * Sets the source this item originated from to \a source.
    */
   void setSource(const QString& sourceFile);
   /*!
    * Returns the source this item originated from.
    */
   QString sourceFile() const;

   /*!
    * Sets the name of this item to \a name.
    */
   void setName(const QString& name);
   /*!
    * Returns the name of this item.
    */
   QString name() const;

   /*!
    * Sets the link this item refers to to \a link.
    */
   void setLink(const QUrl& link);
   /*!
    * Returns the link this item refers to.
    */
   QUrl link() const;

   /*!
    * Sets the position of the link this item refers to within the source tp \a linkPosition.
    */
   void setLinkPosition(const SourcePosition& linkPosition);
   /*!
    * Returns the position of the link this item refers to within the source.
    */
   SourcePosition linkPosition() const;

   /*!
    * Appends the tag \a tag to the tags attached to this item.
    */
   void appendTag(const QString& tag);
   /*!
    * Appends all tags in \a tags to the tags attached to this item.
    */
   void appendTags(const QStringList &tags);
   /*!
    * Sets the tags attached to this item to \a tags.
    */
   void setTags(const QStringList& tags);
   /*!
    * Returns the tags attached to this item.
    */
   QStringList tags() const;

private:
   /*!
    * The source this item originated from.
    */
   QString sourceFile_;

   /*!
    * The name of this item.
    */
   QString name_;

   /*!
    * The link this item refers to.
    */
   QUrl link_;
   /*!
    * The position of the link this item refers to within the source.
    */
   SourcePosition linkPosition_;

   /*!
    * The tags attached to this item.
    */
   QStringList tags_;

   /*!
    * Inserts the item \a item into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const Item &item);
};


#endif // ITEM_H
