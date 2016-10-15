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
   virtual ~ItemSource();

   /*!
    * Read the item source from the device \a device and returns \a true if the source could be
    * successfully read; \a false otherwise. The device must be already opened for reading. The
    * item group is not reset before reading, use \a reset().
    */
   virtual bool read(QIODevice *device) = 0;
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
    * \reimp
    */
   inline bool isContainerOf(Item::Type type) const override
   {
      return ((type == Item::Type::Link) ||
              (type == Item::Type::LinkGroup) ||
              (type == Item::Type::ImportGroup));
   }

protected:
   /*!
    * Constructs the item source.
    */
   explicit ItemSource();
};

#endif // ITEMSOURCE_H
