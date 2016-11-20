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
    * The result of link parameter matching.
    */
   enum class LinkMatch
   {
      SufficientParameters, /*< Sufficient parameter match. */
      InsufficientParameters, /*< Insufficient parameter match. */
      ExcessParameters /*< Excess parameter match. */
   };

   /*!
    * Creates an item for the item group \a parent.
    */
   LinkItem();

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
    * Returns the link with the parameters resolved as supplied in \a parameters.
    */
   QString link(const QStringList& parameters);
   /*!
    * Returns \a true if the link matches with there parameters \a parameters; \a false
    * otherwise.
    */
   LinkMatch linkMatches(const QStringList& parameters) const;

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
    * Returns \a true if the item is a or is a base of \a type.
    */
   static bool isBaseOf(Item::Type type)
   {
      return (type == Item::Type::Link);
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
    * The parameter count for the link. If 0, the link has no parameter (expect wildcard).
    */
   int linkParameterCount_ = 0;
   /*!
    * The parameter wildcard is present.
    */
   bool linkParameterWildcard_ = false;

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
