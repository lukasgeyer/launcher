/*!
 * \file itemsource.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCE_H
#define ITEMSOURCE_H

#include <QPoint>
#include <QString>
#include <QVector>

#include "groupitem.h"

class QIODevice;

/*!
 * \brief An abstract item source.
 */
class ItemSource : public GroupItem
{
public:
   /*!
    * Destructs the item source.
    */
   virtual ~ItemSource()
   {
   }

   /*!
    * Resets the item source, discarding all of its content.
    */
   virtual void reset()
   {
      GroupItem::removeItems();
   }

   /*!
    * Read the item source from the device \a device and returns \a true if the source could be
    * successfully read; \a false otherwise. The device must be already opened for reading. The
    * item group is not reset before reading, use \a reset().
    */
   virtual bool read(QIODevice *device, const QString& identifier) = 0;
   /*!
    * Writes the item source to the device \a device and returns \a true if the source could be
    * successfully written; \a false otherwise. The device must be already opened for writing.
    */
   virtual bool write(QIODevice *device) const = 0;

   /*!
    * Returns the string representation of the last error that occurred. The behaviour is undefined
    * if no error has occurred yet.
    */
   virtual QString errorString() const = 0;
   /*!
    * Returns the position within the item source of the last error that occurred. The behaviour is
    * undefined if no error has occurred yet.
    */
   virtual QPoint errorPosition() const = 0;

   /*!
    * Returns the identifier or an empty string if no identifier is set.
    */
   QString identifier() const
   {
      return identifier_;
   }

   /*!
    * \reimp
    */
   bool isContainerOf(Item::Type type) const override
   {
      return ((type == Item::Type::Link) ||
              (type == Item::Type::LinkGroup) ||
              (type == Item::Type::ImportGroup));
   }

   /*!
    * Returns \a true if the item is a or is a base of \a type.
    */
   static bool isBaseOf(Item::Type type)
   {
      return (type == Item::Type::Source);
   }

protected:
   /*!
    * Constructs the item source.
    */
   ItemSource() : GroupItem(Item::Type::Source)
   {
   }

   /*!
    * Sets the identifier to \a identifier.
    */
   void setIdentifier(const QString& identifier)
   {
      identifier_ = identifier;
   }

private:
   /*!
    * The identifier of the item source.
    */
   QString identifier_;
};

#endif // ITEMSOURCE_H
